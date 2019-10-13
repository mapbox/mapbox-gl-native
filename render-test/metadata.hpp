#pragma once

#include <mbgl/util/geo.hpp>
#include <mbgl/util/rapidjson.hpp>
#include <mbgl/util/size.hpp>

#include <mbgl/map/mode.hpp>
#include <mbgl/renderer/query.hpp>

#include "filesystem.hpp"

#include <map>

struct TestStatistics {
    TestStatistics() = default;

    uint32_t ignoreFailedTests = 0;
    uint32_t ignorePassedTests = 0;
    uint32_t erroredTests = 0;
    uint32_t failedTests = 0;
    uint32_t passedTests = 0;
};

struct TestPaths {
    mbgl::filesystem::path stylePath;
    std::vector<mbgl::filesystem::path> expectations;
    std::string id;

    std::string defaultExpectations() const {
        assert(!expectations.empty());
        return expectations.front().string();
    }
};

struct FileSizeProbe {
    FileSizeProbe() = default;
    FileSizeProbe(std::string path_, uintmax_t size_) : path(std::move(path_)), size(size_) {}

    std::string path;
    uintmax_t size;
};

struct MemoryProbe {
    MemoryProbe() = default;
    MemoryProbe(size_t peak_, size_t allocations_)
        : peak(peak_)
        , allocations(allocations_) {}

    size_t peak;
    size_t allocations;
};

class TestMetrics {
public:
    bool isEmpty() const { return fileSize.empty() && memory.empty(); }
    std::map<std::string, FileSizeProbe> fileSize;
    std::map<std::string, MemoryProbe> memory;
};

struct TestMetadata {
    TestMetadata() = default;

    TestPaths paths;
    mbgl::JSDocument document;
    bool renderTest = true;

    mbgl::Size size{ 512u, 512u };
    float pixelRatio = 1.0f;
    double allowed = 0.00015; // diff
    std::string description;
    mbgl::MapMode mapMode = mbgl::MapMode::Static;
    mbgl::MapDebugOptions debug = mbgl::MapDebugOptions::NoDebug;
    bool crossSourceCollisions = true;
    bool axonometric = false;
    double xSkew = 0.0;
    double ySkew = 1.0;
    mbgl::ScreenCoordinate queryGeometry{0u, 0u};
    mbgl::ScreenBox queryGeometryBox{{0u, 0u}, {0u, 0u}};
    mbgl::RenderedQueryOptions queryOptions;

    // TODO
    uint32_t fadeDuration = 0;
    bool addFakeCanvas = false;

    // HTML
    std::string id;
    std::string status;
    std::string color;

    std::string actual;
    std::string actualJson;
    std::string expected;
    std::string diff;

    std::string errorMessage;
    double difference = 0.0;

    TestMetrics metrics;
    TestMetrics expectedMetrics;
};
