# CMake toolchain file for QNX Environment.
#
# Usage:
#
# To use this file, you need to set the 'CMAKE_TOOLCHAIN_FILE' to point to
# 'qnx.cmake' on the command line:
#
#   cmake -DDCMAKE_TOOLCHAIN_FILE=platform/qt/qnx.cmake
#
# You will also need to provide the locations of the QNX HOST and TARGET locations.
# This can be done by setting the environment variables(QNX_HOST and QNX_TARGET) from
# the command line or running the environment setup script provided by the QNX Software
# Development Platform:
#
#   source <SDP_DIRECTORY>/qnxsdp-env.sh
#
# Options:
#
# QCC_COMPILER_TARGET:
#   The compiler name. Default: gcc_ntox86_64.
# QCC_NTOARCH:
#   The architecture to compile for. Default: x86_64.
# QNX_HOST:
#   Environment variable to QNX host build tools location.
# QNX_TARGET:
#   Environment variable to QNX target location.

set(CMAKE_SYSTEM_NAME QNX)
set(CMAKE_HOST_SYSTEM_NAME QNX)
SET(CMAKE_SYSTEM_VERSION 7.0.0)
set(QCC_COMPILER_TARGET "$ENV{QCC_COMPILER_TARGET}")
set(QCC_NTOARCH "$ENV{QCC_NTOARCH}")

#Check environment variables
if ("$ENV{QNX_HOST}" STREQUAL "")
    message(FATAL_ERROR "QNX_HOST environment variable not set")
endif()

if ("$ENV{QNX_TARGET}" STREQUAL "")
    message(FATAL_ERROR "QNX_TARGET environment variable not set")
endif()

set(QNX_HOST "$ENV{QNX_HOST}")
set(QNX_TARGET "$ENV{QNX_TARGET}")

# Use 'qcc' instead of nto${QCC_NTOARCH}-gcc and nto${QCC_NTOARCH}-gcc++ once
# the issue https://gitlab.kitware.com/cmake/cmake/issues/17126 is resolved.
#set(CMAKE_C_COMPILER qcc)
#set(CMAKE_C_COMPILER_TARGET ${QCC_COMPILER_TARGET})
#set(CMAKE_CXX_COMPILER QCC)
#set(CMAKE_CXX_COMPILER_TARGET ${QCC_COMPILER_TARGET})

set(CMAKE_C_COMPILER ${QNX_HOST}/usr/bin/nto${QCC_NTOARCH}-gcc)
set(CMAKE_CXX_COMPILER "${QNX_HOST}/usr/bin/nto${QCC_NTOARCH}-g++")

set(CMAKE_LINKER       "${QNX_HOST}/usr/bin/nto${QCC_NTOARCH}-ld"     CACHE PATH "QNX linker program" FORCE)
set(CMAKE_AR           "${QNX_HOST}/usr/bin/nto${QCC_NTOARCH}-ar"      CACHE PATH "QNX ar program" FORCE)
set(CMAKE_NM           "${QNX_HOST}/usr/bin/nto${QCC_NTOARCH}-nm"      CACHE PATH "QNX nm program" FORCE)
set(CMAKE_OBJCOPY      "${QNX_HOST}/usr/bin/nto${QCC_NTOARCH}-objcopy" CACHE PATH "QNX objcopy program" FORCE)
set(CMAKE_OBJDUMP      "${QNX_HOST}/usr/bin/nto${QCC_NTOARCH}-objdump" CACHE PATH "QNX objdump program" FORCE)
set(CMAKE_RANLIB       "${QNX_HOST}/usr/bin/nto${QCC_NTOARCH}-ranlib"  CACHE PATH "QNX ranlib program" FORCE)
set(CMAKE_STRIP        "${QNX_HOST}/usr/bin/nto${QCC_NTOARCH}-strip"   CACHE PATH "QNX strip program" FORCE)
set (CMAKE_SH           "${QNX_HOST}/usr/bin/sh"   CACHE PATH "QNX shell program" FORCE)

set(CMAKE_C_FLAGS_DEBUG "-g")
set(CMAKE_C_FLAGS_MINSIZEREL "-Os -DNDEBUG")
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g")
set(CMAKE_CXX_FLAGS_DEBUG "-g -D_DEBUG")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")

set(CMAKE_FIND_ROOT_PATH "${QNX_TARGET}")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
