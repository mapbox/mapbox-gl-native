#pragma once

#include "metadata.hpp"

#include <mbgl/util/optional.hpp>
#include <mbgl/util/rapidjson.hpp>

#include <cstdlib>
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
    const std::string& getManifestPath() const;
    const std::string& getResultPath() const;
    const std::string& getCachePath() const;
    const std::string& getAccessToken() const;
    const std::set<std::string>& getProbes() const;
    void doShuffle(uint32_t seed);

private:
    friend class ManifestParser;
    std::string manifestPath;
    std::string testRootPath;
    std::string resultPath;
    std::string cachePath;
    std::string accessToken;
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
