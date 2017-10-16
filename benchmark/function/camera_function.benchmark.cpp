#include <benchmark/benchmark.h>

#include <mbgl/style/function/source_function.hpp>
#include <mbgl/style/conversion.hpp>
#include <mbgl/style/conversion/json.hpp>
#include <mbgl/style/conversion/function.hpp>

using namespace mbgl;
using namespace mbgl::style;

static std::string createFunctionJSON(size_t stopCount) {
    std::string stops = "[";
    for (size_t i = 0; i < stopCount; i++) {
        std::string value = std::to_string(24.0f / stopCount * i);
        if (stops.size() > 1) stops += ",";
        stops += "[" + value + ", " + value + "]";
    }
    stops += "]";
    return R"({"type": "exponential", "base": 2,  "stops": )" + stops + "}";
}

static void Parse_CameraFunction(benchmark::State& state) {
    size_t stopCount = state.range(0);
    
    while (state.KeepRunning()) {
        conversion::Error error;
        state.PauseTiming();
        auto doc = createFunctionJSON(stopCount);
        state.ResumeTiming();
        optional<CameraFunction<float>> result = conversion::convertJSON<CameraFunction<float>>(doc, error);
        if (!result) {
            state.SkipWithError(error.message.c_str());
        }
    }
    state.SetLabel(std::to_string(stopCount).c_str());
}

static void Evaluate_CameraFunction(benchmark::State& state) {
    size_t stopCount = state.range(0);
    auto doc = createFunctionJSON(stopCount);
    conversion::Error error;
    optional<CameraFunction<float>> function = conversion::convertJSON<CameraFunction<float>>(doc, error);
    if (!function) {
        state.SkipWithError(error.message.c_str());
    }
    
    while(state.KeepRunning()) {
        float z = 24.0f * static_cast<float>(rand() % 100) / 100;
        function->evaluate(z);
    }
    
    state.SetLabel(std::to_string(stopCount).c_str());
}

BENCHMARK(Parse_CameraFunction)
    ->Arg(1)->Arg(2)->Arg(4)->Arg(6)->Arg(8)->Arg(10)->Arg(12);

BENCHMARK(Evaluate_CameraFunction)
    ->Arg(1)->Arg(2)->Arg(4)->Arg(6)->Arg(8)->Arg(10)->Arg(12);


