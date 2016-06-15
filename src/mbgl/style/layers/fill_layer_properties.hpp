// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#pragma once

#include <mbgl/style/layout_property.hpp>
#include <mbgl/style/paint_property.hpp>
#include <mbgl/util/rapidjson.hpp>

namespace mbgl {
namespace style {

class CascadeParameters;
class CalculationParameters;

class FillPaintProperties {
public:
    void parse(const JSValue&);
    void cascade(const CascadeParameters&);
    bool recalculate(const CalculationParameters&);

    PaintProperty<bool> fillAntialias { true };
    PaintProperty<float> fillOpacity { 1 };
    PaintProperty<Color> fillColor { { 0, 0, 0, 1 } };
    PaintProperty<Color> fillOutlineColor { { 0, 0, 0, -1 } };
    PaintProperty<std::array<float, 2>> fillTranslate { {{ 0, 0 }} };
    PaintProperty<TranslateAnchorType> fillTranslateAnchor { TranslateAnchorType::Map };
    PaintProperty<std::string, CrossFadedPropertyEvaluator> fillPattern { "" };
};

} // namespace style
} // namespace mbgl
