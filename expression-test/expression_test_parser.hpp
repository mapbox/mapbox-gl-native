#pragma once

#include "filesystem.hpp"

#include <mbgl/style/expression/expression.hpp>
#include <mbgl/util/feature.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/rapidjson.hpp>

#include <set>
#include <string>
#include <vector>

using namespace mbgl;

struct Input {
    Input(optional<float> zoom_,
          optional<double> heatmapDensity_,
          std::set<std::string> availableImages_,
          Feature feature_)
        : zoom(std::move(zoom_)),
          heatmapDensity(std::move(heatmapDensity_)),
          availableImages(std::move(availableImages_)),
          feature(std::move(feature_)) {}
    optional<float> zoom;
    optional<double> heatmapDensity;
    std::set<std::string> availableImages;
    Feature feature;
};

struct Compiled {
    bool operator==(const Compiled& other) const {
        bool typeEqual = success == other.success &&
                         isFeatureConstant == other.isFeatureConstant &&
                         isZoomConstant == other.isZoomConstant &&
                         serializedType == other.serializedType &&
                         errors == other.errors;
        return typeEqual;
    }

    bool success = false;
    bool isFeatureConstant = false;
    bool isZoomConstant = false;
    std::string serializedType;
    std::vector<Value> errors;
};

struct TestResult {
    Compiled compiled;
    optional<Value> expression;
    optional<Value> outputs;
    optional<Value> serialized;
};

struct PropertySpec {
    std::string type;
    std::string value;
    std::size_t length = 0;
    bool isDataDriven = false;
    optional<Value> expression;
};

class TestData {
public:
    std::vector<Input> inputs;
    TestResult expected;
    TestResult result;
    TestResult recompiled;
    optional<PropertySpec> spec;
    JSDocument document;
};

struct Ignore {
    Ignore(std::string id_, std::string reason_)
        : id(std::move(id_)),
          reason(std::move(reason_)) {}

    std::string id;
    std::string reason;
};

using Arguments = std::tuple<filesystem::path, std::vector<filesystem::path>, bool, uint32_t>;
Arguments parseArguments(int argc, char** argv);

using Ignores = std::vector<Ignore>;
Ignores parseExpressionIgnores();
optional<TestData> parseTestData(const filesystem::path&);

std::string toJSON(const Value& value, unsigned indent = 0, bool singleLine = false);
JSDocument toDocument(const Value&);
Value toValue(const Compiled&);
optional<Value> toValue(const style::expression::Value&);

std::unique_ptr<style::expression::Expression> parseExpression(const JSValue&,
                                                               optional<PropertySpec>&,
                                                               TestResult&);
std::unique_ptr<style::expression::Expression> parseExpression(const optional<Value>&,
                                                               optional<PropertySpec>&,
                                                               TestResult&);

