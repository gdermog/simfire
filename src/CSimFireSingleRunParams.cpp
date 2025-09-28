//****************************************************************************************************
//! \file CSimFireSingleRunParams.cpp                                                                        
//! Module contains definitions of CSimFireSingleRunParams class, declared in CSimFireSingleRunParams.h, 
//! which ...    
//****************************************************************************************************
//                                                                                                 
//****************************************************************************************************
// 19. 11. 2025, V. Pospíšil, gdermog@seznam.cz                                                     
//****************************************************************************************************

#include <iomanip>

#include <SimFireStringTools.h>
#include <CSimFireSingleRunParams.h>


namespace SimFire
{

   //****** CCSimFireSingleRunParams **************************************************************************

   CSimFireSingleRunParams::CSimFireSingleRunParams():
     mRunIdentifier(),
     mVelocityXCoef( 1.0 ),
     mVelocityYCoef( 0.0 ),
     mVelocityZCoef( 1.0 ),
     mReturnCode( SimResCode_t::kUnknown ),
     mMinDTgtSq( 1e99 ),
     mMinTime( 0.0 ),
     mSimTime( 0.0 ),
     mNearHalfPlane( false ),
     mRaising( false ),
     mBelow( false )
   {

   } /* CSimFireSingleRunParams::CSimFireSingleRunParams */


   //-------------------------------------------------------------------------------------------------

   CSimFireSingleRunParams::~CSimFireSingleRunParams() = default;

   //-------------------------------------------------------------------------------------------------

   std::ostream & CSimFireSingleRunParams::PrpLine( std::ostream & out )
   {
     return out << std::setw( SimFire::gPrintoutIdWidth ) << std::left;
   } // HlpLine

   //-------------------------------------------------------------------------------------------------

   std::ostream& CSimFireSingleRunParams::Preprint( std::ostream & out  )
   { 
     PrpLine( out ) << "RunIdentifier" << mRunIdentifier << std::endl << std::endl;

     PrpLine( out ) << "VelocityXCoef" << mVelocityXCoef << std::endl;
     PrpLine( out ) << "VelocityYCoef" << mVelocityYCoef << std::endl;
     PrpLine( out ) << "VelocityZCoef" << mVelocityZCoef << std::endl << std::endl;

     return out;

   } // CSimFireSingleRunParams::Preprint

   //-------------------------------------------------------------------------------------------------

   void CSimFireSingleRunParams::Reset()
   {
     mVelocityXCoef = 1.0;
     mVelocityYCoef = 0.0;
     mVelocityZCoef = 1.0;
     mMinDTgtSq = 1e99;
     mMinTime = 0.0;
     mSimTime = 0.0;
     mNearHalfPlane = false;
     mRaising = false;
     mBelow = false;
     mReturnCode = SimResCode_t::kNotStarted;
   } // Reset

   //-------------------------------------------------------------------------------------------------

   const std::string & CSimFireSingleRunParams::GetStrValue( SimResCode_t code )
   {

     static const std::string lUnknown( "Unknown" );
     static const std::string lNotStarted( "NotStarted" );
     static const std::string lRunning( "Running" );
     static const std::string lEndedNoActive( "EndedNoActive" );
     static const std::string lEndedCollision( "EndedCollision" );
     static const std::string lEndedMaxTicks( "EndedMaxTicks" );
     static const std::string lError( "Error" );
     static const std::string lInvalidCode( "InvalidCode" );

     switch( code )
     {
       case SimResCode_t::kUnknown:          return lUnknown;
       case SimResCode_t::kNotStarted:       return lNotStarted;
       case SimResCode_t::kRunning:          return lRunning;
       case SimResCode_t::kEndedNoActive:    return lEndedNoActive;
       case SimResCode_t::kEndedCollision:   return lEndedCollision;
       case SimResCode_t::kEndedMaxTicks:    return lEndedMaxTicks;
       case SimResCode_t::kError:            return lError;
       default:                              return lInvalidCode;
     } // switch
   } // GetStrValue

   //-------------------------------------------------------------------------------------------------


   std::string CSimFireSingleRunParams::GetRunDesc()
   {
       return FormatStr( "run [%f, %f, %f], dist = %f m, dt = %.2f s, %s, %s, %s ",
         mVelocityXCoef, mVelocityYCoef, mVelocityZCoef,
         std::sqrt( mMinDTgtSq ), mMinTime,
				 mRaising ? "raising" : "falling",
				 mNearHalfPlane ? "near" : "far",
				 mBelow ? "under" : "above" );
	 } // GetRunDesc


} // namespace PEGLDPCgenerator
