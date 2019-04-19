// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#pragma once

#include <mbgl/style/types.hpp>
#include <mbgl/style/layer_properties.hpp>
#include <mbgl/style/layers/hillshade_layer.hpp>
#include <mbgl/style/layout_property.hpp>
#include <mbgl/style/paint_property.hpp>
#include <mbgl/style/properties.hpp>
#include <mbgl/programs/attributes.hpp>
#include <mbgl/programs/uniforms.hpp>

namespace mbgl {
namespace style {

struct HillshadeIlluminationDirection : PaintProperty<float> {
    static float defaultValue() { return 335; }
};

struct HillshadeIlluminationAnchor : PaintProperty<HillshadeIlluminationAnchorType> {
    static HillshadeIlluminationAnchorType defaultValue() { return HillshadeIlluminationAnchorType::Viewport; }
};

struct HillshadeExaggeration : PaintProperty<float> {
    static float defaultValue() { return 0.5; }
};

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
    HillshadeIlluminationDirection,
    HillshadeIlluminationAnchor,
    HillshadeExaggeration,
    HillshadeShadowColor,
    HillshadeHighlightColor,
    HillshadeAccentColor
> {};

class HillshadeLayerProperties final : public LayerProperties {
public:
    explicit HillshadeLayerProperties(Immutable<HillshadeLayer::Impl>);
    HillshadeLayerProperties(
        Immutable<HillshadeLayer::Impl>,
        HillshadePaintProperties::PossiblyEvaluated);
    ~HillshadeLayerProperties() override;

    const HillshadeLayer::Impl& layerImpl() const;
    // Data members.
    HillshadePaintProperties::PossiblyEvaluated evaluated;
};

} // namespace style
} // namespace mbgl
