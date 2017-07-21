# Do not edit. Regenerate this with ./scripts/generate-benchmark-files.sh

set(MBGL_BENCHMARK_FILES
    # api
    benchmark/api/query.benchmark.cpp
    benchmark/api/render.benchmark.cpp

    # include/mbgl
    benchmark/include/mbgl/benchmark.hpp

    # parse
    benchmark/parse/filter.benchmark.cpp
    benchmark/parse/tile_mask.benchmark.cpp
    benchmark/parse/vector_tile.benchmark.cpp

    # src
    benchmark/src/main.cpp

    # src/mbgl/benchmark
    benchmark/src/mbgl/benchmark/benchmark.cpp

    # util
    benchmark/util/dtoa.benchmark.cpp
)
