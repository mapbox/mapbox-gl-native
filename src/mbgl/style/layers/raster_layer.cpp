// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/raster_layer.hpp>
#include <mbgl/style/layers/raster_layer_impl.hpp>
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

RasterLayer::RasterLayer(const std::string& layerID, const std::string& sourceID)
    : Layer(makeMutable<Impl>(LayerType::Raster, layerID, sourceID)) {
}

RasterLayer::RasterLayer(Immutable<Impl> impl_)
    : Layer(std::move(impl_)) {
}

RasterLayer::~RasterLayer() = default;

const RasterLayer::Impl& RasterLayer::impl() const {
    return static_cast<const Impl&>(*baseImpl);
}

Mutable<RasterLayer::Impl> RasterLayer::mutableImpl() const {
    return makeMutable<Impl>(impl());
}

std::unique_ptr<Layer> RasterLayer::cloneRef(const std::string& id_) const {
    auto impl_ = mutableImpl();
    impl_->id = id_;
    impl_->paint = RasterPaintProperties::Transitionable();
    return std::make_unique<RasterLayer>(std::move(impl_));
}

void RasterLayer::Impl::stringifyLayout(rapidjson::Writer<rapidjson::StringBuffer>&) const {
}

// Source

const std::string& RasterLayer::getSourceID() const {
    return impl().source;
}


// Visibility

void RasterLayer::setVisibility(VisibilityType value) {
    if (value == getVisibility())
        return;
    auto impl_ = mutableImpl();
    impl_->visibility = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

// Zoom range

void RasterLayer::setMinZoom(float minZoom) {
    auto impl_ = mutableImpl();
    impl_->minZoom = minZoom;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void RasterLayer::setMaxZoom(float maxZoom) {
    auto impl_ = mutableImpl();
    impl_->maxZoom = maxZoom;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

// Layout properties


// Paint properties

PropertyValue<float> RasterLayer::getDefaultRasterOpacity() {
    return { 1 };
}

PropertyValue<float> RasterLayer::getRasterOpacity() const {
    return impl().paint.template get<RasterOpacity>().value;
}

void RasterLayer::setRasterOpacity(PropertyValue<float> value) {
    if (value == getRasterOpacity())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<RasterOpacity>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void RasterLayer::setRasterOpacityTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<RasterOpacity>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions RasterLayer::getRasterOpacityTransition() const {
    return impl().paint.template get<RasterOpacity>().options;
}

PropertyValue<float> RasterLayer::getDefaultRasterHueRotate() {
    return { 0 };
}

PropertyValue<float> RasterLayer::getRasterHueRotate() const {
    return impl().paint.template get<RasterHueRotate>().value;
}

void RasterLayer::setRasterHueRotate(PropertyValue<float> value) {
    if (value == getRasterHueRotate())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<RasterHueRotate>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void RasterLayer::setRasterHueRotateTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<RasterHueRotate>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions RasterLayer::getRasterHueRotateTransition() const {
    return impl().paint.template get<RasterHueRotate>().options;
}

PropertyValue<float> RasterLayer::getDefaultRasterBrightnessMin() {
    return { 0 };
}

PropertyValue<float> RasterLayer::getRasterBrightnessMin() const {
    return impl().paint.template get<RasterBrightnessMin>().value;
}

void RasterLayer::setRasterBrightnessMin(PropertyValue<float> value) {
    if (value == getRasterBrightnessMin())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<RasterBrightnessMin>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void RasterLayer::setRasterBrightnessMinTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<RasterBrightnessMin>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions RasterLayer::getRasterBrightnessMinTransition() const {
    return impl().paint.template get<RasterBrightnessMin>().options;
}

PropertyValue<float> RasterLayer::getDefaultRasterBrightnessMax() {
    return { 1 };
}

PropertyValue<float> RasterLayer::getRasterBrightnessMax() const {
    return impl().paint.template get<RasterBrightnessMax>().value;
}

void RasterLayer::setRasterBrightnessMax(PropertyValue<float> value) {
    if (value == getRasterBrightnessMax())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<RasterBrightnessMax>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void RasterLayer::setRasterBrightnessMaxTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<RasterBrightnessMax>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions RasterLayer::getRasterBrightnessMaxTransition() const {
    return impl().paint.template get<RasterBrightnessMax>().options;
}

PropertyValue<float> RasterLayer::getDefaultRasterSaturation() {
    return { 0 };
}

PropertyValue<float> RasterLayer::getRasterSaturation() const {
    return impl().paint.template get<RasterSaturation>().value;
}

void RasterLayer::setRasterSaturation(PropertyValue<float> value) {
    if (value == getRasterSaturation())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<RasterSaturation>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void RasterLayer::setRasterSaturationTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<RasterSaturation>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions RasterLayer::getRasterSaturationTransition() const {
    return impl().paint.template get<RasterSaturation>().options;
}

PropertyValue<float> RasterLayer::getDefaultRasterContrast() {
    return { 0 };
}

PropertyValue<float> RasterLayer::getRasterContrast() const {
    return impl().paint.template get<RasterContrast>().value;
}

void RasterLayer::setRasterContrast(PropertyValue<float> value) {
    if (value == getRasterContrast())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<RasterContrast>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void RasterLayer::setRasterContrastTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<RasterContrast>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions RasterLayer::getRasterContrastTransition() const {
    return impl().paint.template get<RasterContrast>().options;
}

PropertyValue<RasterResamplingType> RasterLayer::getDefaultRasterResampling() {
    return { RasterResamplingType::Linear };
}

PropertyValue<RasterResamplingType> RasterLayer::getRasterResampling() const {
    return impl().paint.template get<RasterResampling>().value;
}

void RasterLayer::setRasterResampling(PropertyValue<RasterResamplingType> value) {
    if (value == getRasterResampling())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<RasterResampling>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void RasterLayer::setRasterResamplingTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<RasterResampling>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions RasterLayer::getRasterResamplingTransition() const {
    return impl().paint.template get<RasterResampling>().options;
}

PropertyValue<float> RasterLayer::getDefaultRasterFadeDuration() {
    return { 300 };
}

PropertyValue<float> RasterLayer::getRasterFadeDuration() const {
    return impl().paint.template get<RasterFadeDuration>().value;
}

void RasterLayer::setRasterFadeDuration(PropertyValue<float> value) {
    if (value == getRasterFadeDuration())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<RasterFadeDuration>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void RasterLayer::setRasterFadeDurationTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<RasterFadeDuration>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions RasterLayer::getRasterFadeDurationTransition() const {
    return impl().paint.template get<RasterFadeDuration>().options;
}

using namespace conversion;

optional<Error> RasterLayer::setPaintProperty(const std::string& name, const Convertible& value) {
    enum class Property {
        Unknown,
        RasterOpacity,
        RasterHueRotate,
        RasterBrightnessMin,
        RasterBrightnessMax,
        RasterSaturation,
        RasterContrast,
        RasterResampling,
        RasterFadeDuration,
        RasterOpacityTransition,
        RasterHueRotateTransition,
        RasterBrightnessMinTransition,
        RasterBrightnessMaxTransition,
        RasterSaturationTransition,
        RasterContrastTransition,
        RasterResamplingTransition,
        RasterFadeDurationTransition,
    };

    Property property = Property::Unknown;
    switch (util::hashFNV1a(name.c_str())) {
    case util::hashFNV1a("raster-opacity"):
        if (name == "raster-opacity") {
            property = Property::RasterOpacity;
        }
        break;
    case util::hashFNV1a("raster-opacity-transition"):
        if (name == "raster-opacity-transition") {
            property = Property::RasterOpacityTransition;
        }
        break;
    case util::hashFNV1a("raster-hue-rotate"):
        if (name == "raster-hue-rotate") {
            property = Property::RasterHueRotate;
        }
        break;
    case util::hashFNV1a("raster-hue-rotate-transition"):
        if (name == "raster-hue-rotate-transition") {
            property = Property::RasterHueRotateTransition;
        }
        break;
    case util::hashFNV1a("raster-brightness-min"):
        if (name == "raster-brightness-min") {
            property = Property::RasterBrightnessMin;
        }
        break;
    case util::hashFNV1a("raster-brightness-min-transition"):
        if (name == "raster-brightness-min-transition") {
            property = Property::RasterBrightnessMinTransition;
        }
        break;
    case util::hashFNV1a("raster-brightness-max"):
        if (name == "raster-brightness-max") {
            property = Property::RasterBrightnessMax;
        }
        break;
    case util::hashFNV1a("raster-brightness-max-transition"):
        if (name == "raster-brightness-max-transition") {
            property = Property::RasterBrightnessMaxTransition;
        }
        break;
    case util::hashFNV1a("raster-saturation"):
        if (name == "raster-saturation") {
            property = Property::RasterSaturation;
        }
        break;
    case util::hashFNV1a("raster-saturation-transition"):
        if (name == "raster-saturation-transition") {
            property = Property::RasterSaturationTransition;
        }
        break;
    case util::hashFNV1a("raster-contrast"):
        if (name == "raster-contrast") {
            property = Property::RasterContrast;
        }
        break;
    case util::hashFNV1a("raster-contrast-transition"):
        if (name == "raster-contrast-transition") {
            property = Property::RasterContrastTransition;
        }
        break;
    case util::hashFNV1a("raster-resampling"):
        if (name == "raster-resampling") {
            property = Property::RasterResampling;
        }
        break;
    case util::hashFNV1a("raster-resampling-transition"):
        if (name == "raster-resampling-transition") {
            property = Property::RasterResamplingTransition;
        }
        break;
    case util::hashFNV1a("raster-fade-duration"):
        if (name == "raster-fade-duration") {
            property = Property::RasterFadeDuration;
        }
        break;
    case util::hashFNV1a("raster-fade-duration-transition"):
        if (name == "raster-fade-duration-transition") {
            property = Property::RasterFadeDurationTransition;
        }
        break;
    
    }

    if (property == Property::Unknown) {
        return Error { "layer doesn't support this property" };
    }

        
    if (property == Property::RasterOpacity || property == Property::RasterHueRotate || property == Property::RasterBrightnessMin || property == Property::RasterBrightnessMax || property == Property::RasterSaturation || property == Property::RasterContrast || property == Property::RasterFadeDuration) {
        Error error;
        optional<PropertyValue<float>> typedValue = convert<PropertyValue<float>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }
        
        if (property == Property::RasterOpacity) {
            setRasterOpacity(*typedValue);
            return nullopt;
        }
        
        if (property == Property::RasterHueRotate) {
            setRasterHueRotate(*typedValue);
            return nullopt;
        }
        
        if (property == Property::RasterBrightnessMin) {
            setRasterBrightnessMin(*typedValue);
            return nullopt;
        }
        
        if (property == Property::RasterBrightnessMax) {
            setRasterBrightnessMax(*typedValue);
            return nullopt;
        }
        
        if (property == Property::RasterSaturation) {
            setRasterSaturation(*typedValue);
            return nullopt;
        }
        
        if (property == Property::RasterContrast) {
            setRasterContrast(*typedValue);
            return nullopt;
        }
        
        if (property == Property::RasterFadeDuration) {
            setRasterFadeDuration(*typedValue);
            return nullopt;
        }
        
    }
    
    if (property == Property::RasterResampling) {
        Error error;
        optional<PropertyValue<RasterResamplingType>> typedValue = convert<PropertyValue<RasterResamplingType>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }
        
        setRasterResampling(*typedValue);
        return nullopt;
        
    }
    

    Error error;
    optional<TransitionOptions> transition = convert<TransitionOptions>(value, error);
    if (!transition) {
        return error;
    }
    
    if (property == Property::RasterOpacityTransition) {
        setRasterOpacityTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::RasterHueRotateTransition) {
        setRasterHueRotateTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::RasterBrightnessMinTransition) {
        setRasterBrightnessMinTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::RasterBrightnessMaxTransition) {
        setRasterBrightnessMaxTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::RasterSaturationTransition) {
        setRasterSaturationTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::RasterContrastTransition) {
        setRasterContrastTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::RasterResamplingTransition) {
        setRasterResamplingTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::RasterFadeDurationTransition) {
        setRasterFadeDurationTransition(*transition);
        return nullopt;
    }
    

    return Error { "layer doesn't support this property" };
}

optional<Error> RasterLayer::setLayoutProperty(const std::string& name, const Convertible& value) {
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
