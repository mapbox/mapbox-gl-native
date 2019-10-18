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

    std::string defaultExpectations() const {
        assert(!expectations.empty());
        return expectations.front().string();
    }
};

inline std::tuple<bool, float> checkValue(float expected, float actual, float tolerance) {
    float delta = expected * tolerance;
    assert(delta >= 0.0f);
    return std::make_tuple(std::abs(expected - actual) <= delta, delta);
}

struct FileSizeProbe {
    FileSizeProbe() = default;
    FileSizeProbe(std::string path_, uintmax_t size_, float tolerance_)
        : path(std::move(path_)), size(size_), tolerance(tolerance_) {}

    std::string path;
    uintmax_t size;
    float tolerance;
};

struct MemoryProbe {
    MemoryProbe() = default;
    MemoryProbe(size_t peak_, size_t allocations_) : peak(peak_), allocations(allocations_), tolerance(0.0f) {}

    size_t peak;
    size_t allocations;
    float tolerance;

    static std::tuple<bool, float> checkPeak(const MemoryProbe& expected, const MemoryProbe& actual) {
        return checkValue(expected.peak, actual.peak, actual.tolerance);
    }

    static std::tuple<bool, float> checkAllocations(const MemoryProbe& expected, const MemoryProbe& actual) {
        return checkValue(expected.allocations, actual.allocations, actual.tolerance);
    }
};

struct NetworkProbe {
    NetworkProbe() = default;
    NetworkProbe(size_t requests_, size_t transferred_) : requests(requests_), transferred(transferred_) {}

    size_t requests;
    size_t transferred;
};

class TestMetrics {
public:
    bool isEmpty() const { return fileSize.empty() && memory.empty() && network.empty(); }
    std::map<std::string, FileSizeProbe> fileSize;
    std::map<std::string, MemoryProbe> memory;
    std::map<std::string, NetworkProbe> network;
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
