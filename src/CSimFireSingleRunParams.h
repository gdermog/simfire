//****************************************************************************************************
//! \file CSimFireSingleRunParams.h                                                                        
//! Module contains declaration of CSimFireSingleRunParams class, which describes parameters and 
//! results of a single simulation run.                                
//****************************************************************************************************
//                                                                                                  
//****************************************************************************************************
// 19. 11. 2025, V. Pospíšil, gdermog@seznam.cz                                                     
//****************************************************************************************************

#ifndef H_CSimFireSingleRunParams
#define H_CSimFireSingleRunParams

#include <iostream>

#include <SimFireGlobals.h>
#include <CSimFireConfig.h>

namespace SimFire
{

  //***** CSimFireSingleRunParams *****************************************************************************

  /*!  \brief Class describes parameters and results of a single simulation run. */
  class CSimFireSingleRunParams
  {

  public:

    enum class SimResCode_t: uint16_t
    {
      kUnknown          = 0,    //!< Unknown result
      kNotStarted       = 1,    //!< Simulation not started
      kRunning          = 2,    //!< Simulation running
      kEndedNoActive    = 3,    //!< Simulation ended: no active objects left in the scene
      kEndedCollision   = 4,    //!< Simulation ended: collision detected
      kEndedMaxTicks    = 5,    //!< Simulation ended: maximum number of ticks reached
      kError            = 100   //!< Simulation ended with error
    };

    //------------------------------------------------------------------------------------------------
    //! @name Constructors, destructor, clonning, assign operators                                   
    //@{----------------------------------------------------------------------------------------------

    CSimFireSingleRunParams();

    virtual ~CSimFireSingleRunParams();

   //@}-----------------------------------------------------------------------------------------------
   //! @name Public methods  */
   //@{-----------------------------------------------------------------------------------------------

    std::ostream & Preprint( std::ostream & out = std::cout );
    /*!< \brief Sends all parameters and results to given output stream
   
         \param[in,out] out Output stream, default is std::cout */

    static const std::string & GetStrValue( SimResCode_t code );
    /*!< \brief Returns string representation of given simulation result code.
   
         \param[in] code Simulation result code
         \return String representation of the code */

    std::string GetRunDesc();
    /*!< \brief Returns brief description of the run (identifier and velocity coefficients).
   
         \return String description of the run */

    void Reset( bool inclVelCoefs = true );
    /*!< \brief Resets all output values to initial state. If inclVelCoefs is true, also input  
         velocity coefficients are reset to zero.
   
         \param[in] inclVelCoefs If true, also input velocity coefficients are reset to zero. */

    //@{}---------------------------------------------------------------------------------------------
    //! @name Public data                                                                            
    //@{----------------------------------------------------------------------------------------------

    std::string mRunIdentifier;   //!< Simulation run identifier
    std::string mThreadIdentifier;//!< Identifier of the thread that processed this run

    double_t mVelocityXCoef;      //!< X aim of the shooter
    double_t mVelocityYCoef;      //!< Y aim of the shooter
    double_t mVelocityZCoef;      //!< Z aim of the shooter

    //@}----------------------------------------------------------------------------------------------
    //! @name Public output data                                                                            
    //@{----------------------------------------------------------------------------------------------

    double_t mMinDTgtSq;//!< Minimal distance of the bullet to the target in the last completed run squared [m^2]
    double_t mMinTime;  //!< Time when the minimal distance was reached in the last completed run [s]
    bool mNearHalfPlane;//!< \b true if the bullet passed target in half-plane containing the shooter
    bool mRaising;      //!< \b true if the bullet was rising when it reached the minimal distance to the target
    bool mBelow;        //!< \b true if the bullet was below the target when it reached the minimal distance to the target

    double_t mSimTime;  //!< Total simulation time [s]
    SimResCode_t mReturnCode;
                        //!< Result code of the simulation run

  protected:

    std::ostream & PrpLine( std::ostream & out );
    //!< \brief Helper function for Preprint(), starts one line of output

    //@}

  }; // CSimFireSingleRunParams

} // namespace SimFire

#endif
