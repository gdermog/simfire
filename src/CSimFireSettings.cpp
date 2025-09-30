//****************************************************************************************************
//! \file CSimFireSettings.cpp                                                                       
//! Module contains definitions of CSimFireSettings class, declared in CSimFireSettings.h, which
//! contains all parameters entered  by the user from outside.                                    
//****************************************************************************************************
//                                                                                                  
//****************************************************************************************************
// 19. 11. 2025, V. Pospíšil, gdermog@seznam.cz                                                     
//****************************************************************************************************

#include <iomanip>

#include <CSimFireSettings.h>
#include <SimFireStringTools.h>

namespace SimFire
{

   //****** CCSimFireSettings **************************************************************************

   CSimFireSettings::CSimFireSettings():
     mSimIdentifier( "SimFire"),
		 mDoTestRun(false),
		 mAimX( 0.0 ),
		 mAimY( 0.0 ),
		 mAimZStart( 0.0 ),
		 mAimZEnd( 0.0 ),
		 mAimZSteps( 1 ),
     mGunX( 0.0 ),
     mGunY( 0.0 ),
     mGunZ( 0.0 ),
     mVelocity( 0.0 ),
     mCd( 0.0 ),
     mMass( 0.0 ),
     mBulletSize( 0.0 ),
     mTgtX( 0.0 ),
     mTgtY( 0.0 ),
     mTgtZ( 0.0 ),
     mTgtSize( 0.0 ),
     mg( 9.81 ),
     mDensity( 1.225 ),
     mdt( 0.01 ),
     mLogInterval( 0.1 ),
     mSeed( -1 ),
     mNumberOfThreads( 8 ),
     mRunsInGeneration( 25 ),
     mMaxGenerations( 50 ) 
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

			 mDoTestRun = inCfg.GetValueBool( "test", "doTestRun", false);
			 mAimX = inCfg.GetValueDouble( "test", "aimX", 0.0 );
			 mAimY = inCfg.GetValueDouble( "test", "aimY", 0.0 );
       mAimZStart = inCfg.GetValueDouble( "test", "aimZStart", 0.0 );
			 mAimZEnd = inCfg.GetValueDouble( "test", "aimZEnd", 0.0 );
			 mAimZSteps = (uint32_t)inCfg.GetValueUnsigned("test", "aimZSteps", 1);
			 mCSVExportTemplate = inCfg.GetValueStr( "test", "csvExportTemplate", "" );
			 mCSVHitsOnly = inCfg.GetValueBool( "test", "csvHitsOnly", true );

       mGunX = inCfg.GetValueDouble( "gun", "x", 0.0 );
       mGunY = inCfg.GetValueDouble( "gun", "y", 0.0 );
       mGunZ = inCfg.GetValueDouble( "gun", "z", 0.0 );
       mVelocity = inCfg.GetValueDouble( "gun", "velocity", 0.0 );
       if( !IsPositive( mVelocity ) )
				 vErrors.emplace_back("Velocity must be positive");
       mCd = inCfg.GetValueDouble( "gun", "cd", 0.0 );
       if (!IsPositive(mCd))
         vErrors.emplace_back("Bullet drag coefficient must be positive");
       mMass = inCfg.GetValueDouble( "gun", "mass", 0.0 );
       if (!IsPositive(mMass))
         vErrors.emplace_back("Bullet mass must be positive");
       mBulletSize = inCfg.GetValueDouble( "gun", "size", 0.0 );
       if (!IsPositive(mBulletSize))
         vErrors.emplace_back("Bullet radius must be positive");

       mTgtX = inCfg.GetValueDouble( "target", "x", 0.0 );
       mTgtY = inCfg.GetValueDouble( "target", "y", 0.0 );
       mTgtZ = inCfg.GetValueDouble( "target", "z", 0.0 );
       mTgtSize = inCfg.GetValueDouble( "target", "size", 0.0 );
       if (!IsPositive(mTgtSize))
         vErrors.emplace_back("Target radius must be positive");

       mg = inCfg.GetValueDouble( "environment", "g", 0.0 );
       if (!IsPositive(mg))
         vErrors.emplace_back("Gravitational acceleration must be positive");
       mDensity = inCfg.GetValueDouble( "environment", "density", 0.0 );

       mdt = inCfg.GetValueDouble( "simulation", "dt", 0.0 );
       if (!IsPositive(mdt))
         vErrors.emplace_back("Time step must be positive");
       mRunsInGeneration = (uint32_t)inCfg.GetValueInteger( "simulation", "generation", 0 );
       if (!IsPositive(mRunsInGeneration))
         vErrors.emplace_back("Generation size must be positive");
       mMaxGenerations = (uint32_t)inCfg.GetValueInteger( "simulation", "maxgens", 0 );
       if (!IsPositive(mMaxGenerations))
         vErrors.emplace_back("Maximum number of generations size must be positive");
       mNumberOfThreads = (int32_t)inCfg.GetValueUnsigned( "simulation", "threads", 0 );
       mSeed = (int32_t)inCfg.GetValueInteger( "simulation", "seed", -1 );

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

   std::ostream& CSimFireSettings::Preprint( std::ostream & out  )
   { 
     PrpLine( out ) << "SimIdentifier:" << mSimIdentifier << std::endl << std::endl;

     if (mDoTestRun)
     {
       PrpLine(out) << "DoTestRun" << (mDoTestRun ? "true" : "false") << std::endl;
       PrpLine(out) << "AimX" << mAimX << std::endl;
       PrpLine(out) << "AimY" << mAimY << std::endl;
       PrpLine(out) << "AimZStart" << mAimZStart << std::endl;
       PrpLine(out) << "AimZEnd" << mAimZEnd << std::endl;
       PrpLine(out) << "AimZSteps" << mAimZSteps << std::endl;
       PrpLine(out) << "mCSVExportTemplate" << mCSVExportTemplate << std::endl;
       PrpLine(out) << "mCSVHitsOnly" << mCSVHitsOnly << std::endl << std::endl;
     }

     PrpLine( out ) << "GunX" << mGunX << " m" << std::endl;
     PrpLine( out ) << "GunY" << mGunY << " m" << std::endl;
     PrpLine( out ) << "GunZ" << mGunZ << " m" << std::endl;
     PrpLine( out ) << "Velocity" << mVelocity << " m/s" << std::endl;
     PrpLine( out ) << "Cd" << mCd << std::endl << std::endl;
     PrpLine( out ) << "Mass" << mMass << std::endl;
     PrpLine( out ) << "BulletSize" << mBulletSize << " m" << std::endl << std::endl; 

     PrpLine( out ) << "TgtX" << mTgtX << " m" << std::endl;
     PrpLine( out ) << "TgtY" << mTgtY << " m" << std::endl;
     PrpLine( out ) << "TgtZ" << mTgtZ << " m" << std::endl;
     PrpLine( out ) << "TgtSize" << mTgtSize << " m" << std::endl << std::endl;

     PrpLine( out ) << "g" << mg << " m/s^2" << std::endl;
     PrpLine( out ) << "Density" << mDensity << " kg/m^3" <<  std::endl << std::endl;

     PrpLine( out ) << "dt" << mdt << " s" << std::endl;
     PrpLine( out ) << "RunsInGeneration" << mRunsInGeneration << std::endl;
     PrpLine( out ) << "MaxGenerations" << mMaxGenerations << std::endl;
     PrpLine( out ) << "NumberOfThreads" << mNumberOfThreads << std::endl << std::endl;

     PrpLine( out ) << "LogInterval" << mLogInterval << " s" << std::endl << std::endl;

		 return out;

   } // CSimFireSettings::Preprint


} // namespace PEGLDPCgenerator
