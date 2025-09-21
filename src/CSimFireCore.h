//****************************************************************************************************
//! \file CSimFireCore.h                                                                        
//! Module contains declaration of CSimFireCore class, which ...                                
//****************************************************************************************************
//                                                                                                  
//****************************************************************************************************
// 19. 11. 2025, V. Pospíšil, gdermog@seznam.cz                                                     
//****************************************************************************************************

#ifndef H_CSimFireCore
#define H_CSimFireCore

#include <iostream>
#include <mutex>

#include <CSimFireSettings.h>
#include <CSimFireSingleRun.h>

namespace SimFire
{

  //***** CSimFireCore *****************************************************************************

  /*!  \brief Brief description of the class

  Detailed description of the class \n\n

  <b>Remark.:</b> ...

  */
  class CSimFireCore
  {

  public:

    //------------------------------------------------------------------------------------------------
    //! @name Constructors, destructor, clonning, assign operators                                   
    //@{----------------------------------------------------------------------------------------------

    CSimFireCore( const CSimFireSettings & settings );

    virtual ~CSimFireCore();

   //@}-----------------------------------------------------------------------------------------------
   //! @name Public methods  */
   //@{-----------------------------------------------------------------------------------------------

    int Run();

    void WriteLogMessage( const std::string & id, const std::string & mssg );

    const std::string & GetLogFilePath();

    //@{}---------------------------------------------------------------------------------------------
    //! @name Public data                                                                            
    //@{----------------------------------------------------------------------------------------------

  protected:

    const CSimFireSettings & mSettings;

    std::recursive_mutex mLogMutex;

    //@}

  }; // CSimFireCore

} // namespace SimFire

#endif
