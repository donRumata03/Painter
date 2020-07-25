cmake_minimum_required(VERSION 3.16)

################################################################################
# Set dependency directories:

set(is_computer OFF)
set(is_notebook ON)

# They are different configurations
if (is_computer)
    set(current_opencv_dir D:/OpenCV4/opencv4)
    set(current_GA_dir D:/Projects/Utilities/GA)
    set(current_pythonic_dir D:/pythonic)

elseif(is_notebook)
    set(current_opencv_dir D:/opencv/opencv4)
#    set(current_GA_dir D:/Projects/Downloaded/PowerfulGA)
#    set(current_pythonic_dir D:/Projects/Downloaded/pythonic)
    set(current_GA_dir D:/Projects/Utilities/GA)
    set(current_pythonic_dir D:/pythonic)

# User code for other platforms:
# elseif(...)
    # ...

else()
    message(FATAL_ERROR "Specify your platform name!")

endif()

################################################################################

project(Painter)

set(CMAKE_CXX_STANDARD 20)

# Add source files:
add_executable(       Painter

main.cpp

utils/stroke.cpp
utils/stroke.h

# painter_pch.h


utils/color.cpp
utils/color.h

utils/image_io_utils.cpp
utils/image_io_utils.h

tests/opencv_test.h
tests/cv_io_test.h
tests/color_test.h
tests/stroke_rasterization_test.h

rasterization/stroke_rasterizer.cpp
rasterization/stroke_rasterizer.h

rasterization/drawer.cpp
rasterization/drawer.h

optimization/error_computing.cpp
optimization/error_computing.h

optimization/stroke_color_optimizing.cpp
optimization/stroke_color_optimizing.h

optimization/image_stroke_optimization.cpp
optimization/image_stroke_optimization.h
tests/test_stroke_color_optimization.h

)


include_directories(/)

## Add libraries to the project:

# Find OpenCV:
set(OpenCV_DIR ${current_opencv_dir})
# list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
message(${OpenCV_DIR})
find_package(OpenCV REQUIRED)

# Add OpenCV
include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries(Painter ${OpenCV_LIBS})

# ______________________________
# Add my libraries:

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake/)
# message(${CMAKE_MODULE_PATH})

# Add pythonic:
set(Pythonic_DIR ${current_pythonic_dir})
message(${Pythonic_DIR})
find_package(Pythonic REQUIRED)

include_directories(${pythonic_include_directories})
target_link_libraries(Painter ${pythonic_static_library})

# Add GA:
set(PowerfulGA_DIR ${current_GA_dir})
find_package(PowerfulGA REQUIRED)

include_directories(${PowerfulGA_include_directories})
target_link_libraries(Painter ${PowerfulGA_static_library})

# Run Cotire on the project in order to speed the build up by deducting precompiled headers:
include(cotire)
cotire(Painter)