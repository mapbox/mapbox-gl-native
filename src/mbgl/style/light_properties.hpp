// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#pragma once

#include <mbgl/style/light_property.hpp>
#include <mbgl/style/types.hpp>
#include <mbgl/style/position.hpp>
#include <mbgl/style/properties.hpp>
#include <mbgl/util/color.hpp>

namespace mbgl {
namespace style {

struct LightAnchor : LightProperty<LightAnchorType> {
    static LightAnchorType defaultValue() { return LightAnchorType::Viewport; }
};

struct LightPosition : LightProperty<Position> {
    static Position defaultValue() { return {{ 1.15, 210, 30 }}; }
};

struct LightColor : LightProperty<Color> {
    static Color defaultValue() { return Color::white(); }
};

struct LightIntensity : LightProperty<float> {
    static float defaultValue() { return 0.5; }
};

class LightProperties {
public:
    class Evaluated {
    public:
        LightAnchorType anchor;
        Position position;
        Color color;
        float intensity;
    };

    class Unevaluated {
    public:
        style::Transitioning<PropertyValue<LightAnchorType>> anchor;
        style::Transitioning<PropertyValue<Position>> position;
        style::Transitioning<PropertyValue<Color>> color;
        style::Transitioning<PropertyValue<float>> intensity;

        bool hasTransition() const;
        Evaluated evaluate(const PropertyEvaluationParameters&) const;
    };

    class Transitionable {
    public:
        style::Transitionable<PropertyValue<LightAnchorType>> anchor;
        style::Transitionable<PropertyValue<Position>> position;
        style::Transitionable<PropertyValue<Color>> color;
        style::Transitionable<PropertyValue<float>> intensity;

        Unevaluated transitioned(const TransitionParameters&, Unevaluated&& prior) const;
        Unevaluated untransitioned() const;
    };
};

} // namespace style
} // namespace mbgl
