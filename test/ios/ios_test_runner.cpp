#include "ios_test_runner.hpp"

#include <mbgl/test.hpp>

#include <mbgl/util/logging.hpp>

#include <unistd.h>
#include <vector>

#define EXPORT __attribute__((visibility("default")))

EXPORT
bool TestRunner::startTest(const std::string& basePath) {
    std::vector<std::string> arguments = {"mbgl-test-runner", "--gtest_output=xml:" + basePath + "/test/results.xml"};
    std::vector<char*> argv;
    for (const auto& arg : arguments) {
        argv.push_back(const_cast<char*>(arg.data()));
    }
    argv.push_back(nullptr);

    if (chdir(basePath.c_str())) {
        mbgl::Log::Error(mbgl::Event::General, "Failed to change the directory to " + basePath);
        return false;
    }

    mbgl::Log::Info(mbgl::Event::General, "Start TestRunner");
    int status = mbgl::runTests(static_cast<uint32_t>(argv.size()), argv.data());
    mbgl::Log::Info(mbgl::Event::General, "TestRunner finished with status: '%d'", status);

    return status == 0;
}
