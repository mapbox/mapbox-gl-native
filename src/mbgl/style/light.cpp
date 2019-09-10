// This file is generated. Do not edit.

#include <mbgl/style/light.hpp>
#include <mbgl/style/light_impl.hpp>
#include <mbgl/style/light_observer.hpp>
#include <mbgl/style/conversion/light.hpp>
#include <mbgl/style/conversion/property_value.hpp>
#include <mbgl/style/conversion/transition_options.hpp>
#include <mbgl/style/conversion/json.hpp>
#include <mbgl/style/conversion_impl.hpp>

#include <mapbox/eternal.hpp>

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

using namespace conversion;

optional<Error> Light::setProperty(const std::string& name, const Convertible& value) {
    enum class Property : uint8_t {
        Anchor,
        Color,
        Intensity,
        Position,
        AnchorTransition,
        ColorTransition,
        IntensityTransition,
        PositionTransition,
    };

    MAPBOX_ETERNAL_CONSTEXPR const auto properties = mapbox::eternal::hash_map<mapbox::eternal::string, uint8_t>({
        { "anchor", static_cast<uint8_t>(Property::Anchor) },
        { "color", static_cast<uint8_t>(Property::Color) },
        { "intensity", static_cast<uint8_t>(Property::Intensity) },
        { "position", static_cast<uint8_t>(Property::Position) },
        { "anchor-transition", static_cast<uint8_t>(Property::AnchorTransition) },
        { "color-transition", static_cast<uint8_t>(Property::ColorTransition) },
        { "intensity-transition", static_cast<uint8_t>(Property::IntensityTransition) },
        { "position-transition", static_cast<uint8_t>(Property::PositionTransition) }
    });

    const auto it = properties.find(name.c_str());
    if (it == properties.end()) {
        return Error { "light doesn't support this property" };
    }

    auto property = static_cast<Property>(it->second);

        
    if (property == Property::Anchor) {
        Error error;
        optional<PropertyValue<LightAnchorType>> typedValue = convert<PropertyValue<LightAnchorType>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }
        
        setAnchor(*typedValue);
        return nullopt;
        
    }
    
    if (property == Property::Color) {
        Error error;
        optional<PropertyValue<Color>> typedValue = convert<PropertyValue<Color>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }
        
        setColor(*typedValue);
        return nullopt;
        
    }
    
    if (property == Property::Intensity) {
        Error error;
        optional<PropertyValue<float>> typedValue = convert<PropertyValue<float>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }
        
        setIntensity(*typedValue);
        return nullopt;
        
    }
    
    if (property == Property::Position) {
        Error error;
        optional<PropertyValue<Position>> typedValue = convert<PropertyValue<Position>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }
        
        setPosition(*typedValue);
        return nullopt;
        
    }
    

    Error error;
    optional<TransitionOptions> transition = convert<TransitionOptions>(value, error);
    if (!transition) {
        return error;
    }
    
    if (property == Property::AnchorTransition) {
        setAnchorTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::ColorTransition) {
        setColorTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::IntensityTransition) {
        setIntensityTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::PositionTransition) {
        setPositionTransition(*transition);
        return nullopt;
    }
    

    return Error { "light doesn't support this property" };
}

LightAnchorType Light::getDefaultAnchor() {
    return LightAnchor::defaultValue();
}

PropertyValue<LightAnchorType> Light::getAnchor() const {
    return impl->properties.template get<LightAnchor>().value;
}

void Light::setAnchor(PropertyValue<LightAnchorType> property) {
    auto impl_ = mutableImpl();
    impl_->properties.template get<LightAnchor>().value = property;
    impl = std::move(impl_);
    observer->onLightChanged(*this);
}

void Light::setAnchorTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->properties.template get<LightAnchor>().options = options;
    impl = std::move(impl_);
    observer->onLightChanged(*this);
}

TransitionOptions Light::getAnchorTransition() const {
    return impl->properties.template get<LightAnchor>().options;
}

Color Light::getDefaultColor() {
    return LightColor::defaultValue();
}

PropertyValue<Color> Light::getColor() const {
    return impl->properties.template get<LightColor>().value;
}

void Light::setColor(PropertyValue<Color> property) {
    auto impl_ = mutableImpl();
    impl_->properties.template get<LightColor>().value = property;
    impl = std::move(impl_);
    observer->onLightChanged(*this);
}

void Light::setColorTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->properties.template get<LightColor>().options = options;
    impl = std::move(impl_);
    observer->onLightChanged(*this);
}

TransitionOptions Light::getColorTransition() const {
    return impl->properties.template get<LightColor>().options;
}

float Light::getDefaultIntensity() {
    return LightIntensity::defaultValue();
}

PropertyValue<float> Light::getIntensity() const {
    return impl->properties.template get<LightIntensity>().value;
}

void Light::setIntensity(PropertyValue<float> property) {
    auto impl_ = mutableImpl();
    impl_->properties.template get<LightIntensity>().value = property;
    impl = std::move(impl_);
    observer->onLightChanged(*this);
}

void Light::setIntensityTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->properties.template get<LightIntensity>().options = options;
    impl = std::move(impl_);
    observer->onLightChanged(*this);
}

TransitionOptions Light::getIntensityTransition() const {
    return impl->properties.template get<LightIntensity>().options;
}

Position Light::getDefaultPosition() {
    return LightPosition::defaultValue();
}

PropertyValue<Position> Light::getPosition() const {
    return impl->properties.template get<LightPosition>().value;
}

void Light::setPosition(PropertyValue<Position> property) {
    auto impl_ = mutableImpl();
    impl_->properties.template get<LightPosition>().value = property;
    impl = std::move(impl_);
    observer->onLightChanged(*this);
}

void Light::setPositionTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->properties.template get<LightPosition>().options = options;
    impl = std::move(impl_);
    observer->onLightChanged(*this);
}

TransitionOptions Light::getPositionTransition() const {
    return impl->properties.template get<LightPosition>().options;
}


} // namespace style
} // namespace mbgl
