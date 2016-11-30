#include <mbgl/style/function.hpp>
#include <mbgl/style/types.hpp>
#include <mbgl/util/color.hpp>
#include <mbgl/util/interpolate.hpp>

#include <cmath>

namespace mbgl {
namespace style {

template <typename T>
T Function<T>::evaluate(float z) const {
    bool smaller = false;
    float smaller_z = 0.0f;
    T smaller_val = T();
    bool larger = false;
    float larger_z = 0.0f;
    T larger_val = T();

    for (uint32_t i = 0; i < stops.size(); i++) {
        float stop_z = stops[i].first;
        T stop_val = stops[i].second;
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
        assert(false);
        return T();
    }
}

template class Function<bool>;
template class Function<float>;
template class Function<Color>;
template class Function<std::vector<float>>;
template class Function<std::vector<std::string>>;
template class Function<std::array<float, 2>>;
template class Function<std::array<float, 4>>;

template class Function<std::string>;
template class Function<TranslateAnchorType>;
template class Function<RotateAnchorType>;
template class Function<CirclePitchScaleType>;
template class Function<LineCapType>;
template class Function<LineJoinType>;
template class Function<SymbolPlacementType>;
template class Function<TextAnchorType>;
template class Function<TextJustifyType>;
template class Function<TextTransformType>;
template class Function<AlignmentType>;
template class Function<IconTextFitType>;

} // namespace style
} // namespace mbgl
