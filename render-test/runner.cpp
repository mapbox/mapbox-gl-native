#include <mbgl/map/camera.hpp>
#include <mbgl/map/map_observer.hpp>
#include <mbgl/style/conversion/filter.hpp>
#include <mbgl/style/conversion/layer.hpp>
#include <mbgl/style/conversion/light.hpp>
#include <mbgl/style/conversion/source.hpp>
#include <mbgl/style/image.hpp>
#include <mbgl/style/layer.hpp>
#include <mbgl/style/light.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/rapidjson_conversion.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/timer.hpp>
#include <mbgl/renderer/renderer.hpp>

#include <mapbox/pixelmatch.hpp>

#include "allocation_index.hpp"
#include "metadata.hpp"
#include "parser.hpp"
#include "runner.hpp"

#include <algorithm>
#include <cassert>
#include <regex>
#include <utility>
#include <sstream>

// static 
const std::string& TestRunner::getBasePath() {
    const static std::string result =
        std::string(TEST_RUNNER_ROOT_PATH).append("/mapbox-gl-js/test/integration/render-tests");
    return result;
}

// static
const std::vector<std::string>& TestRunner::getPlatformExpectationsPaths() {
    // TODO: Populate from command line.
    const static std::vector<std::string> result {
        std::string(TEST_RUNNER_ROOT_PATH).append("/render-test/expected")
    };
    return result;
}

bool TestRunner::checkResults(mbgl::PremultipliedImage&& actualImage, TestMetadata& metadata) {
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
            expectedImagesPaths = readExpectedEntries(*rit);
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
    // Check memory metrics.
    for (const auto& expected : metadata.expectedMetrics.memory) {
        auto actual = metadata.metrics.memory.find(expected.first);
        if (actual == metadata.metrics.memory.end()) {
            metadata.errorMessage = "Failed to find memory probe: " + expected.first;
            return false;
        }
        if (actual->second.peak > expected.second.peak) {
            std::stringstream ss;
            ss << "Allocated memory peak size at probe \"" << expected.first << "\" is "
               << actual->second.peak << " bytes, expected is " << expected.second.peak << " bytes.";

            metadata.errorMessage = ss.str();
            return false;
        }

        if (actual->second.allocations > expected.second.allocations) {
            std::stringstream ss;
            ss << "Number of allocations at probe \"" << expected.first << "\" is "
               << actual->second.allocations << ", expected is " << expected.second.allocations << ".";

            metadata.errorMessage = ss.str();
            return false;
        }
    }

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
    static const std::string setFilterOp("setFilter");
    static const std::string setLayerZoomRangeOp("setLayerZoomRange");
    static const std::string setLightOp("setLight");
    static const std::string addLayerOp("addLayer");
    static const std::string removeLayerOp("removeLayer");
    static const std::string addSourceOp("addSource");
    static const std::string removeSourceOp("removeSource");
    static const std::string setPaintPropertyOp("setPaintProperty");
    static const std::string setLayoutPropertyOp("setLayoutProperty");
    static const std::string memoryProbeOp("probeMemory");
    static const std::string memoryProbeStartOp("probeMemoryStart");
    static const std::string memoryProbeEndOp("probeMemoryEnd");
    static const std::string setFeatureStateOp("setFeatureState");
    static const std::string getFeatureStateOp("getFeatureState");
    static const std::string removeFeatureStateOp("removeFeatureState");

    // wait
    if (operationArray[0].GetString() == waitOp) {
        try {
            frontend.render(map);
        } catch (const std::exception&) {
            return false;
        }
    // sleep
    } else if (operationArray[0].GetString() == sleepOp) {
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

    // addImage | updateImage
    } else if (operationArray[0].GetString() == addImageOp || operationArray[0].GetString() == updateImageOp) {
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

    // removeImage
    } else if (operationArray[0].GetString() == removeImageOp) {
        assert(operationArray.Size() >= 2u);
        assert(operationArray[1].IsString());

        const std::string imageName { operationArray[1].GetString(), operationArray[1].GetStringLength() };
        map.getStyle().removeImage(imageName);

    // setStyle
    } else if (operationArray[0].GetString() == setStyleOp) {
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

    // setCenter
    } else if (operationArray[0].GetString() == setCenterOp) {
        assert(operationArray.Size() >= 2u);
        assert(operationArray[1].IsArray());

        const auto& centerArray = operationArray[1].GetArray();
        assert(centerArray.Size() == 2u);

        map.jumpTo(mbgl::CameraOptions().withCenter(mbgl::LatLng(centerArray[1].GetDouble(), centerArray[0].GetDouble())));

    // setZoom
    } else if (operationArray[0].GetString() == setZoomOp) {
        assert(operationArray.Size() >= 2u);
        assert(operationArray[1].IsNumber());
        map.jumpTo(mbgl::CameraOptions().withZoom(operationArray[1].GetDouble()));

    // setBearing
    } else if (operationArray[0].GetString() == setBearingOp) {
        assert(operationArray.Size() >= 2u);
        assert(operationArray[1].IsNumber());
        map.jumpTo(mbgl::CameraOptions().withBearing(operationArray[1].GetDouble()));

    // setFilter
    } else if (operationArray[0].GetString() == setFilterOp) {
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

    // setLayerZoomRange
    } else if (operationArray[0].GetString() == setLayerZoomRangeOp) {
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

    // setLight
    } else if (operationArray[0].GetString() == setLightOp) {
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

    // addLayer
    } else if (operationArray[0].GetString() == addLayerOp) {
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

    // removeLayer
    } else if (operationArray[0].GetString() == removeLayerOp) {
        assert(operationArray.Size() >= 2u);
        assert(operationArray[1].IsString());
        map.getStyle().removeLayer(operationArray[1].GetString());

    // addSource
    } else if (operationArray[0].GetString() == addSourceOp) {
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

    // removeSource
    } else if (operationArray[0].GetString() == removeSourceOp) {
        assert(operationArray.Size() >= 2u);
        assert(operationArray[1].IsString());
        map.getStyle().removeSource(operationArray[1].GetString());

    // setPaintProperty
    } else if (operationArray[0].GetString() == setPaintPropertyOp) {
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

    // setLayoutProperty
    } else if (operationArray[0].GetString() == setLayoutPropertyOp) {
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
    // probeMemoryStart
    } else if (operationArray[0].GetString() == memoryProbeStartOp) {
        assert(!AllocationIndex::isActive());
        AllocationIndex::setActive(true);
    // probeMemory
    } else if (operationArray[0].GetString() == memoryProbeOp) {
        assert(AllocationIndex::isActive());
        assert(operationArray.Size() >= 2u);
        assert(operationArray[1].IsString());
        std::string mark = std::string(operationArray[1].GetString(), operationArray[1].GetStringLength());

        metadata.metrics.memory.emplace(std::piecewise_construct,
                                        std::forward_as_tuple(std::move(mark)), 
                                        std::forward_as_tuple(AllocationIndex::getAllocatedSizePeak(), AllocationIndex::getAllocationsCount()));
    // probeMemoryEnd
    } else if (operationArray[0].GetString() == memoryProbeEndOp) {
        assert(AllocationIndex::isActive());
        AllocationIndex::setActive(false);
        AllocationIndex::reset();

    // setFeatureState
    } else if (operationArray[0].GetString() == setFeatureStateOp) {
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
            sourceLayer = { featureOptions["sourceLayer"].GetString() };
        }
        if (featureOptions.HasMember("id")) {
            featureID = featureOptions["id"].GetString();
        }
        const JSValue* state = &operationArray[2];

        const std::function<optional<Error> (const std::string&, const Convertible&)> convertFn = [&] (const std::string& k, const Convertible& v) -> optional<Error> {
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
                result[k]= std::move(array);
                stateValue = std::move(result);
                valueParsed = true;
                return {};

            } else if (isObject(v)) {
                eachMember(v, convertFn);
            }

            if (!valueParsed) {
                metadata.errorMessage = std::string("Could not get feature state value, state key: ")  + k;
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

    // getFeatureState
    } else if (operationArray[0].GetString() == getFeatureStateOp) {
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
            sourceLayer = { featureOptions["sourceLayer"].GetString() };
        }
        if (featureOptions.HasMember("id")) {
            featureID = featureOptions["id"].GetString();
        }

        try {
            frontend.render(map);
        } catch (const std::exception&) {
            return false;
        }
        mbgl::FeatureState state;
        frontend.getRenderer()->getFeatureState(state, sourceID, sourceLayer, featureID);

    // removeFeatureState
    } else if (operationArray[0].GetString() == removeFeatureStateOp) {
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
            sourceLayer = { featureOptions["sourceLayer"].GetString() };
        }
        if (featureOptions.HasMember("id")) {
            featureID = featureOptions["id"].GetString();
        }

        if (operationArray.Size() >= 3u) {
            assert(operationArray[2].IsString());
            stateKey = { operationArray[2].GetString() };
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

    return checkResults(std::move(image), metadata);
}

void TestRunner::reset() {
    maps.clear();
}
