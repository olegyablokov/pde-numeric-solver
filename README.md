## Getting started
The program is being developed for numerical solving of some pde equations. Right now it supports:
- The 2d heat equation which is solved using the [alternating direction implicit method](https://en.wikipedia.org/wiki/Alternating_direction_implicit_method#cite_ref-2);
- The 2d wave equation which is solved like the heat equation, but it is very unstable now.

## Build
The only external library the program depends on is Qt5.\
The project is configured with QMake. If you encounter problems with linking please modify the .pro file.

The program is meant to be platform-independent, although it is tested only on Linux Mint 18.2 Cinnamon 64-bit.

## Images
![screenshot](https://github.com/oyyablokov/pde_numeric_solver/blob/master/images/heat_equation1.png)
