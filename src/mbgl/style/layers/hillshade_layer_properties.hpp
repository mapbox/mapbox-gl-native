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

struct HillshadeShadowColor : PaintProperty<Color> {
    static Color defaultValue() { return Color::black(); }
};

struct HillshadeHighlightColor : PaintProperty<Color> {
    static Color defaultValue() { return Color::white(); }
};

struct HillshadeAccentColor : PaintProperty<Color> {
    static Color defaultValue() { return Color::black(); }
};

class HillshadePaintProperties : public Properties<
    HillshadeShadowColor,
    HillshadeHighlightColor,
    HillshadeAccentColor
> {};

} // namespace style
} // namespace mbgl
