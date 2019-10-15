#pragma once

#include <mbgl/gfx/headless_frontend.hpp>
#include <mbgl/map/map.hpp>

#include <memory>
#include <string>

struct RunContext;
class TestRunnerMapObserver;
struct TestMetadata;

class TestRunner {
public:
    TestRunner() = default;
    explicit TestRunner(const std::string& testRootPath);
    bool run(TestMetadata&);
    void reset();

private:
    bool runOperations(const std::string& key, TestMetadata&, RunContext&);
    bool checkQueryTestResults(mbgl::PremultipliedImage&& actualImage,
                               std::vector<mbgl::Feature>&& features,
                               TestMetadata&);
    bool checkRenderTestResults(mbgl::PremultipliedImage&& image, TestMetadata&);

    struct Impl {
        Impl(const TestMetadata&);
        ~Impl();

        std::unique_ptr<TestRunnerMapObserver> observer;
        mbgl::HeadlessFrontend frontend;
        mbgl::Map map;
    };
    std::unordered_map<std::string, std::unique_ptr<Impl>> maps;
    std::string testRootPath{TEST_RUNNER_ROOT_PATH};
};
