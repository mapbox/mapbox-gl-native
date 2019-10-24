#include "manifest_parser.hpp"
#include <mbgl/util/logging.hpp>
#include <mbgl/util/rapidjson.hpp>
#include "parser.hpp"

mbgl::filesystem::path ManifestParser::getValidPath(const std::string& path) {
    const static mbgl::filesystem::path BasePath{rootPath};
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

const std::string ManifestParser::getVendorPath() const {
    return vendorPath;
}
const std::string ManifestParser::getIntegrationPath() const {
    return assetPath;
}
const mbgl::filesystem::path ManifestParser::getBaseTestPath() const {
    return baseTestPath;
}
const mbgl::filesystem::path ManifestParser::getProbeTestPath() const {
    return probeTestPath;
}
const std::vector<mbgl::filesystem::path> ManifestParser::getExpectationsPaths() const {
    return expectationPaths;
}
const std::vector<mbgl::filesystem::path> ManifestParser::getIgnoresPaths() const {
    return ignorePaths;
}
const std::string ManifestParser::getTestPath() const {
    assert(!testPath.empty());
    return testPath;
}
void ManifestParser::setTestPath(const std::string& testPath_) {
    testPath = testPath_;
    if (testPath.back() == '/') {
        testPath.pop_back();
    }
}

bool ManifestParser::parseManifest(const std::string& rootPath_) {
    rootPath = rootPath_;
    auto filePath = rootPath;
#ifdef __APPLE__
    filePath += "/mac-manifest.json";
#elif __ANDROID__
    filePath += "/android-manifest.json";
#else
    filePath += "/linux-manifest.json";
#endif
    if (!mbgl::filesystem::exists(filePath)) {
        mbgl::Log::Error(mbgl::Event::General, "Provided manifest file: %s is not valid", filePath.c_str());
        return false;
    }
    auto contents = readJson(filePath);
    if (!contents.is<mbgl::JSDocument>()) {
        mbgl::Log::Error(mbgl::Event::General, "Provided manifest file: %s is not a valid json", filePath.c_str());
        return false;
    }
    auto document = std::move(contents.get<mbgl::JSDocument>());
    if (document.HasMember("asset_path")) {
        const auto& assetPathValue = document["asset_path"];
        if (!assetPathValue.IsString()) {
            mbgl::Log::Warning(
                mbgl::Event::General, "Invalid assetPath is provoided inside the manifest file: %s", filePath.c_str());
            return false;
        }

        assetPath = getValidPath(assetPathValue.GetString()).string();
        if (assetPath.back() != '/') {
            assetPath.push_back('/');
        }
    }
    if (document.HasMember("vendor_path")) {
        const auto& vendorPathValue = document["vendor_path"];
        if (!vendorPathValue.IsString()) {
            mbgl::Log::Warning(
                mbgl::Event::General, "Invalid vendorPath is provoided inside the manifest file: %s", filePath.c_str());
            return false;
        }
        auto path = std::string(vendorPathValue.GetString());

        vendorPath = getValidPath(vendorPathValue.GetString()).string();
        if (vendorPath.back() != '/') {
            vendorPath.push_back('/');
        }
    }
    if (document.HasMember("base_test_path")) {
        const auto& testPathValue = document["base_test_path"];
        if (!testPathValue.IsString()) {
            mbgl::Log::Warning(
                mbgl::Event::General, "Invalid testPath is provoided inside the manifest file: %s", filePath.c_str());
            return false;
        }
        auto path = getValidPath(testPathValue.GetString()).string();
        if (path.back() == '/') {
            path.pop_back();
        }
        baseTestPath = mbgl::filesystem::path(path);
        setTestPath(baseTestPath.string());
    }
    if (document.HasMember("probe_test_path")) {
        const auto& testPathValue = document["probe_test_path"];
        if (!testPathValue.IsString()) {
            mbgl::Log::Warning(
                mbgl::Event::General, "Invalid testPath is provoided inside the manifest file: %s", filePath.c_str());
            return false;
        }
        auto path = getValidPath(testPathValue.GetString()).string();
        if (path.back() == '/') {
            path.pop_back();
        }
        probeTestPath = mbgl::filesystem::path(path);
    }
    if (document.HasMember("expectation_paths")) {
        const auto& expectationPathValue = document["expectation_paths"];
        if (!expectationPathValue.IsArray()) {
            mbgl::Log::Warning(mbgl::Event::General,
                               "Invalid expectationPath is provoided inside the manifest file: %s",
                               filePath.c_str());
            return false;
        }
        for (const auto& value : expectationPathValue.GetArray()) {
            if (!value.IsString()) {
                return false;
            }
            expectationPaths.emplace_back(getValidPath(value.GetString()));
            if (expectationPaths.back().empty()) {
                return false;
            }
        }
    }
    if (document.HasMember("ignore_paths")) {
        const auto& ignorePathValue = document["ignore_paths"];
        if (!ignorePathValue.IsArray()) {
            mbgl::Log::Warning(
                mbgl::Event::General, "Invalid ignorePath is provoided inside the manifest file: %s", filePath.c_str());
            return false;
        }
        for (const auto& value : ignorePathValue.GetArray()) {
            if (!value.IsString()) {
                return false;
            }
            ignorePaths.emplace_back(getValidPath(value.GetString()));
            if (ignorePaths.back().empty()) {
                return false;
            }
        }
    }
    return true;
}
