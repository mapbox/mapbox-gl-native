// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#pragma once

#include <mbgl/style/layout_property.hpp>
#include <mbgl/style/paint_property.hpp>
#include <mbgl/util/rapidjson.hpp>

namespace mbgl {

class StyleCascadeParameters;
class StyleCalculationParameters;

class BackgroundPaintProperties {
public:
    void parse(const JSValue&);
    void cascade(const StyleCascadeParameters&);
    bool recalculate(const StyleCalculationParameters&);

    PaintProperty<Color> backgroundColor { {{ 0, 0, 0, 1 }} };
    PaintProperty<std::string, CrossFadedPropertyEvaluator> backgroundPattern { "" };
    PaintProperty<float> backgroundOpacity { 1 };
};

} // namespace mbgl
