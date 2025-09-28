//****************************************************************************************************
//! \file CSimFireSingleRunParams.h                                                                        
//! Module contains declaration of CSimFireSingleRunParams class, which ...                                
//****************************************************************************************************
//                                                                                                  
//****************************************************************************************************
// 19. 11. 2025, V. Pospíšil, gdermog@seznam.cz                                                     
//****************************************************************************************************

#ifndef H_CSimFireSingleRunParams
#define H_CSimFireSingleRunParams

#include <iostream>

#include <CSimFireConfig.h>
#include <string>

namespace SimFire
{

  //***** CSimFireSingleRunParams *****************************************************************************

  /*!  \brief Brief description of the class

  Detailed description of the class \n\n

  <b>Remark.:</b> ...

  */
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

    static const std::string & GetStrValue( SimResCode_t code );

    std::string GetRunDesc();

    void Reset();

    //@{}---------------------------------------------------------------------------------------------
    //! @name Public data                                                                            
    //@{----------------------------------------------------------------------------------------------

    std::string mRunIdentifier;  //!< Simulation run identifier
    std::string mThreadIdentifier;//!< Identifier of the thread that processed this run

    double_t mVelocityXCoef;     //!< X aim of the shooter
    double_t mVelocityYCoef;     //!< Y aim of the shooter
    double_t mVelocityZCoef;     //!< Z aim of the shooter

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

  protected:

    std::ostream & PrpLine( std::ostream & out );

    //@}

  }; // CSimFireSingleRunParams

} // namespace SimFire

#endif
