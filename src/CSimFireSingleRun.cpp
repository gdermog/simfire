//****************************************************************************************************
//! \file CSimFireSingleRun.cpp                                                                        
//! Module contains definitions of CSimFireSingleRun class, declared in CSimFireSingleRun.h, which ...    
//****************************************************************************************************
//                                                                                                 
//****************************************************************************************************
// 19. 11. 2025, V. Pospíšil, gdermog@seznam.cz                                                     
//****************************************************************************************************


#include <CSimFireSingleRun.h>

namespace SimFire
{

  //****** component: position ***********************************************************************

  struct cpPosition {
    double_t X;         //!< X position (of centre of mass) [m]
    double_t Y;         //!< Y position (of centre of mass) [m]
    double_t Z;         //!< Z position (of centre of mass) [m]
  };

  //****** component: velocity ***********************************************************************

  struct cpVelocity {
    double_t vX;         //!< X translation velocity (of centre of mass) [m/s]
    double_t vY;         //!< Y translation velocity (of centre of mass) [m/s]
    double_t vZ;         //!< Z translation velocity (of centre of mass) [m/s]
  };

  //****** component: geometry ***********************************************************************

  struct cpGeometry {

    double_t radius;    //!< Radius of the sphere (ideal shape) [m]
    double_t radiusSq;  //!< Radius squared [m^2] - used for proximity testing. Two objects are in 
                        //!  contact if the distance of their centres is less than sum of their radii,
                        //!  which is tested as squared values for simplicity.
  };

  //****** component: physical properties ************************************************************

  struct cpPhysProps {
    double_t mass;      //!< Mass of the projectile [kg]
    double_t Cd;        //!< Drag coefficient (ideal sphere) [-]
  };

  //****** processor: uniform rectilinear motion *****************************************************

  struct procURM {

    procURM( double_t dt ):
      mDt( dt )
    {}

    double_t mDt;  //!< Time step [s]

    void update( entt::registry & reg )
    {
      auto view = reg.view<cpPosition, const cpVelocity>();

      view.each( [=]( auto & pos, auto & v )
      {
          pos.X += v.vX * mDt;
          pos.Y += v.vY * mDt;
          pos.Z += v.vZ * mDt;
      } );

    } // procURM::update
  }; 

  //****** processor: change in speed according to acceleration **************************************

  struct procDVA {

    procDVA( double_t dt, double_t g ):
      mDtg( dt * g )
    {}

    double_t mDtg;       //!< Time step * Gravitational acceleration [m/s]

    void update( entt::registry & reg )
    {
      auto view = reg.view<cpVelocity>();

      view.each( [=]( auto & v )
      {
        v.vZ -= mDtg;
      } );

    } // procDVA::update
  };

  //****** CCSimFireSingleRun ************************************************************************

  CSimFireSingleRun::CSimFireSingleRun(
    const CSimFireSettings & settings,
    LogCallback_t fnCall ):
    mSettings( settings ),
    mLogCallback( fnCall ),
    mRunId(),
    mEnTTRegistry()
  {

  } /* CSimFireSingleRun::CSimFireSingleRun */


  //-------------------------------------------------------------------------------------------------

  CSimFireSingleRun::~CSimFireSingleRun() = default;

  //-------------------------------------------------------------------------------------------------

  int CSimFireSingleRun::Run( const CSimFireSingleRunParams & runParams )
  {
    mRunId = runParams.mRunIdentifier;

    mEnTTRegistry.clear();
                        // Setup from previous run, if any, is cleared

    //------ Bullet entity creation ----------------------------------------------------------------

    const auto bullet = mEnTTRegistry.create();

    mEnTTRegistry.emplace<cpPosition>( 
      bullet,
      mSettings.GetGunX(),
      mSettings.GetGunY(), 
      mSettings.GetGunZ() );
                        // Shooter position is given by user setup

    double_t div = std::sqrt( runParams.mVelocityXCoef * runParams.mVelocityXCoef +
                              runParams.mVelocityYCoef * runParams.mVelocityYCoef +
                              runParams.mVelocityZCoef * runParams.mVelocityZCoef );
    if( IsZero( div ) )
    {
      if( mLogCallback )
        mLogCallback( mRunId, "Zero velocity direction coefficients given, cannot proceed." );
      return -1;
    } // if

    double velCoef = mSettings.GetVelocity() / div;
    mEnTTRegistry.emplace<cpVelocity>(
      bullet, 
      runParams.mVelocityXCoef * velCoef, 
      runParams.mVelocityYCoef * velCoef, 
      runParams.mVelocityZCoef * velCoef );
                        // Size of muzzle velocity is given by user setup, its direction, however, 
                        // is given by run parameters (we are looking for right angle to hit the target)

    mEnTTRegistry.emplace<cpGeometry>(
      bullet,
      mSettings.GetBulletSize(), 
      mSettings.GetBulletSize() * mSettings.GetBulletSize()  );
                        // Proximity of the bullet and target is given by distance of centres, for simplicity
                        // calculated as squared values

    mEnTTRegistry.emplace<cpPhysProps>(
      bullet,
      mSettings.GetMass(),
      mSettings.GetCd() );
                        // Mass and drag coefficient are given by user setup

    //------ Target entity creation ----------------------------------------------------------------

    const auto target = mEnTTRegistry.create();

    mEnTTRegistry.emplace<cpPosition>(
      target,
      mSettings.GetTgtX(),
      mSettings.GetTgtY(),
      mSettings.GetTgtZ() );
                        // Target position is given by user setup

    mEnTTRegistry.emplace<cpGeometry>(
      target,
      mSettings.GetTgtSize(), 
      mSettings.GetTgtSize() * mSettings.GetTgtSize()  );
                        // Proximity of the bullet and target is given by distance of centres, for simplicity
                        // calculated as squared values



    return 0;

  } // CSimFireSingleRun::Run

  //-------------------------------------------------------------------------------------------------


} // namespace SimFire
