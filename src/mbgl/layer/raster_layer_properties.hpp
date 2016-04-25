// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#pragma once

#include <mbgl/style/layout_property.hpp>
#include <mbgl/style/paint_property.hpp>
#include <mbgl/util/rapidjson.hpp>

namespace mbgl {

class StyleCascadeParameters;
class StyleCalculationParameters;

class RasterPaintProperties {
public:
    void parse(const JSValue&);
    void cascade(const StyleCascadeParameters&);
    bool recalculate(const StyleCalculationParameters&);

    PaintProperty<float> rasterOpacity { 1 };
    PaintProperty<float> rasterHueRotate { 0 };
    PaintProperty<float> rasterBrightnessMin { 0 };
    PaintProperty<float> rasterBrightnessMax { 1 };
    PaintProperty<float> rasterSaturation { 0 };
    PaintProperty<float> rasterContrast { 0 };
    PaintProperty<float> rasterFadeDuration { 300 };
};

} // namespace mbgl
