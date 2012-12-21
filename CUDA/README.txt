Hey cs87ers

I use cmake instead of make for most of my work including stuff I do in graphics and stuff I do with CUDA. So in lieu of a Makefile, I'll give you a crash course in cmake.

quickstart:

 mkdir build
 cd build
 cmake ..
 make -j4  #since it is a parallel and distributed course

cmake is a meta build system. It takes a CMakeLists.txt file and builds a Makefile using the cmake program. After the initial cmake, you can run make as normal inside the build directory to make your executables. The build directory is for out of source builds, so binary code doesn't pollute your source tree. You can safely remove the build directory at any time as everything in there is generated automatically from the CMakeLists.txt description and cmake. 

Executables will be in the build directory. 

Many of the executables are examples from the book "CUDA by Example" available in the CS lab, the library, and fine retailers worldwide (amazon). It is a nice walkthrough of the basic features of CUDA. 
