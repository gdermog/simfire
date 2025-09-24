//****************************************************************************************************
//! \file CSimFireSettings.h                                                                         
//! Module contains declaration of CSimFireSettings class, which contains all parameters entered     
//! by the user from outside.                                                                        
//****************************************************************************************************
//                                                                                                  
//****************************************************************************************************
// 19. 11. 2025, V. Pospíšil, gdermog@seznam.cz                                                     
//****************************************************************************************************

#ifndef H_CSimFireSettings
#define H_CSimFireSettings

#include <iostream>

#include <CSimFireConfig.h>

namespace SimFire
{

  //***** CSimFireSettings *****************************************************************************

  /*! \brief This class contains all parameters entered by the user from outside. They are given and 
      constant, they cannot be changed during program execution. */
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
    //! @name Input data getters                                                                            
    //@{----------------------------------------------------------------------------------------------

    const std::string & GetSimIdentifier() const { return mSimIdentifier; }
    //!< \brief Returns simulation identifier

    double_t GetGunX() const { return mGunX; }
    //!< \brief Returns X position of the shooter [m]

    double_t GetGunY() const { return mGunY; }
    //!< \brief Returns Y position of the shooter [m]

    double_t GetGunZ() const { return mGunZ; }
    //!< \brief Returns Z position of the shooter [m]

    double_t GetVelocity() const { return mVelocity; }
    //!< \brief Returns muzzle velocity of the bullet [m/s]

    double_t GetCd() const { return mCd; }
    //!< \brief Returns drag coefficient of the bullet (ideal sphere) [-]

    double_t GetMass() const { return mMass; }
    //!< \brief Returns mass of the bullet [kg]

    double_t GetBulletSize() const { return mBulletSize; }
    //!< \brief Returns bullet radius (ideal sphere) [m]

    double_t GetTgtX() const { return mTgtX; }
    //!< \brief Returns X position of the target [m]

    double_t GetTgtY() const { return mTgtY; }
    //!< \brief Returns Y position of the target [m]

    double_t GetTgtZ() const { return mTgtZ; }
    //!< \brief Returns Z position of the target [m]

    double_t GetTgtSize() const { return mTgtSize; }
    //!< \brief Returns target radius (ideal sphere) [m]

    double_t GetG() const { return mg; }
    //!< \brief Returns gravitational acceleration [m/s^2]

    double_t GetDensity() const { return mDensity; }
    //!< \brief Returns air density at sea level [kg/m^3]

    double_t GetDt() const { return mdt; }
    //!< \brief Returns time step [s]

    const std::string & GetOutFile() const { return mOutFile; }
    //!< \brief Returns output file name template

    double_t GetLogInterval() const { return mLogInterval; }
    //!< \brief Returns time interval between entries [s]

    int32_t GetNumberOfThreads() const { return mNumberOfThreads; }
    //!< \brief Returns number of threads to be used for parallel processing

    uint32_t GetRunsInGeneration() const { return mRunsInGeneration; }
    //!< \brief Returns number of runs in one generation (genetic algorithm)

  protected:

    //@}----------------------------------------------------------------------------------------------
    //! @name Protected input data                                                                            
    //@{----------------------------------------------------------------------------------------------

    std::string mSimIdentifier;
                        //!< Simulation identifier

    double_t mGunX;     //!< X position of the shooter [m]
    double_t mGunY;     //!< Y position of the shooter [m]
    double_t mGunZ;     //!< Z position of the shooter [m]
    double_t mVelocity; //!< Muzzle velocity of the bullet [m/s]
    double_t mCd;       //!< Drag coefficient of the bullet (ideal sphere) [-]
    double_t mMass;     //!< Mass of the bullet [kg]
    double_t mBulletSize; //!< Bullet radius (ideal sphere) [m]

    double_t mTgtX;     //!< X position of the target [m]
    double_t mTgtY;     //!< Y position of the target [m]
    double_t mTgtZ;     //!< Z position of the target [m]
    double_t mTgtSize;  //!< Target radius (ideal sphere) [m]

    double_t mg;        //!< Gravitational acceleration [m/s^2]
    double_t mDensity;  //!< Air density at sea level [kg/m^3]

    double_t mdt;       //!< Time step [s]

    std::string mOutFile;
                        //!< Output file name template
    double_t mLogInterval; 
                        //!< Time interval between entries [s]

    int32_t mNumberOfThreads;
                        //!< Number of threads to be used for parallel processing

    uint32_t mRunsInGeneration;
                        //!< Number of runs in one generation (for genetic algorithms)


    std::ostream & PrpLine( std::ostream & out );

    //@}

  }; // CSimFireSettings

} // namespace SimFire

#endif
