# Painter
C++ program which deducts the best combination of strokes required to draw an image by a robot painter

## Usage
The project depends on several other my projects:
  1) https://github.com/donRumata03/pythonic
  2) https://github.com/donRumata03/PowerfulGA  
  
They are situated in dependencies directory.
So, to make the prject work, clone it with --recusive option 
  `git clone --recusive https://github.com/donRumata03/Painter`
  
  After that you should go to CMakeLists.txt and provide the corresponding paths.

## Setup

To link libraries, add *CMake/LibraryConfig.cmake* with following content:
``
set(CURRENT_OPENCV_DIR <...>)
set(CURRENT_GA_DIR <...>)
set(CURRENT_PYTHONIC_DIR <...>)
set(CURRENT_GTEST_DIR <...>)
``