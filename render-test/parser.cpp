#include <mbgl/util/logging.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/rapidjson.hpp>
#include <mbgl/util/string.hpp>

#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <mapbox/geojson_impl.hpp>
#include <mbgl/style/conversion/filter.hpp>
#include <mbgl/style/conversion/json.hpp>

#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/insert_linebreaks.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/archive/iterators/ostream_iterator.hpp>

#include "filesystem.hpp"
#include "metadata.hpp"
#include "parser.hpp"
#include "runner.hpp"

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

TestMetadata parseTestMetadata(const TestPaths& paths, const Manifest& manifest) {
    TestMetadata metadata;
    metadata.paths = paths;

    auto maybeJson = readJson(paths.stylePath.string());
    if (!maybeJson.is<mbgl::JSDocument>()) { // NOLINT
        metadata.errorMessage = std::string("Unable to parse: ") + metadata.paths.stylePath.string();
        return metadata;
    }

    metadata.document = std::move(maybeJson.get<mbgl::JSDocument>());
    manifest.localizeStyleURLs(metadata.document, metadata.document);

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
        if (mapModeStr == "tile")
            metadata.mapMode = mbgl::MapMode::Tile;
        else if (mapModeStr == "continuous") {
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
    if (metadata.status != "errored") {
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
