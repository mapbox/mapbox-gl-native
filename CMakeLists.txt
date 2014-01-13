cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)
project(llmr)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")

add_subdirectory(src)
add_subdirectory(macosx)
