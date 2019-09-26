// clang-format off

// This file is generated. Do not edit.

#pragma once

#include <mbgl/style/conversion.hpp>
#include <mbgl/style/property_value.hpp>
#include <mbgl/style/style_property.hpp>
#include <mbgl/style/transition_options.hpp>
#include <mbgl/style/types.hpp>
#include <mbgl/util/immutable.hpp>

namespace mbgl {
namespace style {

class LightObserver;

class Light {
public:
    Light();
    ~Light();

    // Dynamic properties
    optional<conversion::Error> setProperty(const std::string& name, const conversion::Convertible& value);
    StyleProperty getProperty(const std::string&) const;

    static LightAnchorType getDefaultAnchor();
    PropertyValue<LightAnchorType> getAnchor() const;
    void setAnchor(PropertyValue<LightAnchorType>);
    void setAnchorTransition(const TransitionOptions&);
    TransitionOptions getAnchorTransition() const;

    static Color getDefaultColor();
    PropertyValue<Color> getColor() const;
    void setColor(PropertyValue<Color>);
    void setColorTransition(const TransitionOptions&);
    TransitionOptions getColorTransition() const;

    static float getDefaultIntensity();
    PropertyValue<float> getIntensity() const;
    void setIntensity(PropertyValue<float>);
    void setIntensityTransition(const TransitionOptions&);
    TransitionOptions getIntensityTransition() const;

    static Position getDefaultPosition();
    PropertyValue<Position> getPosition() const;
    void setPosition(PropertyValue<Position>);
    void setPositionTransition(const TransitionOptions&);
    TransitionOptions getPositionTransition() const;

    class Impl;
    Immutable<Impl> impl;
    Mutable<Impl> mutableImpl() const;

    LightObserver* observer = nullptr;
    void setObserver(LightObserver*);
};

} // namespace style
} // namespace mbgl

// clang-format on
