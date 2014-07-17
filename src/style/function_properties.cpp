#include <mbgl/style/function_properties.hpp>
#include <mbgl/style/types.hpp>

#include <cmath>

namespace mbgl {


template <>
bool LinearFunction<bool>::evaluate(float z) const {
    return z < z_base ? slope >= 0 : z > z_base ? slope >= 0 : value;
}

template <>
float LinearFunction<float>::evaluate(float z) const {
    return std::fmin(std::fmax(min, value + (z - z_base) * slope), max);
}

template <>
Color LinearFunction<Color>::evaluate(float z) const {
    return {{
        std::fmin(std::fmax(min[0], value[0] + (z - z_base) * slope), max[0]),
        std::fmin(std::fmax(min[1], value[1] + (z - z_base) * slope), max[1]),
        std::fmin(std::fmax(min[2], value[2] + (z - z_base) * slope), max[2]),
        std::fmin(std::fmax(min[3], value[3] + (z - z_base) * slope), max[3])
    }};
}


template <>
bool ExponentialFunction<bool>::evaluate(float z) const {
    return z < z_base ? slope >= 0 : z > z_base ? slope >= 0 : value;
}

template <>
float ExponentialFunction<float>::evaluate(float z) const {
    return std::fmin(std::fmax(min, value + std::pow(exp_base, (z - z_base)) * slope), max);
}

template <>
Color ExponentialFunction<Color>::evaluate(float z) const {
    return {{
        std::fmin(std::fmax(min[0], value[0] + float(std::pow(exp_base, (z - z_base))) * slope), max[0]),
        std::fmin(std::fmax(min[1], value[1] + float(std::pow(exp_base, (z - z_base))) * slope), max[1]),
        std::fmin(std::fmax(min[2], value[2] + float(std::pow(exp_base, (z - z_base))) * slope), max[2]),
        std::fmin(std::fmax(min[3], value[3] + float(std::pow(exp_base, (z - z_base))) * slope), max[3])
    }};
}

template <typename T>
inline T exponentialInterpolate(T smaller, T larger, const float factor);

template <>
inline float exponentialInterpolate(const float smaller, const float larger, const float factor) {
    // Linear interpolation if base is 0
    if (smaller == 0.0f) {
        return factor * larger;
    }
    // Exponential interpolation between the values
    return smaller * std::pow(larger / smaller, factor);
}

template <>
inline bool exponentialInterpolate(const bool smaller, const bool larger, const float factor) {
    return exponentialInterpolate(float(smaller), float(larger), factor);
}

template <>
inline Color exponentialInterpolate(const Color smaller, const Color larger, const float factor) {
    return {{
        exponentialInterpolate(smaller[0], larger[0], factor),
        exponentialInterpolate(smaller[1], larger[1], factor),
        exponentialInterpolate(smaller[2], larger[2], factor),
        exponentialInterpolate(smaller[3], larger[3], factor)
    }};
}


template <typename T>
T exponentialDefault();

template <> bool exponentialDefault() { return true; }
template <> float exponentialDefault() { return 1.0f; }
template <> Color exponentialDefault() { return {{ 0, 0, 0, 1 }}; }


template <typename T>
T StopsFunction<T>::evaluate(float z) const {
    bool smaller = false;
    float smaller_z = 0.0f;
    T smaller_val {};
    bool larger = false;
    float larger_z = 0.0f;
    T larger_val {};

    for (uint32_t i = 0; i < values.size(); i++) {
        float stop_z = values[i].first;
        T stop_val = values[i].second;
        if (stop_z <= z && (!smaller || smaller_z < stop_z)) {
            smaller = true;
            smaller_z = stop_z;
            smaller_val = stop_val;
        }
        if (stop_z >= z && (!larger || larger_z > stop_z)) {
            larger = true;
            larger_z = stop_z;
            larger_val = stop_val;
        }
    }

    if (smaller && larger) {
        if (larger_z == smaller_z || larger_val == smaller_val) {
            return smaller_val;
        }
        float factor = (z - smaller_z) / (larger_z - smaller_z);
        return exponentialInterpolate<T>(smaller_val, larger_val, factor);
    } else if (larger) {
        return larger_val;
    } else if (smaller) {
        return smaller_val;
    } else {
        // No stop defined.
        return exponentialDefault<T>();
    }
}

template bool StopsFunction<bool>::evaluate(float z) const;
template float StopsFunction<float>::evaluate(float z) const;
template Color StopsFunction<Color>::evaluate(float z) const;

}
