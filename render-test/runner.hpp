#pragma once

#include <mbgl/gfx/headless_frontend.hpp>
#include <mbgl/map/map.hpp>

#include <memory>

struct TestMetadata;

class TestRunner {
public:
    TestRunner() = default;
    explicit TestRunner(const std::string& rootPath_);

    bool run(TestMetadata&);
    void reset();

private:
    bool runOperations(const std::string& key, TestMetadata&);
    bool checkQueryTestResults(mbgl::PremultipliedImage&& actualImage,
                               std::vector<mbgl::Feature>&& features,
                               TestMetadata&);
    bool checkRenderTestResults(mbgl::PremultipliedImage&& image, TestMetadata&);

    struct Impl {
        Impl(const TestMetadata&);

        mbgl::HeadlessFrontend frontend;
        mbgl::Map map;
    };
    std::unordered_map<std::string, std::unique_ptr<Impl>> maps;
    std::string rootPath{TEST_RUNNER_ROOT_PATH};
};