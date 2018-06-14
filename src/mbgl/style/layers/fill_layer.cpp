// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/fill_layer.hpp>
#include <mbgl/style/layers/fill_layer_impl.hpp>
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

FillLayer::FillLayer(const std::string& layerID, const std::string& sourceID)
    : Layer(makeMutable<Impl>(LayerType::Fill, layerID, sourceID)) {
}

FillLayer::FillLayer(Immutable<Impl> impl_)
    : Layer(std::move(impl_)) {
}

FillLayer::~FillLayer() = default;

const FillLayer::Impl& FillLayer::impl() const {
    return static_cast<const Impl&>(*baseImpl);
}

Mutable<FillLayer::Impl> FillLayer::mutableImpl() const {
    return makeMutable<Impl>(impl());
}

std::unique_ptr<Layer> FillLayer::cloneRef(const std::string& id_) const {
    auto impl_ = mutableImpl();
    impl_->id = id_;
    impl_->paint = FillPaintProperties::Transitionable();
    return std::make_unique<FillLayer>(std::move(impl_));
}

void FillLayer::Impl::stringifyLayout(rapidjson::Writer<rapidjson::StringBuffer>&) const {
}

// Source

const std::string& FillLayer::getSourceID() const {
    return impl().source;
}

void FillLayer::setSourceLayer(const std::string& sourceLayer) {
    auto impl_ = mutableImpl();
    impl_->sourceLayer = sourceLayer;
    baseImpl = std::move(impl_);
}

const std::string& FillLayer::getSourceLayer() const {
    return impl().sourceLayer;
}

// Filter

void FillLayer::setFilter(const Filter& filter) {
    auto impl_ = mutableImpl();
    impl_->filter = filter;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

const Filter& FillLayer::getFilter() const {
    return impl().filter;
}

// Visibility

void FillLayer::setVisibility(VisibilityType value) {
    if (value == getVisibility())
        return;
    auto impl_ = mutableImpl();
    impl_->visibility = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

// Zoom range

void FillLayer::setMinZoom(float minZoom) {
    auto impl_ = mutableImpl();
    impl_->minZoom = minZoom;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void FillLayer::setMaxZoom(float maxZoom) {
    auto impl_ = mutableImpl();
    impl_->maxZoom = maxZoom;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

// Layout properties


// Paint properties

PropertyValue<bool> FillLayer::getDefaultFillAntialias() {
    return { true };
}

PropertyValue<bool> FillLayer::getFillAntialias() const {
    return impl().paint.template get<FillAntialias>().value;
}

void FillLayer::setFillAntialias(PropertyValue<bool> value) {
    if (value == getFillAntialias())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillAntialias>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void FillLayer::setFillAntialiasTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillAntialias>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions FillLayer::getFillAntialiasTransition() const {
    return impl().paint.template get<FillAntialias>().options;
}

PropertyValue<float> FillLayer::getDefaultFillOpacity() {
    return { 1 };
}

PropertyValue<float> FillLayer::getFillOpacity() const {
    return impl().paint.template get<FillOpacity>().value;
}

void FillLayer::setFillOpacity(PropertyValue<float> value) {
    if (value == getFillOpacity())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillOpacity>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void FillLayer::setFillOpacityTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillOpacity>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions FillLayer::getFillOpacityTransition() const {
    return impl().paint.template get<FillOpacity>().options;
}

PropertyValue<Color> FillLayer::getDefaultFillColor() {
    return { Color::black() };
}

PropertyValue<Color> FillLayer::getFillColor() const {
    return impl().paint.template get<FillColor>().value;
}

void FillLayer::setFillColor(PropertyValue<Color> value) {
    if (value == getFillColor())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillColor>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void FillLayer::setFillColorTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillColor>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions FillLayer::getFillColorTransition() const {
    return impl().paint.template get<FillColor>().options;
}

PropertyValue<Color> FillLayer::getDefaultFillOutlineColor() {
    return { {} };
}

PropertyValue<Color> FillLayer::getFillOutlineColor() const {
    return impl().paint.template get<FillOutlineColor>().value;
}

void FillLayer::setFillOutlineColor(PropertyValue<Color> value) {
    if (value == getFillOutlineColor())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillOutlineColor>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void FillLayer::setFillOutlineColorTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillOutlineColor>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions FillLayer::getFillOutlineColorTransition() const {
    return impl().paint.template get<FillOutlineColor>().options;
}

PropertyValue<std::array<float, 2>> FillLayer::getDefaultFillTranslate() {
    return { {{ 0, 0 }} };
}

PropertyValue<std::array<float, 2>> FillLayer::getFillTranslate() const {
    return impl().paint.template get<FillTranslate>().value;
}

void FillLayer::setFillTranslate(PropertyValue<std::array<float, 2>> value) {
    if (value == getFillTranslate())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillTranslate>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void FillLayer::setFillTranslateTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillTranslate>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions FillLayer::getFillTranslateTransition() const {
    return impl().paint.template get<FillTranslate>().options;
}

PropertyValue<TranslateAnchorType> FillLayer::getDefaultFillTranslateAnchor() {
    return { TranslateAnchorType::Map };
}

PropertyValue<TranslateAnchorType> FillLayer::getFillTranslateAnchor() const {
    return impl().paint.template get<FillTranslateAnchor>().value;
}

void FillLayer::setFillTranslateAnchor(PropertyValue<TranslateAnchorType> value) {
    if (value == getFillTranslateAnchor())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillTranslateAnchor>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void FillLayer::setFillTranslateAnchorTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillTranslateAnchor>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions FillLayer::getFillTranslateAnchorTransition() const {
    return impl().paint.template get<FillTranslateAnchor>().options;
}

PropertyValue<std::string> FillLayer::getDefaultFillPattern() {
    return { "" };
}

PropertyValue<std::string> FillLayer::getFillPattern() const {
    return impl().paint.template get<FillPattern>().value;
}

void FillLayer::setFillPattern(PropertyValue<std::string> value) {
    if (value == getFillPattern())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillPattern>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void FillLayer::setFillPatternTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillPattern>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions FillLayer::getFillPatternTransition() const {
    return impl().paint.template get<FillPattern>().options;
}

using namespace conversion;

optional<Error> FillLayer::setPaintProperty(const std::string& name, const Convertible& value) {
    enum class Property {
        Unknown,
        FillAntialias,
        FillOpacity,
        FillColor,
        FillOutlineColor,
        FillTranslate,
        FillTranslateAnchor,
        FillPattern,
        FillAntialiasTransition,
        FillOpacityTransition,
        FillColorTransition,
        FillOutlineColorTransition,
        FillTranslateTransition,
        FillTranslateAnchorTransition,
        FillPatternTransition,
    };

    Property property = Property::Unknown;
    switch (util::hashFNV1a(name.c_str())) {
    case util::hashFNV1a("fill-antialias"):
        if (name == "fill-antialias") {
            property = Property::FillAntialias;
        }
        break;
    case util::hashFNV1a("fill-antialias-transition"):
        if (name == "fill-antialias-transition") {
            property = Property::FillAntialiasTransition;
        }
        break;
    case util::hashFNV1a("fill-opacity"):
        if (name == "fill-opacity") {
            property = Property::FillOpacity;
        }
        break;
    case util::hashFNV1a("fill-opacity-transition"):
        if (name == "fill-opacity-transition") {
            property = Property::FillOpacityTransition;
        }
        break;
    case util::hashFNV1a("fill-color"):
        if (name == "fill-color") {
            property = Property::FillColor;
        }
        break;
    case util::hashFNV1a("fill-color-transition"):
        if (name == "fill-color-transition") {
            property = Property::FillColorTransition;
        }
        break;
    case util::hashFNV1a("fill-outline-color"):
        if (name == "fill-outline-color") {
            property = Property::FillOutlineColor;
        }
        break;
    case util::hashFNV1a("fill-outline-color-transition"):
        if (name == "fill-outline-color-transition") {
            property = Property::FillOutlineColorTransition;
        }
        break;
    case util::hashFNV1a("fill-translate"):
        if (name == "fill-translate") {
            property = Property::FillTranslate;
        }
        break;
    case util::hashFNV1a("fill-translate-transition"):
        if (name == "fill-translate-transition") {
            property = Property::FillTranslateTransition;
        }
        break;
    case util::hashFNV1a("fill-translate-anchor"):
        if (name == "fill-translate-anchor") {
            property = Property::FillTranslateAnchor;
        }
        break;
    case util::hashFNV1a("fill-translate-anchor-transition"):
        if (name == "fill-translate-anchor-transition") {
            property = Property::FillTranslateAnchorTransition;
        }
        break;
    case util::hashFNV1a("fill-pattern"):
        if (name == "fill-pattern") {
            property = Property::FillPattern;
        }
        break;
    case util::hashFNV1a("fill-pattern-transition"):
        if (name == "fill-pattern-transition") {
            property = Property::FillPatternTransition;
        }
        break;
    
    }

    if (property == Property::Unknown) {
        return Error { "layer doesn't support this property" };
    }

        
    if (property == Property::FillAntialias) {
        Error error;
        optional<PropertyValue<bool>> typedValue = convert<PropertyValue<bool>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }
        
        setFillAntialias(*typedValue);
        return nullopt;
        
    }
    
    if (property == Property::FillOpacity) {
        Error error;
        optional<PropertyValue<float>> typedValue = convert<PropertyValue<float>>(value, error, true, false);
        if (!typedValue) {
            return error;
        }
        
        setFillOpacity(*typedValue);
        return nullopt;
        
    }
    
    if (property == Property::FillColor || property == Property::FillOutlineColor) {
        Error error;
        optional<PropertyValue<Color>> typedValue = convert<PropertyValue<Color>>(value, error, true, false);
        if (!typedValue) {
            return error;
        }
        
        if (property == Property::FillColor) {
            setFillColor(*typedValue);
            return nullopt;
        }
        
        if (property == Property::FillOutlineColor) {
            setFillOutlineColor(*typedValue);
            return nullopt;
        }
        
    }
    
    if (property == Property::FillTranslate) {
        Error error;
        optional<PropertyValue<std::array<float, 2>>> typedValue = convert<PropertyValue<std::array<float, 2>>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }
        
        setFillTranslate(*typedValue);
        return nullopt;
        
    }
    
    if (property == Property::FillTranslateAnchor) {
        Error error;
        optional<PropertyValue<TranslateAnchorType>> typedValue = convert<PropertyValue<TranslateAnchorType>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }
        
        setFillTranslateAnchor(*typedValue);
        return nullopt;
        
    }
    
    if (property == Property::FillPattern) {
        Error error;
        optional<PropertyValue<std::string>> typedValue = convert<PropertyValue<std::string>>(value, error, true, false);
        if (!typedValue) {
            return error;
        }
        
        setFillPattern(*typedValue);
        return nullopt;
        
    }
    

    Error error;
    optional<TransitionOptions> transition = convert<TransitionOptions>(value, error);
    if (!transition) {
        return error;
    }
    
    if (property == Property::FillAntialiasTransition) {
        setFillAntialiasTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::FillOpacityTransition) {
        setFillOpacityTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::FillColorTransition) {
        setFillColorTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::FillOutlineColorTransition) {
        setFillOutlineColorTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::FillTranslateTransition) {
        setFillTranslateTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::FillTranslateAnchorTransition) {
        setFillTranslateAnchorTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::FillPatternTransition) {
        setFillPatternTransition(*transition);
        return nullopt;
    }
    

    return Error { "layer doesn't support this property" };
}

optional<Error> FillLayer::setLayoutProperty(const std::string& name, const Convertible& value) {
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
