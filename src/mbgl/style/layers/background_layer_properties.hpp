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

struct BackgroundColor : PaintProperty<Color> {
    static Color defaultValue() { return Color::black(); }
};

struct BackgroundPattern : CrossFadedPaintProperty<std::string> {
    static std::string defaultValue() { return ""; }
};

struct BackgroundOpacity : PaintProperty<float> {
    static float defaultValue() { return 1; }
};

class BackgroundPaintProperties {
public:

    class PossiblyEvaluated {
    public:
        Color backgroundColor;
        Faded<std::string> backgroundPattern;
        float backgroundOpacity;
    };

    class Unevaluated {
    public:
        style::Transitioning<PropertyValue<Color>> backgroundColor;
        style::Transitioning<PropertyValue<std::string>> backgroundPattern;
        style::Transitioning<PropertyValue<float>> backgroundOpacity;

        bool hasTransition() const;
        PossiblyEvaluated evaluate(const PropertyEvaluationParameters&) const;
    };

    class Transitionable {
    public:
        style::Transitionable<PropertyValue<Color>> backgroundColor;
        style::Transitionable<PropertyValue<std::string>> backgroundPattern;
        style::Transitionable<PropertyValue<float>> backgroundOpacity;

        Unevaluated transitioned(const TransitionParameters&, Unevaluated&& prior) const;
        Unevaluated untransitioned() const;

        bool hasDataDrivenPropertyDifference(const Transitionable& other) const;
    };
};


} // namespace style
} // namespace mbgl
