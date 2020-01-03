#include "parser.hpp"

#include "allocation_index.hpp"
#include "file_source.hpp"
#include "filesystem.hpp"
#include "metadata.hpp"
#include "runner.hpp"

#include <mbgl/map/map.hpp>
#include <mbgl/renderer/renderer.hpp>
#include <mbgl/style/conversion/filter.hpp>
#include <mbgl/style/conversion/json.hpp>
#include <mbgl/style/conversion/layer.hpp>
#include <mbgl/style/conversion/light.hpp>
#include <mbgl/style/conversion/source.hpp>
#include <mbgl/style/layer.hpp>
#include <mbgl/style/light.hpp>
#include <mbgl/style/source.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/util/compression.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/util/rapidjson.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/timer.hpp>

#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <mapbox/geojson_impl.hpp>

#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/insert_linebreaks.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/archive/iterators/ostream_iterator.hpp>

#include <regex>
#include <sstream>

namespace {

const char* resultsStyle = R"HTML(
<style>
    body { font: 18px/1.2 -apple-system, BlinkMacSystemFont, "Helvetica Neue", Helvetica, Arial, sans-serif; padding: 10px; }
    h1 { font-size: 32px; margin-bottom: 0; }
    button { vertical-align: middle; }
    h2 { font-size: 24px; font-weight: normal; margin: 10px 0 10px; line-height: 1; }
    img { margin: 0 10px 10px 0; border: 1px dotted #ccc; }
    .stats { margin-top: 10px; }
    .test { border-bottom: 1px dotted #bbb; padding-bottom: 5px; }
    .tests { border-top: 1px dotted #bbb; margin-top: 10px; }
    .diff { color: #777; }
    .test p, .test pre { margin: 0 0 10px; }
    .test pre { font-size: 14px; }
    .label { color: white; font-size: 18px; padding: 2px 6px 3px; border-radius: 3px; margin-right: 3px; vertical-align: bottom; display: inline-block; }
    .hide { display: none; }
</style>
)HTML";

const char* resultsScript = R"HTML(
<script>
document.addEventListener('mouseover', handleHover);
document.addEventListener('mouseout', handleHover);

function handleHover(e) {
    var el = e.target;
    if (el.tagName === 'IMG' && el.dataset.altSrc) {
        var tmp = el.src;
        el.src = el.dataset.altSrc;
        el.dataset.altSrc = tmp;
    }
}

document.getElementById('toggle-passed').addEventListener('click', function (e) {
    for (const row of document.querySelectorAll('.test.passed')) {
        row.classList.toggle('hide');
    }
});
document.getElementById('toggle-ignored').addEventListener('click', function (e) {
    for (const row of document.querySelectorAll('.test.ignored')) {
        row.classList.toggle('hide');
    }
});
document.getElementById('toggle-sequence').addEventListener('click', function (e) {
    document.getElementById('test-sequence').classList.toggle('hide');
});
</script>
)HTML";

const char* resultsHeaderButtons = R"HTML(
    <button id='toggle-sequence'>Toggle test sequence</button>
    <button id='toggle-passed'>Toggle passed tests</button>
    <button id='toggle-ignored'>Toggle ignored tests</button>
</h1>
)HTML";

void writeJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const mbgl::Value& value) {
    value.match([&writer](const mbgl::NullValue&) { writer.Null(); },
                [&writer](bool b) { writer.Bool(b); },
                [&writer](uint64_t u) { writer.Uint64(u); },
                [&writer](int64_t i) { writer.Int64(i); },
                [&writer](double d) { d == std::floor(d) ? writer.Int64(d) : writer.Double(d); },
                [&writer](const std::string& s) { writer.String(s); },
                [&writer](const std::vector<mbgl::Value>& arr) {
                    writer.StartArray();
                    for (const auto& item : arr) {
                        writeJSON(writer, item);
                    }
                    writer.EndArray();
                },
                [&writer](const std::unordered_map<std::string, mbgl::Value>& obj) {
                    writer.StartObject();
                    std::map<std::string, mbgl::Value> sorted(obj.begin(), obj.end());
                    for (const auto& entry : sorted) {
                        writer.Key(entry.first.c_str());
                        writeJSON(writer, entry.second);
                    }
                    writer.EndObject();
                });
}

} // namespace

std::string toJSON(const mbgl::Value& value, unsigned indent, bool singleLine) {
    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    if (singleLine) {
        writer.SetFormatOptions(rapidjson::kFormatSingleLineArray);
    }
    writer.SetIndent(' ', indent);
    writeJSON(writer, value);
    return buffer.GetString();
}

std::string toJSON(const std::vector<mbgl::Feature>& features, unsigned indent, bool singleLine) {
    rapidjson::CrtAllocator allocator;
    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    if (singleLine) {
        writer.SetFormatOptions(rapidjson::kFormatSingleLineArray);
    }
    writer.SetIndent(' ', indent);
    writer.StartArray();
    for (const auto& feature : features) {
        mbgl::JSValue result(rapidjson::kObjectType);
        result.AddMember("type", "Feature", allocator);
        if (!feature.id.is<mbgl::NullValue>()) {
            result.AddMember(
                "id", mapbox::geojson::identifier::visit(feature.id, mapbox::geojson::to_value{allocator}), allocator);
        }
        result.AddMember("geometry", mapbox::geojson::convert(feature.geometry, allocator), allocator);
        result.AddMember("properties", mapbox::geojson::to_value{allocator}(feature.properties), allocator);
        result.AddMember("source", feature.source, allocator);
        if (!feature.sourceLayer.empty()) {
            result.AddMember("sourceLayer", feature.sourceLayer, allocator);
        }
        result.AddMember("state", mapbox::geojson::to_value{allocator}(feature.state), allocator);
        result.Accept(writer);
    }
    writer.EndArray();
    return buffer.GetString();
}

JSONReply readJson(const mbgl::filesystem::path& jsonPath) {
    auto maybeJSON = mbgl::util::readFile(jsonPath);
    if (!maybeJSON) {
        return { std::string("Unable to open file ") + jsonPath.string() };
    }

    mbgl::JSDocument document;
    document.Parse<0>(*maybeJSON);
    if (document.HasParseError()) {
        return { mbgl::formatJSONParseError(document) };
    }

    return {std::move(document)};
}

std::string serializeJsonValue(const mbgl::JSValue& value) {
    rapidjson::StringBuffer buffer;
    buffer.Clear();
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    value.Accept(writer);
    return buffer.GetString();
}

std::string serializeMetrics(const TestMetrics& metrics) {
    rapidjson::StringBuffer s;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(s);

    writer.StartObject();

    // Start file-size section.
    if (!metrics.fileSize.empty()) {
        writer.Key("file-size");
        writer.StartArray();
        for (const auto& fileSizeProbe : metrics.fileSize) {
            assert(!fileSizeProbe.first.empty());
            writer.StartArray();
            writer.String(fileSizeProbe.first.c_str());
            writer.String(fileSizeProbe.second.path);
            writer.Uint64(fileSizeProbe.second.size);
            writer.EndArray();
        }
        writer.EndArray();
    }

    // Start memory section.
    if (!metrics.memory.empty()) {
        writer.Key("memory");
        writer.StartArray();
        for (const auto& memoryProbe : metrics.memory) {
            assert(!memoryProbe.first.empty());
            writer.StartArray();
            writer.String(memoryProbe.first.c_str());
            writer.Uint64(memoryProbe.second.peak);
            writer.Uint64(memoryProbe.second.allocations);
            writer.EndArray();
        }
        writer.EndArray();
    }

    // Start network section
    if (!metrics.network.empty()) {
        writer.Key("network");
        writer.StartArray();
        for (const auto& networkProbe : metrics.network) {
            assert(!networkProbe.first.empty());
            writer.StartArray();
            writer.String(networkProbe.first.c_str());
            writer.Uint64(networkProbe.second.requests);
            writer.Uint64(networkProbe.second.transferred);
            writer.EndArray();
        }
        writer.EndArray();
    }

    if (!metrics.fps.empty()) {
        // Start fps section
        writer.Key("fps");
        writer.StartArray();
        for (const auto& fpsProbe : metrics.fps) {
            assert(!fpsProbe.first.empty());
            writer.StartArray();
            writer.String(fpsProbe.first.c_str());
            writer.Double(fpsProbe.second.average);
            writer.Double(fpsProbe.second.minOnePc);
            writer.EndArray();
        }
        writer.EndArray();
        // End fps section
    }

    if (!metrics.gfx.empty()) {
        // Start gfx section
        writer.Key("gfx");
        writer.StartArray();
        for (const auto& gfxProbe : metrics.gfx) {
            assert(!gfxProbe.first.empty());
            writer.StartArray();
            writer.String(gfxProbe.first.c_str());
            writer.Int(gfxProbe.second.numDrawCalls);
            writer.Int(gfxProbe.second.numTextures);
            writer.Int(gfxProbe.second.numBuffers);
            writer.Int(gfxProbe.second.numFrameBuffers);
            writer.StartArray();
            writer.Int(gfxProbe.second.memTextures.allocated);
            writer.Int(gfxProbe.second.memTextures.peak);
            writer.EndArray();
            writer.StartArray();
            writer.Int(gfxProbe.second.memIndexBuffers.allocated);
            writer.Int(gfxProbe.second.memIndexBuffers.peak);
            writer.EndArray();
            writer.StartArray();
            writer.Int(gfxProbe.second.memVertexBuffers.allocated);
            writer.Int(gfxProbe.second.memVertexBuffers.peak);
            writer.EndArray();
            writer.EndArray();
        }
        writer.EndArray();
        // End gfx section
    }

    writer.EndObject();

    return s.GetString();
}

namespace {
std::vector<std::string> readExpectedEntries(const std::regex& regex, const mbgl::filesystem::path& base) {
    std::vector<std::string> expectedEntries;
    for (const auto& entry : mbgl::filesystem::directory_iterator(base)) {
        if (entry.is_regular_file()) {
            const std::string path = entry.path().string();
            if (std::regex_match(path, regex)) {
                expectedEntries.emplace_back(std::move(path));
            }
        }
    }
    return expectedEntries;
}
} // namespace

std::vector<std::string> readExpectedImageEntries(const mbgl::filesystem::path& base) {
    static const std::regex regex(".*/expected.*.png");
    return readExpectedEntries(regex, base);
}

std::vector<std::string> readExpectedMetricEntries(const mbgl::filesystem::path& base) {
    static const std::regex regex(".*/metrics.json");
    return readExpectedEntries(regex, base);
}

std::vector<std::string> readExpectedJSONEntries(const mbgl::filesystem::path& base) {
    static const std::regex regex(".*/expected.*.json");
    return readExpectedEntries(regex, base);
}

TestMetrics readExpectedMetrics(const mbgl::filesystem::path& path) {
    TestMetrics result;

    auto maybeJson = readJson(path.string());
    if (!maybeJson.is<mbgl::JSDocument>()) { // NOLINT
        return result;
    }

    const auto& document = maybeJson.get<mbgl::JSDocument>();

    if (document.HasMember("file-size")) {
        const mbgl::JSValue& fileSizeValue = document["file-size"];
        assert(fileSizeValue.IsArray());
        for (auto& probeValue : fileSizeValue.GetArray()) {
            assert(probeValue.IsArray());
            assert(probeValue.Size() >= 3u);
            assert(probeValue[0].IsString());
            assert(probeValue[1].IsString());
            assert(probeValue[2].IsNumber());

            std::string mark{probeValue[0].GetString(), probeValue[0].GetStringLength()};
            assert(!mark.empty());

            std::string filePath{probeValue[1].GetString(), probeValue[1].GetStringLength()};
            assert(!filePath.empty());

            result.fileSize.emplace(std::piecewise_construct,
                                    std::forward_as_tuple(std::move(mark)),
                                    std::forward_as_tuple(std::move(filePath), probeValue[2].GetUint64(), 0.f));
        }
    }

    if (document.HasMember("memory")) {
        const mbgl::JSValue& memoryValue = document["memory"];
        assert(memoryValue.IsArray());
        for (auto& probeValue : memoryValue.GetArray()) {
            assert(probeValue.IsArray());
            assert(probeValue.Size() >= 3u);
            assert(probeValue[0].IsString());
            assert(probeValue[1].IsNumber());
            assert(probeValue[2].IsNumber());

            std::string mark{probeValue[0].GetString(), probeValue[0].GetStringLength()};
            assert(!mark.empty());
            result.memory.emplace(std::piecewise_construct,
                                  std::forward_as_tuple(std::move(mark)),
                                  std::forward_as_tuple(probeValue[1].GetUint64(), probeValue[2].GetUint64()));
        }
    }

    if (document.HasMember("network")) {
        const mbgl::JSValue& networkValue = document["network"];
        assert(networkValue.IsArray());
        for (auto& probeValue : networkValue.GetArray()) {
            assert(probeValue.IsArray());
            assert(probeValue.Size() >= 3u);
            assert(probeValue[0].IsString());
            assert(probeValue[1].IsNumber());
            assert(probeValue[2].IsNumber());

            std::string mark{probeValue[0].GetString(), probeValue[0].GetStringLength()};
            assert(!mark.empty());

            result.network.emplace(std::piecewise_construct,
                                   std::forward_as_tuple(std::move(mark)),
                                   std::forward_as_tuple(probeValue[1].GetUint64(), probeValue[2].GetUint64()));
        }
    }

    if (document.HasMember("fps")) {
        const mbgl::JSValue& fpsValue = document["fps"];
        assert(fpsValue.IsArray());
        for (auto& probeValue : fpsValue.GetArray()) {
            assert(probeValue.IsArray());
            assert(probeValue.Size() >= 4u);
            assert(probeValue[0].IsString());
            assert(probeValue[1].IsNumber()); // Average
            assert(probeValue[2].IsNumber()); // Minimum
            assert(probeValue[3].IsNumber()); // Tolerance
            const std::string mark{probeValue[0].GetString(), probeValue[0].GetStringLength()};
            assert(!mark.empty());
            result.fps.insert(
                {std::move(mark), {probeValue[1].GetFloat(), probeValue[2].GetFloat(), probeValue[3].GetFloat()}});
        }
    }

    if (document.HasMember("gfx")) {
        const mbgl::JSValue& gfxValue = document["gfx"];
        assert(gfxValue.IsArray());
        for (auto& probeValue : gfxValue.GetArray()) {
            assert(probeValue.IsArray());
            assert(probeValue.Size() >= 8u);
            assert(probeValue[0].IsString());
            assert(probeValue[1].IsInt());
            assert(probeValue[2].IsInt());
            assert(probeValue[3].IsInt());
            assert(probeValue[4].IsInt());
            assert(probeValue[5].IsArray());
            assert(probeValue[6].IsArray());
            assert(probeValue[7].IsArray());

            const std::string mark{probeValue[0].GetString(), probeValue[0].GetStringLength()};
            assert(!mark.empty());

            GfxProbe probe;
            probe.numDrawCalls = probeValue[1].GetInt();
            probe.numTextures = probeValue[2].GetInt();
            probe.numBuffers = probeValue[3].GetInt();
            probe.numFrameBuffers = probeValue[4].GetInt();
            probe.memTextures.allocated = probeValue[5].GetArray()[0].GetInt();
            probe.memTextures.peak = probeValue[5].GetArray()[1].GetInt();
            probe.memIndexBuffers.allocated = probeValue[6].GetArray()[0].GetInt();
            probe.memIndexBuffers.peak = probeValue[6].GetArray()[1].GetInt();
            probe.memVertexBuffers.allocated = probeValue[7].GetArray()[0].GetInt();
            probe.memVertexBuffers.peak = probeValue[7].GetArray()[1].GetInt();

            result.gfx.insert({mark, std::move(probe)});
        }
    }

    return result;
}

TestMetadata parseTestMetadata(const TestPaths& paths) {
    TestMetadata metadata;
    metadata.paths = paths;

    auto maybeJson = readJson(paths.stylePath.string());
    if (!maybeJson.is<mbgl::JSDocument>()) { // NOLINT
        metadata.errorMessage = std::string("Unable to parse: ") + metadata.paths.stylePath.string();
        return metadata;
    }

    metadata.document = std::move(maybeJson.get<mbgl::JSDocument>());
    if (!metadata.document.HasMember("metadata")) {
        mbgl::Log::Warning(mbgl::Event::ParseStyle, "Style has no 'metadata': %s", paths.stylePath.c_str());
        return metadata;
    }

    const mbgl::JSValue& metadataValue = metadata.document["metadata"];
    if (!metadataValue.HasMember("test")) {
        mbgl::Log::Warning(mbgl::Event::ParseStyle, "Style has no 'metadata.test': %s", paths.stylePath.c_str());
        return metadata;
    }

    const mbgl::JSValue& testValue = metadataValue["test"];

    if (testValue.HasMember("width")) {
        assert(testValue["width"].IsNumber());
        metadata.size.width = testValue["width"].GetInt();
    }

    if (testValue.HasMember("height")) {
        assert(testValue["height"].IsNumber());
        metadata.size.height = testValue["height"].GetInt();
    }

    if (testValue.HasMember("pixelRatio")) {
        assert(testValue["pixelRatio"].IsNumber());
        metadata.pixelRatio = testValue["pixelRatio"].GetFloat();
    }

    if (testValue.HasMember("allowed")) {
        assert(testValue["allowed"].IsNumber());
        metadata.allowed = testValue["allowed"].GetDouble();
    }

    if (testValue.HasMember("description")) {
        assert(testValue["description"].IsString());
        metadata.description =
            std::string{testValue["description"].GetString(), testValue["description"].GetStringLength()};
    }

    if (testValue.HasMember("mapMode")) {
        metadata.outputsImage = true;
        assert(testValue["mapMode"].IsString());
        std::string mapModeStr = testValue["mapMode"].GetString();
        if (mapModeStr == "tile") {
            metadata.mapMode = mbgl::MapMode::Tile;
            metadata.size = {uint32_t(mbgl::util::tileSize), uint32_t(mbgl::util::tileSize)};
        } else if (mapModeStr == "continuous") {
            metadata.mapMode = mbgl::MapMode::Continuous;
            metadata.outputsImage = false;
        } else if (mapModeStr == "static")
            metadata.mapMode = mbgl::MapMode::Static;
        else {
            mbgl::Log::Warning(
                mbgl::Event::ParseStyle, "Unknown map mode: %s. Falling back to static mode", mapModeStr.c_str());
            metadata.mapMode = mbgl::MapMode::Static;
        }
    }

    // Test operations handled in runner.cpp.

    if (testValue.HasMember("debug")) {
        metadata.debug |= mbgl::MapDebugOptions::TileBorders;
    }

    if (testValue.HasMember("collisionDebug")) {
        metadata.debug |= mbgl::MapDebugOptions::Collision;
    }

    if (testValue.HasMember("showOverdrawInspector")) {
        metadata.debug |= mbgl::MapDebugOptions::Overdraw;
    }

    if (testValue.HasMember("crossSourceCollisions")) {
        assert(testValue["crossSourceCollisions"].IsBool());
        metadata.crossSourceCollisions = testValue["crossSourceCollisions"].GetBool();
    }

    if (testValue.HasMember("axonometric")) {
        assert(testValue["axonometric"].IsBool());
        metadata.axonometric = testValue["axonometric"].GetBool();
    }

    if (testValue.HasMember("skew")) {
        assert(testValue["skew"].IsArray());
        metadata.xSkew = testValue["skew"][0].GetDouble();
        metadata.ySkew = testValue["skew"][1].GetDouble();
    }

    if (testValue.HasMember("queryGeometry")) {
        assert(testValue["queryGeometry"].IsArray());
        if (testValue["queryGeometry"][0].IsNumber() && testValue["queryGeometry"][1].IsNumber()) {
            metadata.queryGeometry.x = testValue["queryGeometry"][0].GetDouble();
            metadata.queryGeometry.y = testValue["queryGeometry"][1].GetDouble();
        } else if (testValue["queryGeometry"][0].IsArray() && testValue["queryGeometry"][1].IsArray()) {
            metadata.queryGeometryBox.min.x = testValue["queryGeometry"][0][0].GetDouble();
            metadata.queryGeometryBox.min.y = testValue["queryGeometry"][0][1].GetDouble();
            metadata.queryGeometryBox.max.x = testValue["queryGeometry"][1][0].GetDouble();
            metadata.queryGeometryBox.max.y = testValue["queryGeometry"][1][1].GetDouble();
        }
        metadata.renderTest = false;
    }

    if (testValue.HasMember("queryOptions")) {
        assert(testValue["queryOptions"].IsObject());

        if (testValue["queryOptions"].HasMember("layers")) {
            assert(testValue["queryOptions"]["layers"].IsArray());
            auto layersArray = testValue["queryOptions"]["layers"].GetArray();
            std::vector<std::string> layersVec;
            for (uint32_t i = 0; i < layersArray.Size(); i++) {
                layersVec.emplace_back(testValue["queryOptions"]["layers"][i].GetString());
            }
            metadata.queryOptions.layerIDs = layersVec;
        }

        using namespace mbgl::style;
        using namespace mbgl::style::conversion;
        if (testValue["queryOptions"].HasMember("filter")) {
            assert(testValue["queryOptions"]["filter"].IsArray());
            auto& filterVal = testValue["queryOptions"]["filter"];
            Error error;
            mbgl::optional<Filter> converted = convert<Filter>(filterVal, error);
            assert(converted);
            metadata.queryOptions.filter = std::move(*converted);
        }
    }

    // TODO: fadeDuration
    // TODO: addFakeCanvas

    return metadata;
}

namespace TestOperationNames {
const std::string waitOp("wait");
const std::string sleepOp("sleep");
const std::string addImageOp("addImage");
const std::string updateImageOp("updateImage");
const std::string removeImageOp("removeImage");
const std::string setStyleOp("setStyle");
const std::string setCenterOp("setCenter");
const std::string setZoomOp("setZoom");
const std::string setBearingOp("setBearing");
const std::string setPitchOp("setPitch");
const std::string setFilterOp("setFilter");
const std::string setLayerZoomRangeOp("setLayerZoomRange");
const std::string setLightOp("setLight");
const std::string addLayerOp("addLayer");
const std::string removeLayerOp("removeLayer");
const std::string addSourceOp("addSource");
const std::string removeSourceOp("removeSource");
const std::string setPaintPropertyOp("setPaintProperty");
const std::string setLayoutPropertyOp("setLayoutProperty");
const std::string fileSizeProbeOp("probeFileSize");
const std::string memoryProbeOp("probeMemory");
const std::string memoryProbeStartOp("probeMemoryStart");
const std::string memoryProbeEndOp("probeMemoryEnd");
const std::string networkProbeOp("probeNetwork");
const std::string networkProbeStartOp("probeNetworkStart");
const std::string networkProbeEndOp("probeNetworkEnd");
const std::string setFeatureStateOp("setFeatureState");
const std::string getFeatureStateOp("getFeatureState");
const std::string removeFeatureStateOp("removeFeatureState");
const std::string panGestureOp("panGesture");
const std::string gfxProbeOp("probeGFX");
const std::string gfxProbeStartOp("probeGFXStart");
const std::string gfxProbeEndOp("probeGFXEnd");
} // namespace TestOperationNames

using namespace TestOperationNames;

TestOperations parseTestOperations(TestMetadata& metadata) {
    TestOperations result;
    if (!metadata.document.HasMember("metadata") || !metadata.document["metadata"].HasMember("test") ||
        !metadata.document["metadata"]["test"].HasMember("operations")) {
        return result;
    }
    assert(metadata.document["metadata"]["test"]["operations"].IsArray());

    const auto& operationsArray = metadata.document["metadata"]["test"]["operations"].GetArray();
    if (operationsArray.Empty()) {
        return result;
    }
    for (auto& operationArray : operationsArray) {
        assert(operationArray.Size() >= 1u);

        if (operationArray[0].GetString() == waitOp) {
            // wait
            result.emplace_back([](TestContext& ctx) {
                try {
                    ctx.getFrontend().render(ctx.getMap());
                    return true;
                } catch (const std::exception&) {
                    return false;
                }
            });
        } else if (operationArray[0].GetString() == sleepOp) {
            // sleep
            mbgl::Duration duration = mbgl::Seconds(3);
            if (operationArray.Size() >= 2u) {
                duration = mbgl::Milliseconds(operationArray[1].GetUint());
            }
            result.emplace_back([duration](TestContext&) {
                mbgl::util::Timer sleepTimer;
                bool sleeping = true;

                sleepTimer.start(duration, mbgl::Duration::zero(), [&]() { sleeping = false; });

                while (sleeping) {
                    mbgl::util::RunLoop::Get()->runOnce();
                }
                return true;
            });

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

            result.emplace_back([imagePath, imageName, sdf, pixelRatio](TestContext& ctx) {
                mbgl::optional<std::string> maybeImage = mbgl::util::readFile(imagePath);
                if (!maybeImage) {
                    ctx.getMetadata().errorMessage = std::string("Failed to load expected image ") + imagePath;
                    return false;
                }

                ctx.getMap().getStyle().addImage(
                    std::make_unique<mbgl::style::Image>(imageName, mbgl::decodeImage(*maybeImage), pixelRatio, sdf));
                return true;
            });

        } else if (operationArray[0].GetString() == removeImageOp) {
            // removeImage
            assert(operationArray.Size() >= 2u);
            assert(operationArray[1].IsString());

            std::string imageName{operationArray[1].GetString(), operationArray[1].GetStringLength()};
            result.emplace_back([imageName](TestContext& ctx) {
                ctx.getMap().getStyle().removeImage(imageName);
                return true;
            });
        } else if (operationArray[0].GetString() == setStyleOp) {
            // setStyle
            assert(operationArray.Size() >= 2u);
            if (operationArray[1].IsString()) {
                std::string url = operationArray[1].GetString();

                result.emplace_back([url](TestContext& ctx) {
                    ctx.getMap().getStyle().loadURL(url);
                    return true;
                });
            } else {
                std::string json = serializeJsonValue(operationArray[1]);

                result.emplace_back([json](TestContext& ctx) {
                    ctx.getMap().getStyle().loadJSON(json);
                    return true;
                });
            }
        } else if (operationArray[0].GetString() == setCenterOp) {
            // setCenter
            assert(operationArray.Size() >= 2u);
            assert(operationArray[1].IsArray());

            const auto& centerArray = operationArray[1].GetArray();
            assert(centerArray.Size() == 2u);
            mbgl::LatLng center{centerArray[1].GetDouble(), centerArray[0].GetDouble()};
            result.emplace_back([center](TestContext& ctx) {
                ctx.getMap().jumpTo(mbgl::CameraOptions().withCenter(center));
                return true;
            });
        } else if (operationArray[0].GetString() == setZoomOp) {
            // setZoom
            assert(operationArray.Size() >= 2u);
            assert(operationArray[1].IsNumber());
            double zoom = operationArray[1].GetDouble();
            result.emplace_back([zoom](TestContext& ctx) {
                ctx.getMap().jumpTo(mbgl::CameraOptions().withZoom(zoom));
                return true;
            });
        } else if (operationArray[0].GetString() == setBearingOp) {
            // setBearing
            assert(operationArray.Size() >= 2u);
            assert(operationArray[1].IsNumber());
            double bearing = operationArray[1].GetDouble();
            result.emplace_back([bearing](TestContext& ctx) {
                ctx.getMap().jumpTo(mbgl::CameraOptions().withBearing(bearing));
                return true;
            });
        } else if (operationArray[0].GetString() == setPitchOp) {
            // setPitch
            assert(operationArray.Size() >= 2u);
            assert(operationArray[1].IsNumber());
            double pitch = operationArray[1].GetDouble();
            result.emplace_back([pitch](TestContext& ctx) {
                ctx.getMap().jumpTo(mbgl::CameraOptions().withPitch(pitch));
                return true;
            });
        } else if (operationArray[0].GetString() == setFilterOp) {
            // setFilter
            assert(operationArray.Size() >= 3u);
            assert(operationArray[1].IsString());

            std::string layerName{operationArray[1].GetString(), operationArray[1].GetStringLength()};
            mbgl::style::conversion::Error error;
            auto converted = mbgl::style::conversion::convert<mbgl::style::Filter>(operationArray[2], error);
            result.emplace_back([converted, layerName, error](TestContext& ctx) {
                if (!converted) {
                    ctx.getMetadata().errorMessage = std::string("Unable to convert filter: ") + error.message;
                    return false;
                }
                auto layer = ctx.getMap().getStyle().getLayer(layerName);
                if (!layer) {
                    ctx.getMetadata().errorMessage = std::string("Layer not found: ") + layerName;
                    return false;
                }
                layer->setFilter(std::move(*converted));
                return true;
            });

        } else if (operationArray[0].GetString() == setLayerZoomRangeOp) {
            // setLayerZoomRange
            assert(operationArray.Size() >= 4u);
            assert(operationArray[1].IsString());
            assert(operationArray[2].IsNumber());
            assert(operationArray[3].IsNumber());

            std::string layerName{operationArray[1].GetString(), operationArray[1].GetStringLength()};
            float minZoom = operationArray[2].GetFloat();
            float maxZoom = operationArray[3].GetFloat();
            result.emplace_back([layerName, minZoom, maxZoom](TestContext& ctx) {
                auto layer = ctx.getMap().getStyle().getLayer(layerName);
                if (!layer) {
                    ctx.getMetadata().errorMessage = std::string("Layer not found: ") + layerName;
                    return false;
                }
                layer->setMinZoom(minZoom);
                layer->setMaxZoom(maxZoom);
                return true;
            });
        } else if (operationArray[0].GetString() == setLightOp) {
            // setLight
            assert(operationArray.Size() >= 2u);
            assert(operationArray[1].IsObject());
            mbgl::style::conversion::Error error;
            auto converted = mbgl::style::conversion::convert<mbgl::style::Light>(operationArray[1], error);
            if (!converted) {
                metadata.errorMessage = std::string("Unable to convert light: ") + error.message;
                return {};
            }
            result.emplace_back([impl = converted->impl](TestContext& ctx) {
                ctx.getMap().getStyle().setLight(std::make_unique<mbgl::style::Light>(impl));
                return true;
            });
        } else if (operationArray[0].GetString() == addLayerOp) {
            // addLayer
            assert(operationArray.Size() >= 2u);
            assert(operationArray[1].IsObject());
            result.emplace_back([json = serializeJsonValue(operationArray[1])](TestContext& ctx) {
                mbgl::style::conversion::Error error;
                auto converted = mbgl::style::conversion::convertJSON<std::unique_ptr<mbgl::style::Layer>>(json, error);
                if (!converted) {
                    ctx.getMetadata().errorMessage = std::string("Unable to convert layer: ") + error.message;
                    return false;
                }
                ctx.getMap().getStyle().addLayer(std::move(*converted));
                return true;
            });
        } else if (operationArray[0].GetString() == removeLayerOp) {
            // removeLayer
            assert(operationArray.Size() >= 2u);
            assert(operationArray[1].IsString());
            std::string layerName = operationArray[1].GetString();
            result.emplace_back(
                [layerName](TestContext& ctx) { return bool(ctx.getMap().getStyle().removeLayer(layerName)); });
        } else if (operationArray[0].GetString() == addSourceOp) {
            // addSource
            assert(operationArray.Size() >= 3u);
            assert(operationArray[1].IsString());
            assert(operationArray[2].IsObject());
            std::string sourceName = operationArray[1].GetString();

            result.emplace_back([sourceName, json = serializeJsonValue(operationArray[2])](TestContext& ctx) {
                mbgl::style::conversion::Error error;
                auto converted =
                    mbgl::style::conversion::convertJSON<std::unique_ptr<mbgl::style::Source>>(json, error, sourceName);
                if (!converted) {
                    ctx.getMetadata().errorMessage = std::string("Unable to convert source: ") + error.message;
                    return false;
                }
                ctx.getMap().getStyle().addSource(std::move(*converted));
                return true;
            });
        } else if (operationArray[0].GetString() == removeSourceOp) {
            // removeSource
            assert(operationArray.Size() >= 2u);
            assert(operationArray[1].IsString());
            std::string sourceName = operationArray[1].GetString();
            result.emplace_back(
                [sourceName](TestContext& ctx) { return bool(ctx.getMap().getStyle().removeSource(sourceName)); });
        } else if (operationArray[0].GetString() == setLayoutPropertyOp ||
                   operationArray[0].GetString() == setPaintPropertyOp) {
            // set{Paint|Layout}Property
            assert(operationArray.Size() >= 4u);
            assert(operationArray[1].IsString());
            assert(operationArray[2].IsString());

            std::string layerName{operationArray[1].GetString(), operationArray[1].GetStringLength()};
            std::string propertyName{operationArray[2].GetString(), operationArray[2].GetStringLength()};
            result.emplace_back(
                [layerName, propertyName, json = serializeJsonValue(operationArray[3])](TestContext& ctx) {
                    auto layer = ctx.getMap().getStyle().getLayer(layerName);
                    if (!layer) {
                        ctx.getMetadata().errorMessage = std::string("Layer not found: ") + layerName;
                        return false;
                    }
                    mbgl::JSDocument d;
                    d.Parse(json.c_str(), json.length());
                    const mbgl::JSValue* propertyValue = &d;
                    layer->setProperty(propertyName, propertyValue);
                    return true;
                });
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
            result.emplace_back([filePath, path, mark, tolerance, compressed](TestContext& ctx) {
                if (!mbgl::filesystem::exists(filePath)) {
                    ctx.getMetadata().errorMessage = std::string("File not found: ") + path;
                    return false;
                }
                size_t size = 0;
                if (compressed) {
                    size = mbgl::util::compress(*mbgl::util::readFile(filePath)).size();
                } else {
                    size = mbgl::filesystem::file_size(filePath);
                }

                ctx.getMetadata().metrics.fileSize.emplace(std::piecewise_construct,
                                                           std::forward_as_tuple(std::move(mark)),
                                                           std::forward_as_tuple(std::move(path), size, tolerance));
                return true;
            });
        } else if (operationArray[0].GetString() == memoryProbeStartOp) {
            // probeMemoryStart
            result.emplace_back([](TestContext&) {
                assert(!AllocationIndex::isActive());
                AllocationIndex::setActive(true);
                return true;
            });
        } else if (operationArray[0].GetString() == memoryProbeOp) {
            // probeMemory
            assert(operationArray.Size() >= 2u);
            assert(operationArray[1].IsString());
            std::string mark = std::string(operationArray[1].GetString(), operationArray[1].GetStringLength());
            float tolerance = -1.0f;
            if (operationArray.Size() >= 3u) {
                assert(operationArray[2].IsNumber());
                tolerance = float(operationArray[2].GetDouble());
            }
            result.emplace_back([mark, tolerance](TestContext& ctx) {
                assert(AllocationIndex::isActive());
                auto emplaced = ctx.getMetadata().metrics.memory.emplace(
                    std::piecewise_construct,
                    std::forward_as_tuple(std::move(mark)),
                    std::forward_as_tuple(AllocationIndex::getAllocatedSizePeak(),
                                          AllocationIndex::getAllocationsCount()));
                if (tolerance >= 0.0f) emplaced.first->second.tolerance = tolerance;
                return true;
            });
        } else if (operationArray[0].GetString() == memoryProbeEndOp) {
            // probeMemoryEnd
            result.emplace_back([](TestContext&) {
                assert(AllocationIndex::isActive());
                AllocationIndex::setActive(false);
                AllocationIndex::reset();
                return true;
            });
        } else if (operationArray[0].GetString() == networkProbeStartOp) {
            // probeNetworkStart
            result.emplace_back([](TestContext&) {
                assert(!mbgl::ProxyFileSource::isTrackingActive());
                mbgl::ProxyFileSource::setTrackingActive(true);
                return true;
            });
        } else if (operationArray[0].GetString() == networkProbeOp) {
            // probeNetwork
            assert(operationArray.Size() >= 2u);
            assert(operationArray[1].IsString());
            std::string mark = std::string(operationArray[1].GetString(), operationArray[1].GetStringLength());
            result.emplace_back([mark](TestContext& ctx) {
                assert(mbgl::ProxyFileSource::isTrackingActive());
                ctx.getMetadata().metrics.network.emplace(
                    std::piecewise_construct,
                    std::forward_as_tuple(std::move(mark)),
                    std::forward_as_tuple(mbgl::ProxyFileSource::getRequestCount(),
                                          mbgl::ProxyFileSource::getTransferredSize()));
                return true;
            });
        } else if (operationArray[0].GetString() == networkProbeEndOp) {
            // probeNetworkEnd
            result.emplace_back([](TestContext&) {
                assert(mbgl::ProxyFileSource::isTrackingActive());
                mbgl::ProxyFileSource::setTrackingActive(false);
                return true;
            });
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
                    std::unordered_map<std::string, Value> values;
                    values[k] = std::move(array);
                    stateValue = std::move(values);
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
            result.emplace_back([sourceID, sourceLayer, featureID, parsedState](TestContext& ctx) {
                auto& frontend = ctx.getFrontend();
                try {
                    frontend.render(ctx.getMap());
                } catch (const std::exception&) {
                    return false;
                }
                frontend.getRenderer()->setFeatureState(sourceID, sourceLayer, featureID, parsedState);
                return true;
            });
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
            result.emplace_back([sourceID, sourceLayer, featureID](TestContext& ctx) {
                auto& frontend = ctx.getFrontend();
                try {
                    frontend.render(ctx.getMap());
                } catch (const std::exception&) {
                    return false;
                }
                mbgl::FeatureState state;
                frontend.getRenderer()->getFeatureState(state, sourceID, sourceLayer, featureID);
                return true;
            });
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

            result.emplace_back([sourceID, sourceLayer, featureID, stateKey](TestContext& ctx) {
                auto& frontend = ctx.getFrontend();
                try {
                    frontend.render(ctx.getMap());
                } catch (const std::exception&) {
                    return false;
                }
                frontend.getRenderer()->removeFeatureState(sourceID, sourceLayer, featureID, stateKey);
                return true;
            });
        } else if (operationArray[0].GetString() == panGestureOp) {
            // benchmarkPanGesture
            assert(operationArray.Size() >= 4u);
            assert(operationArray[1].IsString()); // identifier
            assert(operationArray[2].IsNumber()); // duration
            assert(operationArray[3].IsArray());  // start [lat, lng, zoom]
            assert(operationArray[4].IsArray());  // end [lat, lng, zoom]

            if (metadata.mapMode != mbgl::MapMode::Continuous) {
                metadata.errorMessage = "Map mode must be Continous for " + panGestureOp + " operation";
                return {};
            }

            std::string mark = operationArray[1].GetString();
            int duration = operationArray[2].GetFloat();
            mbgl::LatLng startPos, endPos;
            double startZoom, endZoom;

            auto parsePosition = [](auto arr) -> std::tuple<mbgl::LatLng, double> {
                assert(arr.Size() >= 3);
                return {{arr[1].GetDouble(), arr[0].GetDouble()}, arr[2].GetDouble()};
            };

            std::tie(startPos, startZoom) = parsePosition(operationArray[3].GetArray());
            std::tie(endPos, endZoom) = parsePosition(operationArray[4].GetArray());

            result.emplace_back([mark, duration, startPos, endPos, startZoom, endZoom](TestContext& ctx) {
                auto& map = ctx.getMap();
                auto& observer = ctx.getObserver();
                auto& frontend = ctx.getFrontend();
                std::vector<float> samples;
                // Jump to the starting point of the segment and make sure there's something to render
                map.jumpTo(mbgl::CameraOptions().withCenter(startPos).withZoom(startZoom));

                observer.reset();
                while (!observer.finishRenderingMap) {
                    frontend.renderOnce(map);
                }

                if (observer.mapLoadFailure) return false;

                size_t frames = 0;
                float totalTime = 0.0;
                bool transitionFinished = false;

                mbgl::AnimationOptions animationOptions(mbgl::Milliseconds(duration * 1000));
                animationOptions.minZoom = mbgl::util::min(startZoom, endZoom);
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

                int sampleCount = mbgl::util::max(1, (int)samples.size() / 100);
                for (auto it = samples.rbegin(); it != samples.rbegin() + sampleCount; it++) minFrameTime += *it;

                float minOnePcFps = sampleCount / minFrameTime;

                ctx.getMetadata().metrics.fps.insert({mark, {averageFps, minOnePcFps, 0.0f}});
                return true;
            });
        } else if (operationArray[0].GetString() == gfxProbeStartOp) {
            // probeGFXStart
            result.emplace_back([](TestContext& ctx) {
                assert(!ctx.gfxProbeActive);
                ctx.gfxProbeActive = true;
                ctx.baselineGfxProbe = ctx.activeGfxProbe;
                return true;
            });
        } else if (operationArray[0].GetString() == gfxProbeEndOp) {
            // probeGFXEnd
            result.emplace_back([](TestContext& ctx) {
                assert(ctx.gfxProbeActive);
                ctx.gfxProbeActive = false;
                return true;
            });
        } else if (operationArray[0].GetString() == gfxProbeOp) {
            // probeGFX
            assert(operationArray.Size() >= 2u);
            assert(operationArray[1].IsString());

            std::string mark = std::string(operationArray[1].GetString(), operationArray[1].GetStringLength());
            result.emplace_back([mark](TestContext& ctx) {
                auto& frontend = ctx.getFrontend();
                // Render the map and fetch rendering stats
                try {
                    mbgl::gfx::RenderingStats stats = frontend.render(ctx.getMap()).stats;
                    ctx.activeGfxProbe = GfxProbe(stats, ctx.activeGfxProbe);
                } catch (const std::exception&) {
                    return false;
                }
                // Compare memory allocations to the baseline probe
                GfxProbe metricProbe = ctx.activeGfxProbe;
                metricProbe.memIndexBuffers.peak -= ctx.baselineGfxProbe.memIndexBuffers.peak;
                metricProbe.memVertexBuffers.peak -= ctx.baselineGfxProbe.memVertexBuffers.peak;
                metricProbe.memTextures.peak -= ctx.baselineGfxProbe.memTextures.peak;
                ctx.getMetadata().metrics.gfx.insert({mark, metricProbe});
                return true;
            });
        } else {
            metadata.errorMessage = std::string("Unsupported operation: ") + operationArray[0].GetString();
            return {};
        }
    }

    return result;
}

// https://stackoverflow.com/questions/7053538/how-do-i-encode-a-string-to-base64-using-only-boost
std::string encodeBase64(const std::string& data) {
    using namespace boost::archive::iterators;
    using base64 = insert_linebreaks<base64_from_binary<transform_width<const char*, 6, 8>>, 72>;

    std::stringstream os;
    std::copy(base64(data.c_str()), base64(data.c_str() + data.size()), ostream_iterator<char>(os));
    return os.str();
}

std::string createResultItem(const TestMetadata& metadata, bool hasFailedTests) {
    const bool shouldHide =
        (hasFailedTests && metadata.status == "passed") || (metadata.status.find("ignored") != std::string::npos);

    std::string html;
    html.append("<div class=\"test " + metadata.status + (shouldHide ? " hide" : "") + "\">\n");
    html.append(R"(<h2><span class="label" style="background: )" + metadata.color + "\">" + metadata.status + "</span> " + metadata.id + "</h2>\n");

    if (!metadata.renderErrored) {
        if (metadata.outputsImage) {
            if (metadata.renderTest) {
                html.append("<img width=" + mbgl::util::toString(metadata.size.width));
                html.append(" height=" + mbgl::util::toString(metadata.size.height));
                html.append(" src=\"data:image/png;base64," + encodeBase64(metadata.actual) + "\"");
                html.append(" data-alt-src=\"data:image/png;base64," + encodeBase64(metadata.expected) + "\">\n");

                html.append("<img width=" + mbgl::util::toString(metadata.size.width));
                html.append(" height=" + mbgl::util::toString(metadata.size.height));
                html.append(" src=\"data:image/png;base64," + encodeBase64(metadata.diff) + "\">\n");
            } else {
                html.append("<img width=" + mbgl::util::toString(metadata.size.width));
                html.append(" height=" + mbgl::util::toString(metadata.size.height));
                html.append(" src=\"data:image/png;base64," + encodeBase64(metadata.actual) + "\">\n");
            }
        }
    } else {
        // FIXME: there are several places that errorMessage is not filled
        // comment out assert(!metadata.errorMessage.empty());
        html.append("<p style=\"color: red\"><strong>Error:</strong> " + metadata.errorMessage + "</p>\n");
    }

    if (metadata.metricsFailed || metadata.metricsErrored) {
        html.append("<p style=\"color: red\"><strong>Error:</strong> " + metadata.errorMessage + "</p>\n");
    }

    if (metadata.difference != 0.0) {
        if (metadata.renderTest) {
            html.append("<p class=\"diff\"><strong>Diff:</strong> " + mbgl::util::toString(metadata.difference) +
                        "</p>\n");
        } else {
            html.append("<p class=\"diff\"><strong>Diff:</strong> " + metadata.diff + "</p>\n");
        }
    }
    html.append("</div>\n");

    return html;
}

std::string createResultPage(const TestStatistics& stats, const std::vector<TestMetadata>& metadatas, bool shuffle, uint32_t seed) {
    const uint32_t unsuccessful = stats.erroredTests + stats.failedTests;
    std::string resultsPage;

    // Style
    resultsPage.append(resultsStyle);

    // Header
    if (unsuccessful) {
        resultsPage.append(R"HTML(<h1 style="color: red;">)HTML");
        resultsPage.append(mbgl::util::toString(unsuccessful) + " tests failed.");
    } else {
        resultsPage.append(R"HTML(<h1 style="color: green;">)HTML");
        resultsPage.append("All tests passed!");
    }
    resultsPage.append(resultsHeaderButtons);

    // stats
    resultsPage.append(R"HTML(<p class="stats">)HTML");
    if (stats.ignoreFailedTests) {
        resultsPage.append(mbgl::util::toString(stats.ignoreFailedTests) + " ignored failed, ");
    }
    if (stats.ignorePassedTests) {
        resultsPage.append(mbgl::util::toString(stats.ignorePassedTests) + " ignored passed, ");
    }
    if (stats.erroredTests) {
        resultsPage.append(mbgl::util::toString(stats.erroredTests) + " errored, ");
    }
    if (stats.failedTests) {
        resultsPage.append(mbgl::util::toString(stats.failedTests) + " failed, ");
    }
    resultsPage.append(mbgl::util::toString(stats.passedTests) + " passed.\n");
    resultsPage.append("</p>\n");

    // Test sequence
    {
        resultsPage.append("<div id='test-sequence' class='hide'>\n");

        // Failed tests
        if (unsuccessful) {
            resultsPage.append("<p><strong>Failed tests:</strong>");
            for (const auto& metadata : metadatas) {
                if (metadata.status == "failed" || metadata.status == "errored") {
                    resultsPage.append(metadata.id + " ");
                }
            }
            resultsPage.append("</p>\n");
        }

        // Test sequence
        resultsPage.append("<p><strong>Test sequence:</strong>");
        for (const auto& metadata : metadatas) {
            resultsPage.append(metadata.id + " ");
        }
        resultsPage.append("</p>\n");

        // Shuffle
        if (shuffle) {
            resultsPage.append("<p><strong>Shuffle seed</strong>: " + mbgl::util::toString(seed) + "</p>\n");
        }

        resultsPage.append("</div>\n");
    }

    // Script
    resultsPage.append(resultsScript);

    // Tests
    resultsPage.append("<div class=\"tests\">\n");
    for (const auto& metadata : metadatas) {
        resultsPage.append(createResultItem(metadata, unsuccessful));
    }
    resultsPage.append("</div>\n");

    return resultsPage;
}
