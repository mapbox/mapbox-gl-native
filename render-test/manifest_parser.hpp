#pragma once

#include <string>
#include <vector>
#include "filesystem.hpp"

class ManifestParser {
public:
    static ManifestParser& getInstance() {
        static ManifestParser instance;
        return instance;
    }
    const std::string getVendorPath() const;
    const std::string getIntegrationPath() const;
    const mbgl::filesystem::path getBaseTestPath() const;
    const mbgl::filesystem::path getProbeTestPath() const;
    const std::vector<mbgl::filesystem::path> getExpectationsPaths() const;
    const std::vector<mbgl::filesystem::path> getIgnoresPaths() const;
    bool parseManifest(const std::string& rootPath_);
    const std::string getTestPath() const;
    void setTestPath(const std::string& testPath_);

private:
    ManifestParser() = default;
    mbgl::filesystem::path getValidPath(const std::string& path);

    std::string rootPath{};
    std::string testPath{};
    std::string vendorPath{};
    std::string assetPath{};
    mbgl::filesystem::path baseTestPath{};
    mbgl::filesystem::path probeTestPath{};
    std::vector<mbgl::filesystem::path> expectationPaths{};
    std::vector<mbgl::filesystem::path> ignorePaths{};
};
