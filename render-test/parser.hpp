#pragma once

#include "metadata.hpp"

#include <mbgl/util/rapidjson.hpp>
#include <mbgl/util/variant.hpp>

#include <string>
#include <tuple>
#include <vector>

class Manifest;

using ErrorMessage = std::string;
using JSONReply = mbgl::variant<mbgl::JSDocument, ErrorMessage>;

JSONReply readJson(const mbgl::filesystem::path&);
std::string serializeJsonValue(const mbgl::JSValue&);
std::string serializeMetrics(const TestMetrics&);

std::vector<std::string> readExpectedImageEntries(const mbgl::filesystem::path& base);
std::vector<std::string> readExpectedMetricEntries(const mbgl::filesystem::path& base);
std::vector<std::string> readExpectedJSONEntries(const mbgl::filesystem::path& base);

TestMetrics readExpectedMetrics(const mbgl::filesystem::path& path);

TestMetadata parseTestMetadata(const TestPaths& paths, const Manifest& manifest);

std::string createResultPage(const TestStatistics&, const std::vector<TestMetadata>&, bool shuffle, uint32_t seed);

std::string toJSON(const mbgl::Value& value, unsigned indent, bool singleLine);
std::string toJSON(const std::vector<mbgl::Feature>& features, unsigned indent, bool singleLine);
