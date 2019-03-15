// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#pragma once

#include <mbgl/style/types.hpp>
#include <mbgl/style/layout_property.hpp>
#include <mbgl/style/paint_property.hpp>
#include <mbgl/style/properties.hpp>
#include <mbgl/programs/attributes.hpp>
#include <mbgl/programs/uniforms.hpp>

namespace mbgl {
namespace style {

struct HeatmapRadius : DataDrivenPaintProperty<float, attributes::radius, uniforms::radius> {
    static float defaultValue() { return 30; }
};

struct HeatmapWeight : DataDrivenPaintProperty<float, attributes::weight, uniforms::weight> {
    static float defaultValue() { return 1; }
};

struct HeatmapIntensity : PaintProperty<float> {
    static float defaultValue() { return 1; }
};

struct HeatmapColor : ColorRampProperty {
};

struct HeatmapOpacity : PaintProperty<float> {
    static float defaultValue() { return 1; }
};

class HeatmapPaintProperties : public Properties<
    HeatmapRadius,
    HeatmapWeight,
    HeatmapIntensity,
    HeatmapColor,
    HeatmapOpacity
> {};

} // namespace style
} // namespace mbgl
