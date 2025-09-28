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

     size_t actGeneration = 0;
		 double_t avgDist = 0.0;
     double_t minDist = 1e99;
     double_t maxDist = 0.0;

		 uint32_t risingShots = 0;
		 uint32_t fallingShots = 0;

		 uint32_t nearShots = 0;
		 uint32_t farShots = 0;

     uint32_t overShots = 0;
     uint32_t underShots = 0;

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

       std::map<double_t, CSimFireSingleRunParams *> simpleDistanceSortedMiss;
       for( auto & item : vRunParams )
       {
				 simpleDistanceSortedMiss[item.mVelocityZCoef] = &item;
       }

			 for( auto & item : simpleDistanceSortedMiss )
       {
         WriteLogMessage( "CORE",
           FormatStr( "%s: %s %s ended with code %s in t = %.2f s",
             item.second->mReturnCode != CSimFireSingleRunParams::SimResCode_t::kEndedCollision ? "MISS" : "HIT",
             item.second->mRunIdentifier,
             item.second->GetRunDesc(),
             CSimFireSingleRunParams::GetStrValue( item.second->mReturnCode ),
             item.second->mSimTime ) );
       }

       gaContinue = ContinueNextGAIteration( vRunParams, actGeneration );

       avgDist = 0.0;
       minDist = 1e99;
       maxDist = 0.0;
       risingShots = 0;
       fallingShots = 0;
       nearShots = 0;
       farShots = 0;
       overShots = 0;
       underShots = 0;

       for( auto & item : vRunParams )
       {

         auto dist = std::sqrt( item.mMinDTgtSq );
         if( dist < minDist )
           minDist = dist;
				 if( dist > maxDist )
					 maxDist = dist;
         avgDist += dist;

         if( item.mRaising )
         {
           ++risingShots;
           if( item.mBelow )
             ++underShots;
           else
             ++overShots;
				 } // if
         else
         {
           ++fallingShots;
           if( item.mNearHalfPlane )
             ++nearShots;
           else
             ++farShots;
				 } // else

			 } // for
       avgDist /= (double_t)vRunParams.size();

       WriteLogMessage( "CORE", FormatStr( "Minimal distance: %f m", minDist ) );
       WriteLogMessage( "CORE", FormatStr( "Average distance: %f m", avgDist ) );
			 WriteLogMessage( "CORE", FormatStr( "Maximal distance: %f m", maxDist ) );

			 WriteLogMessage( "CORE", FormatStr( "Rising shots: %u", risingShots ) );
       WriteLogMessage( "CORE", FormatStr( " -> Under shots: %u", underShots ) );
			 WriteLogMessage( "CORE", FormatStr( " -> Over shots: %u", overShots ) );
			 WriteLogMessage( "CORE", FormatStr( "Falling shots: %u", fallingShots ) );
       WriteLogMessage( "CORE", FormatStr( " -> Near shots: %u", nearShots ) );
			 WriteLogMessage( "CORE", FormatStr( " -> Far shots: %u", farShots ) );

       if( gaContinue )
       {
         CreateFollowingGeneration( vRunParams, avgDist, actGeneration );
         ++actGeneration;
			 } // if

     } while( gaContinue );

     //------- Results overview --------------------------------------------------------------------------

     WriteLogMessage( "CORE", "\n\n****************************************\nSimulation finished, results overview\n****************************************\n" );

     size_t nHits = 0;
     for( auto & item : vRunParams )
     {
       if( item.mReturnCode == CSimFireSingleRunParams::SimResCode_t::kEndedCollision )
       {
         ++nHits;
         WriteLogMessage( "CORE",
           FormatStr( "Run [%f, %f, %f] hits the target after %.2f s",
             item.mVelocityXCoef, item.mVelocityYCoef, item.mVelocityZCoef,
             item.mSimTime  ) );
       } // if
     } // for

     if( 0 == nHits )
       WriteLogMessage( "CORE", "No hits were achieved." );
     else
       WriteLogMessage( "CORE", FormatStr( "%zu hits were achieved.", nHits ) );

     WriteLogMessage( "CORE", "\n\n****************************************\n" );

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

     distZ *= 2.0;      // Initial elevation angle is higher than angle to the target.
                        // Bullet spray (bunches) will be constructed around it.

     double zRatio = 0.25;

     uint32_t runNr = 0;
     for( auto & item : runParams )
     {
       item.mRunIdentifier = FormatStr( "RUN_%02u", ++runNr );
       item.mVelocityXCoef = distX;
       item.mVelocityYCoef = distY;
       item.mVelocityZCoef = distZ * ( 1.0 + ( (double_t)rand() / (double_t)RAND_MAX - 0.5 ) * zRatio );
                        // Initial velocity direction coefficients are generated as random values around
                        // the LOS vector to the target in Z axis, X and Y axis are constant. This only 
                        // places the unknown variable to the Z axis (angle) - the entire simulation takes 
                        // place in a plane perpendicular to the ground, oriented so that it points directly 
                        // from the shooter to the target. This simplifying assumption is given by the absence 
                        // of additional disturbing influences for the X and Y directions - but in a more 
                        // complex simulation they would have to be taken into account.

       item.mMinDTgtSq = std::numeric_limits<double_t>::max();
       item.mMinTime = 0.0;
       item.mNearHalfPlane = false;
       item.mSimTime = 0.0;
       item.mReturnCode = CSimFireSingleRunParams::SimResCode_t::kNotStarted;
     } // for

   } // CSimFireCore::GenerateInitialGeneration

   //-------------------------------------------------------------------------------------------------

   void CSimFireCore::Spawn(
     std::map<double_t, CSimFireSingleRunParams> & queue,
     ListOfRunDescriptors_t & runParams,
     size_t & fillingNewItem,
     size_t spawners,
     double_t fineTuneCoef )
   {
		 auto it = queue.begin();
     while( 0 < spawners && it != queue.end() )
     {
			 auto &item = it->second;

       double_t coef1 = 1.0;
			 double_t coef2 = 1.0;

       if( item.mRaising )
       {
         if( item.mBelow )
         {
           coef1 += fineTuneCoef;
					 coef2 += 2 * fineTuneCoef;
         }
         else
         {
           coef1 -= fineTuneCoef;
           coef2 -= 2 * fineTuneCoef;
         }
       } // if
       else
       {
         if( item.mNearHalfPlane )
         {
           coef1 += fineTuneCoef;
           coef2 += 2 * fineTuneCoef;
         }
         else
         {
           coef1 -= fineTuneCoef;
           coef2 -= 2 * fineTuneCoef;
         }
       } // else

       std::string tmpOut = FormatStr( "SPAWNING\n\n%s", item.GetRunDesc() );

       if( fillingNewItem >= runParams.size() )
         return;
       auto & newItem0 = runParams[fillingNewItem++];
       newItem0.Reset();
       newItem0.mVelocityXCoef = item.mVelocityXCoef;
       newItem0.mVelocityYCoef = item.mVelocityYCoef;
       newItem0.mVelocityZCoef = item.mVelocityZCoef;

       if( fillingNewItem >= runParams.size() )
         return;
       auto & newItem1 = runParams[fillingNewItem++];
       newItem1.Reset();
       newItem1.mVelocityXCoef = item.mVelocityXCoef;
       newItem1.mVelocityYCoef = item.mVelocityYCoef;
       newItem1.mVelocityZCoef = item.mVelocityZCoef * coef1;

       if( fillingNewItem >= runParams.size() )
         return;
       auto & newItem2 = runParams[fillingNewItem++];
       newItem2.Reset();
       newItem2.mVelocityXCoef = item.mVelocityXCoef;
       newItem2.mVelocityYCoef = item.mVelocityYCoef;
       newItem2.mVelocityZCoef = item.mVelocityZCoef * coef2;

       tmpOut += FormatStr( "\n----------------------------------\n[%f, %f, %f], [%f, %f, %f]\n\n",
         newItem1.mVelocityXCoef, newItem1.mVelocityYCoef, newItem1.mVelocityZCoef,
         newItem2.mVelocityXCoef, newItem2.mVelocityYCoef, newItem2.mVelocityZCoef );
       WriteLogMessage( "CORE", tmpOut );

       ++it;
       --spawners;
		 } // while

	 } // CSimFireCore::Spawn

   //-------------------------------------------------------------------------------------------------

   void CSimFireCore::Recombine(
     std::map<double_t, CSimFireSingleRunParams> & queue1,
     std::map<double_t, CSimFireSingleRunParams> & queue2,
     ListOfRunDescriptors_t & runParams,
     size_t &fillingNewItem,
     double_t incCoef,
     double_t decCoef )
   {
     auto nm = queue1.extract( queue1.begin() );
     auto & nmItem = nm.mapped();

     auto fm = queue2.extract( queue2.begin() );
     auto & fmItem = fm.mapped();

     std::string tmpOut = FormatStr( "RECOMBININING\n\n%s\n%s",
       nmItem.GetRunDesc(), fmItem.GetRunDesc() );

     if( fillingNewItem >= runParams.size() )
       return;
     auto & newItem1 = runParams[fillingNewItem++];
     newItem1.Reset();
     newItem1.mVelocityXCoef = ( nmItem.mVelocityXCoef + fmItem.mVelocityXCoef ) * 0.5;
     newItem1.mVelocityYCoef = ( nmItem.mVelocityYCoef + fmItem.mVelocityYCoef ) * 0.5;
     newItem1.mVelocityZCoef = ( incCoef * nmItem.mVelocityZCoef + decCoef * fmItem.mVelocityZCoef ) / ( decCoef + incCoef );

     if( fillingNewItem >= runParams.size() )
       return;
     auto & newItem2 = runParams[fillingNewItem++];
     newItem2.Reset();
     newItem2.mVelocityXCoef = ( nmItem.mVelocityXCoef + fmItem.mVelocityXCoef ) * 0.5;
     newItem2.mVelocityYCoef = ( nmItem.mVelocityYCoef + fmItem.mVelocityYCoef ) * 0.5;
     newItem2.mVelocityZCoef = ( decCoef * nmItem.mVelocityZCoef + incCoef * fmItem.mVelocityZCoef ) / ( decCoef + incCoef );

     if( fillingNewItem >= runParams.size() )
       return;
     auto & newItem3 = runParams[fillingNewItem++];
     newItem3.Reset();
     newItem3.mVelocityXCoef = ( nmItem.mVelocityXCoef + fmItem.mVelocityXCoef ) * 0.5;
     newItem3.mVelocityYCoef = ( nmItem.mVelocityYCoef + fmItem.mVelocityYCoef ) * 0.5;
     newItem3.mVelocityZCoef = ( nmItem.mVelocityZCoef + fmItem.mVelocityZCoef ) * 0.5;


     tmpOut += FormatStr( "\n----------------------------------\n[%f, %f, %f], [%f, %f, %f], [%f, %f, %f]\n\n",
			 newItem1.mVelocityXCoef, newItem1.mVelocityYCoef, newItem1.mVelocityZCoef,
       newItem2.mVelocityXCoef, newItem2.mVelocityYCoef, newItem2.mVelocityZCoef,
       newItem3.mVelocityXCoef, newItem3.mVelocityYCoef, newItem3.mVelocityZCoef );
		 WriteLogMessage( "CORE", tmpOut );

   }

   //-------------------------------------------------------------------------------------------------

   void CSimFireCore::Mutate(
     CSimFireSingleRunParams & item,
     ListOfRunDescriptors_t & runParams,
     size_t & fillingNewItem,
     double_t coef ) 
   {
     if( fillingNewItem >= runParams.size() )
       return;
     auto & newItem = runParams[fillingNewItem++];
     newItem.Reset();
     newItem.mVelocityXCoef = item.mVelocityXCoef;
     newItem.mVelocityYCoef = item.mVelocityYCoef;
     newItem.mVelocityZCoef = item.mVelocityZCoef * ( 1.0 + ( (double_t)rand() / (double_t)RAND_MAX ) * coef );

     std::string tmpOut = FormatStr( "MUTATING\n\n%s",
       item.GetRunDesc() );
     tmpOut += FormatStr( "\n----------------------------------\n[%f, %f, %f]\n\n",
       newItem.mVelocityXCoef, newItem.mVelocityYCoef, newItem.mVelocityZCoef  );
     WriteLogMessage( "CORE", tmpOut );
   }

   //-------------------------------------------------------------------------------------------------

   void CSimFireCore::Hallucinate(
     ListOfRunDescriptors_t & runParams,
     size_t & fillingNewItem,
     double_t coarseTuneCoef )
   {
     double_t distX = mSettings.GetTgtX() - mSettings.GetGunX();
     double_t distY = mSettings.GetTgtY() - mSettings.GetGunY();
     double_t distZ = mSettings.GetTgtZ() - mSettings.GetGunZ();

     if( fillingNewItem >= runParams.size() )
       return;
     auto & newItem = runParams[fillingNewItem++];
     newItem.Reset();
     newItem.mVelocityXCoef = distX;
     newItem.mVelocityYCoef = distY;
     newItem.mVelocityZCoef = distZ * ( 1.0 + ( (double_t)rand() / (double_t)RAND_MAX - 0.5 ) * coarseTuneCoef );

     std::string tmpOut = FormatStr( "FILLING BY RANDOM\n\n" );
     tmpOut += FormatStr( "\n----------------------------------\n[%f, %f, %f]\n\n",
       newItem.mVelocityXCoef, newItem.mVelocityYCoef, newItem.mVelocityZCoef );
     WriteLogMessage( "CORE", tmpOut );

	 } // CSimFireCore::Hallucinate

   //-------------------------------------------------------------------------------------------------

   void CSimFireCore::CreateFollowingGeneration( 
     ListOfRunDescriptors_t & runParams, 
     double_t avgDist,
     size_t actGeneration )
   {

     //std::map<double_t, CSimFireSingleRunParams> simpleDistanceSortedMiss;

     std::map<double_t, CSimFireSingleRunParams> nearWhileFallingMiss;
     std::map<double_t, CSimFireSingleRunParams> farWhileFallingMiss;

     std::map<double_t, CSimFireSingleRunParams> overWhileRaisingMiss;
     std::map<double_t, CSimFireSingleRunParams> underWhileRaisingMiss;

     double_t avgDistCutCoef = 1.5;
     double_t decCoef = 0.5;
     double_t incCoef = 1.5;
     double_t fineTuneCoef = 0.015;
     double_t fineTuneDecay = 0.9;

     double_t zRatioRnd = 0.25;
     size_t spawners = 2; // 5;

     std::string tmpOut;

     for( auto it = runParams.begin(); it != runParams.end(); ++it )
     {                  // Sorts results into categories for further processing and copies them, 
                        // so original bunch can be overwritten by new generation.
       if( it->mReturnCode == CSimFireSingleRunParams::SimResCode_t::kEndedCollision )
         continue;

       if( it->mMinDTgtSq >= avgDistCutCoef * avgDist * avgDist )
       {
         WriteLogMessage( "CORE", FormatStr( "%s EXCLUDED as it is too distant", it->GetRunDesc() ) );
         continue;
			 } // if

       //simpleDistanceSortedMiss[it->mMinDTgtSq] = *it;

       if( it->mRaising )
       {
         if( it->mBelow )
           overWhileRaisingMiss[it->mMinDTgtSq] = *it;
         else
           underWhileRaisingMiss[it->mMinDTgtSq] = *it;
       } // if
       else
       {
         if( it->mNearHalfPlane )
           nearWhileFallingMiss[it->mMinDTgtSq] = *it;
         else
           farWhileFallingMiss[it->mMinDTgtSq] = *it;
       } // else

     } // for

     size_t fillingNewItem = 0;

     for( uint32_t i = 0; i < actGeneration; ++i )
       fineTuneCoef *= fineTuneDecay;

		 //Spawn( simpleDistanceSortedMiss, runParams, fillingNewItem, spawners, fineTunecCoef );
     Spawn( nearWhileFallingMiss, runParams, fillingNewItem, spawners, fineTuneCoef );
     Spawn( farWhileFallingMiss, runParams, fillingNewItem, spawners, fineTuneCoef );
		 Spawn( overWhileRaisingMiss, runParams, fillingNewItem, spawners, fineTuneCoef );
		 Spawn( underWhileRaisingMiss, runParams, fillingNewItem, spawners, fineTuneCoef );

     size_t cpairsNr = std::min( nearWhileFallingMiss.size(), farWhileFallingMiss.size() );
     while( 0 < cpairsNr )
     {                  // A simple GA recombination operator. It takes a pair of shots, one of which is "long"
                        // and the other "short", and produces two new ones with elevations between the original
                        // two (one slightly more than the arithmetic mean, the other slightly less than the
                        // arithmetic mean). Works for "upper arc" shooting.
       Recombine( nearWhileFallingMiss, farWhileFallingMiss, runParams, fillingNewItem, incCoef, decCoef );
       --cpairsNr;
     } // while
       
     for( auto nmItem: nearWhileFallingMiss )
     {                  // Fill the rest of the new generation by mutating the best results from leftovers 
                        // of the previous generation (slight random increase of elevation angle for near
                        // misses).
       Mutate( nmItem.second, runParams, fillingNewItem, incCoef );
     } // for

     for( auto nmItem : farWhileFallingMiss )
     {                  // Fill the rest of the new generation by mutating the best results from leftovers 
                        // of the previous generation (slight random decrease of elevation angle for far
                        // misses).
       Mutate( nmItem.second, runParams, fillingNewItem, decCoef );
     } // for

     cpairsNr = std::min( overWhileRaisingMiss.size(), underWhileRaisingMiss.size() );
     while( 0 < cpairsNr )
     {
       Recombine( overWhileRaisingMiss, underWhileRaisingMiss, runParams, fillingNewItem, incCoef, decCoef );
       --cpairsNr;
		 } // while

     for( auto nmItem : overWhileRaisingMiss )
     {
       Mutate( nmItem.second, runParams, fillingNewItem, decCoef );
     } // for

     for( auto nmItem : underWhileRaisingMiss )
     {
       Mutate( nmItem.second, runParams, fillingNewItem, incCoef );
     } // for

     while( fillingNewItem < runParams.size() )
     {
       Hallucinate( runParams, fillingNewItem, zRatioRnd );
		 } // while

     WriteLogMessage( "CORE", "\n\n****************************************\nNew generation created\n****************************************\n" );

     for( auto & item : runParams )
     {
        WriteLogMessage( "CORE",
          FormatStr( "Run firing at target with aim [%f, %f, %f]",
             item.mVelocityXCoef, item.mVelocityYCoef, item.mVelocityZCoef  ) );
     } // for

     WriteLogMessage( "CORE", "\n\n****************************************\n" );

   } // CSimFireCore::GenerateFollowingGeneration


   //-------------------------------------------------------------------------------------------------

   bool CSimFireCore::ContinueNextGAIteration( 
     const ListOfRunDescriptors_t & runParams, size_t actGeneration )
   {
     if( mSettings.GetMaxGenerations() <= actGeneration )
       return false;

     for( const auto & item : runParams )
     {
       if( item.mReturnCode == CSimFireSingleRunParams::SimResCode_t::kEndedCollision )
         return false;
     } // for

     return true;

   } // CSimFireCore::ContinueNextGAIteration

   //-------------------------------------------------------------------------------------------------


} // namespace PEGLDPCgenerator
