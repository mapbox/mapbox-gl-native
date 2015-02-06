#include <mbgl/style/function_properties.hpp>
#include <mbgl/style/types.hpp>
#include <mbgl/util/interpolate.hpp>

#include <cmath>

namespace mbgl {

template <typename T>
inline T defaultStopsValue();

template <> inline bool defaultStopsValue() { return true; }
template <> inline float defaultStopsValue() { return 1.0f; }
template <> inline Color defaultStopsValue() { return {{ 0, 0, 0, 1 }}; }
template <> inline std::vector<float> defaultStopsValue() { return {{ 1, 0 }}; }
template <> inline std::array<float, 2> defaultStopsValue() { return {{ 0, 0 }}; }

template <> inline std:: string defaultStopsValue() { return {}; }
template <> inline TranslateAnchorType defaultStopsValue() { return {}; };
template <> inline RotateAnchorType defaultStopsValue() { return {}; };
template <> inline CapType defaultStopsValue() { return {}; };
template <> inline JoinType defaultStopsValue() { return {}; };
template <> inline PlacementType defaultStopsValue() { return {}; };
template <> inline TextAnchorType defaultStopsValue() { return {}; };
template <> inline TextJustifyType defaultStopsValue() { return {}; };
template <> inline TextTransformType defaultStopsValue() { return {}; };
template <> inline RotationAlignmentType defaultStopsValue() { return {}; };

template <typename T>
T StopsFunction<T>::evaluate(float z) const {
    bool smaller = false;
    float smaller_z = 0.0f;
    T smaller_val = T();
    bool larger = false;
    float larger_z = 0.0f;
    T larger_val = T();

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
            return util::interpolate(smaller_val, larger_val, t);
        } else {
            const float t = (std::pow(base, zoomProgress) - 1) / (std::pow(base, zoomDiff) - 1);
            return util::interpolate(smaller_val, larger_val, t);
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
template std::vector<float> StopsFunction<std::vector<float>>::evaluate(float z) const;
template std::array<float, 2> StopsFunction<std::array<float, 2>>::evaluate(float z) const;

template std::string StopsFunction<std::string>::evaluate(float z) const;
template TranslateAnchorType StopsFunction<TranslateAnchorType>::evaluate(float z) const;
template RotateAnchorType StopsFunction<RotateAnchorType>::evaluate(float z) const;
template CapType StopsFunction<CapType>::evaluate(float z) const;
template JoinType StopsFunction<JoinType>::evaluate(float z) const;
template PlacementType StopsFunction<PlacementType>::evaluate(float z) const;
template TextAnchorType StopsFunction<TextAnchorType>::evaluate(float z) const;
template TextJustifyType StopsFunction<TextJustifyType>::evaluate(float z) const;
template TextTransformType StopsFunction<TextTransformType>::evaluate(float z) const;
template RotationAlignmentType StopsFunction<RotationAlignmentType>::evaluate(float z) const;
}
