cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)
project(llmr)

set(VERSION_MAJOR "0")
set(VERSION_MINOR "0")
set(VERSION_PATCH "1")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -stdlib=libc++")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")

add_subdirectory(src)


if(APPLE)
    add_subdirectory(macosx)
endif(APPLE)
