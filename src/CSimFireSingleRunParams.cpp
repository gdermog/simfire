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
     mNearHalfPlane( false )
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

   void CSimFireSingleRunParams::Preprint( std::ostream & out  )
   { 
     PrpLine( out ) << "RunIdentifier" << mRunIdentifier << std::endl << std::endl;

     PrpLine( out ) << "VelocityXCoef" << mVelocityXCoef << std::endl;
     PrpLine( out ) << "VelocityYCoef" << mVelocityYCoef << std::endl;
     PrpLine( out ) << "VelocityZCoef" << mVelocityZCoef << std::endl << std::endl;

   } // CSimFireSingleRunParams::Preprint


} // namespace PEGLDPCgenerator
