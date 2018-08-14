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

class HillshadePaintProperties {
public:

    class PossiblyEvaluated {
    public:
        float hillshadeIlluminationDirection;
        HillshadeIlluminationAnchorType hillshadeIlluminationAnchor;
        float hillshadeExaggeration;
        Color hillshadeShadowColor;
        Color hillshadeHighlightColor;
        Color hillshadeAccentColor;
    };

    class Unevaluated {
    public:
        style::Transitioning<PropertyValue<float>> hillshadeIlluminationDirection;
        style::Transitioning<PropertyValue<HillshadeIlluminationAnchorType>> hillshadeIlluminationAnchor;
        style::Transitioning<PropertyValue<float>> hillshadeExaggeration;
        style::Transitioning<PropertyValue<Color>> hillshadeShadowColor;
        style::Transitioning<PropertyValue<Color>> hillshadeHighlightColor;
        style::Transitioning<PropertyValue<Color>> hillshadeAccentColor;

        bool hasTransition() const;
        PossiblyEvaluated evaluate(const PropertyEvaluationParameters&) const;
    };

    class Transitionable {
    public:
        style::Transitionable<PropertyValue<float>> hillshadeIlluminationDirection;
        style::Transitionable<PropertyValue<HillshadeIlluminationAnchorType>> hillshadeIlluminationAnchor;
        style::Transitionable<PropertyValue<float>> hillshadeExaggeration;
        style::Transitionable<PropertyValue<Color>> hillshadeShadowColor;
        style::Transitionable<PropertyValue<Color>> hillshadeHighlightColor;
        style::Transitionable<PropertyValue<Color>> hillshadeAccentColor;

        Unevaluated transitioned(const TransitionParameters&, Unevaluated&& prior) const;
        Unevaluated untransitioned() const;

        bool hasDataDrivenPropertyDifference(const Transitionable& other) const;
    };
};


} // namespace style
} // namespace mbgl
