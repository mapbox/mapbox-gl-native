add_library(
    mbgl-benchmark STATIC EXCLUDE_FROM_ALL
    ${PROJECT_SOURCE_DIR}/benchmark/api/query.benchmark.cpp
    ${PROJECT_SOURCE_DIR}/benchmark/api/render.benchmark.cpp
    ${PROJECT_SOURCE_DIR}/benchmark/function/camera_function.benchmark.cpp
    ${PROJECT_SOURCE_DIR}/benchmark/function/composite_function.benchmark.cpp
    ${PROJECT_SOURCE_DIR}/benchmark/function/source_function.benchmark.cpp
    ${PROJECT_SOURCE_DIR}/benchmark/parse/filter.benchmark.cpp
    ${PROJECT_SOURCE_DIR}/benchmark/parse/tile_mask.benchmark.cpp
    ${PROJECT_SOURCE_DIR}/benchmark/parse/vector_tile.benchmark.cpp
    ${PROJECT_SOURCE_DIR}/benchmark/src/mbgl/benchmark/benchmark.cpp
    ${PROJECT_SOURCE_DIR}/benchmark/storage/offline_database.benchmark.cpp
    ${PROJECT_SOURCE_DIR}/benchmark/util/dtoa.benchmark.cpp
    ${PROJECT_SOURCE_DIR}/benchmark/util/tilecover.benchmark.cpp
)

target_include_directories(
    mbgl-benchmark
    PRIVATE ${PROJECT_SOURCE_DIR}/benchmark/src ${PROJECT_SOURCE_DIR}/platform/default/include ${PROJECT_SOURCE_DIR}/src
)

target_include_directories(
    mbgl-benchmark
    PUBLIC ${PROJECT_SOURCE_DIR}/benchmark/include ${PROJECT_SOURCE_DIR}/include
)

include(${PROJECT_SOURCE_DIR}/vendor/benchmark.cmake)

if(CMAKE_SYSTEM_NAME STREQUAL iOS)
    set_target_properties(mbgl-vendor-benchmark PROPERTIES XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "${IOS_DEPLOYMENT_TARGET}")
    set_target_properties(mbgl-vendor-benchmark PROPERTIES XCODE_ATTRIBUTE_ENABLE_BITCODE "YES")
    set_target_properties(mbgl-vendor-benchmark PROPERTIES XCODE_ATTRIBUTE_BITCODE_GENERATION_MODE bitcode)
    set_target_properties(mbgl-vendor-benchmark PROPERTIES XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH $<$<CONFIG:Debug>:YES>)

    set_target_properties(mbgl-benchmark PROPERTIES XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "${IOS_DEPLOYMENT_TARGET}")
    set_target_properties(mbgl-benchmark PROPERTIES XCODE_ATTRIBUTE_ENABLE_BITCODE "YES")
    set_target_properties(mbgl-benchmark PROPERTIES XCODE_ATTRIBUTE_BITCODE_GENERATION_MODE bitcode)
    set_target_properties(mbgl-benchmark PROPERTIES XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH $<$<CONFIG:Debug>:YES>)
endif()

# Needed for testing private classes
get_target_property(MBGL_CORE_PRIVATE_LIBRARIES mbgl-core LINK_LIBRARIES)

target_link_libraries(
    mbgl-benchmark
    PRIVATE ${MBGL_CORE_PRIVATE_LIBRARIES} mbgl-vendor-benchmark mbgl-compiler-options
    PUBLIC mbgl-core
)

set_property(TARGET mbgl-benchmark PROPERTY FOLDER Core)
