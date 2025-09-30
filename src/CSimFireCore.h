//****************************************************************************************************
//! \file CSimFireCore.h                                                                        
//! Module contains declaration of CSimFireCore class, which handles the core of the simulation engine.                              
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

  //***** CSimFireCore *******************************************************************************

  /*! \brief This class handles the core of the simulation engine. Finds best barrel angle to achieve
      a hit on target using genetic algorithm. */
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
    /*!< \brief Runs the whole simulation according to given settings.
     
         \return true if simulation was successful, false otherwise */

    void WriteLogMessage( const std::string & id, const std::string & mssg );
    /*!< \brief Writes given message to log file (thread safe).
     
         \param[in] id    Identifier of the message source (module, thread, ...)
         \param[in] mssg  Message text */

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
    /*!< \brief Runs a bunch of simulations in one thread.
     
         \param[in] runParamsBegin  Iterator to the beginning of the list of run parameters
         \param[in] runParamsEnd    Iterator to the end of the list of run parameters
         \param[in] threadId        Identifier of the thread */

    void GenerateInitialGeneration( ListOfRunDescriptors_t & runParams );
    /*!< \brief Generates initial generation of runs with random parameters above the line of sight
         from shooter to target.
     
         \param[in,out] runParams  List of run parameters to be filled */

    void CreateFollowingGeneration( 
      ListOfRunDescriptors_t & runParams, 
      double_t avgDist,
      size_t actGeneration );
    /*!< \brief Creates following generation of runs based on results of previous generation using
         genetic algorithm methods (see below).
     
         \param[in,out] runParams      List of run parameters to be filled
         \param[in]     avgDist        Average distance of all runs in previous generation
         \param[in]     actGeneration  Number of actual generation (0 for initial generation) */

    void Spawn(
      std::map<double_t, CSimFireSingleRunParams> & queue,
      ListOfRunDescriptors_t & runParams,
      size_t & fillingNewItem,
			size_t spawners,
      double_t fineTuneCoef );
    /*!< \brief Spawns new runs by slight mutation of the best results from previous generation.
     
         \param[in]     queue           Sorted map of best results from previous generation
         \param[in,out] runParams      List of run parameters to be filled
         \param[in,out] fillingNewItem Index of the next item to be filled in runParams
         \param[in]     spawners       Number of new runs to be spawned from one best result
         \param[in]     fineTuneCoef   Coefficient determining the size of mutation */

    void Recombine(
      std::map<double_t, CSimFireSingleRunParams> & queue1,
      std::map<double_t, CSimFireSingleRunParams> & queue2,
      ListOfRunDescriptors_t & runParams,
      size_t & fillingNewItem,
      double_t incCoef,
      double_t decCoef );
    /*!< \brief Recombines pairs of "long"/"short" or "below"/"above" results from previous generation 
         to produce new runs with elevation angles between the original pair.
     
         \param[in]     queue1         Sorted map of "long" results from previous generation
         \param[in]     queue2         Sorted map of "short" results from previous generation
         \param[in,out] runParams      List of run parameters to be filled
         \param[in,out] fillingNewItem Index of the next item to be filled in runParams
         \param[in]     incCoef        Coefficient determining how much the higher new elevation angle
                                       is above the arithmetic mean of the original pair
         \param[in]     decCoef        Coefficient determining how much the lower new elevation angle
                                       is below the arithmetic mean of the original pair */

    void Mutate(
      CSimFireSingleRunParams & item,
      ListOfRunDescriptors_t & runParams,
      size_t & fillingNewItem,
      double_t coef );
    /*!< \brief Mutates given run parameters slightly to produce a new run.
     
         \param[in]     item           Run parameters to be mutated
         \param[in,out] runParams      List of run parameters to be filled
         \param[in,out] fillingNewItem Index of the next item to be filled in runParams
         \param[in]     coef           Coefficient determining the amount of mutation */

    void Hallucinate(
      ListOfRunDescriptors_t & runParams,
      size_t & fillingNewItem,
      double_t coarseTuneCoef );
    /*!< \brief Produces a completely random run roughly pointing to the target direction.
     
         \param[in,out] runParams      List of run parameters to be filled
         \param[in,out] fillingNewItem Index of the next item to be filled in runParams
         \param[in]     coarseTuneCoef Coefficient determining the amount of randomness */

    bool ContinueNextGAIteration( const ListOfRunDescriptors_t & runParams, size_t actGeneration );
    /*!< \brief Decides whether the genetic algorithm should continue to next iteration or stop.
     
         \param[in] runParams      List of run parameters of actual generation
         \param[in] actGeneration  Number of actual generation (0 for initial generation)
         \return true if another iteration should be performed, false if the GA should stop */

    const CSimFireSettings & mSettings;
    //<! Reference to settings object

    std::recursive_mutex mLogMutex;
    //<! Mutex for thread-safe logging

    //@}

  }; // CSimFireCore

} // namespace SimFire

#endif
