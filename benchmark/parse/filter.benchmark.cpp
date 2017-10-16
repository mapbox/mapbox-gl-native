#include <benchmark/benchmark.h>

#include <mbgl/style/filter.hpp>
#include <mbgl/style/filter_evaluator.hpp>
#include <mbgl/style/conversion.hpp>
#include <mbgl/style/conversion/json.hpp>
#include <mbgl/style/conversion/filter.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>

using namespace mbgl;

style::Filter parse(const char* expression) {
    style::conversion::Error error;
    return *style::conversion::convertJSON<style::Filter>(expression, error);
}

static void Parse_Filter(benchmark::State& state) {
    while (state.KeepRunning()) {
        parse(R"FILTER(["==", "foo", "bar"])FILTER");
    }
}

static void Parse_EvaluateFilter(benchmark::State& state) {
    const style::Filter filter = parse(R"FILTER(["==", "foo", "bar"])FILTER");
    const PropertyMap properties = { { "foo", std::string("bar") } };

    while (state.KeepRunning()) {
        filter(FeatureType::Unknown, {}, [&] (const std::string& key) -> optional<Value> {
            auto it = properties.find(key);
            if (it == properties.end())
                return {};
            return it->second;
        });
    }
}

BENCHMARK(Parse_Filter);
BENCHMARK(Parse_EvaluateFilter);
