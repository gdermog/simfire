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

    void Preprint( std::ostream & out = std::cout );

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

    SimResCode_t mReturnCode;

  protected:

    std::ostream & PrpLine( std::ostream & out );

    //@}

  }; // CSimFireSingleRunParams

} // namespace SimFire

#endif
