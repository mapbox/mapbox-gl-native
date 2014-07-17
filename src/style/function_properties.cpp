#include <mbgl/style/function_properties.hpp>
#include <mbgl/style/types.hpp>

#include <cmath>

namespace mbgl {


template <typename T>
inline T interpolate(T smaller, T larger, const float factor);

template <>
inline float interpolate(const float smaller, const float larger, const float factor) {
    return (smaller * (1 - factor)) + (larger * factor);
}

template <>
inline bool interpolate(const bool smaller, const bool larger, const float factor) {
    return interpolate(float(smaller), float(larger), factor);
}

template <>
inline Color interpolate(const Color smaller, const Color larger, const float factor) {
    return {{
        interpolate(smaller[0], larger[0], factor),
        interpolate(smaller[1], larger[1], factor),
        interpolate(smaller[2], larger[2], factor),
        interpolate(smaller[3], larger[3], factor)
    }};
}


template <typename T>
inline T defaultStopsValue();

template <> inline bool defaultStopsValue() { return true; }
template <> inline float defaultStopsValue() { return 1.0f; }
template <> inline Color defaultStopsValue() { return {{ 0, 0, 0, 1 }}; }


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
        const float zoomDiff = larger_z - smaller_z;
        const float zoomProgress = z - smaller_z;
        if (base == 1.0f) {
            const float t = zoomProgress / zoomDiff;
            return interpolate<T>(smaller_val, larger_val, t);
        } else {
            const float t = (std::pow(base, zoomProgress) - 1) / (std::pow(base, zoomDiff) - 1);
            return interpolate<T>(smaller_val, larger_val, t);
        }
    } else if (larger) {
        return larger_val;
    } else if (smaller) {
        return smaller_val;
    } else {
        // No stop defined.
        return defaultStopsValue<T>();
    }
}

template bool StopsFunction<bool>::evaluate(float z) const;
template float StopsFunction<float>::evaluate(float z) const;
template Color StopsFunction<Color>::evaluate(float z) const;

}
