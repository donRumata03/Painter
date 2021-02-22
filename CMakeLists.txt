cmake_minimum_required(VERSION 3.16)

# Set compiler:

if (UNIX)
    message("UNIX => Setting Compiler to gcc-10")
    set(CMAKE_CXX_COMPILER "/usr/bin/g++-10")
    set(CMAKE_C_COMPILER "/usr/bin/gcc-10")
endif()


################################################################################
# Set dependency directories:

if (UNIX)
    set(is_computer OFF)
    set(is_notebook ON)
endif()

if (WIN32)
    set(is_computer ON)
    set(is_notebook OFF)
endif()

# Override if tou want:
# set(is_computer ...)
# set(is_notebook ...)


# Library locations:
include("CMake/LibraryConfig.cmake")

################################################################################



project(Painter)

set(CMAKE_CXX_STANDARD 20)

# Add source files:
add_executable(Painter

        main.cpp

        data_representation/stroke.cpp
        data_representation/stroke.h

        # painter_pch.h

        data_representation/color.cpp
        data_representation/color.h

        data_representation/range_rectangle.h

        io_api/image_io_utils.cpp
        io_api/image_io_utils.h

        tests/opencv_test.h
        tests/cv_io_test.h
        tests/color_test.h
        tests/stroke_rasterization_test.h
        tests/stroke_counting_test.h
        tests/draw_test.h
        tests/test_base.h
        tests/error_computing_tests/image_mse_test.h
        tests/GA_tests/stroke_constraining_test.h
        tests/GA_tests/population_generation_tests.h
        tests/example_stroke_image_generation.h
        tests/stroke_color_optimization_test.h
        tests/image_splitting_test.h
        tests/strokes_to_json_tests.h

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

        GA_operations/mutation.cpp
        GA_operations/mutation.h
        GA_operations/final_fitness_function.h
        GA_operations/util.h
        GA_operations/final_constrainer.h

        common_operations/stroke_limits.h
        common_operations/basic_constraining.cpp
        common_operations/basic_constraining.h
        common_operations/stroke_generation.cpp
        common_operations/stroke_generation.h
        common_operations/image_splitting_scheduling.cpp
        common_operations/image_splitting_scheduling.h
        common_operations/image_segregation.cpp
        common_operations/image_segregation.h

        GA_operations/all.h
        GA_operations/stroke_range_generator.h
        GA_operations/callbacks/image_logging_callback.cpp
        GA_operations/callbacks/image_logging_callback.h
        GA_operations/callbacks/GA_informer.h
        GA_operations/population_generator.h
        GA_operations/final_crossover.cpp
        GA_operations/final_crossover.h

        launching/launch_main.cpp
        launching/launch_main.h
        launching/GA_parameter_sets.h
        launching/multizoner.cpp
        launching/multizoner.h
        launching/single_zone_workers/GA_worker.cpp
        launching/single_zone_workers/GA_worker.h
        launching/single_zone_workers/single_zone_worker.h
        launching/single_zone_workers/annealing_worker.cpp
        launching/single_zone_workers/annealing_worker.h

        vector_graphics/SVG_service.cpp
        vector_graphics/SVG_service.h GA_operations/genome_generator.h GA_operations/adjustable_genome_mutation.h)


include_directories(./)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake/)

## Add libraries to the project:

# They are different configurations
set(PROJECT_DEPENDENCIES_DIR ${PROJECT_SOURCE_DIR}/dependencies)

message("Loading dependencies from: ${PROJECT_DEPENDENCIES_DIR}")
set(current_lunasvg_dir ${PROJECT_DEPENDENCIES_DIR}/lunasvg)


#if (is_computer)
#    set(current_opencv_dir D:/Workspace/C++/libs/opencv4/build)
#    set(current_boost_dir D:/Cpp_Libs/boost_1_72_0/boost_1_72_0)
#
#    set(current_GA_dir D:/Workspace/C++/libs/PowerfulGA)
#    set(current_pythonic_dir D:/Workspace/C++/libs/pythonic)
#elseif(is_notebook)
#    set(current_opencv_dir DOESNT_MATTER)
#    set(current_boost_dir D:/lib/boost_1_72_0)
#    set(current_lunasvg_dir ${PROJECT_DEPENDENCIES_DIR}/lunasvg)
#
#    set(current_GA_dir /home/vova/dev/PowerfulGA)
#    set(current_pythonic_dir /home/vova/dev/pythonic)
#else()
#    message(FATAL_ERROR "Specify your platform name!")

#endif()
## User code for other platforms:
## elseif(...)
## ...


# Find OpenCV:
if (NOT ${current_opencv_dir} STREQUAL DOESNT_MATTER)
    set(OpenCV_DIR ${current_opencv_dir})
endif()
find_package(OpenCV REQUIRED)

# Add OpenCV
include_directories(${OpenCV_INCLUDE_DIRS})
if(${CMAKE_BUILD_TYPE} STREQUAL Debug)
    message("Debug ${OpenCV_LIBS}")
    target_link_libraries(Painter debug ${OpenCV_LIBS})
else()
    message("Release ${OpenCV_LIBS}")
    target_link_libraries(Painter ${OpenCV_LIBS})
endif()

# Find LunaSVG
if (NOT ${current_lunasvg_dir} STREQUAL DOESNT_MATTER)
    set(LunaSVG_DIR ${current_lunasvg_dir})
endif()

find_package(LunaSVG REQUIRED)

# Add LunaSVG
include_directories(${LunaSVG_INCLUDE_DIRS})
if(${CMAKE_BUILD_TYPE} STREQUAL Debug)
    message("Debug ${LunaSVG_LIBRARY}")
    target_link_libraries(Painter ${LunaSVG_LIBRARY})
else()
    message("Release ${LunaSVG_LIBRARY}")
    target_link_libraries(Painter ${LunaSVG_LIBRARY})
endif()



# ______________________________
# Add my libraries:

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake/)
# message(${CMAKE_MODULE_PATH})

# Add pythonic:
set(Pythonic_DIR ${current_pythonic_dir})
# message(${Pythonic_DIR})
find_package(Pythonic REQUIRED)

include_directories(${pythonic_include_directories})
target_link_libraries(Painter ${pythonic_static_libraries})

# Add GA:
set(PowerfulGA_DIR ${current_GA_dir})
find_package(PowerfulGA REQUIRED)

include_directories(${PowerfulGA_include_directories})
target_link_libraries(Painter ${PowerfulGA_static_libraries})


# Add boost:
# see https://cmake.org/cmake/help/latest/module/FindBoost.html
# set(Boost_DIR ${current_boost_dir})

# set(Boost_INCLUDE_DIR ${current_boost_dir})
# find_package(Boost REQUIRED)
# include_directories(${Boost_INCLUDE_DIR})



# Run Cotire on the project in order to speed the build up by deducting precompiled headers:
include(cotire)
cotire(Painter)
