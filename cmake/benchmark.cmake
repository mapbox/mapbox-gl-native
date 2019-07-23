# Run scripts/generate-file-lists.js to change the source files for this target.
load_sources_list(MBGL_BENCHMARK_FILES benchmark/benchmark-files.json)
add_executable(mbgl-benchmark ${MBGL_BENCHMARK_FILES})

target_include_directories(mbgl-benchmark
    PRIVATE src
    PRIVATE benchmark/include
    PRIVATE benchmark/src
    PRIVATE platform/default/include
)

target_link_libraries(mbgl-benchmark
    PRIVATE mbgl-core
    PRIVATE mbgl-vendor-benchmark
)

mbgl_platform_benchmark()

create_source_groups(mbgl-benchmark)

set_target_properties(mbgl-benchmark PROPERTIES FOLDER "Executables")

initialize_xcode_cxx_build_settings(mbgl-benchmark)

xcode_create_scheme(
    TARGET mbgl-benchmark
    OPTIONAL_ARGS
        "--benchmark_filter=Category.*"
        "--benchmark_repetitions=1"
)
