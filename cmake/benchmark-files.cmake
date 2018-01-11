# This file is generated. Do not edit. Regenerate this with scripts/generate-cmake-files.js

set(MBGL_BENCHMARK_FILES
    # api
    benchmark/api/query.benchmark.cpp
    benchmark/api/render.benchmark.cpp

    # benchmark
    benchmark/include/mbgl/benchmark.hpp
    benchmark/src/main.cpp
    benchmark/src/mbgl/benchmark/benchmark.cpp
    benchmark/src/mbgl/benchmark/stub_geometry_tile_feature.hpp

    # function
    benchmark/function/camera_function.benchmark.cpp
    benchmark/function/composite_function.benchmark.cpp
    benchmark/function/source_function.benchmark.cpp

    # parse
    benchmark/parse/filter.benchmark.cpp
    benchmark/parse/tile_mask.benchmark.cpp
    benchmark/parse/vector_tile.benchmark.cpp

    # util
    benchmark/util/dtoa.benchmark.cpp

)
