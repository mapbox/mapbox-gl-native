#include <benchmark/benchmark.h>

#include <mbgl/benchmark/stub_geometry_tile_feature.hpp>

#include <mbgl/style/conversion.hpp>
#include <mbgl/style/conversion/json.hpp>
#include <mbgl/style/conversion/function.hpp>
#include <mbgl/style/conversion/data_driven_property_value.hpp>

using namespace mbgl;
using namespace mbgl::style;

static std::string createFunctionJSON(size_t stopCount) {
    std::string stops = "[";
    for (size_t outerStop = 0; outerStop < stopCount; outerStop++) {
        for (size_t innerStop = 0; innerStop < stopCount; innerStop++) {
            std::string zoom = std::to_string(24.0f / stopCount * outerStop);
            std::string value = std::to_string(100.0f / stopCount * innerStop);

            if (stops.size() > 1) stops += ",";
            stops += R"([{"zoom":)" + zoom +  R"(,"value":)" + value + "}, " + value + "]";
        }
    }
    stops += "]";
    return R"({"type": "exponential", "base": 2,  "stops": )" + stops + R"(, "property": "x"})";
}

static void Parse_CompositeFunction(benchmark::State& state) {
    size_t stopCount = state.range(0);
    
    while (state.KeepRunning()) {
        conversion::Error error;
        state.PauseTiming();
        auto doc = createFunctionJSON(stopCount);
        state.ResumeTiming();
        optional<DataDrivenPropertyValue<float>> result = conversion::convertJSON<DataDrivenPropertyValue<float>>(doc, error);
        if (!result) {
            state.SkipWithError(error.message.c_str());
        }
    }
    state.SetLabel(std::to_string(stopCount).c_str());
}

static void Evaluate_CompositeFunction(benchmark::State& state) {
    size_t stopCount = state.range(0);
    auto doc = createFunctionJSON(stopCount);
    conversion::Error error;
    optional<DataDrivenPropertyValue<float>> function = conversion::convertJSON<DataDrivenPropertyValue<float>>(doc, error);
    if (!function) {
        state.SkipWithError(error.message.c_str());
    }
    
    while(state.KeepRunning()) {
        float z = 24.0f * static_cast<float>(rand() % 100) / 100;
        function->asExpression().evaluate(z, StubGeometryTileFeature(PropertyMap { { "x", static_cast<int64_t>(rand() % 100) } }), -1.0f);
    }
    
    state.SetLabel(std::to_string(stopCount).c_str());
}

BENCHMARK(Parse_CompositeFunction)
    ->Arg(1)->Arg(2)->Arg(4)->Arg(6)->Arg(8)->Arg(10)->Arg(12);

BENCHMARK(Evaluate_CompositeFunction)
    ->Arg(1)->Arg(2)->Arg(4)->Arg(6)->Arg(8)->Arg(10)->Arg(12);


