#include "manifest_parser.hpp"
#include "filesystem.hpp"
#include "parser.hpp"

#include <mbgl/util/logging.hpp>

#include <algorithm>
#include <random>

Manifest::Manifest() {
    const char* envToken = getenv("MAPBOX_ACCESS_TOKEN");
    if (envToken != nullptr) {
        accessToken = envToken;
    }
}

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
const std::string& Manifest::getResultPath() const {
    return resultPath;
}

const std::string& Manifest::getCachePath() const {
    return cachePath;
}

const std::string& Manifest::getAccessToken() const {
    return accessToken;
}

const std::set<std::string>& Manifest::getProbes() const {
    return probes;
}

void Manifest::doShuffle(uint32_t seed) {
    std::seed_seq sequence{seed};
    std::mt19937 shuffler(sequence);
    std::shuffle(testPaths.begin(), testPaths.end(), shuffler);
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

// defaultExpectationPath: absolute path that constains the style.json file for testing
// testId: Test case id that used for composing expectation path
// expectatedPaths: absolute paths that constain possible expected.png/metrics.json files for result checking
std::vector<mbgl::filesystem::path> getTestExpectations(const mbgl::filesystem::path& defaultExpectationPath,
                                                        const std::string& testId,
                                                        std::vector<mbgl::filesystem::path> expectatedPaths) {
    std::vector<mbgl::filesystem::path> expectations{defaultExpectationPath};
    for (const auto& expectedPath : expectatedPaths) {
        expectations.emplace_back(expectedPath / testId);
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
        return result.lexically_normal();
    }
    mbgl::Log::Warning(mbgl::Event::General, "Invalid path is provided inside the manifest file: %s", path.c_str());
    return mbgl::filesystem::path{};
}

} // namespace

mbgl::optional<Manifest> ManifestParser::parseManifest(const std::string& manifestPath,
                                                       std::string testFilter) {
    Manifest manifest;
    const auto filePath = mbgl::filesystem::path(manifestPath);
    manifest.manifestPath = manifestPath.substr(0, manifestPath.find(filePath.filename()));

    auto contents = readJson(filePath);
    if (!contents.is<mbgl::JSDocument>()) {
        mbgl::Log::Error(mbgl::Event::General, "Provided manifest file: %s is not a valid json", filePath.c_str());
        return mbgl::nullopt;
    }

    auto document = std::move(contents.get<mbgl::JSDocument>());
    if (document.HasMember("result_path")) {
        const auto& resultPathValue = document["result_path"];
        if (!resultPathValue.IsString()) {
            mbgl::Log::Warning(
                mbgl::Event::General, "Invalid result_path is provided inside the manifest file: %s", filePath.c_str());
            return mbgl::nullopt;
        }
        manifest.resultPath = (getValidPath(manifest.manifestPath, resultPathValue.GetString()) / "").string();
        if (manifest.resultPath.empty()) {
            return mbgl::nullopt;
        }
    }
    if (document.HasMember("cache_path")) {
        const auto& cachePathValue = document["cache_path"];
        if (!cachePathValue.IsString()) {
            mbgl::Log::Warning(
                mbgl::Event::General, "Invalid cache_path is provided inside the manifest file: %s", filePath.c_str());
            return mbgl::nullopt;
        }
        manifest.cachePath = (getValidPath(manifest.manifestPath, ".") / cachePathValue.GetString()).string();
        if (manifest.cachePath.empty()) {
            return mbgl::nullopt;
        }
    }
    if (document.HasMember("access_token")) {
        const auto& accessTokenValue = document["access_token"];
        if (!accessTokenValue.IsString()) {
            mbgl::Log::Warning(mbgl::Event::General,
                               "Invalid access_token is provided inside the manifest file: %s",
                               filePath.c_str());
            return mbgl::nullopt;
        }
        manifest.accessToken = accessTokenValue.GetString();
        if (manifest.accessToken.empty()) {
            return mbgl::nullopt;
        }
    }
    mbgl::filesystem::path baseTestPath;
    if (document.HasMember("base_test_path")) {
        const auto& testPathValue = document["base_test_path"];
        if (!testPathValue.IsString()) {
            mbgl::Log::Warning(mbgl::Event::General,
                               "Invalid base_test_path is provided inside the manifest file: %s",
                               filePath.c_str());
            return mbgl::nullopt;
        }
        baseTestPath = getValidPath(manifest.manifestPath, testPathValue.GetString());
        if (baseTestPath.empty()) {
            return mbgl::nullopt;
        }
    }
    mbgl::filesystem::path expectedMetricPath;
    if (document.HasMember("metric_path")) {
        const auto& metricPathValue = document["metric_path"];
        if (!metricPathValue.IsString()) {
            mbgl::Log::Warning(
                mbgl::Event::General, "Invalid metric_path is provided inside the manifest file: %s", filePath.c_str());
            return mbgl::nullopt;
        }
        expectedMetricPath = getValidPath(manifest.manifestPath, metricPathValue.GetString());
        if (expectedMetricPath.empty()) {
            return mbgl::nullopt;
        }
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
                                   "Invalid expectation path item is provided inside the manifest file: %s",
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
                                   "Invalid ignore path item is provided inside the manifest file: %s",
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

    if (document.HasMember("probes")) {
        const auto& probesValue = document["probes"];
        if (!probesValue.IsArray()) {
            mbgl::Log::Warning(mbgl::Event::General,
                               "Provided probes inside the manifest file: %s is not a valid array",
                               filePath.c_str());
            return mbgl::nullopt;
        }
        for (const auto& value : probesValue.GetArray()) {
            if (!value.IsString()) {
                mbgl::Log::Warning(mbgl::Event::General,
                                   "Invalid probe type is provided inside the manifest file: %s",
                                   filePath.c_str());
                return mbgl::nullopt;
            }
            manifest.probes.emplace(value.GetString());
        }
    }

    if (testFilter.empty() && document.HasMember("filter")) {
        const auto& filterValue = document["filter"];
        if (!filterValue.IsString()) {
            mbgl::Log::Warning(
                mbgl::Event::General, "Invalid filter is provided inside the manifest file: %s", filePath.c_str());
            return mbgl::nullopt;
        }

        testFilter = filterValue.GetString();
    }

    manifest.testRootPath = baseTestPath.string();
    if (manifest.testRootPath.back() == '/') {
        manifest.testRootPath.pop_back();
    }
    if (manifest.manifestPath.back() == '/') {
        manifest.manifestPath.pop_back();
    }
    if (manifest.resultPath.empty()) {
        manifest.resultPath = manifest.manifestPath;
    } else if (manifest.resultPath.back() == '/') {
        manifest.resultPath.pop_back();
    }

    auto& path = manifest.testRootPath;
    auto& testPaths = manifest.testPaths;

    for (auto& testPath : mbgl::filesystem::recursive_directory_iterator(path)) {
        // Skip paths that fail regexp search.
        if (!testFilter.empty() && !std::regex_search(testPath.path().string(), std::regex(testFilter))) {
            continue;
        }

        if (testPath.path().filename() == "style.json") {
            const auto defaultExpectationPath{std::move(mbgl::filesystem::path(testPath).remove_filename())};
            const auto rootLength = manifest.testRootPath.length();
            auto testId = defaultExpectationPath.string();
            testId = testId.substr(rootLength + 1, testId.length() - rootLength - 1);

            std::vector<mbgl::filesystem::path> expectedMetricPaths{expectedMetricPath};
#if defined(__ANDROID__)
            expectedMetricPaths.emplace_back("/sdcard/baselines/");
#elif defined(__APPLE__)
            expectedMetricPaths.emplace_back(manifest.manifestPath + "/baselines/");
#endif
            testPaths.emplace_back(testPath,
                                   getTestExpectations(defaultExpectationPath, testId, expectationPaths),
                                   getTestExpectations(defaultExpectationPath, testId, expectedMetricPaths));
        }
    }

    return mbgl::optional<Manifest>(manifest);
}
