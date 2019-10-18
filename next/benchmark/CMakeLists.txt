add_library(
    mbgl-benchmark SHARED EXCLUDE_FROM_ALL
    ${MBGL_ROOT}/benchmark/api/query.benchmark.cpp
    ${MBGL_ROOT}/benchmark/api/render.benchmark.cpp
    ${MBGL_ROOT}/benchmark/function/camera_function.benchmark.cpp
    ${MBGL_ROOT}/benchmark/function/composite_function.benchmark.cpp
    ${MBGL_ROOT}/benchmark/function/source_function.benchmark.cpp
    ${MBGL_ROOT}/benchmark/parse/filter.benchmark.cpp
    ${MBGL_ROOT}/benchmark/parse/tile_mask.benchmark.cpp
    ${MBGL_ROOT}/benchmark/parse/vector_tile.benchmark.cpp
    ${MBGL_ROOT}/benchmark/src/mbgl/benchmark/benchmark.cpp
    ${MBGL_ROOT}/benchmark/storage/offline_database.benchmark.cpp
    ${MBGL_ROOT}/benchmark/util/dtoa.benchmark.cpp
    ${MBGL_ROOT}/benchmark/util/tilecover.benchmark.cpp
)

target_include_directories(
    mbgl-benchmark
    PRIVATE ${MBGL_ROOT}/benchmark/src ${MBGL_ROOT}/platform/default/include ${MBGL_ROOT}/src
)

target_include_directories(
    mbgl-benchmark
    PUBLIC ${MBGL_ROOT}/benchmark/include ${MBGL_ROOT}/include
)

include(${PROJECT_SOURCE_DIR}/vendor/benchmark.cmake)

# Needed for testing private classes
get_target_property(MBGL_CORE_PRIVATE_LIBRARIES mbgl-core LINK_LIBRARIES)

target_link_libraries(
    mbgl-benchmark
    PRIVATE
        ${MBGL_CORE_PRIVATE_LIBRARIES}
        mbgl-vendor-benchmark
        mbgl-core
        $<$<PLATFORM_ID:Android>:mbgl-core-android>
)

if(CMAKE_SYSTEM_NAME STREQUAL Android)
    set_target_properties(mbgl-benchmark PROPERTIES LINK_FLAGS_RELEASE "-fuse-ld=gold -O2 -flto -Wl,--icf=safe")
endif()

set_property(TARGET mbgl-benchmark PROPERTY FOLDER Core)
