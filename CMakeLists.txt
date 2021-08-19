cmake_minimum_required(VERSION 3.16)

# Setup compiler:
if (UNIX)
    message("UNIX => Setting Compiler to gcc-10")
    set(CMAKE_CXX_COMPILER "/usr/bin/g++-10")
    set(CMAKE_C_COMPILER "/usr/bin/gcc-10")
endif()

include("CMake/LibraryConfig.cmake") # Library locations

################################################################################

project(Painter)

# Setup
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake/)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Export build command to analyze them in CppDepend


# Main program
add_executable(Painter

        main.cpp

        # Data


        # Launch
        launch/launch.cpp
        )

include_directories(./)

# Add libraries to the project

set(PROJECT_DEPENDENCIES_DIR ${PROJECT_SOURCE_DIR}/dependencies)
message(STATUS "Loading dependencies from: ${PROJECT_DEPENDENCIES_DIR}")


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
    target_link_libraries(Painter debug ${OpenCV_LIBS})
else()
    message("Release ${OpenCV_LIBS}")
    target_link_libraries(Painter ${OpenCV_LIBS})
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
    target_link_libraries(Painter ${LunaSVG_LIBRARY})
else()
    message("Release ${LunaSVG_LIBRARY}")
    target_link_libraries(Painter ${LunaSVG_LIBRARY})
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
target_link_libraries(Painter ${pythonic_static_libraries})


## PowerfulGA
message(STATUS "Load PowerfulGA")
### Add PowerfulGA
set(PowerfulGA_DIR ${CURRENT_GA_DIR})
find_package(PowerfulGA REQUIRED)

include_directories(${PowerfulGA_include_directories})
target_link_libraries(Painter ${PowerfulGA_static_libraries})


# Run Cotire on the project in order to speed the build up by deducting precompiled headers
include(cotire)
cotire(Painter)
