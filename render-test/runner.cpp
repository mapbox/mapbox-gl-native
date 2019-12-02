#include <mbgl/map/camera.hpp>
#include <mbgl/map/map_observer.hpp>
#include <mbgl/renderer/renderer.hpp>
#include <mbgl/renderer/renderer_observer.hpp>
#include <mbgl/style/conversion/filter.hpp>
#include <mbgl/style/conversion/layer.hpp>
#include <mbgl/style/conversion/light.hpp>
#include <mbgl/style/conversion/source.hpp>
#include <mbgl/style/image.hpp>
#include <mbgl/style/layer.hpp>
#include <mbgl/style/light.hpp>
#include <mbgl/style/rapidjson_conversion.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/compression.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/timer.hpp>

#include <mapbox/pixelmatch.hpp>

#include <../expression-test/test_runner_common.hpp>
#include "allocation_index.hpp"
#include "file_source.hpp"
#include "metadata.hpp"
#include "parser.hpp"
#include "runner.hpp"

#include <algorithm>
#include <cassert>
#include <utility>
#include <sstream>

using namespace mbgl;

namespace {
static const std::string waitOp("wait");
static const std::string sleepOp("sleep");
static const std::string addImageOp("addImage");
static const std::string updateImageOp("updateImage");
static const std::string removeImageOp("removeImage");
static const std::string setStyleOp("setStyle");
static const std::string setCenterOp("setCenter");
static const std::string setZoomOp("setZoom");
static const std::string setBearingOp("setBearing");
static const std::string setPitchOp("setPitch");
static const std::string setFilterOp("setFilter");
static const std::string setLayerZoomRangeOp("setLayerZoomRange");
static const std::string setLightOp("setLight");
static const std::string addLayerOp("addLayer");
static const std::string removeLayerOp("removeLayer");
static const std::string addSourceOp("addSource");
static const std::string removeSourceOp("removeSource");
static const std::string setPaintPropertyOp("setPaintProperty");
static const std::string setLayoutPropertyOp("setLayoutProperty");
static const std::string fileSizeProbeOp("probeFileSize");
static const std::string memoryProbeOp("probeMemory");
static const std::string memoryProbeStartOp("probeMemoryStart");
static const std::string memoryProbeEndOp("probeMemoryEnd");
static const std::string networkProbeOp("probeNetwork");
static const std::string networkProbeStartOp("probeNetworkStart");
static const std::string networkProbeEndOp("probeNetworkEnd");
static const std::string setFeatureStateOp("setFeatureState");
static const std::string getFeatureStateOp("getFeatureState");
static const std::string removeFeatureStateOp("removeFeatureState");
static const std::string panGestureOp("panGesture");
static const std::string gfxProbeOp("probeGFX");
static const std::string gfxProbeStartOp("probeGFXStart");
static const std::string gfxProbeEndOp("probeGFXEnd");
} // namespace

GfxProbe::GfxProbe(const mbgl::gfx::RenderingStats& stats, const GfxProbe& prev)
    : numBuffers(stats.numBuffers),
      numDrawCalls(stats.numDrawCalls),
      numFrameBuffers(stats.numFrameBuffers),
      numTextures(stats.numActiveTextures),
      memIndexBuffers(stats.memIndexBuffers, std::max(stats.memIndexBuffers, prev.memIndexBuffers.peak)),
      memVertexBuffers(stats.memVertexBuffers, std::max(stats.memVertexBuffers, prev.memVertexBuffers.peak)),
      memTextures(stats.memTextures, std::max(stats.memTextures, prev.memTextures.peak)) {}

struct RunContext {
    RunContext() = default;

    GfxProbe activeGfxProbe;
    GfxProbe baselineGfxProbe;
    bool gfxProbeActive;
};

class TestRunnerMapObserver : public MapObserver {
public:
    TestRunnerMapObserver() : mapLoadFailure(false), finishRenderingMap(false), idle(false) {}

    void onDidFailLoadingMap(MapLoadError, const std::string&) override { mapLoadFailure = true; }

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

// static
gfx::HeadlessBackend::SwapBehaviour swapBehavior(MapMode mode) {
    return mode == MapMode::Continuous ? gfx::HeadlessBackend::SwapBehaviour::Flush
                                       : gfx::HeadlessBackend::SwapBehaviour::NoFlush;
}

std::string simpleDiff(const Value& result, const Value& expected) {
    std::vector<std::string> resultTokens{tokenize(toJSON(result, 2, false))};
    std::vector<std::string> expectedTokens{tokenize(toJSON(expected, 2, false))};
    std::size_t maxLength = std::max(resultTokens.size(), expectedTokens.size());
    std::ostringstream diff;

    diff << "<pre>" << std::endl;
    const auto flush =
        [](const std::vector<std::string>& vec, std::size_t pos, std::ostringstream& out, std::string separator) {
            for (std::size_t j = pos; j < vec.size(); ++j) {
                out << separator << vec[j] << std::endl;
            }
        };

    for (std::size_t i = 0; i < maxLength; ++i) {
        if (resultTokens.size() <= i) {
            flush(expectedTokens, i, diff, "-");
            break;
        }

        if (expectedTokens.size() <= i) {
            flush(resultTokens, i, diff, "+");
            break;
        }

        if (!deepEqual(resultTokens[i], expectedTokens[i])) {
            diff << "<b>"
                 << "-" << expectedTokens[i] << "</b>" << std::endl;
            diff << "<b>"
                 << "+" << resultTokens[i] << "</b>" << std::endl;
        } else {
            diff << resultTokens[i] << std::endl;
        }
    }
    diff << "</pre>" << std::endl;
    return diff.str();
}

TestRunner::TestRunner(Manifest manifest_, UpdateResults updateResults_)
    : manifest(std::move(manifest_)), updateResults(updateResults_) {}

const Manifest& TestRunner::getManifest() const {
    return manifest;
}

void TestRunner::doShuffle(uint32_t seed) {
    manifest.doShuffle(seed);
}

bool TestRunner::checkQueryTestResults(mbgl::PremultipliedImage&& actualImage,
                                       std::vector<mbgl::Feature>&& features,
                                       TestMetadata& metadata) {
    const std::string& base = metadata.paths.defaultExpectations();
    const std::vector<mbgl::filesystem::path>& expectations = metadata.paths.expectations;

    metadata.actual = mbgl::encodePNG(actualImage);

    if (actualImage.size.isEmpty()) {
        metadata.errorMessage = "Invalid size for actual image";
        return false;
    }

    metadata.actualJson = toJSON(features, 2, false);

    if (metadata.actualJson.empty()) {
        metadata.errorMessage = "Invalid size for actual JSON";
        return false;
    }

#if !TEST_READ_ONLY
    if (updateResults == UpdateResults::PLATFORM) {
        mbgl::filesystem::create_directories(expectations.back());
        mbgl::util::write_file(expectations.back().string() + "/expected.json", metadata.actualJson);
        return true;
    } else if (updateResults == UpdateResults::DEFAULT) {
        mbgl::util::write_file(base + "/expected.json", metadata.actualJson);
        return true;
    }

    mbgl::util::write_file(base + "/actual.json", metadata.actualJson);
#endif

    std::vector<std::string> expectedJsonPaths;
    mbgl::filesystem::path expectedMetricsPath;
    for (auto rit = expectations.rbegin(); rit != expectations.rend(); ++rit) {
        if (mbgl::filesystem::exists(*rit)) {
            expectedJsonPaths = readExpectedJSONEntries(*rit);
            if (!expectedJsonPaths.empty()) break;
        }
    }

    if (expectedJsonPaths.empty()) {
        metadata.errorMessage = "Failed to find expectations for: " + metadata.paths.stylePath.string();
        return false;
    }

    for (const auto& entry : expectedJsonPaths) {
        auto maybeExpectedJson = readJson(entry);
        if (maybeExpectedJson.is<mbgl::JSDocument>()) {
            auto& expected = maybeExpectedJson.get<mbgl::JSDocument>();

            mbgl::JSDocument actual;
            actual.Parse<0>(metadata.actualJson);
            if (actual.HasParseError()) {
                metadata.errorMessage = "Error parsing actual JSON for: " + metadata.paths.stylePath.string();
                return false;
            }

            auto actualVal = mapbox::geojson::convert<mapbox::geojson::value>(actual);
            auto expectedVal = mapbox::geojson::convert<mapbox::geojson::value>(expected);
            bool equal = deepEqual(actualVal, expectedVal);

            metadata.difference = !equal;
            if (equal) {
                metadata.diff = "Match";
            } else {
                metadata.diff = simpleDiff(actualVal, expectedVal);
            }
        } else {
            metadata.errorMessage = "Failed to load expected JSON " + entry;
            return false;
        }
    }

    return true;
}

bool TestRunner::checkRenderTestResults(mbgl::PremultipliedImage&& actualImage, TestMetadata& metadata) {
    const std::string& base = metadata.paths.defaultExpectations();
    const std::vector<mbgl::filesystem::path>& expectations = metadata.paths.expectations;

    if (metadata.outputsImage) {
        metadata.actual = mbgl::encodePNG(actualImage);

        if (actualImage.size.isEmpty()) {
            metadata.errorMessage = "Invalid size for actual image";
            return false;
        }

#if !TEST_READ_ONLY
        if (updateResults == UpdateResults::PLATFORM) {
            mbgl::filesystem::create_directories(expectations.back());
            mbgl::util::write_file(expectations.back().string() + "/expected.png", mbgl::encodePNG(actualImage));
            return true;
        } else if (updateResults == UpdateResults::DEFAULT) {
            mbgl::util::write_file(base + "/expected.png", mbgl::encodePNG(actualImage));
            return true;
        }

        mbgl::util::write_file(base + "/actual.png", metadata.actual);
#endif

        mbgl::PremultipliedImage expectedImage{actualImage.size};
        mbgl::PremultipliedImage imageDiff{actualImage.size};

        double pixels = 0.0;
        std::vector<std::string> expectedImagesPaths;
        for (auto rit = expectations.rbegin(); rit != expectations.rend(); ++rit) {
            if (mbgl::filesystem::exists(*rit)) {
                expectedImagesPaths = readExpectedImageEntries(*rit);
                if (!expectedImagesPaths.empty()) break;
            }
        }

        if (expectedImagesPaths.empty()) {
            metadata.errorMessage = "Failed to find expectations for: " + metadata.paths.stylePath.string();
            return false;
        }

        for (const auto& entry : expectedImagesPaths) {
            mbgl::optional<std::string> maybeExpectedImage = mbgl::util::readFile(entry);
            if (!maybeExpectedImage) {
                metadata.errorMessage = "Failed to load expected image " + entry;
                return false;
            }

            metadata.expected = *maybeExpectedImage;

            expectedImage = mbgl::decodeImage(*maybeExpectedImage);

            pixels = // implicitly converting from uint64_t
                mapbox::pixelmatch(actualImage.data.get(),
                                   expectedImage.data.get(),
                                   expectedImage.size.width,
                                   expectedImage.size.height,
                                   imageDiff.data.get(),
                                   0.1285); // Defined in GL JS

            metadata.diff = mbgl::encodePNG(imageDiff);

#if !TEST_READ_ONLY
            mbgl::util::write_file(base + "/diff.png", metadata.diff);
#endif

            metadata.difference = pixels / expectedImage.size.area();
            if (metadata.difference <= metadata.allowed) {
                break;
            }
        }
    }
    return true;
}

bool TestRunner::checkProbingResults(TestMetadata& metadata) {
    if (metadata.metrics.isEmpty()) return true;
    const std::vector<mbgl::filesystem::path>& expectedMetrics = metadata.paths.expectedMetrics;
#if !TEST_READ_ONLY
    if (updateResults == UpdateResults::METRICS) {
        mbgl::filesystem::create_directories(expectedMetrics.back());
        mbgl::util::write_file(expectedMetrics.back().string() + "/metrics.json", serializeMetrics(metadata.metrics));
        return true;
    }
#endif

    // Check the possible paths in reverse order, so that the default path with the test style will only be checked in
    // the very end.
    std::vector<std::string> expectedMetricsPaths;
    for (auto rit = expectedMetrics.rbegin(); rit != expectedMetrics.rend(); ++rit) {
        if (mbgl::filesystem::exists(*rit)) {
            expectedMetricsPaths = readExpectedMetricEntries(*rit);
            if (!expectedMetricsPaths.empty()) break;
        }
    }

    // In case no metrics.json is found, skip assigning the expectedMetrics to metadata, otherwise, take the first found
    // metrics.
    for (const auto& entry : expectedMetricsPaths) {
        auto maybeExpectedMetrics = readExpectedMetrics(entry);
        if (maybeExpectedMetrics.isEmpty()) {
            metadata.errorMessage = "Failed to load expected metrics " + entry;
            return false;
        }
        metadata.expectedMetrics = maybeExpectedMetrics;
        break;
    }

    if (metadata.expectedMetrics.isEmpty()) {
        metadata.errorMessage = "Failed to find metric expectations for: " + metadata.paths.stylePath.string();
        return false;
    }
    // Check file size metrics.
    auto checkFileSize = [](TestMetadata& metadata) -> bool {
        if (metadata.metrics.fileSize.empty()) return true;
        bool passed = true;
        for (const auto& expected : metadata.expectedMetrics.fileSize) {
            auto actual = metadata.metrics.fileSize.find(expected.first);
            if (actual == metadata.metrics.fileSize.end()) {
                metadata.errorMessage = "Failed to find fileSize probe: " + expected.first;
                return false;
            }
            if (actual->second.path != expected.second.path) {
                std::stringstream ss;
                ss << "Comparing different files at probe \"" << expected.first << "\": " << actual->second.path
                   << ", expected is " << expected.second.path << ".";
                metadata.errorMessage = ss.str();

                return false;
            }

            auto result = checkValue(expected.second.size, actual->second.size, actual->second.tolerance);
            if (!std::get<bool>(result)) {
                std::stringstream ss;
                ss << "File size does not match at probe \"" << expected.first << "\" for file \""
                   << expected.second.path << "\": " << actual->second.size << ", expected is " << expected.second.size
                   << ".";

                metadata.errorMessage += metadata.errorMessage.empty() ? ss.str() : "\n" + ss.str();
                passed = false;
                continue;
            }
        }
        return passed;
    };
    auto checkMemory = [](TestMetadata& metadata) -> bool {
        if (metadata.metrics.memory.empty()) return true;
#if !defined(SANITIZE)
        // Check memory metrics.
        for (const auto& expected : metadata.expectedMetrics.memory) {
            auto actual = metadata.metrics.memory.find(expected.first);
            if (actual == metadata.metrics.memory.end()) {
                metadata.errorMessage = "Failed to find memory probe: " + expected.first;
                return false;
            }
            bool passed{false};
            float delta{0.0f};
            std::stringstream errorStream;
            std::tie(passed, delta) = MemoryProbe::checkPeak(expected.second, actual->second);
            if (!passed) {
                errorStream << "Allocated memory peak size at probe \"" << expected.first << "\" is "
                            << actual->second.peak << " bytes, expected is " << expected.second.peak << "±" << delta
                            << " bytes.";
            }

            std::tie(passed, delta) = MemoryProbe::checkAllocations(expected.second, actual->second);
            if (!passed) {
                errorStream << "Number of allocations at probe \"" << expected.first << "\" is "
                            << actual->second.allocations << ", expected is " << expected.second.allocations << "±"
                            << std::round(delta) << " allocations.";
            }

            metadata.errorMessage = errorStream.str();
            if (!metadata.errorMessage.empty()) return false;
        }

#endif // !defined(SANITIZE)
        return true;
    };

    // Check network metrics.
    auto checkNetwork = [](TestMetadata& metadata) -> bool {
        if (metadata.metrics.network.empty()) return true;
#if !defined(SANITIZE)
        for (const auto& expected : metadata.expectedMetrics.network) {
            auto actual = metadata.metrics.network.find(expected.first);
            if (actual == metadata.metrics.network.end()) {
                metadata.errorMessage = "Failed to find network probe: " + expected.first;
                return false;
            }
            bool failed = false;
            if (actual->second.requests != expected.second.requests) {
                std::stringstream ss;
                ss << "Number of requests at probe \"" << expected.first << "\" is " << actual->second.requests
                   << ", expected is " << expected.second.requests << ". ";

                metadata.errorMessage = ss.str();
                failed = true;
            }
            if (actual->second.transferred != expected.second.transferred) {
                std::stringstream ss;
                ss << "Transferred data at probe \"" << expected.first << "\" is " << actual->second.transferred
                   << " bytes, expected is " << expected.second.transferred << " bytes.";

                metadata.errorMessage += ss.str();
                failed = true;
            }
            if (failed) {
                return false;
            }
        }
#endif // !defined(SANITIZE)
        return true;
    };
    // Check fps metrics
    auto checkFps = [](TestMetadata& metadata) -> bool {
        if (metadata.metrics.fps.empty()) return true;
        for (const auto& expected : metadata.expectedMetrics.fps) {
            auto actual = metadata.metrics.fps.find(expected.first);
            if (actual == metadata.metrics.fps.end()) {
                metadata.errorMessage = "Failed to find fps probe: " + expected.first;
                return false;
            }
            auto result = checkValue(expected.second.average, actual->second.average, expected.second.tolerance);
            if (!std::get<bool>(result)) {
                std::stringstream ss;
                ss << "Average fps at probe \"" << expected.first << "\" is " << actual->second.average
                   << ", expected to be " << expected.second.average << " with tolerance of "
                   << expected.second.tolerance;
                metadata.errorMessage = ss.str();
                return false;
            }
            result = checkValue(expected.second.minOnePc, actual->second.minOnePc, expected.second.tolerance);
            if (!std::get<bool>(result)) {
                std::stringstream ss;
                ss << "Minimum(1%) fps at probe \"" << expected.first << "\" is " << actual->second.minOnePc
                   << ", expected to be " << expected.second.minOnePc << " with tolerance of "
                   << expected.second.tolerance;
                metadata.errorMessage = ss.str();
                return false;
            }
        }
        return true;
    };
    // Check gfx metrics
    auto checkGfx = [](TestMetadata& metadata) -> bool {
        if (metadata.metrics.gfx.empty()) return true;
        for (const auto& expected : metadata.expectedMetrics.gfx) {
            auto actual = metadata.metrics.gfx.find(expected.first);
            if (actual == metadata.metrics.gfx.end()) {
                metadata.errorMessage = "Failed to find gfx probe: " + expected.first;
                return false;
            }

            const auto& probeName = expected.first;
            const auto& expectedValue = expected.second;
            const auto& actualValue = actual->second;
            bool failed = false;

            if (expectedValue.numDrawCalls != actualValue.numDrawCalls) {
                std::stringstream ss;
                if (!metadata.errorMessage.empty()) ss << std::endl;
                ss << "Number of draw calls at probe\"" << probeName << "\" is " << actualValue.numDrawCalls
                   << ", expected is " << expectedValue.numDrawCalls;
                metadata.errorMessage += ss.str();
                failed = true;
            }

            if (expectedValue.numTextures != actualValue.numTextures) {
                std::stringstream ss;
                if (!metadata.errorMessage.empty()) ss << std::endl;
                ss << "Number of textures at probe \"" << probeName << "\" is " << actualValue.numTextures
                   << ", expected is " << expectedValue.numTextures;
                metadata.errorMessage += ss.str();
                failed = true;
            }

            if (expectedValue.numBuffers != actualValue.numBuffers) {
                std::stringstream ss;
                if (!metadata.errorMessage.empty()) ss << std::endl;
                ss << "Number of vertex and index buffers at probe \"" << probeName << "\" is "
                   << actualValue.numBuffers << ", expected is " << expectedValue.numBuffers;
                metadata.errorMessage += ss.str();
                failed = true;
            }

            if (expectedValue.numFrameBuffers != actualValue.numFrameBuffers) {
                std::stringstream ss;
                if (!metadata.errorMessage.empty()) ss << std::endl;
                ss << "Number of frame buffers at probe \"" << probeName << "\" is " << actualValue.numFrameBuffers
                   << ", expected is " << expectedValue.numFrameBuffers;
                metadata.errorMessage += ss.str();
                failed = true;
            }

            if (expectedValue.memTextures.peak != actualValue.memTextures.peak) {
                std::stringstream ss;
                if (!metadata.errorMessage.empty()) ss << std::endl;
                ss << "Allocated texture memory peak size at probe \"" << probeName << "\" is "
                   << actualValue.memTextures.peak << " bytes, expected is " << expectedValue.memTextures.peak
                   << " bytes";
                metadata.errorMessage += ss.str();
                failed = true;
            }

            if (expectedValue.memIndexBuffers.peak != actualValue.memIndexBuffers.peak) {
                std::stringstream ss;
                if (!metadata.errorMessage.empty()) ss << std::endl;
                ss << "Allocated index buffer memory peak size at probe \"" << probeName << "\" is "
                   << actualValue.memIndexBuffers.peak << " bytes, expected is " << expectedValue.memIndexBuffers.peak
                   << " bytes";
                metadata.errorMessage += ss.str();
                failed = true;
            }

            if (expectedValue.memVertexBuffers.peak != actualValue.memVertexBuffers.peak) {
                std::stringstream ss;
                if (!metadata.errorMessage.empty()) ss << std::endl;
                ss << "Allocated vertex buffer memory peak size at probe \"" << probeName << "\" is "
                   << actualValue.memVertexBuffers.peak << " bytes, expected is " << expectedValue.memVertexBuffers.peak
                   << " bytes";
                metadata.errorMessage += ss.str();
                failed = true;
            }

            if (failed) return false;
        }
        return true;
    };

    return checkFileSize(metadata) && checkMemory(metadata) && checkNetwork(metadata) && checkFps(metadata) &&
           checkGfx(metadata);
}

bool TestRunner::runOperations(const std::string& key, TestMetadata& metadata, RunContext& ctx) {
    if (!metadata.document.HasMember("metadata") || !metadata.document["metadata"].HasMember("test") ||
        !metadata.document["metadata"]["test"].HasMember("operations")) {
        return true;
    }
    assert(metadata.document["metadata"]["test"]["operations"].IsArray());

    const auto& operationsArray = metadata.document["metadata"]["test"]["operations"].GetArray();
    if (operationsArray.Empty()) {
        return true;
    }

    const auto& operationIt = operationsArray.Begin();
    assert(operationIt->IsArray());

    const auto& operationArray = operationIt->GetArray();
    assert(operationArray.Size() >= 1u);

    auto& frontend = maps[key]->frontend;
    auto& map = maps[key]->map;
    auto& observer = maps[key]->observer;

    if (operationArray[0].GetString() == waitOp) {
        // wait
        try {
            frontend.render(map);
        } catch (const std::exception&) {
            return false;
        }
    } else if (operationArray[0].GetString() == sleepOp) {
        // sleep
        mbgl::util::Timer sleepTimer;
        bool sleeping = true;

        mbgl::Duration duration = mbgl::Seconds(3);
        if (operationArray.Size() >= 2u) {
            duration = mbgl::Milliseconds(operationArray[1].GetUint());
        }

        sleepTimer.start(duration, mbgl::Duration::zero(), [&]() {
            sleeping = false;
        });

        while (sleeping) {
            mbgl::util::RunLoop::Get()->runOnce();
        }
    } else if (operationArray[0].GetString() == addImageOp || operationArray[0].GetString() == updateImageOp) {
        // addImage | updateImage
        assert(operationArray.Size() >= 3u);

        float pixelRatio = 1.0f;
        bool sdf = false;

        if (operationArray.Size() == 4u) {
            assert(operationArray[3].IsObject());
            const auto& imageOptions = operationArray[3].GetObject();
            if (imageOptions.HasMember("pixelRatio")) {
                pixelRatio = imageOptions["pixelRatio"].GetFloat();
            }
            if (imageOptions.HasMember("sdf")) {
                sdf = imageOptions["sdf"].GetBool();
            }
        }

        std::string imageName = operationArray[1].GetString();
        imageName.erase(std::remove(imageName.begin(), imageName.end(), '"'), imageName.end());

        std::string imagePath = operationArray[2].GetString();
        imagePath.erase(std::remove(imagePath.begin(), imagePath.end(), '"'), imagePath.end());

        const mbgl::filesystem::path filePath = (mbgl::filesystem::path(manifest.getAssetPath()) / imagePath);

        mbgl::optional<std::string> maybeImage = mbgl::util::readFile(filePath.string());
        if (!maybeImage) {
            metadata.errorMessage = std::string("Failed to load expected image ") + filePath.string();
            return false;
        }

        map.getStyle().addImage(std::make_unique<mbgl::style::Image>(imageName, mbgl::decodeImage(*maybeImage), pixelRatio, sdf));
    } else if (operationArray[0].GetString() == removeImageOp) {
        // removeImage
        assert(operationArray.Size() >= 2u);
        assert(operationArray[1].IsString());

        const std::string imageName { operationArray[1].GetString(), operationArray[1].GetStringLength() };
        map.getStyle().removeImage(imageName);
    } else if (operationArray[0].GetString() == setStyleOp) {
        // setStyle
        assert(operationArray.Size() >= 2u);
        if (operationArray[1].IsString()) {
            std::string stylePath = manifest.localizeURL(operationArray[1].GetString());
            auto maybeStyle = readJson(stylePath);
            if (maybeStyle.is<mbgl::JSDocument>()) {
                auto& style = maybeStyle.get<mbgl::JSDocument>();
                manifest.localizeStyleURLs((mbgl::JSValue&)style, style);
                map.getStyle().loadJSON(serializeJsonValue(style));
            }
        } else {
            manifest.localizeStyleURLs(operationArray[1], metadata.document);
            map.getStyle().loadJSON(serializeJsonValue(operationArray[1]));
        }
    } else if (operationArray[0].GetString() == setCenterOp) {
        // setCenter
        assert(operationArray.Size() >= 2u);
        assert(operationArray[1].IsArray());

        const auto& centerArray = operationArray[1].GetArray();
        assert(centerArray.Size() == 2u);

        map.jumpTo(mbgl::CameraOptions().withCenter(mbgl::LatLng(centerArray[1].GetDouble(), centerArray[0].GetDouble())));
    } else if (operationArray[0].GetString() == setZoomOp) {
        // setZoom
        assert(operationArray.Size() >= 2u);
        assert(operationArray[1].IsNumber());
        map.jumpTo(mbgl::CameraOptions().withZoom(operationArray[1].GetDouble()));
    } else if (operationArray[0].GetString() == setBearingOp) {
        // setBearing
        assert(operationArray.Size() >= 2u);
        assert(operationArray[1].IsNumber());
        map.jumpTo(mbgl::CameraOptions().withBearing(operationArray[1].GetDouble()));
    } else if (operationArray[0].GetString() == setPitchOp) {
        // setPitch
        assert(operationArray.Size() >= 2u);
        assert(operationArray[1].IsNumber());
        map.jumpTo(mbgl::CameraOptions().withPitch(operationArray[1].GetDouble()));
    } else if (operationArray[0].GetString() == setFilterOp) {
        // setFilter
        assert(operationArray.Size() >= 3u);
        assert(operationArray[1].IsString());

        const std::string layerName { operationArray[1].GetString(), operationArray[1].GetStringLength() };

        mbgl::style::conversion::Error error;
        auto converted = mbgl::style::conversion::convert<mbgl::style::Filter>(operationArray[2], error);
        if (!converted) {
            metadata.errorMessage = std::string("Unable to convert filter: ")  + error.message;
            return false;
        } else {
            auto layer = map.getStyle().getLayer(layerName);
            if (!layer) {
                metadata.errorMessage = std::string("Layer not found: ")  + layerName;
                return false;
            } else {
                layer->setFilter(std::move(*converted));
            }
        }
    } else if (operationArray[0].GetString() == setLayerZoomRangeOp) {
        // setLayerZoomRange
        assert(operationArray.Size() >= 4u);
        assert(operationArray[1].IsString());
        assert(operationArray[2].IsNumber());
        assert(operationArray[3].IsNumber());

        const std::string layerName { operationArray[1].GetString(), operationArray[1].GetStringLength() };
        auto layer = map.getStyle().getLayer(layerName);
        if (!layer) {
            metadata.errorMessage = std::string("Layer not found: ")  + layerName;
            return false;
        } else {
            layer->setMinZoom(operationArray[2].GetFloat());
            layer->setMaxZoom(operationArray[3].GetFloat());
        }
    } else if (operationArray[0].GetString() == setLightOp) {
        // setLight
        assert(operationArray.Size() >= 2u);
        assert(operationArray[1].IsObject());

        mbgl::style::conversion::Error error;
        auto converted = mbgl::style::conversion::convert<mbgl::style::Light>(operationArray[1], error);
        if (!converted) {
            metadata.errorMessage = std::string("Unable to convert light: ")  + error.message;
            return false;
        } else {
            map.getStyle().setLight(std::make_unique<mbgl::style::Light>(std::move(*converted)));
        }
    } else if (operationArray[0].GetString() == addLayerOp) {
        // addLayer
        assert(operationArray.Size() >= 2u);
        assert(operationArray[1].IsObject());

        mbgl::style::conversion::Error error;
        auto converted = mbgl::style::conversion::convert<std::unique_ptr<mbgl::style::Layer>>(operationArray[1], error);
        if (!converted) {
            metadata.errorMessage = std::string("Unable to convert layer: ")  + error.message;
            return false;
        } else {
            map.getStyle().addLayer(std::move(*converted));
        }
    } else if (operationArray[0].GetString() == removeLayerOp) {
        // removeLayer
        assert(operationArray.Size() >= 2u);
        assert(operationArray[1].IsString());
        map.getStyle().removeLayer(operationArray[1].GetString());
    } else if (operationArray[0].GetString() == addSourceOp) {
        // addSource
        assert(operationArray.Size() >= 3u);
        assert(operationArray[1].IsString());
        assert(operationArray[2].IsObject());

        manifest.localizeSourceURLs(operationArray[2], metadata.document);

        mbgl::style::conversion::Error error;
        auto converted = mbgl::style::conversion::convert<std::unique_ptr<mbgl::style::Source>>(operationArray[2], error, operationArray[1].GetString());
        if (!converted) {
            metadata.errorMessage = std::string("Unable to convert source: ")  + error.message;
            return false;
        } else {
            map.getStyle().addSource(std::move(*converted));
        }
    } else if (operationArray[0].GetString() == removeSourceOp) {
        // removeSource
        assert(operationArray.Size() >= 2u);
        assert(operationArray[1].IsString());
        map.getStyle().removeSource(operationArray[1].GetString());
    } else if (operationArray[0].GetString() == setLayoutPropertyOp ||
               operationArray[0].GetString() == setPaintPropertyOp) {
        // set{Paint|Layout}Property
        assert(operationArray.Size() >= 4u);
        assert(operationArray[1].IsString());
        assert(operationArray[2].IsString());

        const std::string layerName { operationArray[1].GetString(), operationArray[1].GetStringLength() };
        const std::string propertyName { operationArray[2].GetString(), operationArray[2].GetStringLength() };

        auto layer = map.getStyle().getLayer(layerName);
        if (!layer) {
            metadata.errorMessage = std::string("Layer not found: ")  + layerName;
            return false;
        } else {
            const mbgl::JSValue* propertyValue = &operationArray[3];
            layer->setProperty(propertyName, propertyValue);
        }
    } else if (operationArray[0].GetString() == fileSizeProbeOp) {
        // probeFileSize
        assert(operationArray.Size() >= 4u);
        assert(operationArray[1].IsString());
        assert(operationArray[2].IsString());
        assert(operationArray[3].IsNumber());

        std::string mark = std::string(operationArray[1].GetString(), operationArray[1].GetStringLength());
        std::string path = std::string(operationArray[2].GetString(), operationArray[2].GetStringLength());
        assert(!path.empty());

        float tolerance = operationArray[3].GetDouble();
        mbgl::filesystem::path filePath(path);

        bool compressed = false;
        if (operationArray.Size() == 5) {
            assert(operationArray[4].IsString());
            assert(std::string(operationArray[4].GetString(), operationArray[4].GetStringLength()) == "compressed");
            compressed = true;
        }

        if (!filePath.is_absolute()) {
            filePath = metadata.paths.defaultExpectations() / filePath;
        }

        if (mbgl::filesystem::exists(filePath)) {
            size_t size = 0;
            if (compressed) {
                size = mbgl::util::compress(*mbgl::util::readFile(filePath)).size();
            } else {
                size = mbgl::filesystem::file_size(filePath);
            }

            metadata.metrics.fileSize.emplace(std::piecewise_construct,
                                              std::forward_as_tuple(std::move(mark)),
                                              std::forward_as_tuple(std::move(path), size, tolerance));
        } else {
            metadata.errorMessage = std::string("File not found: ") + path;
            return false;
        }
    } else if (operationArray[0].GetString() == memoryProbeStartOp) {
        // probeMemoryStart
        assert(!AllocationIndex::isActive());
        AllocationIndex::setActive(true);
    } else if (operationArray[0].GetString() == memoryProbeOp) {
        // probeMemory
        assert(AllocationIndex::isActive());
        assert(operationArray.Size() >= 2u);
        assert(operationArray[1].IsString());
        std::string mark = std::string(operationArray[1].GetString(), operationArray[1].GetStringLength());

        auto emplaced = metadata.metrics.memory.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(std::move(mark)),
            std::forward_as_tuple(AllocationIndex::getAllocatedSizePeak(), AllocationIndex::getAllocationsCount()));
        assert(emplaced.second);
        if (operationArray.Size() >= 3u) {
            assert(operationArray[2].IsNumber());
            emplaced.first->second.tolerance = float(operationArray[2].GetDouble());
        }
    } else if (operationArray[0].GetString() == memoryProbeEndOp) {
        // probeMemoryEnd
        assert(AllocationIndex::isActive());
        AllocationIndex::setActive(false);
        AllocationIndex::reset();
    } else if (operationArray[0].GetString() == networkProbeStartOp) {
        // probeNetworkStart
        assert(!ProxyFileSource::isTrackingActive());
        ProxyFileSource::setTrackingActive(true);
    } else if (operationArray[0].GetString() == networkProbeOp) {
        // probeNetwork
        assert(ProxyFileSource::isTrackingActive());
        assert(operationArray.Size() >= 2u);
        assert(operationArray[1].IsString());
        std::string mark = std::string(operationArray[1].GetString(), operationArray[1].GetStringLength());

        metadata.metrics.network.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(std::move(mark)),
            std::forward_as_tuple(ProxyFileSource::getRequestCount(), ProxyFileSource::getTransferredSize()));
    } else if (operationArray[0].GetString() == networkProbeEndOp) {
        // probeNetworkEnd
        assert(ProxyFileSource::isTrackingActive());
        ProxyFileSource::setTrackingActive(false);
    } else if (operationArray[0].GetString() == setFeatureStateOp) {
        // setFeatureState
        assert(operationArray.Size() >= 3u);
        assert(operationArray[1].IsObject());
        assert(operationArray[2].IsObject());

        using namespace mbgl;
        using namespace mbgl::style::conversion;

        std::string sourceID;
        mbgl::optional<std::string> sourceLayer;
        std::string featureID;
        std::string stateKey;
        Value stateValue;
        bool valueParsed = false;
        FeatureState parsedState;

        const auto& featureOptions = operationArray[1].GetObject();
        if (featureOptions.HasMember("source")) {
            sourceID = featureOptions["source"].GetString();
        }
        if (featureOptions.HasMember("sourceLayer")) {
            sourceLayer = {featureOptions["sourceLayer"].GetString()};
        }
        if (featureOptions.HasMember("id")) {
            if (featureOptions["id"].IsString()) {
                featureID = featureOptions["id"].GetString();
            } else if (featureOptions["id"].IsNumber()) {
                featureID = mbgl::util::toString(featureOptions["id"].GetUint64());
            }
        }
        const JSValue* state = &operationArray[2];

        const std::function<optional<Error>(const std::string&, const Convertible&)> convertFn =
            [&](const std::string& k, const Convertible& v) -> optional<Error> {
            optional<Value> value = toValue(v);
            if (value) {
                stateValue = std::move(*value);
                valueParsed = true;
            } else if (isArray(v)) {
                std::vector<Value> array;
                std::size_t length = arrayLength(v);
                array.reserve(length);
                for (size_t i = 0; i < length; ++i) {
                    optional<Value> arrayVal = toValue(arrayMember(v, i));
                    if (arrayVal) {
                        array.emplace_back(*arrayVal);
                    }
                }
                std::unordered_map<std::string, Value> result;
                result[k] = std::move(array);
                stateValue = std::move(result);
                valueParsed = true;
                return nullopt;

            } else if (isObject(v)) {
                eachMember(v, convertFn);
            }

            if (!valueParsed) {
                metadata.errorMessage = std::string("Could not get feature state value, state key: ") + k;
                return nullopt;
            }
            stateKey = k;
            parsedState[stateKey] = stateValue;
            return nullopt;
        };

        eachMember(state, convertFn);

        try {
            frontend.render(map);
        } catch (const std::exception&) {
            return false;
        }
        frontend.getRenderer()->setFeatureState(sourceID, sourceLayer, featureID, parsedState);
    } else if (operationArray[0].GetString() == getFeatureStateOp) {
        // getFeatureState
        assert(operationArray.Size() >= 2u);
        assert(operationArray[1].IsObject());

        std::string sourceID;
        mbgl::optional<std::string> sourceLayer;
        std::string featureID;

        const auto& featureOptions = operationArray[1].GetObject();
        if (featureOptions.HasMember("source")) {
            sourceID = featureOptions["source"].GetString();
        }
        if (featureOptions.HasMember("sourceLayer")) {
            sourceLayer = {featureOptions["sourceLayer"].GetString()};
        }
        if (featureOptions.HasMember("id")) {
            if (featureOptions["id"].IsString()) {
                featureID = featureOptions["id"].GetString();
            } else if (featureOptions["id"].IsNumber()) {
                featureID = mbgl::util::toString(featureOptions["id"].GetUint64());
            }
        }

        try {
            frontend.render(map);
        } catch (const std::exception&) {
            return false;
        }
        mbgl::FeatureState state;
        frontend.getRenderer()->getFeatureState(state, sourceID, sourceLayer, featureID);
    } else if (operationArray[0].GetString() == removeFeatureStateOp) {
        // removeFeatureState
        assert(operationArray.Size() >= 2u);
        assert(operationArray[1].IsObject());

        std::string sourceID;
        mbgl::optional<std::string> sourceLayer;
        std::string featureID;
        mbgl::optional<std::string> stateKey;

        const auto& featureOptions = operationArray[1].GetObject();
        if (featureOptions.HasMember("source")) {
            sourceID = featureOptions["source"].GetString();
        }
        if (featureOptions.HasMember("sourceLayer")) {
            sourceLayer = {featureOptions["sourceLayer"].GetString()};
        }
        if (featureOptions.HasMember("id")) {
            if (featureOptions["id"].IsString()) {
                featureID = featureOptions["id"].GetString();
            } else if (featureOptions["id"].IsNumber()) {
                featureID = mbgl::util::toString(featureOptions["id"].GetUint64());
            }
        }

        if (operationArray.Size() >= 3u) {
            assert(operationArray[2].IsString());
            stateKey = {operationArray[2].GetString()};
        }

        try {
            frontend.render(map);
        } catch (const std::exception&) {
            return false;
        }
        frontend.getRenderer()->removeFeatureState(sourceID, sourceLayer, featureID, stateKey);
    } else if (operationArray[0].GetString() == panGestureOp) {
        // benchmarkPanGesture
        assert(operationArray.Size() >= 4u);
        assert(operationArray[1].IsString()); // identifier
        assert(operationArray[2].IsNumber()); // duration
        assert(operationArray[3].IsArray());  // start [lat, lng, zoom]
        assert(operationArray[4].IsArray());  // end [lat, lng, zoom]

        if (metadata.mapMode != mbgl::MapMode::Continuous) {
            metadata.errorMessage = "Map mode must be Continous for " + panGestureOp + " operation";
            return false;
        }

        std::string mark = operationArray[1].GetString();
        int duration = operationArray[2].GetFloat();
        LatLng startPos, endPos;
        double startZoom, endZoom;
        std::vector<float> samples;

        auto parsePosition = [](auto arr) -> std::tuple<LatLng, double> {
            assert(arr.Size() >= 3);
            return {{arr[1].GetDouble(), arr[0].GetDouble()}, arr[2].GetDouble()};
        };

        std::tie(startPos, startZoom) = parsePosition(operationArray[3].GetArray());
        std::tie(endPos, endZoom) = parsePosition(operationArray[4].GetArray());

        // Jump to the starting point of the segment and make sure there's something to render
        map.jumpTo(mbgl::CameraOptions().withCenter(startPos).withZoom(startZoom));

        observer->reset();
        while (!observer->finishRenderingMap) {
            frontend.renderOnce(map);
        }

        if (observer->mapLoadFailure) return false;

        size_t frames = 0;
        float totalTime = 0.0;
        bool transitionFinished = false;

        mbgl::AnimationOptions animationOptions(mbgl::Milliseconds(duration * 1000));
        animationOptions.minZoom = util::min(startZoom, endZoom);
        animationOptions.transitionFinishFn = [&]() { transitionFinished = true; };

        map.flyTo(mbgl::CameraOptions().withCenter(endPos).withZoom(endZoom), animationOptions);

        while (!transitionFinished) {
            frames++;
            frontend.renderOnce(map);
            float frameTime = (float)frontend.getFrameTime();
            totalTime += frameTime;

            samples.push_back(frameTime);
        }

        float averageFps = totalTime > 0.0 ? frames / totalTime : 0.0;
        float minFrameTime = 0.0;

        // Use 1% of the longest frames to compute the minimum fps
        std::sort(samples.begin(), samples.end());

        int sampleCount = util::max(1, (int)samples.size() / 100);
        for (auto it = samples.rbegin(); it != samples.rbegin() + sampleCount; it++) minFrameTime += *it;

        float minOnePcFps = sampleCount / minFrameTime;

        metadata.metrics.fps.insert({std::move(mark), {averageFps, minOnePcFps, 0.0f}});

    } else if (operationArray[0].GetString() == gfxProbeStartOp) {
        // probeGFXStart
        assert(!ctx.gfxProbeActive);
        ctx.gfxProbeActive = true;
        ctx.baselineGfxProbe = ctx.activeGfxProbe;
    } else if (operationArray[0].GetString() == gfxProbeEndOp) {
        // probeGFXEnd
        assert(ctx.gfxProbeActive);
        ctx.gfxProbeActive = false;
    } else if (operationArray[0].GetString() == gfxProbeOp) {
        // probeGFX
        assert(operationArray.Size() >= 2u);
        assert(operationArray[1].IsString());

        std::string mark = std::string(operationArray[1].GetString(), operationArray[1].GetStringLength());

        // Render the map and fetch rendering stats
        gfx::RenderingStats stats;

        try {
            stats = frontend.render(map).stats;
        } catch (const std::exception&) {
            return false;
        }

        ctx.activeGfxProbe = GfxProbe(stats, ctx.activeGfxProbe);

        // Compare memory allocations to the baseline probe
        GfxProbe metricProbe = ctx.activeGfxProbe;
        metricProbe.memIndexBuffers.peak -= ctx.baselineGfxProbe.memIndexBuffers.peak;
        metricProbe.memVertexBuffers.peak -= ctx.baselineGfxProbe.memVertexBuffers.peak;
        metricProbe.memTextures.peak -= ctx.baselineGfxProbe.memTextures.peak;
        metadata.metrics.gfx.insert({mark, metricProbe});

    } else {
        metadata.errorMessage = std::string("Unsupported operation: ") + operationArray[0].GetString();
        return false;
    }

    operationsArray.Erase(operationIt);
    return runOperations(key, metadata, ctx);
}

TestRunner::Impl::Impl(const TestMetadata& metadata)
    : observer(std::make_unique<TestRunnerMapObserver>()),
      frontend(metadata.size, metadata.pixelRatio, swapBehavior(metadata.mapMode)),
      map(frontend,
          *observer.get(),
          mbgl::MapOptions()
              .withMapMode(metadata.mapMode)
              .withSize(metadata.size)
              .withPixelRatio(metadata.pixelRatio)
              .withCrossSourceCollisions(metadata.crossSourceCollisions),
          mbgl::ResourceOptions().withCacheOnlyRequestsSupport(false)) {}

TestRunner::Impl::~Impl() {}

bool TestRunner::run(TestMetadata& metadata) {
    AllocationIndex::setActive(false);
    AllocationIndex::reset();
    ProxyFileSource::setTrackingActive(false);

    RunContext ctx{};
    // Run 'begin' probes provided via command line arguments.
    if (!runInjectedProbesBegin(metadata, ctx)) {
        return false;
    }

    std::string key = mbgl::util::toString(uint32_t(metadata.mapMode))
        + "/" + mbgl::util::toString(metadata.pixelRatio)
        + "/" + mbgl::util::toString(uint32_t(metadata.crossSourceCollisions));

    if (maps.find(key) == maps.end()) {
        maps[key] = std::make_unique<TestRunner::Impl>(metadata);
    }

    auto& frontend = maps[key]->frontend;
    auto& map = maps[key]->map;

    frontend.setSize(metadata.size);
    map.setSize(metadata.size);

    map.setProjectionMode(mbgl::ProjectionMode().withAxonometric(metadata.axonometric).withXSkew(metadata.xSkew).withYSkew(metadata.ySkew));
    map.setDebug(metadata.debug);

    map.getStyle().loadJSON(serializeJsonValue(metadata.document));
    map.jumpTo(map.getStyle().getDefaultCamera());

    if (!runOperations(key, metadata, ctx)) return false;

    HeadlessFrontend::RenderResult result;
    try {
        if (metadata.outputsImage) result = frontend.render(map);
    } catch (const std::exception&) {
        return false;
    }

    // Run 'end' probes provided via command line arguments
    if (!runInjectedProbesEnd(metadata, ctx, result.stats)) {
        return false;
    }

    if (metadata.renderTest) {
        return checkRenderTestResults(std::move(result.image), metadata) && checkProbingResults(metadata);
    } else {
        std::vector<mbgl::Feature> features;
        assert(metadata.document["metadata"]["test"]["queryGeometry"].IsArray());
        if (metadata.document["metadata"]["test"]["queryGeometry"][0].IsNumber() &&
            metadata.document["metadata"]["test"]["queryGeometry"][1].IsNumber()) {
            features = frontend.getRenderer()->queryRenderedFeatures(metadata.queryGeometry, metadata.queryOptions);
        } else {
            features = frontend.getRenderer()->queryRenderedFeatures(metadata.queryGeometryBox, metadata.queryOptions);
        }
        return checkQueryTestResults(std::move(result.image), std::move(features), metadata);
    }
}

using InjectedProbeMap = std::map<std::string, std::function<void(TestMetadata&, RunContext&)>>;
bool runInjectedProbe(TestMetadata& metadata,
                      const std::set<std::string>& probes,
                      RunContext& ctx,
                      const InjectedProbeMap& probeMap) {
    for (const auto& probe : probes) {
        auto it = probeMap.find(probe);
        if (it == probeMap.end()) {
            metadata.errorMessage = std::string("Unsupported operation: ") + probe;
            return false;
        }
        it->second(metadata, ctx);
    }
    return true;
}

bool TestRunner::runInjectedProbesBegin(TestMetadata& metadata, RunContext& ctx) {
    const std::string mark = " - default - start";
    static const InjectedProbeMap beginInjectedProbeMap = {
        {// Injected memory probe begin
         memoryProbeOp,
         [&mark](TestMetadata& metadata, RunContext&) {
             assert(!AllocationIndex::isActive());
             AllocationIndex::setActive(true);
             metadata.metrics.memory.emplace(std::piecewise_construct,
                                             std::forward_as_tuple(memoryProbeOp + mark),
                                             std::forward_as_tuple(AllocationIndex::getAllocatedSizePeak(),
                                                                   AllocationIndex::getAllocationsCount()));
         }},
        {// Injected gfx probe begin
         gfxProbeOp,
         [](TestMetadata&, RunContext& ctx) {
             assert(!ctx.gfxProbeActive);
             ctx.gfxProbeActive = true;
             ctx.baselineGfxProbe = ctx.activeGfxProbe;
         }},
        {// Injected network probe begin
         networkProbeOp,
         [&mark](TestMetadata& metadata, RunContext&) {
             assert(!ProxyFileSource::isTrackingActive());
             ProxyFileSource::setTrackingActive(true);
             metadata.metrics.network.emplace(
                 std::piecewise_construct,
                 std::forward_as_tuple(networkProbeOp + mark),
                 std::forward_as_tuple(ProxyFileSource::getRequestCount(), ProxyFileSource::getTransferredSize()));
         }}};

    return runInjectedProbe(metadata, manifest.getProbes(), ctx, beginInjectedProbeMap);
}

bool TestRunner::runInjectedProbesEnd(TestMetadata& metadata,
                                      RunContext& ctx,
                                      mbgl::gfx::RenderingStats stats) {
    const std::string mark = " - default - end";
    static const InjectedProbeMap endInjectedProbeMap = {
        {// Injected memory probe end
         memoryProbeOp,
         [&mark](TestMetadata& metadata, RunContext&) {
             assert(AllocationIndex::isActive());
             auto emplaced =
                 metadata.metrics.memory.emplace(std::piecewise_construct,
                                                 std::forward_as_tuple(memoryProbeOp + mark),
                                                 std::forward_as_tuple(AllocationIndex::getAllocatedSizePeak(),
                                                                       AllocationIndex::getAllocationsCount()));
             assert(emplaced.second);
             // TODO: Improve tolerance handling for memory tests.
             emplaced.first->second.tolerance = 0.2f;
             AllocationIndex::setActive(false);
             AllocationIndex::reset();
         }},
        {// Injected gfx probe end
         gfxProbeOp,
         [&mark, &stats](TestMetadata& metadata, RunContext& ctx) {
             assert(ctx.gfxProbeActive);
             ctx.activeGfxProbe = GfxProbe(stats, ctx.activeGfxProbe);

             // Compare memory allocations to the baseline probe
             GfxProbe metricProbe = ctx.activeGfxProbe;
             metricProbe.memIndexBuffers.peak -= ctx.baselineGfxProbe.memIndexBuffers.peak;
             metricProbe.memVertexBuffers.peak -= ctx.baselineGfxProbe.memVertexBuffers.peak;
             metricProbe.memTextures.peak -= ctx.baselineGfxProbe.memTextures.peak;
             metadata.metrics.gfx.insert({gfxProbeOp + mark, metricProbe});

             ctx.gfxProbeActive = false;
         }},
        {// Injected network probe end
         networkProbeOp,
         [&mark](TestMetadata& metadata, RunContext&) {
             assert(ProxyFileSource::isTrackingActive());
             metadata.metrics.network.emplace(
                 std::piecewise_construct,
                 std::forward_as_tuple(networkProbeOp + mark),
                 std::forward_as_tuple(ProxyFileSource::getRequestCount(), ProxyFileSource::getTransferredSize()));
             ProxyFileSource::setTrackingActive(false);
         }}};

    return runInjectedProbe(metadata, manifest.getProbes(), ctx, endInjectedProbeMap);
}

void TestRunner::reset() {
    maps.clear();
}
