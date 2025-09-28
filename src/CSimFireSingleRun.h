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

#include <CSimFireSettings.h>
#include <CSimFireSingleRunParams.h>

namespace SimFire
{

  //***** CSimFireSingleRun *****************************************************************************

	/*!  \brief This class handles a single simulation run.

  */
  class CSimFireSingleRun
  {

  public:

    using LogCallback_t = std::function <void( const std::string &, const std::string & )>;
    //!< This is a type of a method that responds to a journal message request.

#define BIND_SINGLE_RUN_LOG_CALLBACK( fnName ) std::bind( &fnName, this, std::placeholders::_1, std::placeholders::_2 )
    //!< This macro allows one to specify a member function of other class as a logging callback.

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

    //@{}---------------------------------------------------------------------------------------------
    //! @name Public data                                                                            
    //@{----------------------------------------------------------------------------------------------

  protected:

    const CSimFireSettings & mSettings;
		//<! Reference to settings object, which contains all parameters entered by user on startup

    LogCallback_t mLogCallback;
		//!< Callback method for logging messages

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
