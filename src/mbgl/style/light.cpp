// This file is generated. Do not edit.

#include <mbgl/style/light.hpp>
#include <mbgl/style/light_impl.hpp>
#include <mbgl/style/light_observer.hpp>

namespace mbgl {
namespace style {

static LightObserver nullObserver;

Light::Light()
    : impl(makeMutable<Impl>()),
      observer(&nullObserver) {
}

Light::~Light() = default;

void Light::setObserver(LightObserver* observer_) {
    observer = observer_ ? observer_ : &nullObserver;
}

Mutable<Light::Impl> Light::mutableImpl() const {
    return makeMutable<Impl>(*impl);
}

LightAnchorType Light::getDefaultAnchor() {
    return LightAnchor::defaultValue();
}

PropertyValue<LightAnchorType> Light::getAnchor() const {
    return impl->properties.anchor.value;
}

void Light::setAnchor(PropertyValue<LightAnchorType> property) {
    auto impl_ = mutableImpl();
    impl_->properties.anchor.value = property;
    impl = std::move(impl_);
    observer->onLightChanged(*this);
}

void Light::setAnchorTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->properties.anchor.options = options;
    impl = std::move(impl_);
    observer->onLightChanged(*this);
}

TransitionOptions Light::getAnchorTransition() const {
    return impl->properties.anchor.options;
}

Position Light::getDefaultPosition() {
    return LightPosition::defaultValue();
}

PropertyValue<Position> Light::getPosition() const {
    return impl->properties.position.value;
}

void Light::setPosition(PropertyValue<Position> property) {
    auto impl_ = mutableImpl();
    impl_->properties.position.value = property;
    impl = std::move(impl_);
    observer->onLightChanged(*this);
}

void Light::setPositionTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->properties.position.options = options;
    impl = std::move(impl_);
    observer->onLightChanged(*this);
}

TransitionOptions Light::getPositionTransition() const {
    return impl->properties.position.options;
}

Color Light::getDefaultColor() {
    return LightColor::defaultValue();
}

PropertyValue<Color> Light::getColor() const {
    return impl->properties.color.value;
}

void Light::setColor(PropertyValue<Color> property) {
    auto impl_ = mutableImpl();
    impl_->properties.color.value = property;
    impl = std::move(impl_);
    observer->onLightChanged(*this);
}

void Light::setColorTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->properties.color.options = options;
    impl = std::move(impl_);
    observer->onLightChanged(*this);
}

TransitionOptions Light::getColorTransition() const {
    return impl->properties.color.options;
}

float Light::getDefaultIntensity() {
    return LightIntensity::defaultValue();
}

PropertyValue<float> Light::getIntensity() const {
    return impl->properties.intensity.value;
}

void Light::setIntensity(PropertyValue<float> property) {
    auto impl_ = mutableImpl();
    impl_->properties.intensity.value = property;
    impl = std::move(impl_);
    observer->onLightChanged(*this);
}

void Light::setIntensityTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->properties.intensity.options = options;
    impl = std::move(impl_);
    observer->onLightChanged(*this);
}

TransitionOptions Light::getIntensityTransition() const {
    return impl->properties.intensity.options;
}


} // namespace style
} // namespace mbgl
