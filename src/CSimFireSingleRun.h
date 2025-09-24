//****************************************************************************************************
//! \file CSimFireSingleRun.h                                                                        
//! Module contains declaration of CSimFireSingleRun class, which ...                                
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

  /*!  \brief Brief description of the class

  Detailed description of the class \n\n

  <b>Remark.:</b> ...

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

    //@{}---------------------------------------------------------------------------------------------
    //! @name Public data                                                                            
    //@{----------------------------------------------------------------------------------------------

  protected:

    const CSimFireSettings & mSettings;

    LogCallback_t mLogCallback;

    std::string mRunId;

    std::string mThreadId;

    entt::registry mEnTTRegistry;

    static const char * mBulletIdString;

    static const char * mTargetIdString;

    //@}

  }; // CSimFireSingleRun

} // namespace SimFire

#endif
