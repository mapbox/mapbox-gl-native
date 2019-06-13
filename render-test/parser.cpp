#include <mbgl/util/logging.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/rapidjson.hpp>
#include <mbgl/util/string.hpp>

#include <args.hxx>

#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/insert_linebreaks.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/archive/iterators/ostream_iterator.hpp>

#include "parser.hpp"
#include "metadata.hpp"

#include <sstream>
#include <regex>

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

std::string removeURLArguments(const std::string &url) {
    std::string::size_type index = url.find('?');
    if (index != std::string::npos) {
        return url.substr(0, index);
    }
    return url;
}

std::string prependFileScheme(const std::string &url) {
    static const std::string fileScheme("file://");
    return fileScheme + url;
}

mbgl::optional<std::string> getVendorPath(const std::string& url, const std::regex& regex, bool glyphsPath = false) {
    static const mbgl::filesystem::path vendorPath(std::string(TEST_RUNNER_ROOT_PATH) + "/vendor/");

    mbgl::filesystem::path file = std::regex_replace(url, regex, vendorPath.string());
    if (mbgl::filesystem::exists(file.parent_path())) {
        return removeURLArguments(file.string());
    }

    if (glyphsPath && mbgl::filesystem::exists(file.parent_path().parent_path())) {
        return removeURLArguments(file.string());
    }

    return {};
}

mbgl::optional<std::string> getIntegrationPath(const std::string& url, const std::string& parent, const std::regex& regex, bool glyphsPath = false) {
    static const mbgl::filesystem::path integrationPath(std::string(TEST_RUNNER_ROOT_PATH) + "/mapbox-gl-js/test/integration/");

    mbgl::filesystem::path file = std::regex_replace(url, regex, integrationPath.string() + parent);
    if (mbgl::filesystem::exists(file.parent_path())) {
        return removeURLArguments(file.string());
    }

    if (glyphsPath && mbgl::filesystem::exists(file.parent_path().parent_path())) {
        return removeURLArguments(file.string());
    }

    return {};
}

mbgl::optional<std::string> localizeLocalURL(const std::string& url, bool glyphsPath = false) {
    static const std::regex regex { "local://" };
    if (auto vendorPath = getVendorPath(url, regex, glyphsPath)) {
        return vendorPath;
    } else {
        return getIntegrationPath(url, "", regex, glyphsPath);
    }
}

mbgl::optional<std::string> localizeHttpURL(const std::string& url) {
    static const std::regex regex { "http://localhost:2900" };
    if (auto vendorPath = getVendorPath(url, regex)) {
        return vendorPath;
    } else {
        return getIntegrationPath(url, "", regex);
    }
}

mbgl::optional<std::string> localizeMapboxSpriteURL(const std::string& url) {
    static const std::regex regex { "mapbox://" };
    return getIntegrationPath(url, "", regex);
}

mbgl::optional<std::string> localizeMapboxFontsURL(const std::string& url) {
    static const std::regex regex { "mapbox://fonts" };
    return getIntegrationPath(url, "glyphs/", regex, true);
}

mbgl::optional<std::string> localizeMapboxTilesURL(const std::string& url) {
    static const std::regex regex { "mapbox://" };
    if (auto vendorPath = getVendorPath(url, regex)) {
        return vendorPath;
    } else {
        return getIntegrationPath(url, "tiles/", regex);
    }
}

mbgl::optional<std::string> localizeMapboxTilesetURL(const std::string& url) {
    static const std::regex regex { "mapbox://" };
    return getIntegrationPath(url, "tilesets/", regex);
}

} // namespace

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

    return { std::move(document) };
}

std::string serializeJsonValue(const mbgl::JSValue& value) {
    rapidjson::StringBuffer buffer;
    buffer.Clear();
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    value.Accept(writer);
    return buffer.GetString();
}

std::vector<std::string> readExpectedEntries(const mbgl::filesystem::path& base) {
    static const std::regex regex(".*expected.*.png");

    std::vector<std::string> expectedImages;
    for (const auto& entry : mbgl::filesystem::directory_iterator(base)) {
        if (entry.is_regular_file()) {
            const std::string path = entry.path().string();
            if (std::regex_match(path, regex)) {
                expectedImages.emplace_back(std::move(path));
            }
        }
    }
    return expectedImages;
}


ArgumentsTuple parseArguments(int argc, char** argv) {
    args::ArgumentParser argumentParser("Mapbox GL Test Runner");

    args::HelpFlag helpFlag(argumentParser, "help", "Display this help menu", { 'h', "help" });

    args::Flag recycleMapFlag(argumentParser, "recycle map", "Toggle reusing the map object",
                              { 'r', "recycle-map" });
    args::Flag shuffleFlag(argumentParser, "shuffle", "Toggle shuffling the tests order",
                           { 's', "shuffle" });
    args::ValueFlag<uint32_t> seedValue(argumentParser, "seed", "Shuffle seed (default: random)",
                                        { "seed" });
    args::ValueFlag<std::string> testPathValue(argumentParser, "rootPath", "Test root rootPath",
                                               { 'p', "rootPath" });
    args::PositionalList<std::string> testNameValues(argumentParser, "URL", "Test name(s)");

    try {
        argumentParser.ParseCLI(argc, argv);
    } catch (const args::Help&) {
        std::ostringstream stream;
        stream << argumentParser;
        mbgl::Log::Info(mbgl::Event::General, stream.str());
        exit(0);
    } catch (const args::ParseError& e) {
        std::ostringstream stream;
        stream << argumentParser;
        mbgl::Log::Info(mbgl::Event::General, stream.str());
        mbgl::Log::Error(mbgl::Event::General, e.what());
        exit(1);
    } catch (const args::ValidationError& e) {
        std::ostringstream stream;
        stream << argumentParser;
        mbgl::Log::Info(mbgl::Event::General, stream.str());
        mbgl::Log::Error(mbgl::Event::General, e.what());
        exit(2);
    }

    const std::string testDefaultPath =
        std::string(TEST_RUNNER_ROOT_PATH).append("/mapbox-gl-js/test/integration/render-tests");

    std::vector<std::string> ids;
    for (const auto& id : args::get(testNameValues)) {
        ids.emplace_back(testDefaultPath + "/" + id);
    }

    if (ids.empty()) {
        ids.emplace_back(testDefaultPath);
    }

    return ArgumentsTuple {
        recycleMapFlag ? args::get(recycleMapFlag) : false,
        shuffleFlag ? args::get(shuffleFlag) : false, seedValue ? args::get(seedValue) : 1u,
        testPathValue ? args::get(testPathValue) : testDefaultPath, ids
    };
}

std::vector<std::pair<std::string, std::string>> parseIgnores() {
    std::vector<std::pair<std::string, std::string>> ignores;

    auto path = mbgl::filesystem::path(TEST_RUNNER_ROOT_PATH).append("platform/node/test/ignores.json");

    auto maybeIgnores = readJson(path.string());
    if (maybeIgnores.is<mbgl::JSDocument>()) {
        for (const auto& property : maybeIgnores.get<mbgl::JSDocument>().GetObject()) {
            const std::string ignore = { property.name.GetString(),
                                         property.name.GetStringLength() };
            const std::string reason = { property.value.GetString(),
                                         property.value.GetStringLength() };
            ignores.emplace_back(std::make_pair(ignore, reason));
        }
    }

    return ignores;
}

TestMetadata parseTestMetadata(const mbgl::filesystem::path& path) {
    TestMetadata metadata;
    metadata.path = path;

    auto maybeJson = readJson(path.string());
    if (!maybeJson.is<mbgl::JSDocument>()) { // NOLINT
        metadata.errorMessage = std::string("Unable to parse: ") + path.string();
        return metadata;
    }

    metadata.document = std::move(maybeJson.get<mbgl::JSDocument>());
    localizeStyleURLs(metadata.document, metadata.document);

    if (!metadata.document.HasMember("metadata")) {
        mbgl::Log::Warning(mbgl::Event::ParseStyle, "Style has no 'metadata': %s",
                           path.c_str());
        return metadata;
    }

    const mbgl::JSValue& metadataValue = metadata.document["metadata"];
    if (!metadataValue.HasMember("test")) {
        mbgl::Log::Warning(mbgl::Event::ParseStyle, "Style has no 'metadata.test': %s",
                           path.c_str());
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
        metadata.description = std::string{ testValue["description"].GetString(),
                                                testValue["description"].GetStringLength() };
    }

    if (testValue.HasMember("mapMode")) {
        assert(testValue["mapMode"].IsString());
        metadata.mapMode = testValue["mapMode"].GetString() == std::string("tile") ? mbgl::MapMode::Tile : mbgl::MapMode::Static;
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
    const bool shouldHide = (hasFailedTests && metadata.status == "passed") || (metadata.status.find("ignored") != std::string::npos);
    
    std::string html;
    html.append("<div class=\"test " + metadata.status + (shouldHide ? " hide" : "") + "\">\n");
    html.append(R"(<h2><span class="label" style="background: )" + metadata.color + "\">" + metadata.status + "</span> " + metadata.id + "</h2>\n");
    if (metadata.status != "errored") {
        html.append("<img width=" + mbgl::util::toString(metadata.size.width));
        html.append(" height=" + mbgl::util::toString(metadata.size.height));
        html.append(" src=\"data:image/png;base64," + encodeBase64(metadata.actual) + "\"");
        html.append(" data-alt-src=\"data:image/png;base64," + encodeBase64(metadata.expected) + "\">\n");

        html.append("<img width=" + mbgl::util::toString(metadata.size.width));
        html.append(" height=" + mbgl::util::toString(metadata.size.height));
        html.append(" src=\"data:image/png;base64," + encodeBase64(metadata.diff) + "\">\n");
    } else {
        assert(!metadata.errorMessage.empty());
        html.append("<p style=\"color: red\"><strong>Error:</strong> " + metadata.errorMessage + "</p>\n");
    }
    if (metadata.difference != 0.0) {
        html.append("<p class=\"diff\"><strong>Diff:</strong> " + mbgl::util::toString(metadata.difference) + "</p>\n");
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

std::string localizeURL(const std::string& url) {
    static const std::regex regex { "local://" };
    if (auto vendorPath = getVendorPath(url, regex)) {
        return *vendorPath;
    } else {
        return getIntegrationPath(url, "", regex).value_or(url);
    }
}

void localizeSourceURLs(mbgl::JSValue& root, mbgl::JSDocument& document) {
    if (root.HasMember("urls") && root["urls"].IsArray()) {
        for (auto& urlValue : root["urls"].GetArray()) {
            const std::string path = prependFileScheme(localizeMapboxTilesetURL(urlValue.GetString())
                .value_or(localizeLocalURL(urlValue.GetString())
                    .value_or(urlValue.GetString())));
            urlValue.Set<std::string>(path, document.GetAllocator());
        }
    }

    if (root.HasMember("url")) {
        static const std::string image("image");
        static const std::string video("video");

        mbgl::JSValue& urlValue = root["url"];
        const std::string path = prependFileScheme(localizeMapboxTilesetURL(urlValue.GetString())
            .value_or(localizeLocalURL(urlValue.GetString())
                .value_or(urlValue.GetString())));
        urlValue.Set<std::string>(path, document.GetAllocator());

        if (root["type"].GetString() != image && root["type"].GetString() != video) {
            const auto tilesetPath = std::string(urlValue.GetString()).erase(0u, 7u); // remove "file://"
            auto maybeTileset = readJson(tilesetPath);
            if (maybeTileset.is<mbgl::JSDocument>()) {
                const auto& tileset = maybeTileset.get<mbgl::JSDocument>();
                assert(tileset.HasMember("tiles"));
                root.AddMember("tiles", (mbgl::JSValue&)tileset["tiles"], document.GetAllocator());
                root.RemoveMember("url");
            }
        }
    }

    if (root.HasMember("tiles")) {
        mbgl::JSValue& tilesValue = root["tiles"];
        assert(tilesValue.IsArray());
        for (auto& tileValue : tilesValue.GetArray()) {
            const std::string path = prependFileScheme(localizeMapboxTilesURL(tileValue.GetString())
                .value_or(localizeLocalURL(tileValue.GetString())
                    .value_or(localizeHttpURL(tileValue.GetString())
                        .value_or(tileValue.GetString()))));
            tileValue.Set<std::string>(path, document.GetAllocator());
        }
    }

    if (root.HasMember("data") && root["data"].IsString()) {
        mbgl::JSValue& dataValue = root["data"];
        const std::string path = prependFileScheme(localizeLocalURL(dataValue.GetString())
            .value_or(dataValue.GetString()));
        dataValue.Set<std::string>(path, document.GetAllocator());
    }
}

void localizeStyleURLs(mbgl::JSValue& root, mbgl::JSDocument& document) {
    if (root.HasMember("sources")) {
        mbgl::JSValue& sourcesValue = root["sources"];
        for (auto& sourceProperty : sourcesValue.GetObject()) {
            localizeSourceURLs(sourceProperty.value, document);
        }
    }

    if (root.HasMember("glyphs")) {
        mbgl::JSValue& glyphsValue = root["glyphs"];
        const std::string path = prependFileScheme(localizeMapboxFontsURL(glyphsValue.GetString())
            .value_or(localizeLocalURL(glyphsValue.GetString(), true)
                .value_or(glyphsValue.GetString())));
        glyphsValue.Set<std::string>(path, document.GetAllocator());
    }

    if (root.HasMember("sprite")) {
        mbgl::JSValue& spriteValue = root["sprite"];
        const std::string path = prependFileScheme(localizeMapboxSpriteURL(spriteValue.GetString())
            .value_or(localizeLocalURL(spriteValue.GetString())
                .value_or(spriteValue.GetString())));
        spriteValue.Set<std::string>(path, document.GetAllocator());
    }
}
