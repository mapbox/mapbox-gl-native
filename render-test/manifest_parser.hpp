#pragma once

#include "metadata.hpp"

#include <mbgl/util/optional.hpp>
#include <mbgl/util/rapidjson.hpp>

#include <regex>
#include <string>
#include <utility>
#include <vector>

class Manifest {
public:
    Manifest();
    ~Manifest();
    const std::vector<std::pair<std::string, std::string>>& getIgnores() const;
    const std::vector<TestPaths>& getTestPaths() const;
    const std::string& getTestRootPath() const;
    const std::string& getAssetPath() const;
    const std::string& getManifestPath() const;
    const std::string& getResultPath() const;
    const std::set<std::string>& getProbes() const;
    void doShuffle(uint32_t seed);

    std::string localizeURL(const std::string& url) const;
    void localizeSourceURLs(mbgl::JSValue& root, mbgl::JSDocument& document) const;
    void localizeStyleURLs(mbgl::JSValue& root, mbgl::JSDocument& document) const;

private:
    friend class ManifestParser;
    mbgl::optional<std::string> localizeLocalURL(const std::string& url, bool glyphsPath = false) const;
    mbgl::optional<std::string> localizeHttpURL(const std::string& url) const;
    mbgl::optional<std::string> localizeMapboxSpriteURL(const std::string& url) const;
    mbgl::optional<std::string> localizeMapboxFontsURL(const std::string& url) const;
    mbgl::optional<std::string> localizeMapboxTilesURL(const std::string& url) const;
    mbgl::optional<std::string> localizeMapboxTilesetURL(const std::string& url) const;
    mbgl::optional<std::string> getVendorPath(const std::string& url,
                                              const std::regex& regex,
                                              bool glyphsPath = false) const;
    mbgl::optional<std::string> getIntegrationPath(const std::string& url,
                                                   const std::string& parent,
                                                   const std::regex& regex,
                                                   bool glyphsPath = false) const;
    std::string manifestPath;
    std::string testRootPath;
    std::string vendorPath;
    std::string assetPath;
    std::string resultPath;
    std::vector<std::pair<std::string, std::string>> ignores;
    std::vector<TestPaths> testPaths;
    std::set<std::string> probes;
};

class ManifestParser {
public:
    static mbgl::optional<Manifest> parseManifest(const std::string& manifestPath,
                                                  const std::vector<std::string>& testNames,
                                                  std::string testFilter);
};
