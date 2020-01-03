#pragma once

#include <mbgl/gfx/headless_frontend.hpp>
#include <mbgl/map/map.hpp>

#include "manifest_parser.hpp"

#include <memory>
#include <string>

class TestRunnerMapObserver;
struct TestMetadata;

class TestRunnerMapObserver : public mbgl::MapObserver {
public:
    TestRunnerMapObserver() = default;
    void onDidFailLoadingMap(mbgl::MapLoadError, const std::string&) override { mapLoadFailure = true; }

    void onDidFinishRenderingMap(RenderMode mode) override final {
        if (!finishRenderingMap) finishRenderingMap = mode == RenderMode::Full;
    }

    void onDidBecomeIdle() override final { idle = true; }

    void reset() {
        mapLoadFailure = false;
        finishRenderingMap = false;
        idle = false;
    }

    bool mapLoadFailure;
    bool finishRenderingMap;
    bool idle;
};

class TestRunner {
public:
    enum class UpdateResults { NO, DEFAULT, PLATFORM, METRICS, REBASELINE };

    TestRunner(Manifest, UpdateResults);
    void run(TestMetadata&);
    void reset();

    // Manifest
    const Manifest& getManifest() const;
    void doShuffle(uint32_t seed);

private:
    mbgl::HeadlessFrontend::RenderResult runTest(TestMetadata& metadata, TestContext& ctx);
    void checkQueryTestResults(mbgl::PremultipliedImage&& actualImage,
                               std::vector<mbgl::Feature>&& features,
                               TestMetadata&);
    void checkRenderTestResults(mbgl::PremultipliedImage&& image, TestMetadata&);
    void checkProbingResults(TestMetadata&);

    struct Impl {
        Impl(const TestMetadata&, const Manifest&);
        ~Impl();

        std::unique_ptr<TestRunnerMapObserver> observer;
        mbgl::HeadlessFrontend frontend;
        mbgl::Map map;
    };
    std::unordered_map<std::string, std::unique_ptr<Impl>> maps;
    Manifest manifest;
    UpdateResults updateResults;
};
