// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#pragma once

#include <mbgl/style/types.hpp>
#include <mbgl/style/layout_property.hpp>
#include <mbgl/style/paint_property.hpp>
#include <mbgl/programs/attributes.hpp>

namespace mbgl {
namespace style {

struct RasterOpacity : PaintProperty<float> {
    static float defaultValue() { return 1; }
};

struct RasterHueRotate : PaintProperty<float> {
    static float defaultValue() { return 0; }
};

struct RasterBrightnessMin : PaintProperty<float> {
    static float defaultValue() { return 0; }
};

struct RasterBrightnessMax : PaintProperty<float> {
    static float defaultValue() { return 1; }
};

struct RasterSaturation : PaintProperty<float> {
    static float defaultValue() { return 0; }
};

struct RasterContrast : PaintProperty<float> {
    static float defaultValue() { return 0; }
};

struct RasterFadeDuration : PaintProperty<float> {
    static float defaultValue() { return 300; }
};

class RasterPaintProperties : public PaintProperties<
    RasterOpacity,
    RasterHueRotate,
    RasterBrightnessMin,
    RasterBrightnessMax,
    RasterSaturation,
    RasterContrast,
    RasterFadeDuration
> {};

} // namespace style
} // namespace mbgl
