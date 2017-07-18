add_executable(mbgl-benchmark
    platform/default/mbgl/renderer/async_renderer_frontend.cpp
    platform/default/mbgl/renderer/async_renderer_frontend.hpp
    ${MBGL_BENCHMARK_FILES}
)

target_compile_options(mbgl-benchmark
    PRIVATE -fvisibility-inlines-hidden
)

target_include_directories(mbgl-benchmark
    PRIVATE src
    PRIVATE benchmark/include
    PRIVATE benchmark/src
    PRIVATE platform/default
)

target_link_libraries(mbgl-benchmark
    PRIVATE mbgl-core
)

target_add_mason_package(mbgl-benchmark PRIVATE benchmark)
target_add_mason_package(mbgl-benchmark PRIVATE rapidjson)
target_add_mason_package(mbgl-benchmark PRIVATE protozero)
target_add_mason_package(mbgl-benchmark PRIVATE vector-tile)

mbgl_platform_benchmark()

create_source_groups(mbgl-benchmark)

xcode_create_scheme(
    TARGET mbgl-benchmark
    OPTIONAL_ARGS
        "--benchmark_filter=Category.*"
        "--benchmark_repetitions=1"
)
