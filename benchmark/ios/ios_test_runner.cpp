#include <ios_test_runner.hpp>

#include <mbgl/benchmark.hpp>

#include <mbgl/util/logging.hpp>

#include <unistd.h>
#include <vector>

#define EXPORT __attribute__((visibility("default")))

EXPORT
bool TestRunner::startTest(const std::string& basePath) {
    std::vector<std::string> arguments = {"mbgl-benchmark-runner",
                                          "--benchmark_repetitions=10",
                                          "--benchmark_format=json",
                                          "--benchmark_out=" + basePath + "/benchmark/results.json"};
    std::vector<char*> argv;
    for (const auto& arg : arguments) {
        argv.push_back(const_cast<char*>(arg.data()));
    }
    argv.push_back(nullptr);

    if (chdir(basePath.c_str())) {
        mbgl::Log::Error(mbgl::Event::General, "Failed to change the directory to " + basePath);
        return false;
    }

    mbgl::Log::Info(mbgl::Event::General, "Start BenchmarkRunner");
    int status = mbgl::runBenchmark(static_cast<uint32_t>(argv.size()), argv.data());
    mbgl::Log::Info(mbgl::Event::General, "BenchmarkRunner finished with status: '%d'", status);

    return status == 0;
}
