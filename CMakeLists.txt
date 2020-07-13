cmake_minimum_required(VERSION 3.16)
project(Painter)

set(CMAKE_CXX_STANDARD 20)

# Find OpenCV:
set(OpenCV_DIR "D:/opencv/opencv4")
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
find_package(OpenCV REQUIRED)

# Add source files:
add_executable(       Painter

main.cpp

stroke.cpp
stroke.h

        # painter_pch.h


        utils/color.cpp utils/color.h utils/image_io_utils.cpp utils/image_io_utils.h)
include_directories(.)


# Add OpenCV
include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries(Painter ${OpenCV_LIBS})

# ______________________________
# Add my libraries:

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake/)
# message(${CMAKE_MODULE_PATH})

# Add pythonic:
set(pythonic_dir D:/Projects/Downloaded/pythonic)
find_package(Pythonic REQUIRED)

include_directories(${pythonic_include_directories})
target_link_libraries(Painter ${pythonic_static_library})

# Add GA:
set(PowerfulGA_dir D:/Projects/Downloaded/PowerfulGA)
find_package(PowerfulGA REQUIRED)

include_directories(${PowerfulGA_include_directories})
target_link_libraries(Painter ${PowerfulGA_static_library})

# Run Cotire on the project in order to speed the build up by deducting precompiled headers:
include(cotire)
cotire(Painter)