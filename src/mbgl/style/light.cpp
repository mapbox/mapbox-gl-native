// This file is generated. Do not edit.

#include <mbgl/style/light.hpp>
#include <mbgl/style/light_impl.hpp>
#include <mbgl/style/light_properties.hpp>

namespace mbgl {
namespace style {

LightAnchorType Light::getDefaultAnchor() {
    return LightAnchor::defaultValue();
}

PropertyValue<LightAnchorType> Light::getAnchor() const {
    return properties.get<LightAnchor>().value;
}

void Light::setAnchor(PropertyValue<LightAnchorType> property) {
    properties.get<LightAnchor>().value = property;
    if (observer) {
        observer->onLightChanged(*this);
    }
}

void Light::setAnchorTransition(const TransitionOptions& transition) {
    properties.get<LightAnchor>().transition = transition;
    if (observer) {
        observer->onLightChanged(*this);
    }
}

TransitionOptions Light::getAnchorTransition() const {
    return properties.get<LightAnchor>().transition;
}

Position Light::getDefaultPosition() {
    return LightPosition::defaultValue();
}

PropertyValue<Position> Light::getPosition() const {
    return properties.get<LightPosition>().value;
}

void Light::setPosition(PropertyValue<Position> property) {
    properties.get<LightPosition>().value = property;
    if (observer) {
        observer->onLightChanged(*this);
    }
}

void Light::setPositionTransition(const TransitionOptions& transition) {
    properties.get<LightPosition>().transition = transition;
    if (observer) {
        observer->onLightChanged(*this);
    }
}

TransitionOptions Light::getPositionTransition() const {
    return properties.get<LightPosition>().transition;
}

Color Light::getDefaultColor() {
    return LightColor::defaultValue();
}

PropertyValue<Color> Light::getColor() const {
    return properties.get<LightColor>().value;
}

void Light::setColor(PropertyValue<Color> property) {
    properties.get<LightColor>().value = property;
    if (observer) {
        observer->onLightChanged(*this);
    }
}

void Light::setColorTransition(const TransitionOptions& transition) {
    properties.get<LightColor>().transition = transition;
    if (observer) {
        observer->onLightChanged(*this);
    }
}

TransitionOptions Light::getColorTransition() const {
    return properties.get<LightColor>().transition;
}

float Light::getDefaultIntensity() {
    return LightIntensity::defaultValue();
}

PropertyValue<float> Light::getIntensity() const {
    return properties.get<LightIntensity>().value;
}

void Light::setIntensity(PropertyValue<float> property) {
    properties.get<LightIntensity>().value = property;
    if (observer) {
        observer->onLightChanged(*this);
    }
}

void Light::setIntensityTransition(const TransitionOptions& transition) {
    properties.get<LightIntensity>().transition = transition;
    if (observer) {
        observer->onLightChanged(*this);
    }
}

TransitionOptions Light::getIntensityTransition() const {
    return properties.get<LightIntensity>().transition;
}

} // namespace style
} // namespace mbgl
