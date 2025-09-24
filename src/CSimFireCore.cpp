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








/**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**/
/* Initial generation preparation                             */
/**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**/
     std::srand( 1 ); 
     unsigned idx = 0;
     for( auto & item : vRunParams )
     {
       item.mRunIdentifier = FormatStr( "RUN_%02u", ++idx );
       item.mVelocityXCoef = (double_t)rand() / (double_t)RAND_MAX;
       item.mVelocityYCoef = (double_t)rand() / (double_t)RAND_MAX;
       item.mVelocityZCoef = (double_t)rand() / (double_t)RAND_MAX;
     }
/**//**//**//**//**//**//**//**/


/**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**/
/*  GA loop start                                             */
/**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**/

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
     {                  // Wait for all simulation tasks to complete
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

/**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**/
/*  GA loop - check condition for break                       */
/**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**/

/**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**/
/*  GA loop - create next generation                          */
/**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**/

/**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**/
/*  GA loop end                                               */
/**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**/

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


} // namespace PEGLDPCgenerator
