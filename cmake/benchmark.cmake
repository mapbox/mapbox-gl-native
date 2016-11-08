add_executable(mbgl-benchmark
    ${MBGL_BENCHMARK_FILES}
)

target_compile_options(mbgl-benchmark
    PRIVATE -fvisibility-inlines-hidden
)

target_include_directories(mbgl-benchmark
    PRIVATE include
    PRIVATE src # TODO: eliminate
    PRIVATE benchmark/include
    PRIVATE benchmark/src
    PRIVATE platform/default
)

target_link_libraries(mbgl-benchmark
    PRIVATE mbgl-core
)

target_add_mason_package(mbgl-benchmark PRIVATE benchmark)
target_add_mason_package(mbgl-benchmark PRIVATE rapidjson)

mbgl_platform_benchmark()

create_source_groups(mbgl-benchmark)
