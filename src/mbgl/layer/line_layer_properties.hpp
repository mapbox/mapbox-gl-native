// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#pragma once

#include <mbgl/style/layout_property.hpp>
#include <mbgl/style/paint_property.hpp>
#include <mbgl/util/rapidjson.hpp>

namespace mbgl {

class StyleCascadeParameters;
class StyleCalculationParameters;

class LineLayoutProperties {
public:
    void parse(const JSValue&);
    void recalculate(const StyleCalculationParameters&);

    LayoutProperty<LineCapType> lineCap { LineCapType::Butt };
    LayoutProperty<LineJoinType> lineJoin { LineJoinType::Miter };
    LayoutProperty<float> lineMiterLimit { 2 };
    LayoutProperty<float> lineRoundLimit { 1.05 };
};

class LinePaintProperties {
public:
    void parse(const JSValue&);
    void cascade(const StyleCascadeParameters&);
    bool recalculate(const StyleCalculationParameters&);

    PaintProperty<float> lineOpacity { 1 };
    PaintProperty<Color> lineColor { {{ 0, 0, 0, 1 }} };
    PaintProperty<std::array<float, 2>> lineTranslate { {{ 0, 0 }} };
    PaintProperty<TranslateAnchorType> lineTranslateAnchor { TranslateAnchorType::Map };
    PaintProperty<float> lineWidth { 1 };
    PaintProperty<float> lineGapWidth { 0 };
    PaintProperty<float> lineOffset { 0 };
    PaintProperty<float> lineBlur { 0 };
    PaintProperty<std::vector<float>, CrossFadedFunctionEvaluator> lineDasharray { {  } };
    PaintProperty<std::string, CrossFadedFunctionEvaluator> linePattern { "" };
};

} // namespace mbgl
