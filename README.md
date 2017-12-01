# Getting started
The program is being developed for numerical solving of some pde equations. It now supports:
- The heat equation which is solved using the [alternating direction implicit method](https://en.wikipedia.org/wiki/Alternating_direction_implicit_method#cite_ref-2);
- The wave equation which is solved just like the heat equation, but it is very unstable now.

# Build
The program depends on Qt5 (for GUI and lots of other useful stuff) and [Cubature](https://github.com/stevengj/cubature) (for numerical integration) which is distributed under a GNU General Public License v3.0 license.\
The project is configured with QMake. For linking the Cubature please modify the .pro file (if necessary).

The program is meant to be platform-independent, although it is tested only on Linux Mint 18.2 Cinnamon 64-bit.
