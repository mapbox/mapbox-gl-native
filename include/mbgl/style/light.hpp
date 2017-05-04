// This file is generated. Do not edit.

#pragma once
#include <mbgl/style/property_value.hpp>
#include <mbgl/style/transition_options.hpp>
#include <mbgl/style/types.hpp>

#include <memory>

namespace mbgl {
namespace style {

class Light {
public:

    class Impl;

    Light();
    ~Light();

    static LightAnchorType getDefaultAnchor();
    PropertyValue<LightAnchorType> getAnchor() const;
    void setAnchor(PropertyValue<LightAnchorType>);
    void setAnchorTransition(const TransitionOptions&);
    TransitionOptions getAnchorTransition() const;

    static Position getDefaultPosition();
    PropertyValue<Position> getPosition() const;
    void setPosition(PropertyValue<Position>);
    void setPositionTransition(const TransitionOptions&);
    TransitionOptions getPositionTransition() const;

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

    std::shared_ptr<Impl> impl;
};

} // namespace style
} // namespace mbgl
