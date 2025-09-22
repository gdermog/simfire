//****************************************************************************************************
//! \file CSimFireSingleRun.cpp                                                                        
//! Module contains definitions of CSimFireSingleRun class, declared in CSimFireSingleRun.h, which ...    
//****************************************************************************************************
//                                                                                                 
//****************************************************************************************************
// 19. 11. 2025, V. Pospíšil, gdermog@seznam.cz                                                     
//****************************************************************************************************

#include <ranges>

#include <SimFireStringTOols.h>
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

    double_t mDt;       //!< Time step [s]

    void reset() {}

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

    void reset() {}

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

    void reset()
    {
      mCollidedPairs.clear();
    }

    std::vector<std::pair<entt::entity, entt::entity>> mCollidedPairs;
                        //!< List of pairs of entities that collided in the last update.

    void update( entt::registry & reg )
    {

      // The code will go through all relevant pairs of objects and check if they collide. Since our 
      // simulation is very small, this approach is fine, but for large scenes with thousands of objects,
      // this approach would be completely unusable because lack of efficiency. Some filtering algorithms
      // would have to be added to quickly determine which groups of objects can't collide at all (based 
      // on spatial grid, distances, flight levels, something like that).

      auto view = reg.view<const cpPosition, const cpGeometry>();
      std::vector<entt::entity> entities;

      view.each( [&entities]( entt::entity entity, const auto & pos, const auto & geom ) 
      {                 // There is no comparison operator for view iterators (view.begin() == view.end()  
                        // results in compile error), so we have to copy entities to a vector first.
        entities.push_back( entity );
      });

      for( size_t i = 0; i < entities.size(); ++i )
      {
        for( size_t j = i + 1; j < entities.size(); ++j ) 
        {

          auto [pos1, geom1] = view.get<cpPosition, cpGeometry>( entities[i] );
          auto [pos2, geom2] = view.get<cpPosition, cpGeometry>( entities[j] );

          auto distX = pos1.X - pos2.X;
          auto distY = pos1.Y - pos2.Y;
          auto distZ = pos1.Z - pos2.Z;

          auto distSq = distX * distX + distY * distY + distZ * distZ;
          auto radiusSum = geom1.radius + geom2.radius;

          if( distSq < radiusSum * radiusSum )
          {             // Collision detected between entities[i] and entities[j] (ideal spheres)
            mCollidedPairs.emplace_back( std::make_pair( entities[i], entities[j] ) );
          } // if

          // Váš kód pro zpracování dvojice
        } // for
      } // for

    } // procOCS::update

  }; // procOCC

  //****** processor: out of scene check *************************************************************

  struct procOCS {

    void reset() {}

    void update( entt::registry & reg )
    {
      auto view = reg.view<cpId, cpPosition>();

      view.each( [=]( auto & id, const auto & pos )
      {
        if( ! id.active )
          return;       // Already deactivated object are not checked again

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
      1ul,
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
      mSettings.GetBulletSize() );
                        // Proximity of the bullet and target is given by distance of centres.

    mEnTTRegistry.emplace<cpPhysProps>(
      bullet,
      mSettings.GetMass(),
      mSettings.GetCd() );
                        // Mass and drag coefficient are given by user setup


    //------ Target entity creation ----------------------------------------------------------------

    const auto target = mEnTTRegistry.create();

    mEnTTRegistry.emplace<cpId>(
      target,
      1000ul,
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
      mSettings.GetTgtSize() );
                        // Proximity of the bullet and target is given by distance of centres.

    bool noActiveObjects = false;
                        // The simulation should continue if at least one entity is active (true here).

    bool collisionDetected = false;
                        // Collision of bullet and target detected

    //------ Main simulation loop --------------------------------------------------------------------

    procURM uniformRectilinearMotionProcessor( mSettings.GetDt() );
    procDVA deltaVelocityAccelerationProcessor( mSettings.GetDt(), mSettings.GetG() );
    procOCC objectCollisionCheckProcessor;
    procOCS outOfSceneProcessor;
    procActCheck activityCheckProcessor;

    double_t dt = mSettings.GetDt();
    uint64_t logTicks = (uint32_t)( mSettings.GetLogInterval() / dt );
    uint64_t actualTick = 0;
    uint64_t maxTicks = /**//**//**/1000000000/**//**//**/;
    double_t actSimTime = 0.0;

    while( !( noActiveObjects || collisionDetected ) )
    {
      uniformRectilinearMotionProcessor.reset();
      uniformRectilinearMotionProcessor.update( mEnTTRegistry );

      deltaVelocityAccelerationProcessor.reset();
      deltaVelocityAccelerationProcessor.update( mEnTTRegistry );

      outOfSceneProcessor.reset();
      outOfSceneProcessor.update( mEnTTRegistry );

      activityCheckProcessor.reset();
      activityCheckProcessor.update( mEnTTRegistry );

      noActiveObjects = !activityCheckProcessor.mAnythingActive;

      objectCollisionCheckProcessor.reset();
      objectCollisionCheckProcessor.update( mEnTTRegistry );

      collisionDetected = !objectCollisionCheckProcessor.mCollidedPairs.empty();

      ++actualTick;
      if( nullptr != mLogCallback && 0 < logTicks && 0 == ( actualTick % logTicks ) )
      {

        auto view = mEnTTRegistry.view<const cpId, const cpPosition, const cpVelocity>();
        auto [id, pos, vel] = view.get<const cpId, const cpPosition, const cpVelocity>( bullet );

        std::string mssg = FormatStr( "In t = %.4f: Bullet pos = [%.3f, %.3f, %.3f], v = [%.3f, %.3f, %.3f], active = %s",
          actSimTime,
          pos.X, pos.Y, pos.Z,
          vel.vX, vel.vY, vel.vZ,
          ( id.active ? "true" : "false" ) );

        mLogCallback( mRunId, mssg );
      } // if

      if( actualTick >= maxTicks )
        break;

      actSimTime += dt;

    } // while

    if( nullptr != mLogCallback )
    {
      if( noActiveObjects )
        mLogCallback( mRunId, "Simulation ended: no active objects left in the scene." );
      else if( collisionDetected )
        mLogCallback( mRunId, "Simulation ended: collision detected." );
      else
        mLogCallback( mRunId, "Simulation ended: maximum number of ticks reached." );
    } // if

    return 0;

  } // CSimFireSingleRun::Run

  //-------------------------------------------------------------------------------------------------


} // namespace SimFire
