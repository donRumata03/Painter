cmake_minimum_required(VERSION 3.16)

# Setup compiler
if (UNIX)
    message("UNIX => Setting Compiler to gcc-10")
    set(CMAKE_CXX_COMPILER "/usr/bin/g++-10")
    set(CMAKE_C_COMPILER "/usr/bin/gcc-10")
endif()

################################################################################

project(Painter)

# Setup
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake/)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Export build command to analyze them in CppDepend
add_compile_options(/utf-8)

set(SOURCE_FILES
        # Data
        data/color.cpp
        data/canvas.cpp
        data/paint_plan.cpp
        data/range_rectangle.cpp
        data/stroke.cpp
        data/units.cpp
        data/common_stroking_params.cpp
        data/stroke_limits.cpp
        data/vector_region.cpp
        data/algorithm_statistics.cpp

        # Operations
        operations/stroke/limits.cpp
        operations/stroke/generation.cpp
        operations/stroke/sorting.cpp
        operations/color/deduction.cpp
        operations/color/optimization.cpp
        operations/error.cpp

        # Rasterization
        rasterization/stroke.cpp
        rasterization/pixel_layer_tracker.cpp
        rasterization/rasterized_painting.cpp

        # Optimization adapters
        optimization_adapters/utils.cpp
        optimization_adapters/fitness.cpp
        optimization_adapters/GA/constrainer.cpp
        optimization_adapters/GA/crossover.cpp
        optimization_adapters/GA/mutator.cpp
        optimization_adapters/GA/generator.cpp
        optimization_adapters/GA/population_generator.cpp
        optimization_adapters/GA/logging_callback.cpp
        optimization_adapters/GA/informer.cpp
        optimization_adapters/annealing/generator.cpp
        optimization_adapters/annealing/logging_callback.cpp
        optimization_adapters/annealing/mutator.cpp

        # Vector graphics
        vector_graphics/svg_service.cpp
        vector_graphics/utils.cpp

        # Launch
        launch/launch.cpp
        launch/multizone_launcher.cpp
        launch/vector_zone_launcher.cpp
        launch/workers/simple.cpp
        launch/workers/ga.cpp
        launch/workers/annealing.cpp

        # Utils
        utils/image.cpp
        )

add_library(Painter-lib STATIC ${SOURCE_FILES})

# Main program
add_executable(Painter main.cpp)
target_link_libraries(Painter PRIVATE Painter-lib)

include_directories(./)

# Add libraries to the project

set(PROJECT_DEPENDENCIES_DIR ${PROJECT_SOURCE_DIR}/dependencies)
message(STATUS "Loading dependencies from: ${PROJECT_DEPENDENCIES_DIR}")

include("CMake/LibraryConfig.cmake") # Library locations
set(OpenCV_STATIC ON)
## OpenCV
message(STATUS "Load OpenCV")
### Find OpenCV:
if (NOT ${CURRENT_OPENCV_DIR} STREQUAL DOESNT_MATTER)
    set(OpenCV_DIR ${CURRENT_OPENCV_DIR})
endif()
find_package(OpenCV REQUIRED)

### Add OpenCV
include_directories(${OpenCV_INCLUDE_DIRS})
if(${CMAKE_BUILD_TYPE} STREQUAL Debug)
    message("Debug ${OpenCV_LIBS}")
    target_link_libraries(Painter-lib debug ${OpenCV_LIBS})
else()
    message("Release ${OpenCV_LIBS}")
    target_link_libraries(Painter-lib ${OpenCV_LIBS})
endif()


## LunaSVG
message(STATUS "Load LunaSVG")
set(CURRENT_LUNASVG_DIR ${PROJECT_DEPENDENCIES_DIR}/lunasvg)

### Find LunaSVG
if (NOT ${CURRENT_LUNASVG_DIR} STREQUAL DOESNT_MATTER)
    set(LunaSVG_DIR ${CURRENT_LUNASVG_DIR})
endif()

find_package(LunaSVG REQUIRED)

### Add LunaSVG
include_directories(${LunaSVG_INCLUDE_DIRS})
if(${CMAKE_BUILD_TYPE} STREQUAL Debug)
    message("Debug ${LunaSVG_LIBRARY}")
    target_link_libraries(Painter-lib ${LunaSVG_LIBRARY})
else()
    message("Release ${LunaSVG_LIBRARY}")
    target_link_libraries(Painter-lib ${LunaSVG_LIBRARY})
endif()


## Argparse
message(STATUS "Load Argparse")
set(CURRENT_ARGPARSE_DIR ${PROJECT_DEPENDENCIES_DIR}/argparse)

### Find Argparse
if (NOT ${CURRENT_ARGPARSE_DIR} STREQUAL DOESNT_MATTER)
    set(Argparse_DIR ${CURRENT_ARGPARSE_DIR})
endif()

find_package(Argparse REQUIRED)

### Add Argparse
include_directories(${Argparse_INCLUDE_DIRS})


## Pythonic
message(STATUS "Load Pythonic")
### Add pythonic
set(Pythonic_DIR ${CURRENT_PYTHONIC_DIR})

find_package(Pythonic REQUIRED)

include_directories(${pythonic_include_directories})
target_link_libraries(Painter-lib ${pythonic_static_libraries})


## PowerfulGA
message(STATUS "Load PowerfulGA")
### Add PowerfulGA
set(PowerfulGA_DIR ${CURRENT_GA_DIR})
find_package(PowerfulGA REQUIRED)

include_directories(${PowerfulGA_include_directories})
target_link_libraries(Painter-lib ${PowerfulGA_static_libraries})


# Testing
add_subdirectory(${CURRENT_GTEST_DIR})
add_subdirectory(tests)

# Run Cotire on the project in order to speed the build up by deducting precompiled headers
include(cotire)
cotire(Painter-lib)
cotire(Painter)
