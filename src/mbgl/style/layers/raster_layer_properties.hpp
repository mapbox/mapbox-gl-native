// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#pragma once

#include <mbgl/style/types.hpp>
#include <mbgl/style/layout_property.hpp>
#include <mbgl/style/paint_property.hpp>

namespace mbgl {
namespace style {

class CascadeParameters;
class CalculationParameters;

class RasterPaintProperties {
public:
    void cascade(const CascadeParameters&);
    bool recalculate(const CalculationParameters&);

    PaintProperty<float> rasterOpacity { 1 };
    PaintProperty<float> rasterHueRotate { 0 };
    PaintProperty<float> rasterBrightnessMin { 0 };
    PaintProperty<float> rasterBrightnessMax { 1 };
    PaintProperty<float> rasterSaturation { 0 };
    PaintProperty<float> rasterContrast { 0 };
    PaintProperty<float> rasterFadeDuration { 300 };
};

} // namespace style
} // namespace mbgl
