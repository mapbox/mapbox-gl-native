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

#include <mapbox/eternal.hpp>

namespace mbgl {
namespace style {


// static
const LayerTypeInfo* RasterLayer::Impl::staticTypeInfo() noexcept {
    const static LayerTypeInfo typeInfo
        {"raster",
          LayerTypeInfo::Source::Required,
          LayerTypeInfo::Pass3D::NotRequired,
          LayerTypeInfo::Layout::NotRequired,
          LayerTypeInfo::Clipping::NotRequired,
          LayerTypeInfo::FadingTiles::NotRequired
        };
    return &typeInfo;
}


RasterLayer::RasterLayer(const std::string& layerID, const std::string& sourceID)
    : Layer(makeMutable<Impl>(layerID, sourceID)) {
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

// Layout properties


// Paint properties

PropertyValue<float> RasterLayer::getDefaultRasterOpacity() {
    return { 1 };
}

const PropertyValue<float>& RasterLayer::getRasterOpacity() const {
    return impl().paint.template get<RasterOpacity>().value;
}

void RasterLayer::setRasterOpacity(const PropertyValue<float>& value) {
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

const PropertyValue<float>& RasterLayer::getRasterHueRotate() const {
    return impl().paint.template get<RasterHueRotate>().value;
}

void RasterLayer::setRasterHueRotate(const PropertyValue<float>& value) {
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

const PropertyValue<float>& RasterLayer::getRasterBrightnessMin() const {
    return impl().paint.template get<RasterBrightnessMin>().value;
}

void RasterLayer::setRasterBrightnessMin(const PropertyValue<float>& value) {
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

const PropertyValue<float>& RasterLayer::getRasterBrightnessMax() const {
    return impl().paint.template get<RasterBrightnessMax>().value;
}

void RasterLayer::setRasterBrightnessMax(const PropertyValue<float>& value) {
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

const PropertyValue<float>& RasterLayer::getRasterSaturation() const {
    return impl().paint.template get<RasterSaturation>().value;
}

void RasterLayer::setRasterSaturation(const PropertyValue<float>& value) {
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

const PropertyValue<float>& RasterLayer::getRasterContrast() const {
    return impl().paint.template get<RasterContrast>().value;
}

void RasterLayer::setRasterContrast(const PropertyValue<float>& value) {
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

const PropertyValue<RasterResamplingType>& RasterLayer::getRasterResampling() const {
    return impl().paint.template get<RasterResampling>().value;
}

void RasterLayer::setRasterResampling(const PropertyValue<RasterResamplingType>& value) {
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

const PropertyValue<float>& RasterLayer::getRasterFadeDuration() const {
    return impl().paint.template get<RasterFadeDuration>().value;
}

void RasterLayer::setRasterFadeDuration(const PropertyValue<float>& value) {
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
    enum class Property : uint8_t {
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

    MAPBOX_ETERNAL_CONSTEXPR const auto properties = mapbox::eternal::hash_map<mapbox::eternal::string, uint8_t>({
        { "raster-opacity", static_cast<uint8_t>(Property::RasterOpacity) },
        { "raster-hue-rotate", static_cast<uint8_t>(Property::RasterHueRotate) },
        { "raster-brightness-min", static_cast<uint8_t>(Property::RasterBrightnessMin) },
        { "raster-brightness-max", static_cast<uint8_t>(Property::RasterBrightnessMax) },
        { "raster-saturation", static_cast<uint8_t>(Property::RasterSaturation) },
        { "raster-contrast", static_cast<uint8_t>(Property::RasterContrast) },
        { "raster-resampling", static_cast<uint8_t>(Property::RasterResampling) },
        { "raster-fade-duration", static_cast<uint8_t>(Property::RasterFadeDuration) },
        { "raster-opacity-transition", static_cast<uint8_t>(Property::RasterOpacityTransition) },
        { "raster-hue-rotate-transition", static_cast<uint8_t>(Property::RasterHueRotateTransition) },
        { "raster-brightness-min-transition", static_cast<uint8_t>(Property::RasterBrightnessMinTransition) },
        { "raster-brightness-max-transition", static_cast<uint8_t>(Property::RasterBrightnessMaxTransition) },
        { "raster-saturation-transition", static_cast<uint8_t>(Property::RasterSaturationTransition) },
        { "raster-contrast-transition", static_cast<uint8_t>(Property::RasterContrastTransition) },
        { "raster-resampling-transition", static_cast<uint8_t>(Property::RasterResamplingTransition) },
        { "raster-fade-duration-transition", static_cast<uint8_t>(Property::RasterFadeDurationTransition) }
    });

    const auto it = properties.find(name.c_str());
    if (it == properties.end()) {
        return Error { "layer doesn't support this property" };
    }

    auto property = static_cast<Property>(it->second);

        
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

    return Error { "layer doesn't support this property" };
}

Mutable<Layer::Impl> RasterLayer::mutableBaseImpl() const {
    return staticMutableCast<Layer::Impl>(mutableImpl());
}

} // namespace style
} // namespace mbgl
