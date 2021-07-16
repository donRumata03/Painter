if (${CMAKE_BUILD_TYPE} STREQUAL Release)
    set(lunasvg_build_dir ${LunaSVG_DIR}/cmake-build-release)
elseif(${CMAKE_BUILD_TYPE} STREQUAL Debug) # It`s Debug!
    set(lunasvg_build_dir ${LunaSVG_DIR}/cmake-build-debug)
else()
    message(FATAL_ERROR "Bad CMAKE_BUILD_TYPE (${CMAKE_BUILD_TYPE}). It should be either Release or Debug")
endif()

set(LunaSVG_INCLUDE_DIRS ${LunaSVG_DIR}/include)

if (WIN32)
    set(LunaSVG_LIBRARY ${lunasvg_build_dir}/lunasvg.lib)
elseif(UNIX)
    set(LunaSVG_LIBRARY ${lunasvg_build_dir}/liblunasvg.a)
endif()