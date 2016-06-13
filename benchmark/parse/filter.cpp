#include <benchmark/benchmark.h>

#include <mbgl/style/filter.hpp>
#include <mbgl/style/filter_evaluator.hpp>
#include <mbgl/style/parser.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>

#include <rapidjson/document.h>

#include <map>

using namespace mbgl;

typedef std::multimap<std::string, Value> Properties;

class StubFeature : public GeometryTileFeature {
public:
    inline StubFeature(const Properties& properties_, FeatureType type_)
        : properties(properties_), type(type_) {
    }

    optional<Value> getValue(const std::string& key) const override {
        auto it = properties.find(key);
        if (it == properties.end())
            return optional<Value>();
        return it->second;
    }

    FeatureType getType() const override {
        return type;
    }

    GeometryCollection getGeometries() const override {
        return GeometryCollection();
    }

private:
    const Properties properties;
    FeatureType type;
};

style::Filter parse(const char* expression) {
    rapidjson::GenericDocument<rapidjson::UTF8<>, rapidjson::CrtAllocator> doc;
    doc.Parse<0>(expression);
    return style::parseFilter(doc);
}

static void Parse_Filter(benchmark::State& state) {
    while (state.KeepRunning()) {
        parse(R"FILTER(["==", "foo", "bar"])FILTER");
    }
}

static void Parse_EvaluateFilter(benchmark::State& state) {
    const style::Filter filter = parse(R"FILTER(["==", "foo", "bar"])FILTER");
    const Properties properties = { { "foo", std::string("bar") } };
    const StubFeature feature(properties, FeatureType::Unknown);

    while (state.KeepRunning()) {
        style::Filter::visit(filter, style::FilterEvaluator{ feature });
    }
}

BENCHMARK(Parse_Filter);
BENCHMARK(Parse_EvaluateFilter);
