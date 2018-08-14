// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/circle_layer.hpp>
#include <mbgl/style/layers/circle_layer_impl.hpp>
#include <mbgl/style/layer_observer.hpp>
#include <mbgl/style/conversion.hpp>
#include <mbgl/style/conversion/color_ramp_property_value.hpp>
#include <mbgl/style/conversion/constant.hpp>
#include <mbgl/style/conversion/property_value.hpp>
#include <mbgl/style/conversion/transition_options.hpp>
#include <mbgl/style/conversion/json.hpp>
#include <mbgl/util/fnv_hash.hpp>

namespace mbgl {
namespace style {

CircleLayer::CircleLayer(const std::string& layerID, const std::string& sourceID)
    : Layer(makeMutable<Impl>(LayerType::Circle, layerID, sourceID)) {
}

CircleLayer::CircleLayer(Immutable<Impl> impl_)
    : Layer(std::move(impl_)) {
}

CircleLayer::~CircleLayer() = default;

const CircleLayer::Impl& CircleLayer::impl() const {
    return static_cast<const Impl&>(*baseImpl);
}

Mutable<CircleLayer::Impl> CircleLayer::mutableImpl() const {
    return makeMutable<Impl>(impl());
}

std::unique_ptr<Layer> CircleLayer::cloneRef(const std::string& id_) const {
    auto impl_ = mutableImpl();
    impl_->id = id_;
    impl_->paint = CirclePaintProperties::Transitionable();
    return std::make_unique<CircleLayer>(std::move(impl_));
}

void CircleLayer::Impl::stringifyLayout(rapidjson::Writer<rapidjson::StringBuffer>&) const {
}

// Source

const std::string& CircleLayer::getSourceID() const {
    return impl().source;
}

void CircleLayer::setSourceLayer(const std::string& sourceLayer) {
    auto impl_ = mutableImpl();
    impl_->sourceLayer = sourceLayer;
    baseImpl = std::move(impl_);
}

const std::string& CircleLayer::getSourceLayer() const {
    return impl().sourceLayer;
}

// Filter

void CircleLayer::setFilter(const Filter& filter) {
    auto impl_ = mutableImpl();
    impl_->filter = filter;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

const Filter& CircleLayer::getFilter() const {
    return impl().filter;
}

// Visibility

void CircleLayer::setVisibility(VisibilityType value) {
    if (value == getVisibility())
        return;
    auto impl_ = mutableImpl();
    impl_->visibility = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

// Zoom range

void CircleLayer::setMinZoom(float minZoom) {
    auto impl_ = mutableImpl();
    impl_->minZoom = minZoom;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void CircleLayer::setMaxZoom(float maxZoom) {
    auto impl_ = mutableImpl();
    impl_->maxZoom = maxZoom;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

// Layout properties


// Paint properties

PropertyValue<float> CircleLayer::getDefaultCircleRadius() {
    return { 5 };
}

PropertyValue<float> CircleLayer::getCircleRadius() const {
    return impl().paint.circleRadius.value;
}

void CircleLayer::setCircleRadius(PropertyValue<float> value) {
    if (value == getCircleRadius())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.circleRadius.value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void CircleLayer::setCircleRadiusTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.circleRadius.options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions CircleLayer::getCircleRadiusTransition() const {
    return impl().paint.circleRadius.options;
}

PropertyValue<Color> CircleLayer::getDefaultCircleColor() {
    return { Color::black() };
}

PropertyValue<Color> CircleLayer::getCircleColor() const {
    return impl().paint.circleColor.value;
}

void CircleLayer::setCircleColor(PropertyValue<Color> value) {
    if (value == getCircleColor())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.circleColor.value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void CircleLayer::setCircleColorTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.circleColor.options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions CircleLayer::getCircleColorTransition() const {
    return impl().paint.circleColor.options;
}

PropertyValue<float> CircleLayer::getDefaultCircleBlur() {
    return { 0 };
}

PropertyValue<float> CircleLayer::getCircleBlur() const {
    return impl().paint.circleBlur.value;
}

void CircleLayer::setCircleBlur(PropertyValue<float> value) {
    if (value == getCircleBlur())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.circleBlur.value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void CircleLayer::setCircleBlurTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.circleBlur.options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions CircleLayer::getCircleBlurTransition() const {
    return impl().paint.circleBlur.options;
}

PropertyValue<float> CircleLayer::getDefaultCircleOpacity() {
    return { 1 };
}

PropertyValue<float> CircleLayer::getCircleOpacity() const {
    return impl().paint.circleOpacity.value;
}

void CircleLayer::setCircleOpacity(PropertyValue<float> value) {
    if (value == getCircleOpacity())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.circleOpacity.value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void CircleLayer::setCircleOpacityTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.circleOpacity.options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions CircleLayer::getCircleOpacityTransition() const {
    return impl().paint.circleOpacity.options;
}

PropertyValue<std::array<float, 2>> CircleLayer::getDefaultCircleTranslate() {
    return { {{ 0, 0 }} };
}

PropertyValue<std::array<float, 2>> CircleLayer::getCircleTranslate() const {
    return impl().paint.circleTranslate.value;
}

void CircleLayer::setCircleTranslate(PropertyValue<std::array<float, 2>> value) {
    if (value == getCircleTranslate())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.circleTranslate.value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void CircleLayer::setCircleTranslateTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.circleTranslate.options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions CircleLayer::getCircleTranslateTransition() const {
    return impl().paint.circleTranslate.options;
}

PropertyValue<TranslateAnchorType> CircleLayer::getDefaultCircleTranslateAnchor() {
    return { TranslateAnchorType::Map };
}

PropertyValue<TranslateAnchorType> CircleLayer::getCircleTranslateAnchor() const {
    return impl().paint.circleTranslateAnchor.value;
}

void CircleLayer::setCircleTranslateAnchor(PropertyValue<TranslateAnchorType> value) {
    if (value == getCircleTranslateAnchor())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.circleTranslateAnchor.value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void CircleLayer::setCircleTranslateAnchorTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.circleTranslateAnchor.options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions CircleLayer::getCircleTranslateAnchorTransition() const {
    return impl().paint.circleTranslateAnchor.options;
}

PropertyValue<CirclePitchScaleType> CircleLayer::getDefaultCirclePitchScale() {
    return { CirclePitchScaleType::Map };
}

PropertyValue<CirclePitchScaleType> CircleLayer::getCirclePitchScale() const {
    return impl().paint.circlePitchScale.value;
}

void CircleLayer::setCirclePitchScale(PropertyValue<CirclePitchScaleType> value) {
    if (value == getCirclePitchScale())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.circlePitchScale.value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void CircleLayer::setCirclePitchScaleTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.circlePitchScale.options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions CircleLayer::getCirclePitchScaleTransition() const {
    return impl().paint.circlePitchScale.options;
}

PropertyValue<AlignmentType> CircleLayer::getDefaultCirclePitchAlignment() {
    return { AlignmentType::Viewport };
}

PropertyValue<AlignmentType> CircleLayer::getCirclePitchAlignment() const {
    return impl().paint.circlePitchAlignment.value;
}

void CircleLayer::setCirclePitchAlignment(PropertyValue<AlignmentType> value) {
    if (value == getCirclePitchAlignment())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.circlePitchAlignment.value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void CircleLayer::setCirclePitchAlignmentTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.circlePitchAlignment.options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions CircleLayer::getCirclePitchAlignmentTransition() const {
    return impl().paint.circlePitchAlignment.options;
}

PropertyValue<float> CircleLayer::getDefaultCircleStrokeWidth() {
    return { 0 };
}

PropertyValue<float> CircleLayer::getCircleStrokeWidth() const {
    return impl().paint.circleStrokeWidth.value;
}

void CircleLayer::setCircleStrokeWidth(PropertyValue<float> value) {
    if (value == getCircleStrokeWidth())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.circleStrokeWidth.value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void CircleLayer::setCircleStrokeWidthTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.circleStrokeWidth.options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions CircleLayer::getCircleStrokeWidthTransition() const {
    return impl().paint.circleStrokeWidth.options;
}

PropertyValue<Color> CircleLayer::getDefaultCircleStrokeColor() {
    return { Color::black() };
}

PropertyValue<Color> CircleLayer::getCircleStrokeColor() const {
    return impl().paint.circleStrokeColor.value;
}

void CircleLayer::setCircleStrokeColor(PropertyValue<Color> value) {
    if (value == getCircleStrokeColor())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.circleStrokeColor.value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void CircleLayer::setCircleStrokeColorTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.circleStrokeColor.options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions CircleLayer::getCircleStrokeColorTransition() const {
    return impl().paint.circleStrokeColor.options;
}

PropertyValue<float> CircleLayer::getDefaultCircleStrokeOpacity() {
    return { 1 };
}

PropertyValue<float> CircleLayer::getCircleStrokeOpacity() const {
    return impl().paint.circleStrokeOpacity.value;
}

void CircleLayer::setCircleStrokeOpacity(PropertyValue<float> value) {
    if (value == getCircleStrokeOpacity())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.circleStrokeOpacity.value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void CircleLayer::setCircleStrokeOpacityTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.circleStrokeOpacity.options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions CircleLayer::getCircleStrokeOpacityTransition() const {
    return impl().paint.circleStrokeOpacity.options;
}

using namespace conversion;

optional<Error> CircleLayer::setPaintProperty(const std::string& name, const Convertible& value) {
    enum class Property {
        Unknown,
        CircleRadius,
        CircleColor,
        CircleBlur,
        CircleOpacity,
        CircleTranslate,
        CircleTranslateAnchor,
        CirclePitchScale,
        CirclePitchAlignment,
        CircleStrokeWidth,
        CircleStrokeColor,
        CircleStrokeOpacity,
        CircleRadiusTransition,
        CircleColorTransition,
        CircleBlurTransition,
        CircleOpacityTransition,
        CircleTranslateTransition,
        CircleTranslateAnchorTransition,
        CirclePitchScaleTransition,
        CirclePitchAlignmentTransition,
        CircleStrokeWidthTransition,
        CircleStrokeColorTransition,
        CircleStrokeOpacityTransition,
    };

    Property property = Property::Unknown;
    switch (util::hashFNV1a(name.c_str())) {
    case util::hashFNV1a("circle-radius"):
        if (name == "circle-radius") {
            property = Property::CircleRadius;
        }
        break;
    case util::hashFNV1a("circle-radius-transition"):
        if (name == "circle-radius-transition") {
            property = Property::CircleRadiusTransition;
        }
        break;
    case util::hashFNV1a("circle-color"):
        if (name == "circle-color") {
            property = Property::CircleColor;
        }
        break;
    case util::hashFNV1a("circle-color-transition"):
        if (name == "circle-color-transition") {
            property = Property::CircleColorTransition;
        }
        break;
    case util::hashFNV1a("circle-blur"):
        if (name == "circle-blur") {
            property = Property::CircleBlur;
        }
        break;
    case util::hashFNV1a("circle-blur-transition"):
        if (name == "circle-blur-transition") {
            property = Property::CircleBlurTransition;
        }
        break;
    case util::hashFNV1a("circle-opacity"):
        if (name == "circle-opacity") {
            property = Property::CircleOpacity;
        }
        break;
    case util::hashFNV1a("circle-opacity-transition"):
        if (name == "circle-opacity-transition") {
            property = Property::CircleOpacityTransition;
        }
        break;
    case util::hashFNV1a("circle-translate"):
        if (name == "circle-translate") {
            property = Property::CircleTranslate;
        }
        break;
    case util::hashFNV1a("circle-translate-transition"):
        if (name == "circle-translate-transition") {
            property = Property::CircleTranslateTransition;
        }
        break;
    case util::hashFNV1a("circle-translate-anchor"):
        if (name == "circle-translate-anchor") {
            property = Property::CircleTranslateAnchor;
        }
        break;
    case util::hashFNV1a("circle-translate-anchor-transition"):
        if (name == "circle-translate-anchor-transition") {
            property = Property::CircleTranslateAnchorTransition;
        }
        break;
    case util::hashFNV1a("circle-pitch-scale"):
        if (name == "circle-pitch-scale") {
            property = Property::CirclePitchScale;
        }
        break;
    case util::hashFNV1a("circle-pitch-scale-transition"):
        if (name == "circle-pitch-scale-transition") {
            property = Property::CirclePitchScaleTransition;
        }
        break;
    case util::hashFNV1a("circle-pitch-alignment"):
        if (name == "circle-pitch-alignment") {
            property = Property::CirclePitchAlignment;
        }
        break;
    case util::hashFNV1a("circle-pitch-alignment-transition"):
        if (name == "circle-pitch-alignment-transition") {
            property = Property::CirclePitchAlignmentTransition;
        }
        break;
    case util::hashFNV1a("circle-stroke-width"):
        if (name == "circle-stroke-width") {
            property = Property::CircleStrokeWidth;
        }
        break;
    case util::hashFNV1a("circle-stroke-width-transition"):
        if (name == "circle-stroke-width-transition") {
            property = Property::CircleStrokeWidthTransition;
        }
        break;
    case util::hashFNV1a("circle-stroke-color"):
        if (name == "circle-stroke-color") {
            property = Property::CircleStrokeColor;
        }
        break;
    case util::hashFNV1a("circle-stroke-color-transition"):
        if (name == "circle-stroke-color-transition") {
            property = Property::CircleStrokeColorTransition;
        }
        break;
    case util::hashFNV1a("circle-stroke-opacity"):
        if (name == "circle-stroke-opacity") {
            property = Property::CircleStrokeOpacity;
        }
        break;
    case util::hashFNV1a("circle-stroke-opacity-transition"):
        if (name == "circle-stroke-opacity-transition") {
            property = Property::CircleStrokeOpacityTransition;
        }
        break;
    
    }

    if (property == Property::Unknown) {
        return Error { "layer doesn't support this property" };
    }

        
    if (property == Property::CircleRadius || property == Property::CircleBlur || property == Property::CircleOpacity || property == Property::CircleStrokeWidth || property == Property::CircleStrokeOpacity) {
        Error error;
        optional<PropertyValue<float>> typedValue = convert<PropertyValue<float>>(value, error, true, false);
        if (!typedValue) {
            return error;
        }
        
        if (property == Property::CircleRadius) {
            setCircleRadius(*typedValue);
            return nullopt;
        }
        
        if (property == Property::CircleBlur) {
            setCircleBlur(*typedValue);
            return nullopt;
        }
        
        if (property == Property::CircleOpacity) {
            setCircleOpacity(*typedValue);
            return nullopt;
        }
        
        if (property == Property::CircleStrokeWidth) {
            setCircleStrokeWidth(*typedValue);
            return nullopt;
        }
        
        if (property == Property::CircleStrokeOpacity) {
            setCircleStrokeOpacity(*typedValue);
            return nullopt;
        }
        
    }
    
    if (property == Property::CircleColor || property == Property::CircleStrokeColor) {
        Error error;
        optional<PropertyValue<Color>> typedValue = convert<PropertyValue<Color>>(value, error, true, false);
        if (!typedValue) {
            return error;
        }
        
        if (property == Property::CircleColor) {
            setCircleColor(*typedValue);
            return nullopt;
        }
        
        if (property == Property::CircleStrokeColor) {
            setCircleStrokeColor(*typedValue);
            return nullopt;
        }
        
    }
    
    if (property == Property::CircleTranslate) {
        Error error;
        optional<PropertyValue<std::array<float, 2>>> typedValue = convert<PropertyValue<std::array<float, 2>>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }
        
        setCircleTranslate(*typedValue);
        return nullopt;
        
    }
    
    if (property == Property::CircleTranslateAnchor) {
        Error error;
        optional<PropertyValue<TranslateAnchorType>> typedValue = convert<PropertyValue<TranslateAnchorType>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }
        
        setCircleTranslateAnchor(*typedValue);
        return nullopt;
        
    }
    
    if (property == Property::CirclePitchScale) {
        Error error;
        optional<PropertyValue<CirclePitchScaleType>> typedValue = convert<PropertyValue<CirclePitchScaleType>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }
        
        setCirclePitchScale(*typedValue);
        return nullopt;
        
    }
    
    if (property == Property::CirclePitchAlignment) {
        Error error;
        optional<PropertyValue<AlignmentType>> typedValue = convert<PropertyValue<AlignmentType>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }
        
        setCirclePitchAlignment(*typedValue);
        return nullopt;
        
    }
    

    Error error;
    optional<TransitionOptions> transition = convert<TransitionOptions>(value, error);
    if (!transition) {
        return error;
    }
    
    if (property == Property::CircleRadiusTransition) {
        setCircleRadiusTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::CircleColorTransition) {
        setCircleColorTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::CircleBlurTransition) {
        setCircleBlurTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::CircleOpacityTransition) {
        setCircleOpacityTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::CircleTranslateTransition) {
        setCircleTranslateTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::CircleTranslateAnchorTransition) {
        setCircleTranslateAnchorTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::CirclePitchScaleTransition) {
        setCirclePitchScaleTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::CirclePitchAlignmentTransition) {
        setCirclePitchAlignmentTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::CircleStrokeWidthTransition) {
        setCircleStrokeWidthTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::CircleStrokeColorTransition) {
        setCircleStrokeColorTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::CircleStrokeOpacityTransition) {
        setCircleStrokeOpacityTransition(*transition);
        return nullopt;
    }
    

    return Error { "layer doesn't support this property" };
}

optional<Error> CircleLayer::setLayoutProperty(const std::string& name, const Convertible& value) {
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
