//****************************************************************************************************
//! \file CSimFireCore.cpp                                                                        
//! Module contains definitions of CSimFireCore class, declared in CSimFireCore.h, which ...    
//****************************************************************************************************
//                                                                                                  
//****************************************************************************************************
// 19. 11. 2025, V. Pospíšil, gdermog@seznam.cz                                                     
//****************************************************************************************************

#include <thread>

#include <SimFireStringTools.h>
#include <CSimFireCore.h>

namespace SimFire
{

   //****** CCSimFireCore ****************************************************************************

   CSimFireCore::CSimFireCore( const CSimFireSettings & settings ):
     mSettings( settings )
   {

   } /* CSimFireCore::CSimFireCore */


   //-------------------------------------------------------------------------------------------------

   CSimFireCore::~CSimFireCore() = default;


   //-------------------------------------------------------------------------------------------------

   bool CSimFireCore::Run()
   {
     if( mSettings.GetSeed() < 0 )
       std::srand( std::time( {} ) );
     else
       std::srand( mSettings.GetSeed() );

     //------- Preparing threads and workers for parallel simulations --------------------------------

     auto nrOfRuns = mSettings.GetRunsInGeneration();

     if( 0 == nrOfRuns )
     {
       WriteLogMessage( "CORE", "No runs to perform (number of runs is set to zero)" );
       return false;
     } // if

     uint32_t nThreads = ( mSettings.GetNumberOfThreads() > 0 ) ?
       static_cast<uint32_t>( mSettings.GetNumberOfThreads() ) :
       std::max( 1u, std::thread::hardware_concurrency() );

     std::vector<std::future<void>> vRunFutures( nThreads, {} );
     std::vector<std::string> vRunThreadIds( nThreads, {} );

     ListOfRunDescriptors_t vRunParams( nrOfRuns, {} );
     std::vector<std::pair<ListOfRunDescriptors_t::iterator, ListOfRunDescriptors_t::iterator>> 
       vRunParamsPerThreadBunch( nThreads, {} );
                        // For each simulation there is a set of parameters. Simulations are divided into
                        // bunches, each of them is processed by one thread. vRunParams vectors holds all
                        // parameter setss for all simulations, vRunParamsPerThreadBunch holds division
                        // of vRunParams into bunches for each thread.

     uint32_t itemsPerBatch = nrOfRuns / nThreads;
     uint32_t itemsPerBatchRest = nrOfRuns % nThreads;

     size_t firstItem = 0;
     size_t lastItem = 0;

     for( size_t thrdIdx = 0; thrdIdx < nThreads; ++thrdIdx )
     {                  // Prepare bunch of runs for each thread (by separating dedicated parameter sets 
                        // into separate groups)

       lastItem = firstItem + itemsPerBatch;
       if( 0 < itemsPerBatchRest )
       {
         ++lastItem;
         --itemsPerBatchRest;
       } // if

       if( nrOfRuns <= firstItem )
         break;

       auto bPars = vRunParams.begin();
       std::advance( bPars, firstItem );

       auto ePars = vRunParams.begin();
       if( lastItem < nrOfRuns )
         std::advance( ePars, lastItem );
       else
         ePars = vRunParams.end();

       vRunThreadIds[thrdIdx] = FormatStr( "THRD_%02u", thrdIdx + 1 );
       vRunParamsPerThreadBunch[thrdIdx] = std::make_pair( bPars, ePars );

       firstItem = lastItem;

     } // for


     //------- Main GA loop ------------------------------------------------------------------------------

     bool gaContinue = true;
     GenerateInitialGeneration( vRunParams );

     do
     {
       for( size_t thrdIdx = 0; thrdIdx < nThreads; ++thrdIdx )
       {
         WriteLogMessage( "CORE",
           FormatStr( "Creating thread %s to process runs #%zu to #%zu",
             vRunThreadIds[thrdIdx], firstItem, lastItem - 1 ) );

         vRunFutures[thrdIdx] = std::async( std::launch::async, [=] {
           RunBunch( vRunParamsPerThreadBunch[thrdIdx].first, vRunParamsPerThreadBunch[thrdIdx].second, vRunThreadIds[thrdIdx] );
           } );

         firstItem = lastItem;

       } // for

       for( auto & task : vRunFutures )
       {                // Wait for all simulation tasks to complete
         task.wait();
       } // for

       for( auto & item : vRunParams )
       {
         if( item.mReturnCode != CSimFireSingleRunParams::SimResCode_t::kEndedCollision )
         {
           WriteLogMessage( "CORE",
             FormatStr( "Run %s ended with code %s after %.2f s, min. distance to target was %.3f m, %s, %s, in t = %.2f s",
               item.mRunIdentifier,
               CSimFireSingleRunParams::GetStrValue( item.mReturnCode ),
               item.mSimTime,
               std::sqrt( item.mMinDTgtSq ),
               item.mNearHalfPlane ? "near" : "far",
               item.mLowHalfPlane ? "under" : "above",
               item.mMinTime ) );
         } // if
         else
         {
           WriteLogMessage( "CORE",
             FormatStr( "Run %s ended with code %s after %.2f s, the target was HIT in  t = %.2f s!",
               item.mRunIdentifier,
               CSimFireSingleRunParams::GetStrValue( item.mReturnCode ),
               item.mSimTime,
               item.mMinTime ) );
         } // else
       } // for

       gaContinue = true; // ContinueNextGAIteration( vRunParams );

       if( gaContinue )
         GenerateFollowingGeneration( vRunParams );

     } while( /*gaContinue*/ false );

     return true;

   } // CSimFireCore::Run

   //-------------------------------------------------------------------------------------------------

   void CSimFireCore::WriteLogMessage( const std::string & id, const std::string & mssg )
   {
     std::lock_guard lock( mLogMutex );

     std::cout << "[" << id << "]   " << mssg << std::endl;

   } // CSimFireCore::WriteLogMessage

   //-------------------------------------------------------------------------------------------------

   const std::string & CSimFireCore::GetLogFilePath()
   {
     static std::string dummy( "NOT YET KNOWN" );
     return dummy;
   } // CSimFireCore::GetLogFilePath

   //-------------------------------------------------------------------------------------------------

   void CSimFireCore::RunBunch(
     ListOfRunDescriptors_t::iterator runParamsBegin,
     ListOfRunDescriptors_t::iterator runParamsEnd,
     const std::string & threadId )
   {

     for( auto it = runParamsBegin; it != runParamsEnd; ++it )
       it->mReturnCode = CSimFireSingleRunParams::SimResCode_t::kNotStarted;

     CSimFireSingleRun runWorker( mSettings, BIND_SINGLE_RUN_LOG_CALLBACK( CSimFireCore::WriteLogMessage ) );

     unsigned nr = 0;
     std::string runList;
     for( auto it = runParamsBegin; it != runParamsEnd; ++it, ++nr )
     {
       if( !runList.empty() )
         runList += ", ";
       runList += it->mRunIdentifier;
     }

     WriteLogMessage( threadId, FormatStr( "Starting a batch of %zu runs: %s", 
         nr /*std::distance(runParamsBegin, runParamsEnd)*/, runList ));

     for( auto it = runParamsBegin; it != runParamsEnd; ++it )
     {
       it->mThreadIdentifier = threadId;
       runWorker.Run( *it );
     } // for

   } // CSimFireCore::RunBunch

   //-------------------------------------------------------------------------------------------------

   void CSimFireCore::GenerateInitialGeneration( ListOfRunDescriptors_t & runParams )
   {

     double_t distX = mSettings.GetTgtX() - mSettings.GetGunX();
     double_t distY = mSettings.GetTgtY() - mSettings.GetGunY();
     double_t distZ = mSettings.GetTgtZ() - mSettings.GetGunZ();
                        // Vector of LOS from gunner to target

     distZ *= 2.0;      // Initial elevation angle is twice the angle to the target.
                        // BUllet spray (bunches) will be constructed around it.
   
     double xyRatio = 0.1;
     double zRatio = 0.25;

      for( auto & item : runParams )
      {
        item.mRunIdentifier.clear();
        item.mVelocityXCoef = distX * ( 1.0 + ( (double_t)rand() / (double_t)RAND_MAX - 0.5 ) * xyRatio );
        item.mVelocityYCoef = distY * ( 1.0 + ( (double_t)rand() / (double_t)RAND_MAX - 0.5 ) * xyRatio );
        item.mVelocityZCoef = distZ * ( 1.0 + ( (double_t)rand() / (double_t)RAND_MAX - 0.5 ) * zRatio );
                          // Initial velocity direction coefficients are generated as random values around
                          // the LOS vector to the target.

        item.mMinDTgtSq = std::numeric_limits<double_t>::max();
        item.mMinTime = 0.0;
        item.mNearHalfPlane = false;
        item.mLowHalfPlane = false;
        item.mSimTime = 0.0;
        item.mReturnCode = CSimFireSingleRunParams::SimResCode_t::kNotStarted;
      } // for

   } // CSimFireCore::GenerateInitialGeneration

   //-------------------------------------------------------------------------------------------------

   void CSimFireCore::GenerateFollowingGeneration( ListOfRunDescriptors_t & runParams )
   {

     std::map<double_t, ListOfRunDescriptors_t> nearAbove;
     std::map<double_t, ListOfRunDescriptors_t> nearBelow;
     std::map<double_t, ListOfRunDescriptors_t> farAbove;
     std::map<double_t, ListOfRunDescriptors_t> farBelow;

     for( auto it = runParams.begin(); it != runParams.end(); ++it )
     {                  // Sorts results into categories for further processing and copies them, so
                        // original buch can be overwritten by new generation.
       if( it->mReturnCode == CSimFireSingleRunParams::SimResCode_t::kEndedCollision )
         continue;
       if( it->mNearHalfPlane )
       {
         if( it->mLowHalfPlane )
           nearBelow[it->mMinDTgtSq].push_back( *it );
         else
           nearAbove[it->mMinDTgtSq].push_back( *it );
       } // if
       else
       {
         if( it->mLowHalfPlane )
           farBelow[it->mMinDTgtSq].push_back( *it );
         else
           farAbove[it->mMinDTgtSq].push_back( *it );
       } // else
     } // for


   } // CSimFireCore::GenerateFollowingGeneration

   //-------------------------------------------------------------------------------------------------

   bool CSimFireCore::ContinueNextGAIteration( const ListOfRunDescriptors_t & runParams )
   {
     return false;
   } // CSimFireCore::ContinueNextGAIteration

   //-------------------------------------------------------------------------------------------------


} // namespace PEGLDPCgenerator
