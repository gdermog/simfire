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
      constant, they cannot be changed during program execution. Values are imported through means of 
      CSimFireConfig class. */
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
    /*! \brief Imports settings from configuration object
    
        \param[in] inCfg Configuration object containing all parameters
        \return List of error messages. If empty, import was successful. */

    std::ostream & Preprint( std::ostream & out = std::cout );
    /*! \brief Sends all settings to given output stream
     
        \param[in,out] out Output stream, default is std::cout
				\return Reference to output stream (allowing chains of << operators) */

    //@{}---------------------------------------------------------------------------------------------
    //! @name Input data getters                                                                            
    //@{----------------------------------------------------------------------------------------------

    const std::string & GetSimIdentifier() const { return mSimIdentifier; }
    //!< \brief Returns simulation identifier

		bool GetDoTestRun() const { return mDoTestRun; }
		//!< \brief Returns true if a single test run is performed instead actual simulation

		double_t GetAimX() const { return mAimX; }
		//!< \brief Returns aiming coefficient X [-]

		double_t GetAimY() const { return mAimY; }
		//!< \brief Returns aiming coefficient Y [-]
   
		double_t GetAimZStart() const { return mAimZStart; }
		//!< \brief Returns aiming coefficient Z, start [-]
    
    double_t GetAimZEnd() const { return mAimZEnd; }
    //!< \brief Returns aiming coefficient Z, end [-]
    
		uint32_t GetAimZSteps() const { return mAimZSteps; }
		//!< \brief Returns number of steps between start and end [-]
    
		bool ExportRunsToCSV() const { return !mCSVExportTemplate.empty(); }
		//!< \brief Returns true if CSV export is enabled

		const std::string & GetCSVExportTemplate() const { return mCSVExportTemplate; }
		//!< \brief Returns template for CSV export file name. If empty, no export is done.

		bool GetCSVHitsOnly() const { return mCSVHitsOnly; }
		//!< \brief Returns true if only hits are exported to CSV file
    
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

		bool InVacuum() const { return ! IsPositive( mDensity ); }
		//!< \brief Returns true if the simulation is in vacuum (no air resistance)

    double_t GetDensity() const { return mDensity; }
    //!< \brief Returns air density at sea level [kg/m^3]

    double_t GetDt() const { return mdt; }
    //!< \brief Returns time step [s]

    double_t GetLogInterval() const { return mLogInterval; }
    //!< \brief Returns time interval between entries [s]

    int32_t GetSeed() const { return mSeed; }
    //!< \brief Returns seed for random number generator (if needed)

    int32_t GetNumberOfThreads() const { return mNumberOfThreads; }
    //!< \brief Returns number of threads to be used for parallel processing

    uint32_t GetRunsInGeneration() const { return mRunsInGeneration; }
    //!< \brief Returns number of runs in one generation (genetic algorithm)

    uint32_t GetMaxGenerations() const { return mMaxGenerations; }
    //!< \brief Returns maximum number of generations (genetic algorithm)

  protected:

    //@}----------------------------------------------------------------------------------------------
    //! @name Protected input data                                                                            
    //@{----------------------------------------------------------------------------------------------

    std::string mSimIdentifier;
                        //!< Simulation identifier

    bool mDoTestRun;    //!< If true, a single test run is performed instead actual simulation
    double_t mAimX;     //!< Aiming coefficient X[-]
    double_t mAimY;     //!< Aiming coefficient Y[-]
    double_t mAimZStart;//!< Aiming coefficient Z, start[-]
    double_t mAimZEnd;  //!< Aiming coefficient Z, start[-]
    uint32_t mAimZSteps;//!< Number of steps between start and end [-]
    std::string mCSVExportTemplate;
                        //!< Template for CSV export file name, if empty, no export is done
		bool mCSVHitsOnly;  //!< If true, only hits are exported to CSV file

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

    double_t mLogInterval; 
                        //!< Time interval between entries [s]

    int32_t mSeed;      //!< Seed for random number generator (if needed)

    int32_t mNumberOfThreads;
                        //!< Number of threads to be used for parallel processing

    uint32_t mRunsInGeneration;
                        //!< Number of runs in one generation (for genetic algorithms)
    uint32_t mMaxGenerations;
                        //!< Maximum number of generations (for genetic algorithms)


    std::ostream & PrpLine( std::ostream & out );
		/*!< \brief Helper for formatting output in Preprint method */
    

    //@}

  }; // CSimFireSettings

} // namespace SimFire

#endif
