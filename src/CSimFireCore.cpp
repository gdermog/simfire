//****************************************************************************************************
//! \file CSimFireCore.cpp                                                                        
//! Module contains definitions of CSimFireCore class, declared in CSimFireCore.h, which ...    
//****************************************************************************************************
//                                                                                                  
//****************************************************************************************************
// 19. 11. 2025, V. Pospíšil, gdermog@seznam.cz                                                     
//****************************************************************************************************

#include <thread>

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

   int CSimFireCore::Run()
   {

     //------- Preparing threads and workers for parallel simulations --------------------------------

     auto nrOfRuns = mSettings.GetRunsInGeneration();
     ListOfRunDescriptors_t vRunParams( nrOfRuns, {} );
                        // Run parameters is preset for whole generation 

     uint32_t nThreads = ( mSettings.GetNumberOfThreads() > 0 ) ?
       static_cast<uint32_t>( mSettings.GetNumberOfThreads() ) :
       std::max( 1u, std::thread::hardware_concurrency() );

     std::vector<std::future<void>> vRunFutures;

     auto bPars = vRunParams.begin();
     auto ePars = vRunParams.begin();
     while( 0 < nrOfRuns )
     {             
       auto nThisBatch = std::min( static_cast<uint32_t>( nrOfRuns ), nThreads );
       std::advance( ePars, nThisBatch );
       vRunFutures.emplace_back( std::async( std::launch::async, [&] { RunBunch( bPars, ePars ); }) );
       nrOfRuns -= nThisBatch;
     } // while
     
     for( auto & task : vRunFutures )
     {                  // Wait for all simulation tasks to complete
       task.wait();
     } // for



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
     ListOfRunDescriptors_t::iterator runParamsEnd )
   {

     for( auto it = runParamsBegin; it != runParamsEnd; ++it )
       it->mReturnCode = CSimFireSingleRunParams::SimResCode_t::kNotStarted;

     CSimFireSingleRun runWorker( mSettings, BIND_SINGLE_RUN_LOG_CALLBACK( CSimFireCore::WriteLogMessage ) );

     for( auto it = runParamsBegin; it != runParamsEnd; ++it )
     {



     }


     return;
   }

   //-------------------------------------------------------------------------------------------------


} // namespace PEGLDPCgenerator
