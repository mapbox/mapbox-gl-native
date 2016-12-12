cmake_minimum_required(VERSION 3.2)
project(earcut_hpp)

option(EARCUT_BUILD_TESTS "Build the earcut test program" ON)
option(EARCUT_BUILD_BENCH "Build the earcut benchmark program" ON)
option(EARCUT_BUILD_VIZ "Build the earcut visualizer program" ON)

# configure optimization
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(OPTIMIZATION_FLAGS "-O0") # optional: -fsanitize=signed-integer-overflow,undefined
    set(IS_DEBUG true)
    message("-- Configuring debug build")
else()
    set(OPTIMIZATION_FLAGS "-O3")
    set(IS_DEBUG false)
    message("-- Configuring release build")
endif()

# check if clang or gcc
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang$")
    set(CPP14_FLAG "-std=c++1y") # clang 3.4 compat
    set(CXX_COMPILER_IS_GNU_OR_CLANG true)
elseif (CMAKE_COMPILER_IS_GNUCXX)
    set(CPP14_FLAG "-std=c++14")
    set(CXX_COMPILER_IS_GNU_OR_CLANG true)
endif()
if ("${CMAKE_CC_COMPILER_ID}" STREQUAL "Clang")
    set(CC_COMPILER_IS_GNU_OR_CLANG true)
elseif (CMAKE_COMPILER_IS_GNUCC)
    set(CC_COMPILER_IS_GNU_OR_CLANG true)
endif()

# setup compiler flags
if(CC_COMPILER_IS_GNU_OR_CLANG)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OPTIMIZATION_FLAGS}")
endif()
if(MSVC)
    if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W3" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3")
    endif()
elseif(CXX_COMPILER_IS_GNU_OR_CLANG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CPP14_FLAG} -pipe -Werror")
    # optional: -pedantic -Wconversion -Wall -Wextra
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OPTIMIZATION_FLAGS}")
    # optional: -march=native (builds with the optimizations available on the build machine (only for local use!))
else()
    message(WARNING "Configuring earcut for unsupported compiler ${CMAKE_CXX_COMPILER_ID}.")
endif()

# dependencies
set (EARCUT_INCLUDE_DIRS "")
if (EARCUT_BUILD_VIZ)
    # OpenGL
    # linux: xorg-dev libgl1-mesa-glx libgl1-mesa-dev
    # windows: in the windows sdk
    find_package(OpenGL REQUIRED)
    list(APPEND EARCUT_INCLUDE_DIRS ${OPENGL_INCLUDE_DIRS})

    # GLFW3
    if(EXISTS "${PROJECT_SOURCE_DIR}/.gitmodules")
        execute_process(
                COMMAND             git submodule update --init --recursive
                WORKING_DIRECTORY   ${PROJECT_SOURCE_DIR}
                OUTPUT_QUIET
                ERROR_QUIET
        )
    endif()
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build the GLFW example programs" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "Build the GLFW test programs" FORCE)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build the GLFW documentation" FORCE)
    set(GLFW_INSTALL OFF CACHE BOOL "Generate installation target" FORCE)
    add_subdirectory(glfw)
    list(APPEND EARCUT_INCLUDE_DIRS "glfw/include")
    set(GLFW_LIBS glfw ${GLFW_LIBRARIES})
endif()

# BOOST (optional)
find_package(Boost 1.54 COMPONENTS program_options QUIET)
if(DEFINED ${Boost_INCLUDE_DIR})
    list(APPEND EARCUT_INCLUDE_DIRS ${Boost_INCLUDE_DIR})
else()
    message("-- Configuring without Boost")
endif()

# earcut.hpp
list(APPEND EARCUT_INCLUDE_DIRS "include")

include_directories(${EARCUT_INCLUDE_DIRS})

file(GLOB FIXTURE_SOURCE_FILES test/fixtures/*.cpp test/fixtures/*.hpp)
source_group(fixtures FILES ${FIXTURE_SOURCE_FILES})

file(GLOB COMPARISON_SOURCE_FILES test/comparison/*.cpp test/comparison/*.hpp)
source_group(comparison FILES ${COMPARISON_SOURCE_FILES})

file(GLOB LIBTESS2_SOURCE_FILES test/comparison/libtess2/*.c test/comparison/libtess2/*.h)
source_group(comparison\\libtess2 FILES ${LIBTESS2_SOURCE_FILES})

set(COMMON_SOURCE_FILES ${LIBTESS2_SOURCE_FILES} ${FIXTURE_SOURCE_FILES} ${COMPARISON_SOURCE_FILES})

set(TESTS_SOURCE_FILES ${COMMON_SOURCE_FILES} test/tap.cpp test/tap.hpp test/test.cpp)
set(BENCH_SOURCE_FILES ${COMMON_SOURCE_FILES} test/bench.cpp)
set(VIZ_SOURCE_FILES ${COMMON_SOURCE_FILES} test/viz.cpp)

if (EARCUT_BUILD_TESTS)
    add_executable(tests ${TESTS_SOURCE_FILES})
    target_link_libraries(tests ${Boost_LIBRARIES})
endif()
if (EARCUT_BUILD_BENCH)
    add_executable(bench ${BENCH_SOURCE_FILES})
    target_link_libraries(bench ${Boost_LIBRARIES})
endif()
if (EARCUT_BUILD_VIZ)
    add_executable(viz ${VIZ_SOURCE_FILES})
    target_link_libraries(viz ${Boost_LIBRARIES} ${GLFW_LIBS} ${OPENGL_LIBRARIES})
endif()
