#include <benchmark/benchmark.h>

#include <mbgl/benchmark/stub_geometry_tile_feature.hpp>

#include <mbgl/style/function/composite_exponential_stops.hpp>
#include <mbgl/style/function/composite_function.hpp>

#include <mbgl/style/rapidjson_conversion.hpp>
#include <mbgl/style/conversion.hpp>
#include <mbgl/style/conversion/function.hpp>

#include <rapidjson/document.h>


using namespace mbgl;
using namespace mbgl::style;

static rapidjson::GenericDocument<rapidjson::UTF8<>, rapidjson::CrtAllocator> createFunctionJSON(size_t stopCount) {
    rapidjson::GenericDocument<rapidjson::UTF8<>, rapidjson::CrtAllocator> doc;

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
    
    doc.Parse<0>(R"({"type": "exponential", "base": 2,  "stops": )" + stops + R"(, "property": "x"})");
    return doc;
}

static void Parse_CompositeFunction(benchmark::State& state) {
    size_t stopCount = state.range(0);
    
    while (state.KeepRunning()) {
        conversion::Error error;
        state.PauseTiming();
        auto doc = createFunctionJSON(stopCount);
        state.ResumeTiming();
        optional<CompositeFunction<float>> result = conversion::convert<style::CompositeFunction<float>, JSValue>(doc, error);
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
    optional<CompositeFunction<float>> function = conversion::convert<CompositeFunction<float>, JSValue>(doc, error);
    if (!function) {
        state.SkipWithError(error.message.c_str());
    }
    
    while(state.KeepRunning()) {
        float z = 24.0f * static_cast<float>(rand() % 100) / 100;
        function->evaluate(z, StubGeometryTileFeature(PropertyMap { { "x", static_cast<int64_t>(rand() % 100) } }), -1.0f);
    }
    
    state.SetLabel(std::to_string(stopCount).c_str());
}

BENCHMARK(Parse_CompositeFunction)
    ->Arg(1)->Arg(2)->Arg(4)->Arg(6)->Arg(8)->Arg(10)->Arg(12);

BENCHMARK(Evaluate_CompositeFunction)
    ->Arg(1)->Arg(2)->Arg(4)->Arg(6)->Arg(8)->Arg(10)->Arg(12);


