#include <llmr/style/properties.hpp>
#include <llmr/util/constants.hpp>


#include <cassert>
#include <cmath>

using namespace llmr;


float functions::null(float z, const std::vector<float>&) {
    return 0;
}

bool functions::null(float z, const std::vector<bool>&) {
    return false;
}

float functions::constant(float z, const std::vector<float>& values) {
    assert(values.size() == 1);
    return values.front();
}

bool functions::constant(float z, const std::vector<bool>& values) {
    assert(values.size() == 1);
    return values.front();
}


float functions::stops(float z, const std::vector<float>& stops) {
    // We need an even number of stop values.
    if (stops.size() % 2 != 0) return 0;

    // Accounts for us rendering tiles at another size, so our zoom levels are shifted appropriately.
    z += util::tileSize / 256.0f;

    bool smaller = false;
    float smaller_z, smaller_val;
    bool larger = false;
    float larger_z, larger_val;

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
        return smaller_val * pow(larger_val / smaller_val, factor);
    } else if (larger || smaller) {
        // Do not draw a line.
        return -std::numeric_limits<float>::infinity();

        // Exponential extrapolation of the smaller or larger value
        //var edge = larger || smaller;
        //return Math.pow(2, z) * (edge.val / Math.pow(2, edge.z));
    } else {
        // No stop defined.
        return 1;
    }
}

bool functions::stops(float z, const std::vector<bool>& values) {
    // TODO
    return false;
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

    return fmin(fmax(min, val + (z - z_base) * slope), max);
}

bool functions::linear(float z, const std::vector<bool>&) {
    // TODO
    return false;
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

    return fmin(fmax(min, val + pow(1.75, (z - z_base)) * slope), max);
    // return fmin(fmax(min, val + (z - z_base) * slope), max);
}

bool functions::exponential(float z, const std::vector<bool>&) {
    // TODO
    return false;
}
