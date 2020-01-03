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

TestMetadata parseTestMetadata(const TestPaths& paths);
TestOperations parseTestOperations(TestMetadata& metadata);

std::string createResultPage(const TestStatistics&, const std::vector<TestMetadata>&, bool shuffle, uint32_t seed);

std::string toJSON(const mbgl::Value& value, unsigned indent, bool singleLine);
std::string toJSON(const std::vector<mbgl::Feature>& features, unsigned indent, bool singleLine);

namespace TestOperationNames {
extern const std::string waitOp;
extern const std::string sleepOp;
extern const std::string addImageOp;
extern const std::string updateImageOp;
extern const std::string removeImageOp;
extern const std::string setStyleOp;
extern const std::string setCenterOp;
extern const std::string setZoomOp;
extern const std::string setBearingOp;
extern const std::string setPitchOp;
extern const std::string setFilterOp;
extern const std::string setLayerZoomRangeOp;
extern const std::string setLightOp;
extern const std::string addLayerOp;
extern const std::string removeLayerOp;
extern const std::string addSourceOp;
extern const std::string removeSourceOp;
extern const std::string setPaintPropertyOp;
extern const std::string setLayoutPropertyOp;
extern const std::string fileSizeProbeOp;
extern const std::string memoryProbeOp;
extern const std::string memoryProbeStartOp;
extern const std::string memoryProbeEndOp;
extern const std::string networkProbeOp;
extern const std::string networkProbeStartOp;
extern const std::string networkProbeEndOp;
extern const std::string setFeatureStateOp;
extern const std::string getFeatureStateOp;
extern const std::string removeFeatureStateOp;
extern const std::string panGestureOp;
extern const std::string gfxProbeOp;
extern const std::string gfxProbeStartOp;
extern const std::string gfxProbeEndOp;
} // namespace TestOperationNames
