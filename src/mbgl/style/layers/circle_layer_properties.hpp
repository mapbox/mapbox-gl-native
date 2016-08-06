// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#pragma once

#include <mbgl/style/types.hpp>
#include <mbgl/style/layout_property.hpp>
#include <mbgl/style/paint_property.hpp>

namespace mbgl {
namespace style {

class CascadeParameters;
class CalculationParameters;

class CirclePaintProperties {
public:
    void cascade(const CascadeParameters&);
    bool recalculate(const CalculationParameters&);

    PaintProperty<float> circleRadius { 5 };
    PaintProperty<Color> circleColor { Color::black() };
    PaintProperty<float> circleBlur { 0 };
    PaintProperty<float> circleOpacity { 1 };
    PaintProperty<std::array<float, 2>> circleTranslate { {{ 0, 0 }} };
    PaintProperty<TranslateAnchorType> circleTranslateAnchor { TranslateAnchorType::Map };
    PaintProperty<CirclePitchScaleType> circlePitchScale { CirclePitchScaleType::Map };
};

} // namespace style
} // namespace mbgl
