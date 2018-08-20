// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/heatmap_layer.hpp>
#include <mbgl/style/layers/heatmap_layer_impl.hpp>
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

HeatmapLayer::HeatmapLayer(const std::string& layerID, const std::string& sourceID)
    : Layer(makeMutable<Impl>(LayerType::Heatmap, layerID, sourceID)) {
}

HeatmapLayer::HeatmapLayer(Immutable<Impl> impl_)
    : Layer(std::move(impl_)) {
}

HeatmapLayer::~HeatmapLayer() = default;

const HeatmapLayer::Impl& HeatmapLayer::impl() const {
    return static_cast<const Impl&>(*baseImpl);
}

Mutable<HeatmapLayer::Impl> HeatmapLayer::mutableImpl() const {
    return makeMutable<Impl>(impl());
}

std::unique_ptr<Layer> HeatmapLayer::cloneRef(const std::string& id_) const {
    auto impl_ = mutableImpl();
    impl_->id = id_;
    impl_->paint = HeatmapPaintProperties::Transitionable();
    return std::make_unique<HeatmapLayer>(std::move(impl_));
}

void HeatmapLayer::Impl::stringifyLayout(rapidjson::Writer<rapidjson::StringBuffer>&) const {
}

// Source

const std::string& HeatmapLayer::getSourceID() const {
    return impl().source;
}

void HeatmapLayer::setSourceLayer(const std::string& sourceLayer) {
    auto impl_ = mutableImpl();
    impl_->sourceLayer = sourceLayer;
    baseImpl = std::move(impl_);
}

const std::string& HeatmapLayer::getSourceLayer() const {
    return impl().sourceLayer;
}

// Filter

void HeatmapLayer::setFilter(const Filter& filter) {
    auto impl_ = mutableImpl();
    impl_->filter = filter;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

const Filter& HeatmapLayer::getFilter() const {
    return impl().filter;
}

// Visibility

void HeatmapLayer::setVisibility(VisibilityType value) {
    if (value == getVisibility())
        return;
    auto impl_ = mutableImpl();
    impl_->visibility = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

// Zoom range

void HeatmapLayer::setMinZoom(float minZoom) {
    auto impl_ = mutableImpl();
    impl_->minZoom = minZoom;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void HeatmapLayer::setMaxZoom(float maxZoom) {
    auto impl_ = mutableImpl();
    impl_->maxZoom = maxZoom;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

// Layout properties


// Paint properties

PropertyValue<float> HeatmapLayer::getDefaultHeatmapRadius() {
    return { 30 };
}

PropertyValue<float> HeatmapLayer::getHeatmapRadius() const {
    return impl().paint.template get<HeatmapRadius>().value;
}

void HeatmapLayer::setHeatmapRadius(PropertyValue<float> value) {
    if (value == getHeatmapRadius())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<HeatmapRadius>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void HeatmapLayer::setHeatmapRadiusTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<HeatmapRadius>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions HeatmapLayer::getHeatmapRadiusTransition() const {
    return impl().paint.template get<HeatmapRadius>().options;
}

PropertyValue<float> HeatmapLayer::getDefaultHeatmapWeight() {
    return { 1 };
}

PropertyValue<float> HeatmapLayer::getHeatmapWeight() const {
    return impl().paint.template get<HeatmapWeight>().value;
}

void HeatmapLayer::setHeatmapWeight(PropertyValue<float> value) {
    if (value == getHeatmapWeight())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<HeatmapWeight>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void HeatmapLayer::setHeatmapWeightTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<HeatmapWeight>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions HeatmapLayer::getHeatmapWeightTransition() const {
    return impl().paint.template get<HeatmapWeight>().options;
}

PropertyValue<float> HeatmapLayer::getDefaultHeatmapIntensity() {
    return { 1 };
}

PropertyValue<float> HeatmapLayer::getHeatmapIntensity() const {
    return impl().paint.template get<HeatmapIntensity>().value;
}

void HeatmapLayer::setHeatmapIntensity(PropertyValue<float> value) {
    if (value == getHeatmapIntensity())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<HeatmapIntensity>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void HeatmapLayer::setHeatmapIntensityTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<HeatmapIntensity>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions HeatmapLayer::getHeatmapIntensityTransition() const {
    return impl().paint.template get<HeatmapIntensity>().options;
}

ColorRampPropertyValue HeatmapLayer::getDefaultHeatmapColor() {
    conversion::Error error;
    std::string rawValue = R"JSON(["interpolate",["linear"],["heatmap-density"],0,"rgba(0, 0, 255, 0)",0.1,"royalblue",0.3,"cyan",0.5,"lime",0.7,"yellow",1,"red"])JSON";
    return *conversion::convertJSON<ColorRampPropertyValue>(rawValue, error);
}

ColorRampPropertyValue HeatmapLayer::getHeatmapColor() const {
    return impl().paint.template get<HeatmapColor>().value;
}

void HeatmapLayer::setHeatmapColor(ColorRampPropertyValue value) {
    if (value == getHeatmapColor())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<HeatmapColor>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void HeatmapLayer::setHeatmapColorTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<HeatmapColor>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions HeatmapLayer::getHeatmapColorTransition() const {
    return impl().paint.template get<HeatmapColor>().options;
}

PropertyValue<float> HeatmapLayer::getDefaultHeatmapOpacity() {
    return { 1 };
}

PropertyValue<float> HeatmapLayer::getHeatmapOpacity() const {
    return impl().paint.template get<HeatmapOpacity>().value;
}

void HeatmapLayer::setHeatmapOpacity(PropertyValue<float> value) {
    if (value == getHeatmapOpacity())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<HeatmapOpacity>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void HeatmapLayer::setHeatmapOpacityTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<HeatmapOpacity>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions HeatmapLayer::getHeatmapOpacityTransition() const {
    return impl().paint.template get<HeatmapOpacity>().options;
}

using namespace conversion;

optional<Error> HeatmapLayer::setPaintProperty(const std::string& name, const Convertible& value) {
    enum class Property {
        Unknown,
        HeatmapRadius,
        HeatmapWeight,
        HeatmapIntensity,
        HeatmapColor,
        HeatmapOpacity,
        HeatmapRadiusTransition,
        HeatmapWeightTransition,
        HeatmapIntensityTransition,
        HeatmapColorTransition,
        HeatmapOpacityTransition,
    };

    Property property = Property::Unknown;
    switch (util::hashFNV1a(name.c_str())) {
    case util::hashFNV1a("heatmap-radius"):
        if (name == "heatmap-radius") {
            property = Property::HeatmapRadius;
        }
        break;
    case util::hashFNV1a("heatmap-radius-transition"):
        if (name == "heatmap-radius-transition") {
            property = Property::HeatmapRadiusTransition;
        }
        break;
    case util::hashFNV1a("heatmap-weight"):
        if (name == "heatmap-weight") {
            property = Property::HeatmapWeight;
        }
        break;
    case util::hashFNV1a("heatmap-weight-transition"):
        if (name == "heatmap-weight-transition") {
            property = Property::HeatmapWeightTransition;
        }
        break;
    case util::hashFNV1a("heatmap-intensity"):
        if (name == "heatmap-intensity") {
            property = Property::HeatmapIntensity;
        }
        break;
    case util::hashFNV1a("heatmap-intensity-transition"):
        if (name == "heatmap-intensity-transition") {
            property = Property::HeatmapIntensityTransition;
        }
        break;
    case util::hashFNV1a("heatmap-color"):
        if (name == "heatmap-color") {
            property = Property::HeatmapColor;
        }
        break;
    case util::hashFNV1a("heatmap-color-transition"):
        if (name == "heatmap-color-transition") {
            property = Property::HeatmapColorTransition;
        }
        break;
    case util::hashFNV1a("heatmap-opacity"):
        if (name == "heatmap-opacity") {
            property = Property::HeatmapOpacity;
        }
        break;
    case util::hashFNV1a("heatmap-opacity-transition"):
        if (name == "heatmap-opacity-transition") {
            property = Property::HeatmapOpacityTransition;
        }
        break;
    
    }

    if (property == Property::Unknown) {
        return Error { "layer doesn't support this property" };
    }

        
    if (property == Property::HeatmapRadius || property == Property::HeatmapWeight) {
        Error error;
        optional<PropertyValue<float>> typedValue = convert<PropertyValue<float>>(value, error, true, false);
        if (!typedValue) {
            return error;
        }
        
        if (property == Property::HeatmapRadius) {
            setHeatmapRadius(*typedValue);
            return nullopt;
        }
        
        if (property == Property::HeatmapWeight) {
            setHeatmapWeight(*typedValue);
            return nullopt;
        }
        
    }
    
    if (property == Property::HeatmapIntensity || property == Property::HeatmapOpacity) {
        Error error;
        optional<PropertyValue<float>> typedValue = convert<PropertyValue<float>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }
        
        if (property == Property::HeatmapIntensity) {
            setHeatmapIntensity(*typedValue);
            return nullopt;
        }
        
        if (property == Property::HeatmapOpacity) {
            setHeatmapOpacity(*typedValue);
            return nullopt;
        }
        
    }
    
    if (property == Property::HeatmapColor) {
        Error error;
        optional<ColorRampPropertyValue> typedValue = convert<ColorRampPropertyValue>(value, error, false, false);
        if (!typedValue) {
            return error;
        }
        
        setHeatmapColor(*typedValue);
        return nullopt;
        
    }
    

    Error error;
    optional<TransitionOptions> transition = convert<TransitionOptions>(value, error);
    if (!transition) {
        return error;
    }
    
    if (property == Property::HeatmapRadiusTransition) {
        setHeatmapRadiusTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::HeatmapWeightTransition) {
        setHeatmapWeightTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::HeatmapIntensityTransition) {
        setHeatmapIntensityTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::HeatmapColorTransition) {
        setHeatmapColorTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::HeatmapOpacityTransition) {
        setHeatmapOpacityTransition(*transition);
        return nullopt;
    }
    

    return Error { "layer doesn't support this property" };
}

optional<Error> HeatmapLayer::setLayoutProperty(const std::string& name, const Convertible& value) {
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
