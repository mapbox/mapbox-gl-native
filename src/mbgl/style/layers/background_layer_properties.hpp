// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#pragma once

#include <mbgl/style/types.hpp>
#include <mbgl/style/layout_property.hpp>
#include <mbgl/style/paint_property.hpp>

namespace mbgl {
namespace style {

class CascadeParameters;
class CalculationParameters;

class BackgroundPaintProperties {
public:
    void cascade(const CascadeParameters&);
    bool recalculate(const CalculationParameters&);

    PaintProperty<Color> backgroundColor { Color::black() };
    PaintProperty<std::string, CrossFadedPropertyEvaluator> backgroundPattern { "" };
    PaintProperty<float> backgroundOpacity { 1 };
};

} // namespace style
} // namespace mbgl
