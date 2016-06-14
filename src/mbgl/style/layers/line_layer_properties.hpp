// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#pragma once

#include <mbgl/style/layout_property.hpp>
#include <mbgl/style/paint_property.hpp>
#include <mbgl/util/rapidjson.hpp>

namespace mbgl {
namespace style {

class CascadeParameters;
class CalculationParameters;

class LineLayoutProperties {
public:
    void parse(const JSValue&);
    void recalculate(const CalculationParameters&);

    LayoutProperty<LineCapType> lineCap { LineCapType::Butt };
    LayoutProperty<LineJoinType> lineJoin { LineJoinType::Miter };
    LayoutProperty<float> lineMiterLimit { 2 };
    LayoutProperty<float> lineRoundLimit { 1 };
};

class LinePaintProperties {
public:
    void parse(const JSValue&);
    void cascade(const CascadeParameters&);
    bool recalculate(const CalculationParameters&);

    PaintProperty<float> lineOpacity { 1 };
    PaintProperty<Color> lineColor { Color(0, 0, 0, 1) };
    PaintProperty<std::array<float, 2>> lineTranslate { {{ 0, 0 }} };
    PaintProperty<TranslateAnchorType> lineTranslateAnchor { TranslateAnchorType::Map };
    PaintProperty<float> lineWidth { 1 };
    PaintProperty<float> lineGapWidth { 0 };
    PaintProperty<float> lineOffset { 0 };
    PaintProperty<float> lineBlur { 0 };
    PaintProperty<std::vector<float>, CrossFadedPropertyEvaluator> lineDasharray { {  } };
    PaintProperty<std::string, CrossFadedPropertyEvaluator> linePattern { "" };
};

} // namespace style
} // namespace mbgl
