#pragma once

#include <mbgl/style/property_value.hpp>
#include <mbgl/style/transition_options.hpp>
#include <mbgl/style/types.hpp>
#include <mbgl/style/position.hpp>
#include <mbgl/util/color.hpp>
#include <mbgl/util/indexed_tuple.hpp>

namespace mbgl {
namespace style {

template <class T>
class LightProperty {
public:
    using Type = T;
    using ValueType = PropertyValue<T>;

    PropertyValue<T> value;
    TransitionOptions transition;
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

using LightProperties = TypeList<LightAnchor, LightPosition, LightColor, LightIntensity>;

} // namespace style
} // namespace mbgl
