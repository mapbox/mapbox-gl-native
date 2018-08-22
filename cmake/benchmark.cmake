# Modify cmake/benchmark-files.txt to change the source files for this target.
load_sources_list(MBGL_BENCHMARK_FILES cmake/benchmark-files.txt)
add_executable(mbgl-benchmark ${MBGL_BENCHMARK_FILES})

target_include_directories(mbgl-benchmark
    PRIVATE src
    PRIVATE benchmark/include
    PRIVATE benchmark/src
    PRIVATE platform/default
)

target_link_libraries(mbgl-benchmark
    PRIVATE mbgl-core
)

target_add_mason_package(mbgl-benchmark PRIVATE boost)
target_add_mason_package(mbgl-benchmark PRIVATE benchmark)
target_add_mason_package(mbgl-benchmark PRIVATE geojson)
target_add_mason_package(mbgl-benchmark PRIVATE rapidjson)
target_add_mason_package(mbgl-benchmark PRIVATE protozero)
target_add_mason_package(mbgl-benchmark PRIVATE vector-tile)

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
