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
    return impl().paint.template get<CircleRadius>().value;
}

void CircleLayer::setCircleRadius(PropertyValue<float> value) {
    if (value == getCircleRadius())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<CircleRadius>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void CircleLayer::setCircleRadiusTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<CircleRadius>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions CircleLayer::getCircleRadiusTransition() const {
    return impl().paint.template get<CircleRadius>().options;
}

PropertyValue<Color> CircleLayer::getDefaultCircleColor() {
    return { Color::black() };
}

PropertyValue<Color> CircleLayer::getCircleColor() const {
    return impl().paint.template get<CircleColor>().value;
}

void CircleLayer::setCircleColor(PropertyValue<Color> value) {
    if (value == getCircleColor())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<CircleColor>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void CircleLayer::setCircleColorTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<CircleColor>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions CircleLayer::getCircleColorTransition() const {
    return impl().paint.template get<CircleColor>().options;
}

PropertyValue<float> CircleLayer::getDefaultCircleBlur() {
    return { 0 };
}

PropertyValue<float> CircleLayer::getCircleBlur() const {
    return impl().paint.template get<CircleBlur>().value;
}

void CircleLayer::setCircleBlur(PropertyValue<float> value) {
    if (value == getCircleBlur())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<CircleBlur>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void CircleLayer::setCircleBlurTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<CircleBlur>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions CircleLayer::getCircleBlurTransition() const {
    return impl().paint.template get<CircleBlur>().options;
}

PropertyValue<float> CircleLayer::getDefaultCircleOpacity() {
    return { 1 };
}

PropertyValue<float> CircleLayer::getCircleOpacity() const {
    return impl().paint.template get<CircleOpacity>().value;
}

void CircleLayer::setCircleOpacity(PropertyValue<float> value) {
    if (value == getCircleOpacity())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<CircleOpacity>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void CircleLayer::setCircleOpacityTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<CircleOpacity>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions CircleLayer::getCircleOpacityTransition() const {
    return impl().paint.template get<CircleOpacity>().options;
}

PropertyValue<std::array<float, 2>> CircleLayer::getDefaultCircleTranslate() {
    return { {{ 0, 0 }} };
}

PropertyValue<std::array<float, 2>> CircleLayer::getCircleTranslate() const {
    return impl().paint.template get<CircleTranslate>().value;
}

void CircleLayer::setCircleTranslate(PropertyValue<std::array<float, 2>> value) {
    if (value == getCircleTranslate())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<CircleTranslate>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void CircleLayer::setCircleTranslateTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<CircleTranslate>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions CircleLayer::getCircleTranslateTransition() const {
    return impl().paint.template get<CircleTranslate>().options;
}

PropertyValue<TranslateAnchorType> CircleLayer::getDefaultCircleTranslateAnchor() {
    return { TranslateAnchorType::Map };
}

PropertyValue<TranslateAnchorType> CircleLayer::getCircleTranslateAnchor() const {
    return impl().paint.template get<CircleTranslateAnchor>().value;
}

void CircleLayer::setCircleTranslateAnchor(PropertyValue<TranslateAnchorType> value) {
    if (value == getCircleTranslateAnchor())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<CircleTranslateAnchor>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void CircleLayer::setCircleTranslateAnchorTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<CircleTranslateAnchor>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions CircleLayer::getCircleTranslateAnchorTransition() const {
    return impl().paint.template get<CircleTranslateAnchor>().options;
}

PropertyValue<CirclePitchScaleType> CircleLayer::getDefaultCirclePitchScale() {
    return { CirclePitchScaleType::Map };
}

PropertyValue<CirclePitchScaleType> CircleLayer::getCirclePitchScale() const {
    return impl().paint.template get<CirclePitchScale>().value;
}

void CircleLayer::setCirclePitchScale(PropertyValue<CirclePitchScaleType> value) {
    if (value == getCirclePitchScale())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<CirclePitchScale>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void CircleLayer::setCirclePitchScaleTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<CirclePitchScale>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions CircleLayer::getCirclePitchScaleTransition() const {
    return impl().paint.template get<CirclePitchScale>().options;
}

PropertyValue<AlignmentType> CircleLayer::getDefaultCirclePitchAlignment() {
    return { AlignmentType::Viewport };
}

PropertyValue<AlignmentType> CircleLayer::getCirclePitchAlignment() const {
    return impl().paint.template get<CirclePitchAlignment>().value;
}

void CircleLayer::setCirclePitchAlignment(PropertyValue<AlignmentType> value) {
    if (value == getCirclePitchAlignment())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<CirclePitchAlignment>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void CircleLayer::setCirclePitchAlignmentTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<CirclePitchAlignment>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions CircleLayer::getCirclePitchAlignmentTransition() const {
    return impl().paint.template get<CirclePitchAlignment>().options;
}

PropertyValue<float> CircleLayer::getDefaultCircleStrokeWidth() {
    return { 0 };
}

PropertyValue<float> CircleLayer::getCircleStrokeWidth() const {
    return impl().paint.template get<CircleStrokeWidth>().value;
}

void CircleLayer::setCircleStrokeWidth(PropertyValue<float> value) {
    if (value == getCircleStrokeWidth())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<CircleStrokeWidth>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void CircleLayer::setCircleStrokeWidthTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<CircleStrokeWidth>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions CircleLayer::getCircleStrokeWidthTransition() const {
    return impl().paint.template get<CircleStrokeWidth>().options;
}

PropertyValue<Color> CircleLayer::getDefaultCircleStrokeColor() {
    return { Color::black() };
}

PropertyValue<Color> CircleLayer::getCircleStrokeColor() const {
    return impl().paint.template get<CircleStrokeColor>().value;
}

void CircleLayer::setCircleStrokeColor(PropertyValue<Color> value) {
    if (value == getCircleStrokeColor())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<CircleStrokeColor>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void CircleLayer::setCircleStrokeColorTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<CircleStrokeColor>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions CircleLayer::getCircleStrokeColorTransition() const {
    return impl().paint.template get<CircleStrokeColor>().options;
}

PropertyValue<float> CircleLayer::getDefaultCircleStrokeOpacity() {
    return { 1 };
}

PropertyValue<float> CircleLayer::getCircleStrokeOpacity() const {
    return impl().paint.template get<CircleStrokeOpacity>().value;
}

void CircleLayer::setCircleStrokeOpacity(PropertyValue<float> value) {
    if (value == getCircleStrokeOpacity())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<CircleStrokeOpacity>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void CircleLayer::setCircleStrokeOpacityTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<CircleStrokeOpacity>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions CircleLayer::getCircleStrokeOpacityTransition() const {
    return impl().paint.template get<CircleStrokeOpacity>().options;
}

using namespace conversion;

optional<Error> CircleLayer::setPaintProperty(const std::string& name, const Convertible& value) {
    
    if (name == "circle-radius") {
        Error error;
        optional<PropertyValue<float>> typedValue = convert<PropertyValue<float>>(value, error, true, false);
        if (!typedValue) {
            return error;
        }

        setCircleRadius(*typedValue);
        return nullopt;
    }
    if (name == "circle-radius-transition") {
        Error error;
        optional<TransitionOptions> transition = convert<TransitionOptions>(value, error);
        if (!transition) {
            return error;
        }

        setCircleRadiusTransition(*transition);
        return nullopt;
    }
    
    if (name == "circle-color") {
        Error error;
        optional<PropertyValue<Color>> typedValue = convert<PropertyValue<Color>>(value, error, true, false);
        if (!typedValue) {
            return error;
        }

        setCircleColor(*typedValue);
        return nullopt;
    }
    if (name == "circle-color-transition") {
        Error error;
        optional<TransitionOptions> transition = convert<TransitionOptions>(value, error);
        if (!transition) {
            return error;
        }

        setCircleColorTransition(*transition);
        return nullopt;
    }
    
    if (name == "circle-blur") {
        Error error;
        optional<PropertyValue<float>> typedValue = convert<PropertyValue<float>>(value, error, true, false);
        if (!typedValue) {
            return error;
        }

        setCircleBlur(*typedValue);
        return nullopt;
    }
    if (name == "circle-blur-transition") {
        Error error;
        optional<TransitionOptions> transition = convert<TransitionOptions>(value, error);
        if (!transition) {
            return error;
        }

        setCircleBlurTransition(*transition);
        return nullopt;
    }
    
    if (name == "circle-opacity") {
        Error error;
        optional<PropertyValue<float>> typedValue = convert<PropertyValue<float>>(value, error, true, false);
        if (!typedValue) {
            return error;
        }

        setCircleOpacity(*typedValue);
        return nullopt;
    }
    if (name == "circle-opacity-transition") {
        Error error;
        optional<TransitionOptions> transition = convert<TransitionOptions>(value, error);
        if (!transition) {
            return error;
        }

        setCircleOpacityTransition(*transition);
        return nullopt;
    }
    
    if (name == "circle-translate") {
        Error error;
        optional<PropertyValue<std::array<float, 2>>> typedValue = convert<PropertyValue<std::array<float, 2>>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }

        setCircleTranslate(*typedValue);
        return nullopt;
    }
    if (name == "circle-translate-transition") {
        Error error;
        optional<TransitionOptions> transition = convert<TransitionOptions>(value, error);
        if (!transition) {
            return error;
        }

        setCircleTranslateTransition(*transition);
        return nullopt;
    }
    
    if (name == "circle-translate-anchor") {
        Error error;
        optional<PropertyValue<TranslateAnchorType>> typedValue = convert<PropertyValue<TranslateAnchorType>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }

        setCircleTranslateAnchor(*typedValue);
        return nullopt;
    }
    if (name == "circle-translate-anchor-transition") {
        Error error;
        optional<TransitionOptions> transition = convert<TransitionOptions>(value, error);
        if (!transition) {
            return error;
        }

        setCircleTranslateAnchorTransition(*transition);
        return nullopt;
    }
    
    if (name == "circle-pitch-scale") {
        Error error;
        optional<PropertyValue<CirclePitchScaleType>> typedValue = convert<PropertyValue<CirclePitchScaleType>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }

        setCirclePitchScale(*typedValue);
        return nullopt;
    }
    if (name == "circle-pitch-scale-transition") {
        Error error;
        optional<TransitionOptions> transition = convert<TransitionOptions>(value, error);
        if (!transition) {
            return error;
        }

        setCirclePitchScaleTransition(*transition);
        return nullopt;
    }
    
    if (name == "circle-pitch-alignment") {
        Error error;
        optional<PropertyValue<AlignmentType>> typedValue = convert<PropertyValue<AlignmentType>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }

        setCirclePitchAlignment(*typedValue);
        return nullopt;
    }
    if (name == "circle-pitch-alignment-transition") {
        Error error;
        optional<TransitionOptions> transition = convert<TransitionOptions>(value, error);
        if (!transition) {
            return error;
        }

        setCirclePitchAlignmentTransition(*transition);
        return nullopt;
    }
    
    if (name == "circle-stroke-width") {
        Error error;
        optional<PropertyValue<float>> typedValue = convert<PropertyValue<float>>(value, error, true, false);
        if (!typedValue) {
            return error;
        }

        setCircleStrokeWidth(*typedValue);
        return nullopt;
    }
    if (name == "circle-stroke-width-transition") {
        Error error;
        optional<TransitionOptions> transition = convert<TransitionOptions>(value, error);
        if (!transition) {
            return error;
        }

        setCircleStrokeWidthTransition(*transition);
        return nullopt;
    }
    
    if (name == "circle-stroke-color") {
        Error error;
        optional<PropertyValue<Color>> typedValue = convert<PropertyValue<Color>>(value, error, true, false);
        if (!typedValue) {
            return error;
        }

        setCircleStrokeColor(*typedValue);
        return nullopt;
    }
    if (name == "circle-stroke-color-transition") {
        Error error;
        optional<TransitionOptions> transition = convert<TransitionOptions>(value, error);
        if (!transition) {
            return error;
        }

        setCircleStrokeColorTransition(*transition);
        return nullopt;
    }
    
    if (name == "circle-stroke-opacity") {
        Error error;
        optional<PropertyValue<float>> typedValue = convert<PropertyValue<float>>(value, error, true, false);
        if (!typedValue) {
            return error;
        }

        setCircleStrokeOpacity(*typedValue);
        return nullopt;
    }
    if (name == "circle-stroke-opacity-transition") {
        Error error;
        optional<TransitionOptions> transition = convert<TransitionOptions>(value, error);
        if (!transition) {
            return error;
        }

        setCircleStrokeOpacityTransition(*transition);
        return nullopt;
    }
    
    return Error { "layer doesn't support this property" };
}

optional<Error> CircleLayer::setLayoutProperty(const std::string& name, const Convertible& value) {
    if (name == "visibility") {
        if (isUndefined(value)) {
            setVisibility(VisibilityType::Visible);
            return nullopt;
        }

        Error error;
        optional<VisibilityType> visibility = convert<VisibilityType>(value, error);
        if (!visibility) {
            return error;
        }

        setVisibility(*visibility);
        return nullopt;
    }

    
    return Error { "layer doesn't support this property" };
}

} // namespace style
} // namespace mbgl
