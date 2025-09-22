//****************************************************************************************************
//! \file CSimFireCore.cpp                                                                        
//! Module contains definitions of CSimFireCore class, declared in CSimFireCore.h, which ...    
//****************************************************************************************************
//                                                                                                  
//****************************************************************************************************
// 19. 11. 2025, V. Pospíšil, gdermog@seznam.cz                                                     
//****************************************************************************************************

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

     CSimFireSingleRun testRun( 
       mSettings,
       BIND_SINGLE_RUN_LOG_CALLBACK( CSimFireCore::WriteLogMessage ) );

     CSimFireSingleRunParams runParams;
     runParams.mRunIdentifier = "MyFirstRun";
     runParams.mVelocityXCoef = 1.0;
     runParams.mVelocityYCoef = 0.0;
     runParams.mVelocityZCoef = 1.0;

     return testRun.Run( runParams );

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


} // namespace PEGLDPCgenerator
