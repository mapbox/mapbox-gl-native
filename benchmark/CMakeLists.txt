add_library(mbgl-benchmark SHARED EXCLUDE_FROM_ALL
    ${CMAKE_CURRENT_LIST_DIR}/api/query.benchmark.cpp
    ${CMAKE_CURRENT_LIST_DIR}/api/render.benchmark.cpp
    ${CMAKE_CURRENT_LIST_DIR}/function/camera_function.benchmark.cpp
    ${CMAKE_CURRENT_LIST_DIR}/function/composite_function.benchmark.cpp
    ${CMAKE_CURRENT_LIST_DIR}/function/source_function.benchmark.cpp
    ${CMAKE_CURRENT_LIST_DIR}/parse/filter.benchmark.cpp
    ${CMAKE_CURRENT_LIST_DIR}/parse/tile_mask.benchmark.cpp
    ${CMAKE_CURRENT_LIST_DIR}/parse/vector_tile.benchmark.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mbgl/benchmark/benchmark.cpp
    ${CMAKE_CURRENT_LIST_DIR}/storage/offline_database.benchmark.cpp
    ${CMAKE_CURRENT_LIST_DIR}/util/dtoa.benchmark.cpp
    ${CMAKE_CURRENT_LIST_DIR}/util/tilecover.benchmark.cpp
)

target_include_directories(mbgl-benchmark PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src
    ${PROJECT_SOURCE_DIR}/platform/default/include
    ${PROJECT_SOURCE_DIR}/src
)

target_include_directories(mbgl-benchmark PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
    ${PROJECT_SOURCE_DIR}/include
)

include(${PROJECT_SOURCE_DIR}/vendor/benchmark.cmake)

# Needed for testing private classes
get_target_property(MBGL_CORE_PRIVATE_LIBRARIES mbgl-core LINK_LIBRARIES)

target_link_libraries(mbgl-benchmark PRIVATE
    ${MBGL_CORE_PRIVATE_LIBRARIES}
    mbgl-core
    mbgl-vendor-benchmark
)

if(CMAKE_SYSTEM_NAME STREQUAL Android)
    set_target_properties(mbgl-benchmark PROPERTIES LINK_FLAGS_RELEASE "-fuse-ld=gold -O2 -flto -Wl,--icf=safe")
endif()

set_property(TARGET mbgl-benchmark PROPERTY FOLDER Core)
