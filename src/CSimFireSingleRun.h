//****************************************************************************************************
//! \file CSimFireSingleRun.h                                                                        
//! Module contains declaration of CSimFireSingleRun class, which is handling a single simulation run.                                
//****************************************************************************************************
//                                                                                                  
//****************************************************************************************************
// 19. 11. 2025, V. Pospíšil, gdermog@seznam.cz                                                     
//****************************************************************************************************

#ifndef H_CSimFireSingleRun
#define H_CSimFireSingleRun

#include <entity/registry.hpp>

#include <SimFireGlobals.h>
#include <CSimFireSettings.h>
#include <CSimFireSingleRunParams.h>

namespace SimFire
{

  //***** CSimFireSingleRun *****************************************************************************

	/*!  \brief This class handles a single simulation run. */
  class CSimFireSingleRun
  {

  public:

    using LogCallback_t = std::function <void( const std::string &, const std::string & )>;
    //!< This is a type of a method that responds to a journal message request.

#define BIND_SINGLE_RUN_LOG_CALLBACK( classPtr, fnName )                          \
         std::bind( &fnName, (classPtr), std::placeholders::_1, std::placeholders::_2 )
    //!< This macro allows one to specify a member function of other class as a logging callback.

		using ExportCallback_t = std::function <void(
      double_t x, double_t y, double_t z,
      double_t vX, double_t vY, double_t vZ,
      double_t dist, double_t t, 
      bool raising, bool below, bool nearHalfPlane )>;
		//!< This is a type of a method that exports state of the simulation at given time step.

#define BIND_SINGLE_RUNEXPORT_CALLBACK( classPtr, fnName )                        \
         std::bind( &fnName, (classPtr),                                          \
         std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,     \
         std::placeholders::_4, std::placeholders::_5, std::placeholders::_6,     \
         std::placeholders::_7, std::placeholders::_8,                            \
         std::placeholders::_9, std::placeholders::_10, std::placeholders::_11 )
    //!< This macro allows one to specify a member function of other class as a exporting callback.

    //------------------------------------------------------------------------------------------------
    //! @name Constructors, destructor, clonning, assign operators                                   
    //@{----------------------------------------------------------------------------------------------

    CSimFireSingleRun( 
      const CSimFireSettings & settings,
      LogCallback_t fnCall );

    virtual ~CSimFireSingleRun();

    //@}-----------------------------------------------------------------------------------------------
    //! @name Public methods  */
    //@{-----------------------------------------------------------------------------------------------

    int Run( CSimFireSingleRunParams & runParams );
    /*! \brief Runs a single simulation with given parameters
     
        \param[in,out] runParams Parameters of the run, some output values are returned in this object
				\return 0 if the run was successful, error code otherwise */

		void SetExportCallback(ExportCallback_t fnCall) { mExportCallback = fnCall; }
    /*! \brief Sets callback method for exporting state of the simulation at given time step
     
				\param[in] fnCall Callback method */

    //@}---------------------------------------------------------------------------------------------
    //! @name Protected data                                                                            
    //@{----------------------------------------------------------------------------------------------

  protected:

    const CSimFireSettings & mSettings;
		//<! Reference to settings object, which contains all parameters entered by user on startup

    LogCallback_t mLogCallback;
		//!< Callback method for logging messages
     
		ExportCallback_t mExportCallback;
		//!< Callback method for exporting state of the simulation at given time step

    std::string mRunId;
		//!< Identifier of the current run, taken from run parameters

    std::string mThreadId;
		//!< Identifier of the thread that is processing the current run

    entt::registry mEnTTRegistry;
		//!< EnTT registry containing all entities and components of the current simulation

    static const char * mBulletIdString;
		//<! String identifier of the bullet entity

    static const char * mTargetIdString;
		//<! String identifier of the target entity

    //@}

  }; // CSimFireSingleRun

} // namespace SimFire

#endif
