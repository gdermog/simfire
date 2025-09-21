//****************************************************************************************************
//*! \file CSimFireSettings.h                                                                        *
//*! Module contains declaration of CSimFireSettings class, which ...                                *
//****************************************************************************************************
//*                                                                                                  *
//****************************************************************************************************
//* 19. 11. 2025, V. Pospíšil, gdermog@seznam.cz                                                     *
//****************************************************************************************************

#ifndef H_CSimFireSettings
#define H_CSimFireSettings

#include <iostream>

#include <SimFireGlobals.h>
#include <CSimFireConfig.h>

namespace SimFire
{

  //***** CSimFireSettings *****************************************************************************

  /*!  \brief Brief description of the class

  Detailed description of the class \n\n

  <b>Remark.:</b> ...

  */
  class CSimFireSettings
  {

  public:

    //------------------------------------------------------------------------------------------------
    //! @name Constructors, destructor, clonning, assign operators                                   
    //@{----------------------------------------------------------------------------------------------

    CSimFireSettings();

    virtual ~CSimFireSettings();

   //@}-----------------------------------------------------------------------------------------------
   //! @name Public methods  */
   //@{-----------------------------------------------------------------------------------------------

    std::vector<std::string> ImportSettings( const SimFire::CSimFireConfig & inCfg );

    void Preprint( std::ostream & out = std::cout );

    //@{}---------------------------------------------------------------------------------------------
    //! @name Public data                                                                            
    //@{----------------------------------------------------------------------------------------------

    std::string mSimIdentifier;
                        //!< Simulation identifier

    double_t mGunX;     //!< X position of the shooter [m]
    double_t mGunY;     //!< Y position of the shooter [m]
    double_t mGunZ;     //!< Z position of the shooter [m]
    double_t mVelocity; //!< Muzzle velocity of the bullet [m/s]
    double_t mCd;       //!< Drag coefficient of the bullet (ideal sphere) [-]

    double_t mTgtX;     //!< X position of the target [m]
    double_t mTgtY;     //!< Y position of the target [m]
    double_t mTgtZ;     //!< Z position of the target [m]
    double_t mTgtSize;  //!< Target radius (ideal sphere) [m]

    double_t mg;        //!< Gravitational acceleration [m/s^2]
    double_t mDensity;  //!< Air density at sea level [kg/m^3]

    double_t mdt;       //!< Time step [s]

    std::string mOutFile;
                        //!< Output file name
    double_t mLogInterval; 
                        //!< Time interval between entries [s]


  protected:

    std::ostream & PrpLine( std::ostream & out );

    //@}

  }; // CSimFireSettings

} // namespace SimFire

#endif
