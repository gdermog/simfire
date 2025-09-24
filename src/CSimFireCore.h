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

#include <future>
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

    bool Run();

    void WriteLogMessage( const std::string & id, const std::string & mssg );

    const std::string & GetLogFilePath();

    //@{}---------------------------------------------------------------------------------------------
    //! @name Public data                                                                            
    //@{----------------------------------------------------------------------------------------------

  protected:

    using ListOfRunDescriptors_t = std::vector<CSimFireSingleRunParams>;
                        //!< List of run parameters for whole generation

    using ListOfRunResults_t = std::vector<std::pair<int32_t, double_t>>;
                        //!< List of results of all runs in one generation (pair of run result code and 
                        //!  minimal distance to target)

    void RunBunch(
      ListOfRunDescriptors_t::iterator runParamsBegin,
      ListOfRunDescriptors_t::iterator runParamsEnd,
      const std::string & threadId );

    const CSimFireSettings & mSettings;

    std::recursive_mutex mLogMutex;

    //@}

  }; // CSimFireCore

} // namespace SimFire

#endif
