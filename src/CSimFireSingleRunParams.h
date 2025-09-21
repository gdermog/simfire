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

    double_t mVelocityXCoef;     //!< X aim of the shooter
    double_t mVelocityYCoef;     //!< Y aim of the shooter
    double_t mVelocityZCoef;     //!< Z aim of the shooter

  protected:

    std::ostream & PrpLine( std::ostream & out );

    //@}

  }; // CSimFireSingleRunParams

} // namespace SimFire

#endif
