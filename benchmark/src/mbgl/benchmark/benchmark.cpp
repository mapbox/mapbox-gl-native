#include <mbgl/benchmark.hpp>

#include <benchmark/benchmark.h>

namespace mbgl {

int runBenchmark(int argc, char* argv[]) {
    ::benchmark::Initialize(&argc, argv);
    ::benchmark::RunSpecifiedBenchmarks();
    return 0;
}

} // namespace mbgl
