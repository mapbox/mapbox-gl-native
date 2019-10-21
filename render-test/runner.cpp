#include <mbgl/map/camera.hpp>
#include <mbgl/map/map_observer.hpp>
#include <mbgl/renderer/renderer.hpp>
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

// static 
const std::string& TestRunner::getBasePath() {
    const static std::string result = std::string(TEST_RUNNER_ROOT_PATH).append("/mapbox-gl-js/test/integration");
    return result;
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
    if (getenv("UPDATE_PLATFORM")) {
        mbgl::filesystem::create_directories(expectations.back());
        mbgl::util::write_file(expectations.back().string() + "/expected.json", metadata.actualJson);
        return true;
    } else if (getenv("UPDATE_DEFAULT")) {
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

    metadata.actual = mbgl::encodePNG(actualImage);

    if (actualImage.size.isEmpty()) {
        metadata.errorMessage = "Invalid size for actual image";
        return false;
    }

#if !TEST_READ_ONLY
    if (getenv("UPDATE_PLATFORM")) {
        mbgl::filesystem::create_directories(expectations.back());
        mbgl::util::write_file(expectations.back().string() + "/expected.png", mbgl::encodePNG(actualImage));
        return true;
    } else if (getenv("UPDATE_DEFAULT")) {
        mbgl::util::write_file(base + "/expected.png", mbgl::encodePNG(actualImage));
        return true;
    } else if (getenv("UPDATE_METRICS")) {
        if (!metadata.metrics.isEmpty()) {
            mbgl::filesystem::create_directories(expectations.back());
            mbgl::util::write_file(expectations.back().string() + "/metrics.json", serializeMetrics(metadata.metrics));
            return true;
        }
    }

    mbgl::util::write_file(base + "/actual.png", metadata.actual);
#endif

    mbgl::PremultipliedImage expectedImage { actualImage.size };
    mbgl::PremultipliedImage imageDiff { actualImage.size };

    double pixels = 0.0;
    std::vector<std::string> expectedImagesPaths;
    mbgl::filesystem::path expectedMetricsPath;
    for (auto rit = expectations.rbegin(); rit!= expectations.rend(); ++rit) {
        if (mbgl::filesystem::exists(*rit)) {
            if (metadata.expectedMetrics.isEmpty()) {
                mbgl::filesystem::path maybeExpectedMetricsPath{ *rit };
                maybeExpectedMetricsPath.replace_filename("metrics.json");
                metadata.expectedMetrics = readExpectedMetrics(maybeExpectedMetricsPath);
            }
            expectedImagesPaths = readExpectedImageEntries(*rit);
            if (!expectedImagesPaths.empty()) break;
        }
    }

    if (expectedImagesPaths.empty()) {
        metadata.errorMessage = "Failed to find expectations for: " + metadata.paths.stylePath.string();
        return false;
    }
    
    for (const auto& entry: expectedImagesPaths) {
        mbgl::optional<std::string> maybeExpectedImage = mbgl::util::readFile(entry);
        if (!maybeExpectedImage) {
            metadata.errorMessage = "Failed to load expected image " + entry;
            return false;
        }

        metadata.expected = *maybeExpectedImage;

        expectedImage = mbgl::decodeImage(*maybeExpectedImage);

        pixels = // implicitly converting from uint64_t
            mapbox::pixelmatch(actualImage.data.get(), expectedImage.data.get(), expectedImage.size.width,
                               expectedImage.size.height, imageDiff.data.get(), 0.1285); // Defined in GL JS

        metadata.diff = mbgl::encodePNG(imageDiff);

#if !TEST_READ_ONLY
        mbgl::util::write_file(base + "/diff.png", metadata.diff);
#endif

        metadata.difference = pixels / expectedImage.size.area();
        if (metadata.difference <= metadata.allowed) {
            break;
        }
    }
    // Check file size metrics.
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
            ss << "File size does not match at probe \"" << expected.first << "\": " << actual->second.size
               << ", expected is " << expected.second.size << ".";

            metadata.errorMessage = ss.str();
            return false;
        }
    }
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
            errorStream << "Allocated memory peak size at probe \"" << expected.first << "\" is " << actual->second.peak
                        << " bytes, expected is " << expected.second.peak << "±" << delta << " bytes.";
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

    // Check network metrics.
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
}

bool TestRunner::runOperations(const std::string& key, TestMetadata& metadata) {
    if (!metadata.document.HasMember("metadata") ||
        !metadata.document["metadata"].HasMember("test") ||
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

        const mbgl::filesystem::path filePath(std::string(TEST_RUNNER_ROOT_PATH) + "/mapbox-gl-js/test/integration/" + imagePath);

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
            std::string stylePath = localizeURL(operationArray[1].GetString());
            auto maybeStyle = readJson(stylePath);
            if (maybeStyle.is<mbgl::JSDocument>()) {
                auto& style = maybeStyle.get<mbgl::JSDocument>();
                localizeStyleURLs((mbgl::JSValue&)style, style);
                map.getStyle().loadJSON(serializeJsonValue(style));
            }
        } else {
            localizeStyleURLs(operationArray[1], metadata.document);
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

        localizeSourceURLs(operationArray[2], metadata.document);

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
    } else if (operationArray[0].GetString() == setPaintPropertyOp) {
        // setPaintProperty
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
            layer->setPaintProperty(propertyName, propertyValue);
        }
    } else if (operationArray[0].GetString() == setLayoutPropertyOp) {
        // setLayoutProperty
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
            layer->setLayoutProperty(propertyName, propertyValue);
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

        if (!filePath.is_absolute()) {
            filePath = metadata.paths.defaultExpectations() / filePath;
        }

        if (mbgl::filesystem::exists(filePath)) {
            auto size = mbgl::filesystem::file_size(filePath);
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

    } else {
        metadata.errorMessage = std::string("Unsupported operation: ")  + operationArray[0].GetString();
        return false;
    }

    operationsArray.Erase(operationIt);
    return runOperations(key, metadata);
}

TestRunner::Impl::Impl(const TestMetadata& metadata)
    : frontend(metadata.size, metadata.pixelRatio),
      map(frontend,
          mbgl::MapObserver::nullObserver(),
          mbgl::MapOptions()
              .withMapMode(metadata.mapMode)
              .withSize(metadata.size)
              .withPixelRatio(metadata.pixelRatio)
              .withCrossSourceCollisions(metadata.crossSourceCollisions),
          mbgl::ResourceOptions().withCacheOnlyRequestsSupport(false)) {}

bool TestRunner::run(TestMetadata& metadata) {
    AllocationIndex::setActive(false);
    AllocationIndex::reset();
    ProxyFileSource::setTrackingActive(false);
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

    if (!runOperations(key, metadata)) {
        return false;
    }

    mbgl::PremultipliedImage image;
    try {
        image = frontend.render(map);
    } catch (const std::exception&) {
        return false;
    }

    if (metadata.renderTest) {
        return checkRenderTestResults(std::move(image), metadata);
    } else {
        std::vector<mbgl::Feature> features;
        assert(metadata.document["metadata"]["test"]["queryGeometry"].IsArray());
        if (metadata.document["metadata"]["test"]["queryGeometry"][0].IsNumber() &&
            metadata.document["metadata"]["test"]["queryGeometry"][1].IsNumber()) {
            features = frontend.getRenderer()->queryRenderedFeatures(metadata.queryGeometry, metadata.queryOptions);
        } else {
            features = frontend.getRenderer()->queryRenderedFeatures(metadata.queryGeometryBox, metadata.queryOptions);
        }
        return checkQueryTestResults(std::move(image), std::move(features), metadata);
    }
}

void TestRunner::reset() {
    maps.clear();
}
