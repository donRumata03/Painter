cmake_minimum_required(VERSION 3.16)

################################################################################
# Set dependency directories:

set(is_computer ON)
set(is_notebook OFF)

# They are different configurations
if (is_computer)
    set(current_opencv_dir D:/OpenCV4/build)
    set(current_GA_dir D:/Projects/Utilities/GA)
    set(current_pythonic_dir D:/pythonic)
    set(current_boost_dir D:/lib/boost_1_72_0)

elseif(is_notebook)
    set(current_opencv_dir D:/opencv/opencv4)
#    set(current_GA_dir D:/Projects/Downloaded/PowerfulGA)
#    set(current_pythonic_dir D:/Projects/Downloaded/pythonic)
    set(current_GA_dir D:/Projects/Utilities/GA)
    set(current_pythonic_dir D:/pythonic)
    set(current_boost_dir D:/Cpp_Libs/boost_1_72_0/boost_1_72_0)

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

        data_representation/stroke.cpp
        data_representation/stroke.h

# painter_pch.h


        data_representation/color.cpp
        data_representation/color.h

        io_api/image_io_utils.cpp
        io_api/image_io_utils.h

tests/opencv_test.h
tests/cv_io_test.h
tests/color_test.h
tests/stroke_rasterization_test.h

rasterization/stroke_rasterizer.cpp
rasterization/stroke_rasterizer.h

rasterization/drawer.cpp
rasterization/drawer.h

        optimization/error/error_computing.cpp
        optimization/error/error_computing.h

optimization/stroke_color_optimizing.cpp
optimization/stroke_color_optimizing.h

optimization/image_stroke_optimization.cpp
optimization/image_stroke_optimization.h
tests/stroke_color_optimization_test.h

tests/stroke_counting_test.h
tests/draw_test.h
tests/test_base.h
tests/error_computing_tests/image_mse_test.h

        optimization/GA_operations/mutation.cpp optimization/GA_operations/mutation.h common_operations/stroke_limits.h optimization/GA_operations/final_fitness_function.h optimization/GA_operations/util.h data_representation/range_rectangle.h common_operations/basic_constraining.cpp common_operations/basic_constraining.h optimization/GA_operations/final_constrainer.h tests/GA_tests/stroke_constraining_test.h launching/launch_main_ga.cpp launching/launch_main_ga.h optimization/GA_operations/stroke_range_generator.h tests/GA_tests/population_generation_tests.h optimization/GA_operations/callbacks/image_logging_callback.cpp optimization/GA_operations/callbacks/image_logging_callback.h optimization/GA_operations/callbacks/GA_informer.h)


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
message("${OpenCV_LIBS}")

# ______________________________
# Add my libraries:

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake/)
# message(${CMAKE_MODULE_PATH})

# Add pythonic:
set(Pythonic_DIR ${current_pythonic_dir})
# message(${Pythonic_DIR})
find_package(Pythonic REQUIRED)

include_directories(${pythonic_include_directories})
target_link_libraries(Painter ${pythonic_static_library})

# Add GA:
set(PowerfulGA_DIR ${current_GA_dir})
find_package(PowerfulGA REQUIRED)

include_directories(${PowerfulGA_include_directories})
target_link_libraries(Painter ${PowerfulGA_static_library})

# Add boost:
# see https://cmake.org/cmake/help/latest/module/FindBoost.html
# set(Boost_DIR ${current_boost_dir})

# set(Boost_INCLUDE_DIR ${current_boost_dir})
# find_package(Boost REQUIRED)
# include_directories(${Boost_INCLUDE_DIR})

# Run Cotire on the project in order to speed the build up by deducting precompiled headers:
include(cotire)
cotire(Painter)
