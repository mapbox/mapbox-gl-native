#pragma once

#include <mbgl/style/light.hpp>
#include <mbgl/style/property_value.hpp>
#include <mbgl/style/types.hpp>
#include <mbgl/style/position.hpp>
#include <mbgl/style/properties.hpp>
#include <mbgl/renderer/property_evaluator.hpp>
#include <mbgl/util/color.hpp>
#include <mbgl/util/indexed_tuple.hpp>

namespace mbgl {
namespace style {

template <class T>
class LightProperty {
public:
    using TransitionableType = Transitionable<PropertyValue<T>>;
    using UnevaluatedType = Transitioning<PropertyValue<T>>;
    using EvaluatorType = PropertyEvaluator<T>;
    using PossiblyEvaluatedType = T;
    using Type = T;
    static constexpr bool IsDataDriven = false;
};

struct LightAnchor : LightProperty<LightAnchorType> {
    static LightAnchorType defaultValue() {
        return LightAnchorType::Viewport;
    }
};

struct LightPosition : LightProperty<Position> {
    static Position defaultValue() {
        std::array<float, 3> default_ = { { 1.15, 210, 30 } };
        return Position{ { default_ } };
    }
};

struct LightColor : LightProperty<Color> {
    static Color defaultValue() {
        return Color::white();
    }
};

struct LightIntensity : LightProperty<float> {
    static float defaultValue() {
        return 0.5;
    }
};

using LightProperties = Properties<LightAnchor, LightPosition, LightColor, LightIntensity>;

class Light::Impl {
public:
    LightProperties::Transitionable properties;
};

} // namespace style
} // namespace mbgl
