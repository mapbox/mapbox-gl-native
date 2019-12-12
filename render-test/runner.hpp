#pragma once

#include <mbgl/gfx/headless_frontend.hpp>
#include <mbgl/map/map.hpp>

#include "manifest_parser.hpp"

#include <memory>
#include <string>

class TestRunnerMapObserver;
struct TestMetadata;

class TestContext {
public:
    virtual mbgl::HeadlessFrontend& getFrontend() = 0;
    virtual mbgl::Map& getMap() = 0;
    virtual TestRunnerMapObserver& getObserver() = 0;
    virtual TestMetadata& getMetadata() = 0;

    GfxProbe activeGfxProbe;
    GfxProbe baselineGfxProbe;
    bool gfxProbeActive = false;

protected:
    virtual ~TestContext() = default;
};

using TestOperation = std::function<bool(TestContext&)>;

class TestRunner {
public:
    enum class UpdateResults { NO, DEFAULT, PLATFORM, METRICS, REBASELINE };
    TestRunner(Manifest, UpdateResults);
    bool run(TestMetadata&);
    void reset();

    // Manifest
    const Manifest& getManifest() const;
    void doShuffle(uint32_t seed);

private:
    bool runOperations(TestContext&);
    bool runInjectedProbesBegin(TestContext&);
    bool runInjectedProbesEnd(TestContext&, mbgl::gfx::RenderingStats);

    bool checkQueryTestResults(mbgl::PremultipliedImage&& actualImage,
                               std::vector<mbgl::Feature>&& features,
                               TestMetadata&);
    bool checkRenderTestResults(mbgl::PremultipliedImage&& image, TestMetadata&);
    bool checkProbingResults(TestMetadata&);

    struct Impl {
        Impl(const TestMetadata&);
        ~Impl();

        std::unique_ptr<TestRunnerMapObserver> observer;
        mbgl::HeadlessFrontend frontend;
        mbgl::Map map;
    };
    std::unordered_map<std::string, std::unique_ptr<Impl>> maps;
    Manifest manifest;
    UpdateResults updateResults;
};
