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

    void GenerateInitialGeneration( ListOfRunDescriptors_t & runParams );

    void CreateFollowingGeneration( 
      ListOfRunDescriptors_t & runParams, 
      double_t avgDist,
      size_t actGeneration );

    void Spawn(
      std::map<double_t, CSimFireSingleRunParams> & queue,
      ListOfRunDescriptors_t & runParams,
      size_t & fillingNewItem,
			size_t spawners,
      double_t fineTuneCoef );

    void Recombine(
      std::map<double_t, CSimFireSingleRunParams> & queue1,
      std::map<double_t, CSimFireSingleRunParams> & queue2,
      ListOfRunDescriptors_t & runParams,
      size_t & fillingNewItem,
      double_t incCoef,
      double_t decCoef );

    void Mutate(
      CSimFireSingleRunParams & item,
      ListOfRunDescriptors_t & runParams,
      size_t & fillingNewItem,
      double_t coef );

    void Hallucinate(
      ListOfRunDescriptors_t & runParams,
      size_t & fillingNewItem,
      double_t coarseTuneCoef );

    bool ContinueNextGAIteration( const ListOfRunDescriptors_t & runParams, size_t actGeneration );

    const CSimFireSettings & mSettings;

    std::recursive_mutex mLogMutex;

    //@}

  }; // CSimFireCore

} // namespace SimFire

#endif
