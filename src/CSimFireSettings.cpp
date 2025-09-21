//****************************************************************************************************
//*! \file CSimFireSettings.cpp                                                                        *
//*! Module contains definitions of CSimFireSettings class, declared in CSimFireSettings.h, which ...    *
//****************************************************************************************************
//*                                                                                                  *
//****************************************************************************************************
//* 19. 11. 2025, V. Pospíšil, gdermog@seznam.cz                                                     *
//****************************************************************************************************

#include <iomanip>

#include <CSimFireSettings.h>

#include <SimFireStringTools.h>

namespace SimFire
{

   //****** CCSimFireSettings **************************************************************************

   CSimFireSettings::CSimFireSettings():
     mSimIdentifier( "SimFire")
   {

   } /* CSimFireSettings::CSimFireSettings */


   //-------------------------------------------------------------------------------------------------

   CSimFireSettings::~CSimFireSettings() = default;

   //-------------------------------------------------------------------------------------------------

   std::vector<std::string> CSimFireSettings::ImportSettings( const SimFire::CSimFireConfig & inCfg )
   {
     std::vector<std::string> vErrors;

     try
     {

       mSimIdentifier = inCfg.GetValueStr( {}, "Identifier", "" );
       if( mSimIdentifier.empty() )
       {
         vErrors.emplace_back( "Identifier value not found" );
       } // if
       else
       {
         for( auto & ch : mSimIdentifier )
         {
           if( !IsNormalChar( ch ) )
           {
             vErrors.emplace_back( "Identifier contains unsupported characters" );
             break;
           } // if
         } // for
       } // else

       mGunX = inCfg.GetValueDouble( "gun", "x", 0.0 );
       mGunY = inCfg.GetValueDouble( "gun", "y", 0.0 );
       mGunZ = inCfg.GetValueDouble( "gun", "z", 0.0 );
       mVelocity = inCfg.GetValueDouble( "gun", "velocity", 0.0 );
       mCd = inCfg.GetValueDouble( "gun", "cd", 0.0 );

       mTgtX = inCfg.GetValueDouble( "target", "x", 0.0 );
       mTgtY = inCfg.GetValueDouble( "target", "y", 0.0 );
       mTgtZ = inCfg.GetValueDouble( "target", "z", 0.0 );
       mTgtSize = inCfg.GetValueDouble( "target", "size", 0.0 );

       mg = inCfg.GetValueDouble( "environment", "g", 0.0 );
       mDensity = inCfg.GetValueDouble( "environment", "density", 0.0 );

       mdt = inCfg.GetValueDouble( "simulation", "dt", 0.0 );

       mOutFile = inCfg.GetValueStr( "logging", "outfile", "" );
       mLogInterval = inCfg.GetValueDouble( "logging", "interval", 0.0 );

     }
     catch( std::exception & e )
     {
       vErrors.emplace_back( e.what() );
     } // catch
     catch( ... )
     {
       vErrors.emplace_back( "Unknown exception during settings import" );
     } // catch

     return vErrors;
   } // ImportSettings

   //-------------------------------------------------------------------------------------------------

   std::ostream & CSimFireSettings::PrpLine( std::ostream & out )
   {
     return out << std::setw( SimFire::gPrintoutIdWidth ) << std::left;
   } // HlpLine

   //-------------------------------------------------------------------------------------------------

   void CSimFireSettings::Preprint( std::ostream & out  )
   { 
     PrpLine( out ) << "SimIdentifier:" << mSimIdentifier << std::endl << std::endl;

     PrpLine( out ) << "GunX" << mGunX << " m" << std::endl;
     PrpLine( out ) << "GunY" << mGunY << " m" << std::endl;
     PrpLine( out ) << "GunZ" << mGunZ << " m" << std::endl;
     PrpLine( out ) << "Velocity" << mVelocity << " m/s" << std::endl;
     PrpLine( out ) << "Cd" << mCd << std::endl << std::endl;

     PrpLine( out ) << "TgtX" << mTgtX << " m" << std::endl;
     PrpLine( out ) << "TgtY" << mTgtY << " m" << std::endl;
     PrpLine( out ) << "TgtZ" << mTgtZ << " m" << std::endl;
     PrpLine( out ) << "TgtSize" << mTgtSize << " m" << std::endl << std::endl;

     PrpLine( out ) << "g" << mg << " m/s^2" << std::endl;
     PrpLine( out ) << "Density" << mDensity << " kg/m^3" <<  std::endl << std::endl;

     PrpLine( out ) << "dt" << mdt << " s" << std::endl << std::endl;

     PrpLine( out ) << "OutFile" << mOutFile << std::endl;
     PrpLine( out ) << "LogInterval" << mLogInterval << " s" << std::endl << std::endl;

   } // CSimFireSettings::Preprint


} // namespace PEGLDPCgenerator
