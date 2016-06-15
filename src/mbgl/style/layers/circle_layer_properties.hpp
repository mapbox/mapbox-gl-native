// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#pragma once

#include <mbgl/style/layout_property.hpp>
#include <mbgl/style/paint_property.hpp>
#include <mbgl/util/rapidjson.hpp>

namespace mbgl {
namespace style {

class CascadeParameters;
class CalculationParameters;

class CirclePaintProperties {
public:
    void parse(const JSValue&);
    void cascade(const CascadeParameters&);
    bool recalculate(const CalculationParameters&);

    PaintProperty<float> circleRadius { 5 };
    PaintProperty<Color> circleColor { { 0, 0, 0, 1 } };
    PaintProperty<float> circleBlur { 0 };
    PaintProperty<float> circleOpacity { 1 };
    PaintProperty<std::array<float, 2>> circleTranslate { {{ 0, 0 }} };
    PaintProperty<TranslateAnchorType> circleTranslateAnchor { TranslateAnchorType::Map };
};

} // namespace style
} // namespace mbgl
