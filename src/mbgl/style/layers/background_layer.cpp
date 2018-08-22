// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/background_layer.hpp>
#include <mbgl/style/layers/background_layer_impl.hpp>
#include <mbgl/style/layer_observer.hpp>
#include <mbgl/style/conversion/color_ramp_property_value.hpp>
#include <mbgl/style/conversion/constant.hpp>
#include <mbgl/style/conversion/property_value.hpp>
#include <mbgl/style/conversion/transition_options.hpp>
#include <mbgl/style/conversion/json.hpp>
#include <mbgl/style/conversion_impl.hpp>
#include <mbgl/util/fnv_hash.hpp>

namespace mbgl {
namespace style {

BackgroundLayer::BackgroundLayer(const std::string& layerID)
    : Layer(makeMutable<Impl>(LayerType::Background, layerID, std::string())) {
}

BackgroundLayer::BackgroundLayer(Immutable<Impl> impl_)
    : Layer(std::move(impl_)) {
}

BackgroundLayer::~BackgroundLayer() = default;

const BackgroundLayer::Impl& BackgroundLayer::impl() const {
    return static_cast<const Impl&>(*baseImpl);
}

Mutable<BackgroundLayer::Impl> BackgroundLayer::mutableImpl() const {
    return makeMutable<Impl>(impl());
}

std::unique_ptr<Layer> BackgroundLayer::cloneRef(const std::string& id_) const {
    auto impl_ = mutableImpl();
    impl_->id = id_;
    impl_->paint = BackgroundPaintProperties::Transitionable();
    return std::make_unique<BackgroundLayer>(std::move(impl_));
}

void BackgroundLayer::Impl::stringifyLayout(rapidjson::Writer<rapidjson::StringBuffer>&) const {
}


// Visibility

void BackgroundLayer::setVisibility(VisibilityType value) {
    if (value == getVisibility())
        return;
    auto impl_ = mutableImpl();
    impl_->visibility = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

// Zoom range

void BackgroundLayer::setMinZoom(float minZoom) {
    auto impl_ = mutableImpl();
    impl_->minZoom = minZoom;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void BackgroundLayer::setMaxZoom(float maxZoom) {
    auto impl_ = mutableImpl();
    impl_->maxZoom = maxZoom;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

// Layout properties


// Paint properties

PropertyValue<Color> BackgroundLayer::getDefaultBackgroundColor() {
    return { Color::black() };
}

PropertyValue<Color> BackgroundLayer::getBackgroundColor() const {
    return impl().paint.template get<BackgroundColor>().value;
}

void BackgroundLayer::setBackgroundColor(PropertyValue<Color> value) {
    if (value == getBackgroundColor())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<BackgroundColor>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void BackgroundLayer::setBackgroundColorTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<BackgroundColor>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions BackgroundLayer::getBackgroundColorTransition() const {
    return impl().paint.template get<BackgroundColor>().options;
}

PropertyValue<std::string> BackgroundLayer::getDefaultBackgroundPattern() {
    return { "" };
}

PropertyValue<std::string> BackgroundLayer::getBackgroundPattern() const {
    return impl().paint.template get<BackgroundPattern>().value;
}

void BackgroundLayer::setBackgroundPattern(PropertyValue<std::string> value) {
    if (value == getBackgroundPattern())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<BackgroundPattern>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void BackgroundLayer::setBackgroundPatternTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<BackgroundPattern>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions BackgroundLayer::getBackgroundPatternTransition() const {
    return impl().paint.template get<BackgroundPattern>().options;
}

PropertyValue<float> BackgroundLayer::getDefaultBackgroundOpacity() {
    return { 1 };
}

PropertyValue<float> BackgroundLayer::getBackgroundOpacity() const {
    return impl().paint.template get<BackgroundOpacity>().value;
}

void BackgroundLayer::setBackgroundOpacity(PropertyValue<float> value) {
    if (value == getBackgroundOpacity())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<BackgroundOpacity>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void BackgroundLayer::setBackgroundOpacityTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<BackgroundOpacity>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions BackgroundLayer::getBackgroundOpacityTransition() const {
    return impl().paint.template get<BackgroundOpacity>().options;
}

using namespace conversion;

optional<Error> BackgroundLayer::setPaintProperty(const std::string& name, const Convertible& value) {
    enum class Property {
        Unknown,
        BackgroundColor,
        BackgroundPattern,
        BackgroundOpacity,
        BackgroundColorTransition,
        BackgroundPatternTransition,
        BackgroundOpacityTransition,
    };

    Property property = Property::Unknown;
    switch (util::hashFNV1a(name.c_str())) {
    case util::hashFNV1a("background-color"):
        if (name == "background-color") {
            property = Property::BackgroundColor;
        }
        break;
    case util::hashFNV1a("background-color-transition"):
        if (name == "background-color-transition") {
            property = Property::BackgroundColorTransition;
        }
        break;
    case util::hashFNV1a("background-pattern"):
        if (name == "background-pattern") {
            property = Property::BackgroundPattern;
        }
        break;
    case util::hashFNV1a("background-pattern-transition"):
        if (name == "background-pattern-transition") {
            property = Property::BackgroundPatternTransition;
        }
        break;
    case util::hashFNV1a("background-opacity"):
        if (name == "background-opacity") {
            property = Property::BackgroundOpacity;
        }
        break;
    case util::hashFNV1a("background-opacity-transition"):
        if (name == "background-opacity-transition") {
            property = Property::BackgroundOpacityTransition;
        }
        break;
    
    }

    if (property == Property::Unknown) {
        return Error { "layer doesn't support this property" };
    }

        
    if (property == Property::BackgroundColor) {
        Error error;
        optional<PropertyValue<Color>> typedValue = convert<PropertyValue<Color>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }
        
        setBackgroundColor(*typedValue);
        return nullopt;
        
    }
    
    if (property == Property::BackgroundPattern) {
        Error error;
        optional<PropertyValue<std::string>> typedValue = convert<PropertyValue<std::string>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }
        
        setBackgroundPattern(*typedValue);
        return nullopt;
        
    }
    
    if (property == Property::BackgroundOpacity) {
        Error error;
        optional<PropertyValue<float>> typedValue = convert<PropertyValue<float>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }
        
        setBackgroundOpacity(*typedValue);
        return nullopt;
        
    }
    

    Error error;
    optional<TransitionOptions> transition = convert<TransitionOptions>(value, error);
    if (!transition) {
        return error;
    }
    
    if (property == Property::BackgroundColorTransition) {
        setBackgroundColorTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::BackgroundPatternTransition) {
        setBackgroundPatternTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::BackgroundOpacityTransition) {
        setBackgroundOpacityTransition(*transition);
        return nullopt;
    }
    

    return Error { "layer doesn't support this property" };
}

optional<Error> BackgroundLayer::setLayoutProperty(const std::string& name, const Convertible& value) {
    if (name == "visibility") {
        return Layer::setVisibility(value);
    }

    enum class Property {
        Unknown,
    };

    Property property = Property::Unknown;
    switch (util::hashFNV1a(name.c_str())) {
    
    }

    if (property == Property::Unknown) {
        return Error { "layer doesn't support this property" };
    }

        

    return Error { "layer doesn't support this property" };
}

} // namespace style
} // namespace mbgl
