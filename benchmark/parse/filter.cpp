#include <benchmark/benchmark.h>

#include <mbgl/style/filter.hpp>
#include <mbgl/style/filter_evaluator.hpp>
#include <mbgl/style/parser.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>

#include <rapidjson/document.h>

#include <map>

using namespace mbgl;

typedef std::multimap<std::string, Value> Properties;

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

    while (state.KeepRunning()) {
        filter(FeatureType::Unknown, [&] (const std::string& key) -> optional<Value> {
            auto it = properties.find(key);
            if (it == properties.end())
                return {};
            return it->second;
        });
    }
}

BENCHMARK(Parse_Filter);
BENCHMARK(Parse_EvaluateFilter);
