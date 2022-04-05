# Mandelbrot-Generator
## Requirements
In order to use this program the following libraries are needed:
-OpenGl
-GLFW
-GLEW
-GMP and MPFR 
-BOOST (program_options and multiprecision)
If you are using Debian 11 you can simply install the following packages:
-libopengl-dev
-libglfw3-dev
-libglew-dev
-libgmp-dev and libmpfr-dev
-libboost-dev and libboost-program-options-dev
## Build
If all the required libraries have been installed, simply navigate to the main program folder and run `make`. By default this will build the program in realese mode: if you want to switch to debug mod change `$(RELEASE)` to `$(DEBUG)` in Makefile. 
## Program usage
After the program has been build, type `./mandelbrot_generator.out -h` to get an on-screen guide of the program capabilities.