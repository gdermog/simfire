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

## Time discretization method

\section{Základní myšlenka diskretizace času}


$$ F_{ij}(x _i, y_i, z_i, x _j, y_j, z_j ) = m _i a_i $$
$$ - F_{ij}(x _i, y_i, z_i, x _j, y_j, z_j ) = m _j a_j $$

