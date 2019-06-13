#pragma once

#include <mbgl/util/rapidjson.hpp>
#include <mbgl/util/size.hpp>

#include <mbgl/map/mode.hpp>

#include "filesystem.hpp"

struct TestStatistics {
    TestStatistics() = default;

    uint32_t ignoreFailedTests = 0;
    uint32_t ignorePassedTests = 0;
    uint32_t erroredTests = 0;
    uint32_t failedTests = 0;
    uint32_t passedTests = 0;
};

struct TestMetadata {
    TestMetadata() = default;

    mbgl::filesystem::path path;
    mbgl::JSDocument document;

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

    // TODO
    uint32_t fadeDuration = 0;
    bool addFakeCanvas = false;

    // HTML
    std::string id;
    std::string status;
    std::string color;

    std::string actual;
    std::string expected;
    std::string diff;

    std::string errorMessage;
    double difference = 0.0;
};