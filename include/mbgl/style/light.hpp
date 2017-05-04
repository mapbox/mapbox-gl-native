// This file is generated. Do not edit.

#pragma once
#include <mbgl/style/property_value.hpp>
#include <mbgl/style/transition_options.hpp>
#include <mbgl/style/types.hpp>

namespace mbgl {

class RenderLight;

namespace style {

class Light {
public:

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

private:
    IndexedTuple<LightProperties, LightProperties> properties;

    friend class mbgl::RenderLight;
};

} // namespace style
} // namespace mbgl
