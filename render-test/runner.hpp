#pragma once

#include <mbgl/gfx/headless_frontend.hpp>
#include <mbgl/map/map.hpp>

#include "manifest_parser.hpp"

#include <memory>
#include <string>

class TestRunnerMapObserver;
struct TestMetadata;

class TestRunner {
public:
    explicit TestRunner(Manifest);
    bool run(TestMetadata&);
    void reset();

    // Manifest
    const Manifest& getManifest() const;
    void doShuffle(uint32_t seed);

private:
    bool runOperations(const std::string& key, TestMetadata&);
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
    Manifest manifest;
};
