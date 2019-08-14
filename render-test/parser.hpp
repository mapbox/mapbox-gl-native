#pragma once

#include <mbgl/util/rapidjson.hpp>
#include <mbgl/util/variant.hpp>

#include <tuple>
#include <string>
#include <vector>

#include "filesystem.hpp"

struct TestMetadata;
struct TestStatistics;
struct TestPaths;

using ErrorMessage = std::string;
using JSONReply = mbgl::variant<mbgl::JSDocument, ErrorMessage>;

using ArgumentsTuple = std::tuple<bool, bool, uint32_t, std::string, std::vector<std::string>>;

JSONReply readJson(const mbgl::filesystem::path&);
std::string serializeJsonValue(const mbgl::JSValue&);

std::vector<std::string> readExpectedEntries(const mbgl::filesystem::path& base);

ArgumentsTuple parseArguments(int argc, char** argv);
std::vector<std::pair<std::string, std::string>> parseIgnores();

TestMetadata parseTestMetadata(const TestPaths& paths);

std::string createResultPage(const TestStatistics&, const std::vector<TestMetadata>&, bool shuffle, uint32_t seed);

std::string localizeURL(const std::string& url);

void localizeSourceURLs(mbgl::JSValue& root, mbgl::JSDocument& document);
void localizeStyleURLs(mbgl::JSValue& root, mbgl::JSDocument& document);