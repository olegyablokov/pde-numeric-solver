## Getting started
The program is being developed for numerical solving of some pde equations. Right now it supports:
- The 2d heat equation which is solved using the [alternating direction implicit method](https://en.wikipedia.org/wiki/Alternating_direction_implicit_method#cite_ref-2) (in Cartesian coordinates);
- The 2d center-symmetric wave equation which is solved using the [Crank–Nicolson method](https://en.wikipedia.org/wiki/Crank%E2%80%93Nicolson_method) (in polar coordinates).

## Build
The only external library the program depends on is Qt5.
The project is configured with QMake. If you encounter problems with linking please modify the .pro file.

The program is meant to be platform-independent, although it is tested only on Linux Mint 18.2 Cinnamon 64-bit.

## Docs
The project supports auto-documentation by [Doxygen](http://www.stack.nl/~dimitri/doxygen/). You will need to generate docs to see them:
```shell
cd docs/ 
doxygen Doxyfile
```
`html` and `latex` directories will appear. The default page for html files is `html/index.html`.

## Images
![screenshot](https://github.com/oyyablokov/pde_numeric_solver/blob/master/images/heat_equation1.png)
![screenshot](https://github.com/oyyablokov/pde_numeric_solver/blob/master/images/wave_equation1.png)
