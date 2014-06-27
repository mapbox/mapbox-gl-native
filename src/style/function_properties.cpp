#include <llmr/style/function_properties.hpp>

#include <cmath>

namespace llmr {

float functions::null(float, const std::vector<float>&) {
    return 0;
}

float functions::constant(float, const std::vector<float>& values) {
    return values.size() >= 1 ? values.front() : 0.0f;
}

float functions::min(float z, const std::vector<float>& values) {
    return values.size() >= 1 && z >= values.front();
}

float functions::max(float z, const std::vector<float>& values) {
    return values.size() >= 1 && z <= values.front();
}

float functions::stops(float z, const std::vector<float>& stops) {
    // We need an even number of stop values.
    if (stops.size() % 2 != 0) return 0;

    bool smaller = false;
    float smaller_z = 0.0;
    float smaller_val = 0.0;
    bool larger = false;
    float larger_z = 0.0;
    float larger_val = 0.0;

    for (uint32_t i = 0; i < stops.size(); i += 2) {
        float stop_z = stops[i];
        float stop_val = stops[i + 1];
        if (stop_z <= z && (!smaller || smaller_z < stop_z)) { smaller = true; smaller_z = stop_z; smaller_val = stop_val; }
        if (stop_z >= z && (!larger || larger_z > stop_z)) { larger = true; larger_z = stop_z; larger_val = stop_val; }
    }

    if (smaller && larger) {
        if (larger_z == smaller_z || larger_val == smaller_val) return smaller_val;
        float factor = (z - smaller_z) / (larger_z - smaller_z);
        // Linear interpolation if base is 0
        if (smaller_val == 0) return factor * larger_val;
        // Exponential interpolation between the values
        return smaller_val * std::pow(larger_val / smaller_val, factor);
    } else if (larger) {
        return larger_val;
    } else if (smaller) {
        return smaller_val;
    } else {
        // No stop defined.
        return 1;
    }
}

float functions::linear(float z, const std::vector<float>& values) {
    if (values.size() != 5) {
        return 0;
    }

    const float z_base = values[0];
    const float val = values[1];
    const float slope = values[2];
    const float min = values[3];
    const float max = values[4];

    return std::fmin(std::fmax(min, val + (z - z_base) * slope), max);
}

float functions::exponential(float z, const std::vector<float>& values) {
    if (values.size() != 5) {
        return 0;
    }

    const float z_base = values[0];
    const float val = values[1];
    const float slope = values[2];
    const float min = values[3];
    const float max = values[4];

    return std::fmin(std::fmax(min, val + std::pow(1.75, (z - z_base)) * slope), max);
}

}
