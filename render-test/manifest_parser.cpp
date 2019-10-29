#include "manifest_parser.hpp"
#include "filesystem.hpp"
#include "parser.hpp"

#include <mbgl/util/logging.hpp>

#include <algorithm>
#include <random>

namespace {
std::string removeURLArguments(const std::string& url) {
    std::string::size_type index = url.find('?');
    if (index != std::string::npos) {
        return url.substr(0, index);
    }
    return url;
}

std::string prependFileScheme(const std::string& url) {
    static const std::string fileScheme("file://");
    return fileScheme + url;
}
} // namespace

Manifest::Manifest() = default;
Manifest::~Manifest() = default;

const std::vector<TestPaths>& Manifest::getTestPaths() const {
    return testPaths;
}
const std::vector<std::pair<std::string, std::string>>& Manifest::getIgnores() const {
    return ignores;
}
const std::string& Manifest::getTestRootPath() const {
    return testRootPath;
}
const std::string& Manifest::getManifestPath() const {
    return manifestPath;
}

void Manifest::doShuffle(uint32_t seed) {
    std::seed_seq sequence{seed};
    std::mt19937 shuffler(sequence);
    std::shuffle(testPaths.begin(), testPaths.end(), shuffler);
}

std::string Manifest::localizeURL(const std::string& url) const {
    static const std::regex regex{"local://"};
    if (auto vendorPath = getVendorPath(url, regex)) {
        return *vendorPath;
    }
    return getIntegrationPath(url, "", regex).value_or(url);
}

void Manifest::localizeSourceURLs(mbgl::JSValue& root, mbgl::JSDocument& document) const {
    if (root.HasMember("urls") && root["urls"].IsArray()) {
        for (auto& urlValue : root["urls"].GetArray()) {
            const std::string path =
                prependFileScheme(localizeMapboxTilesetURL(urlValue.GetString())
                                      .value_or(localizeLocalURL(urlValue.GetString()).value_or(urlValue.GetString())));
            urlValue.Set<std::string>(path, document.GetAllocator());
        }
    }

    if (root.HasMember("url")) {
        static const std::string image("image");
        static const std::string video("video");

        mbgl::JSValue& urlValue = root["url"];
        const std::string path =
            prependFileScheme(localizeMapboxTilesetURL(urlValue.GetString())
                                  .value_or(localizeLocalURL(urlValue.GetString()).value_or(urlValue.GetString())));
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
            const std::string path = prependFileScheme(
                localizeMapboxTilesURL(tileValue.GetString())
                    .value_or(localizeLocalURL(tileValue.GetString())
                                  .value_or(localizeHttpURL(tileValue.GetString()).value_or(tileValue.GetString()))));
            tileValue.Set<std::string>(path, document.GetAllocator());
        }
    }

    if (root.HasMember("data") && root["data"].IsString()) {
        mbgl::JSValue& dataValue = root["data"];
        const std::string path =
            prependFileScheme(localizeLocalURL(dataValue.GetString()).value_or(dataValue.GetString()));
        dataValue.Set<std::string>(path, document.GetAllocator());
    }
}

void Manifest::localizeStyleURLs(mbgl::JSValue& root, mbgl::JSDocument& document) const {
    if (root.HasMember("sources")) {
        mbgl::JSValue& sourcesValue = root["sources"];
        for (auto& sourceProperty : sourcesValue.GetObject()) {
            localizeSourceURLs(sourceProperty.value, document);
        }
    }

    if (root.HasMember("glyphs")) {
        mbgl::JSValue& glyphsValue = root["glyphs"];
        const std::string path = prependFileScheme(
            localizeMapboxFontsURL(glyphsValue.GetString())
                .value_or(localizeLocalURL(glyphsValue.GetString(), true).value_or(glyphsValue.GetString())));
        glyphsValue.Set<std::string>(path, document.GetAllocator());
    }

    if (root.HasMember("sprite")) {
        mbgl::JSValue& spriteValue = root["sprite"];
        const std::string path = prependFileScheme(
            localizeMapboxSpriteURL(spriteValue.GetString())
                .value_or(localizeLocalURL(spriteValue.GetString()).value_or(spriteValue.GetString())));
        spriteValue.Set<std::string>(path, document.GetAllocator());
    }
}

mbgl::optional<std::string> Manifest::localizeLocalURL(const std::string& url, bool glyphsPath) const {
    static const std::regex regex{"local://"};
    if (auto vendorPath = getVendorPath(url, regex, glyphsPath)) {
        return vendorPath;
    }
    return getIntegrationPath(url, "", regex, glyphsPath);
}

mbgl::optional<std::string> Manifest::localizeHttpURL(const std::string& url) const {
    static const std::regex regex{"http://localhost:2900"};
    if (auto vendorPath = getVendorPath(url, regex)) {
        return vendorPath;
    }
    return getIntegrationPath(url, "", regex);
}

mbgl::optional<std::string> Manifest::localizeMapboxSpriteURL(const std::string& url) const {
    static const std::regex regex{"mapbox://"};
    return getIntegrationPath(url, "", regex);
}

mbgl::optional<std::string> Manifest::localizeMapboxFontsURL(const std::string& url) const {
    static const std::regex regex{"mapbox://fonts"};
    return getIntegrationPath(url, "glyphs/", regex, true);
}

mbgl::optional<std::string> Manifest::localizeMapboxTilesURL(const std::string& url) const {
    static const std::regex regex{"mapbox://"};
    if (auto vendorPath = getVendorPath(url, regex)) {
        return vendorPath;
    }
    return getIntegrationPath(url, "tiles/", regex);
}

mbgl::optional<std::string> Manifest::localizeMapboxTilesetURL(const std::string& url) const {
    static const std::regex regex{"mapbox://"};
    return getIntegrationPath(url, "tilesets/", regex);
}

mbgl::optional<std::string> Manifest::getVendorPath(const std::string& url,
                                                    const std::regex& regex,
                                                    bool glyphsPath) const {
    mbgl::filesystem::path file = std::regex_replace(url, regex, vendorPath);
    if (mbgl::filesystem::exists(file.parent_path())) {
        return removeURLArguments(file.string());
    }

    if (glyphsPath && mbgl::filesystem::exists(file.parent_path().parent_path())) {
        return removeURLArguments(file.string());
    }

    return mbgl::nullopt;
}

mbgl::optional<std::string> Manifest::getIntegrationPath(const std::string& url,
                                                         const std::string& parent,
                                                         const std::regex& regex,
                                                         bool glyphsPath) const {
    mbgl::filesystem::path file = std::regex_replace(url, regex, assetPath + parent);
    if (mbgl::filesystem::exists(file.parent_path())) {
        return removeURLArguments(file.string());
    }

    if (glyphsPath && mbgl::filesystem::exists(file.parent_path().parent_path())) {
        return removeURLArguments(file.string());
    }

    return mbgl::nullopt;
}

namespace {
std::vector<std::pair<std::string, std::string>> parseIgnores(const std::vector<mbgl::filesystem::path>& ignoresPaths) {
    std::vector<std::pair<std::string, std::string>> ignores;
    for (const auto& path : ignoresPaths) {
        auto maybeIgnores = readJson(path);
        if (!maybeIgnores.is<mbgl::JSDocument>()) {
            continue;
        }
        for (const auto& property : maybeIgnores.get<mbgl::JSDocument>().GetObject()) {
            const std::string ignore = {property.name.GetString(), property.name.GetStringLength()};
            const std::string reason = {property.value.GetString(), property.value.GetStringLength()};
            ignores.emplace_back(std::make_pair(ignore, reason));
        }
    }

    return ignores;
}

std::vector<mbgl::filesystem::path> getTestExpectations(mbgl::filesystem::path testPath,
                                                        const mbgl::filesystem::path& testsRootPath,
                                                        std::vector<mbgl::filesystem::path> expectationsPaths) {
    std::vector<mbgl::filesystem::path> expectations{std::move(testPath.remove_filename())};
    const auto& defaultTestExpectationsPath = expectations.front().string();

    const std::regex regex{testsRootPath.string()};
    for (const auto& path : expectationsPaths) {
        expectations.emplace_back(std::regex_replace(defaultTestExpectationsPath, regex, path.string()));
        assert(!expectations.back().empty());
    }

    return expectations;
}

mbgl::filesystem::path getValidPath(const std::string& manifestPath, const std::string& path) {
    const static mbgl::filesystem::path BasePath{manifestPath};
    mbgl::filesystem::path result{path};
    if (result.is_relative()) {
        result = BasePath / result;
    }
    if (mbgl::filesystem::exists(result)) {
        return result;
    }
    mbgl::Log::Warning(mbgl::Event::General, "Invalid path is provoided inside the manifest file: %s", path.c_str());
    return mbgl::filesystem::path{};
}

} // namespace

mbgl::optional<Manifest> ManifestParser::parseManifest(const std::string& manifestPath,
                                                       const std::vector<std::string>& testNames,
                                                       const std::string& testFilter) {
    Manifest manifest;
    const auto filePath = mbgl::filesystem::path(manifestPath);
    manifest.manifestPath = manifestPath.substr(0, manifestPath.find(filePath.filename()));

    auto contents = readJson(filePath);
    if (!contents.is<mbgl::JSDocument>()) {
        mbgl::Log::Error(mbgl::Event::General, "Provided manifest file: %s is not a valid json", filePath.c_str());
        return mbgl::nullopt;
    }

    auto document = std::move(contents.get<mbgl::JSDocument>());
    if (document.HasMember("asset_path")) {
        const auto& assetPathValue = document["asset_path"];
        if (!assetPathValue.IsString()) {
            mbgl::Log::Warning(
                mbgl::Event::General, "Invalid assetPath is provoided inside the manifest file: %s", filePath.c_str());
            return mbgl::nullopt;
        }
        manifest.assetPath = (getValidPath(manifest.manifestPath, assetPathValue.GetString()) / "").string();
        if (manifest.assetPath.empty()) {
            return mbgl::nullopt;
        }
    }
    if (document.HasMember("vendor_path")) {
        const auto& vendorPathValue = document["vendor_path"];
        if (!vendorPathValue.IsString()) {
            mbgl::Log::Warning(
                mbgl::Event::General, "Invalid vendorPath is provoided inside the manifest file: %s", filePath.c_str());
            return mbgl::nullopt;
        }
        manifest.vendorPath = (getValidPath(manifest.manifestPath, vendorPathValue.GetString()) / "").string();
        if (manifest.vendorPath.empty()) {
            return mbgl::nullopt;
        }
    }
    mbgl::filesystem::path baseTestPath;
    if (document.HasMember("base_test_path")) {
        const auto& testPathValue = document["base_test_path"];
        if (!testPathValue.IsString()) {
            mbgl::Log::Warning(
                mbgl::Event::General, "Invalid testPath is provoided inside the manifest file: %s", filePath.c_str());
            return mbgl::nullopt;
        }
        baseTestPath = getValidPath(manifest.manifestPath, testPathValue.GetString());
        if (baseTestPath.empty()) {
            return mbgl::nullopt;
        }
    }
    mbgl::filesystem::path probeTestPath;
    bool enbaleProbeTest{false};
    if (document.HasMember("probe_test_path")) {
        const auto& testPathValue = document["probe_test_path"];
        if (!testPathValue.IsString()) {
            mbgl::Log::Warning(
                mbgl::Event::General, "Invalid testPath is provoided inside the manifest file: %s", filePath.c_str());
            return mbgl::nullopt;
        }
        probeTestPath = getValidPath(manifest.manifestPath, testPathValue.GetString());
        if (probeTestPath.empty()) {
            return mbgl::nullopt;
        }
        enbaleProbeTest = true;
    }
    std::vector<mbgl::filesystem::path> expectationPaths{};
    if (document.HasMember("expectation_paths")) {
        const auto& expectationPathValue = document["expectation_paths"];
        if (!expectationPathValue.IsArray()) {
            mbgl::Log::Warning(mbgl::Event::General,
                               "Provided expectation_paths inside the manifest file: %s is not a valid array",
                               filePath.c_str());
            return mbgl::nullopt;
        }
        for (const auto& value : expectationPathValue.GetArray()) {
            if (!value.IsString()) {
                mbgl::Log::Warning(mbgl::Event::General,
                                   "Invalid expectation path item is provoided inside the manifest file: %s",
                                   filePath.c_str());
                return mbgl::nullopt;
            }
            expectationPaths.emplace_back(getValidPath(manifest.manifestPath, value.GetString()));
            if (expectationPaths.back().empty()) {
                return mbgl::nullopt;
            }
        }
    }
    std::vector<mbgl::filesystem::path> ignorePaths{};
    if (document.HasMember("ignore_paths")) {
        const auto& ignorePathValue = document["ignore_paths"];
        if (!ignorePathValue.IsArray()) {
            mbgl::Log::Warning(mbgl::Event::General,
                               "Provided ignore_paths inside the manifest file: %s is not a valid array",
                               filePath.c_str());
            return mbgl::nullopt;
        }
        for (const auto& value : ignorePathValue.GetArray()) {
            if (!value.IsString()) {
                mbgl::Log::Warning(mbgl::Event::General,
                                   "Invalid ignore path item is provoided inside the manifest file: %s",
                                   filePath.c_str());
                return mbgl::nullopt;
            }
            ignorePaths.emplace_back(getValidPath(manifest.manifestPath, value.GetString()));
            if (ignorePaths.back().empty()) {
                return mbgl::nullopt;
            }
        }
        manifest.ignores = parseIgnores(ignorePaths);
    }

    manifest.testRootPath = enbaleProbeTest ? probeTestPath.string() : baseTestPath.string();
    if (manifest.testRootPath.back() == '/') {
        manifest.testRootPath.pop_back();
    }
    if (manifest.manifestPath.back() == '/') {
        manifest.manifestPath.pop_back();
    }

    std::vector<mbgl::filesystem::path> paths;
    for (const auto& id : testNames) {
        paths.emplace_back(manifest.testRootPath + "/" + id);
    }
    if (paths.empty()) {
        paths.emplace_back(manifest.testRootPath);
    }

    // Recursively traverse through the test paths and collect test directories containing "style.json".
    auto& testPaths = manifest.testPaths;
    testPaths.reserve(paths.size());
    for (const auto& path : paths) {
        if (!mbgl::filesystem::exists(path)) {
            mbgl::Log::Warning(
                mbgl::Event::General, "Provided test folder '%s' does not exist.", path.string().c_str());
            continue;
        }
        for (auto& testPath : mbgl::filesystem::recursive_directory_iterator(path)) {
            // Skip paths that fail regexp match.
            if (!testFilter.empty() && !std::regex_match(testPath.path().string(), std::regex(testFilter))) {
                continue;
            }

            if (testPath.path().filename() == "style.json") {
                testPaths.emplace_back(testPath, getTestExpectations(testPath, path, expectationPaths));
            }
        }
    }

    return mbgl::optional<Manifest>(manifest);
}
