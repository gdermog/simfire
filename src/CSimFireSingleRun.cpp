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

  const char * CSimFireSingleRun::mBulletIdString = "BULLET";

  const char * CSimFireSingleRun::mTargetIdString = "TARGET";

  //****** component: entity full identifier *********************************************************

  struct cpId {
    uint64_t id;        //!< Full entity identifier (index)
    const char * typeId;//!< Type identifier (for logging and debugging purposes)
    bool active;        //!< \b true if the entity is active (still in simulation). Simulation continues  
                        //!  if at least one entity is active.
  };

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
  }; // procURM

  //****** processor: change in speed according to gravitational acceleration ************************

  struct procDVA {

    procDVA( double_t dt, double_t g ):
      mDtg( dt * g )
    {}

    double_t mDtg;       //!< Time step * Gravitational acceleration (Z axis) [m/s]

    void update( entt::registry & reg )
    {
      auto view = reg.view<cpVelocity>();

      view.each( [=]( auto & v )
      {
        v.vZ -= mDtg;
      } );

    } // procDVA::update
  }; // procDVA

  //****** processor: object collision check *********************************************************

  struct procOCC {

    void update( entt::registry & reg )
    {
      auto view = reg.view<const cpId, const cpPosition, const cpGeometry>();

      using iterator_t = decltype( view.begin() );
      iterator_t i = view.begin();
      iterator_t j = view.end();

      if( view.begin() == view.end() )
      {
        return;
      }

      if( i == j )
      {
        return;
      }

    } // procOCS::update

  }; // procOCC

  //****** processor: out of scene check *************************************************************

  struct procOCS {

    void update( entt::registry & reg )
    {
      auto view = reg.view<cpId, cpPosition>();

      view.each( [=]( auto & id, const auto & pos )
      {
        if( id.active )
          return;

        if( pos.Z <= 0.0 )
          id.active = false;

      });

    } // procOCS::update

  }; // procOCS

  //****** processor: check activity *****************************************************************

  struct procActCheck {

    procActCheck():
      mAnythingActive( false )
    {}

    bool mAnythingActive; 
                        //!< The simulation should continue if at least one entity is active (true here).

    void reset()
    {
      mAnythingActive = false;
    }

    void update( entt::registry & reg )
    {
      auto view = reg.view<cpId>();

      view.each( [&]( const auto & v )
      {
        if( v.active )
          mAnythingActive = true;
      });

    } // procActCheck::update

  }; // procActCheck

  //****** CCSimFireSingleRun ************************************************************************

  CSimFireSingleRun::CSimFireSingleRun(
    const CSimFireSettings & settings,
    LogCallback_t fnCall ):
    mSettings( settings ),
    mLogCallback( fnCall ),
    mRunId(),
    mEnTTRegistry()
  {} 


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

    mEnTTRegistry.emplace<cpId>(
      bullet,
      1,
      mBulletIdString,
      true );           // Bullet is active at the beginning of the simulation. As we have single bullet in the
                        // simulation, its index is for now hardcoded as 1.

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

    mEnTTRegistry.emplace<cpId>(
      target,
      1000,
      mTargetIdString,
      false );          // Target is not active from the beginning of the simulation as its state is not 
                        // significant for continuing the simulation. Because we have target bullet in 
                        // the simulation, its index is hardcoded as 1000.

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
