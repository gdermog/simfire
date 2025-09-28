# SimFire

Exercise (c++ project): Simple shooting simulator using external ballistics.

# Basic problem description

According to following inputs:

- 3D point of shooter's location (terrain is a flat plate starting at [0,0,0])
- 3D point of target location (can be at different heights)
- bullet muzzle velocity
- bullet mass
- athmospheric density
- gravitational acceleration (homognoeus field along Z axis)
- simulation time step length

Find out:

- angle of elevation at which the target is hit (if it can be hit)

# Solution description

The task has two parts. The first part is to simulate the motion of a bullet 
in a gravitational field and in a homogeneous environment with non-zero density. 
This part is solved using the time discretization method. From a data perspective,
the Entity–component–system architecture is used by utilizing EnTT library.

The second part then performs simulated shots and searches for an initial barrel 
direction which will hit the target at a fixed muzzle velocity. This part uses 
a genetic algorithm to find a solution. To increase performance simulations of individual 
shots within one generation in run parallel by standard C++ multi-threading tools.

## Time discretization method - basic idea

The method of time discretization is suitable for simulating the behavior of mechanical
systems with a finite number of particles. Its basic idea is to replace the continuous
flow of time with a discrete flow - in simple terms, jumps along the time axis. In the real
universe, time changes continuously and at any moment the interactions of two arbitrary particles
of the system can be described using Newton's law of motion $F = m \cdot a$:

$$ F_{ij}(x _i, y_i, z_i, x _j, y_j, z_j ) = m _i a_i $$
$$ - F_{ij}(x _i, y_i, z_i, x _j, y_j, z_j ) = m _j a_j $$

According to the law of action and reaction, the forces between two particles are equal
in magnitude and opposite in direction. In the continuous flow of time, both the forces and 
the acceleration change continuously, and with them the velocities and positions of the particles.
Integral and differential calculus is necessary for the solution, but it cannot satisfactorily solve 
all problems in full generality. However, if we replace the continuous flow of time with a sequence

$$t _{n+1} = t _n + \Delta t$$

where $\Delta t$ is a fixed constant (time step) and if we assume that the particles move 
uniformly in a straight line between the individual points of the sequence, the problem becomes
much simpler. Let's say that the observed particle is at time instant $t _n$, its current 
position is $\vec{s _n} = ( x _n, y _n, z _n )$ and its velocity is $\vec{v _n} = ( v _{xn}, v _{yn}, v _{zn} )$. 
The acceleration that acts on the particle here can be found as

$$\vec{a _n}= ( a _{xn}, a _{yn}, a _{zn} ) = \frac{\vec{F}}{m}$$

where $m$ is the mass of the particle and $\vec{F}$ the resultant of all forces acting on
it\-i.e. the sum of the forces from all other particles plus the external field strength, 
if any is introduced in the problem (for example, gravitational). Let us assume that even 
if the particles moves uniformly in a straight line until the moment $t _{n+1}$, acceleration 
acts on it all the time. Therefore, it will be true that

$$\vec{v _{n+1} } = \vec{v _n} + \Delta t \cdot \vec{a _n} = \left( v _{xn} + \Delta t \cdot a _{xn}, 
v _{yn} + \Delta t \cdot a _{yn},  v _{zn} + \Delta t \cdot a _{zn} \right)$$

we then determine the change in position as

$$\vec{s _{n+1} } = \vec{s _n} + \Delta t \cdot \vec{v _n} = \left( x _n + \Delta t \cdot v _{xn}, 
y _n + \Delta t \cdot v _{yn},  z _n + \Delta t \cdot v _{zn} \right)$$

In the next step, we recalculate $\vec{F}$ (which is usually dependent on position, in some cases also
on velocity) and calculate the following velocity and position. We do this in a cycle for all particles
in the system until a specified time elapses or until other selected conditions occur. We thus obtain 
a sequence of positions and velocities of all particles in the system. If we choose a sufficiently small
time step, these sequences will satisfactorily approximate the continuous functions of time, which are
the position and velocity of the particles in the real Newtonian universe.

## Time discretization method in SimFire

In the case of the SimFire ​​application, it is basically a simulation of the movement of a single particle 
(bullet) in a homogeneous gravitational field with environmental resistance. The second object in 
the simulated scene is the target - it remains stationary. At each step, the movement is performed 
according to the previously described principles and it is checked whether the bullet has hit 
the target (i.e. is close enough to it) or whether it has landed on the terrain. 

Both particles (bullet and target) are constructed according to 
[Entity Component System](https://en.wikipedia.org/wiki/Entity_component_system) architecture,
eg. particles are entities and their properties (position, velocity, mass, etc.) are components.
Specifically, the components used are:

  - **cpId**: Entity identifier (mainly for debugging and logging)
  - **cpPosition**: 3D position of the entity
  - **cpVelocity**: 3D velocity of the entity
  - **cpGeometry**: Geometrical representation of the entity (ideal sphere with radius)
  - **cpPhysProps**: Physical properties of the entity (mass, drag coefficient)

  Stationary target entity has only cpId, cpPosition and cpGeometry components. Used processors are:

  - **procURM**: uniform rectilinear motion, updates the position of entities according to their velocity
  - **procDVA**: calculates change in velocity according to gravitational acceleration
  - **procOCC**: detects collisions between entities
  - **procOCS**: detects collisions of entities with the terrain (ground plane at z=0)
  - **procActCheck**: informs main loop if there are still active entities (bullet in flight)

  ESC is implemented using [EnTT](https://github.com/skypjack/entt) library.

  The outlined approach allows for easy expansion of the simulation with additional possible 
  moving objects and stationary bodies - these can be added to the scene using the EnTT library 
  resources and nothing else needs to be changed in the code, the simulation will run in the same way.

  ## Genetic algorithm - basic idea

  Genetic algorithms are search heuristics that use concepts of biological evolution, such as natural
  selection and genetics, to find optimal or near-optimal solutions to complex problems. They begin
  with a population of random candidate solutions (chromosomes), which are then iteratively improved 
  through a process of fitness evaluation, selection of the fittest, and genetic operations like crossover 
  (combining traits from two parents) and mutation (random changes), ultimately evolving towards better 
  solutions.  

  How Genetic Algorithms Work:

  - **Initialization**: The process starts with a population of candidate solutions, often represented as strings of numbers or bits (chromosomes), generated randomly. 
  - **Fitness Evaluation**: A fitness function evaluates each solution in the population, assigning it a score that indicates how well it solves the problem. 
  - **Selection**: Solutions with higher fitness values are selected for reproduction, mimicking "survival of the fittest". 
  - **Crossover (Recombination)**: Two selected "parent" solutions are combined to create new "offspring" solutions, exchanging genetic material in a way analogous to biological reproduction. 
  - **Mutation**: Random changes are introduced into the offspring's genetic code to maintain diversity and explore new parts of the search space, preventing the algorithm from getting stuck in local optima. 
  - **New Generation**: The offspring, along with some of the parents, form the next generation of solutions. 
  - **Iteration**: Pevious steps are repeated for a specified number of generations or until a termination criterion is met (e.g., a satisfactory solution is found). 

## Genetic algorithm in SimFire