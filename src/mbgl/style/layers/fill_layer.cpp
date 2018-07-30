// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/fill_layer.hpp>
#include <mbgl/style/layers/fill_layer_impl.hpp>
#include <mbgl/style/layer_observer.hpp>
#include <mbgl/style/conversion.hpp>
#include <mbgl/style/conversion/color_ramp_property_value.hpp>
#include <mbgl/style/conversion/constant.hpp>
#include <mbgl/style/conversion/property_value.hpp>
#include <mbgl/style/conversion/transition_options.hpp>
#include <mbgl/style/conversion/json.hpp>

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
    
    if (name == "fill-antialias") {
        Error error;
        optional<PropertyValue<bool>> typedValue = convert<PropertyValue<bool>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }

        setFillAntialias(*typedValue);
        return nullopt;
    }
    if (name == "fill-antialias-transition") {
        Error error;
        optional<TransitionOptions> transition = convert<TransitionOptions>(value, error);
        if (!transition) {
            return error;
        }

        setFillAntialiasTransition(*transition);
        return nullopt;
    }
    
    if (name == "fill-opacity") {
        Error error;
        optional<PropertyValue<float>> typedValue = convert<PropertyValue<float>>(value, error, true, false);
        if (!typedValue) {
            return error;
        }

        setFillOpacity(*typedValue);
        return nullopt;
    }
    if (name == "fill-opacity-transition") {
        Error error;
        optional<TransitionOptions> transition = convert<TransitionOptions>(value, error);
        if (!transition) {
            return error;
        }

        setFillOpacityTransition(*transition);
        return nullopt;
    }
    
    if (name == "fill-color") {
        Error error;
        optional<PropertyValue<Color>> typedValue = convert<PropertyValue<Color>>(value, error, true, false);
        if (!typedValue) {
            return error;
        }

        setFillColor(*typedValue);
        return nullopt;
    }
    if (name == "fill-color-transition") {
        Error error;
        optional<TransitionOptions> transition = convert<TransitionOptions>(value, error);
        if (!transition) {
            return error;
        }

        setFillColorTransition(*transition);
        return nullopt;
    }
    
    if (name == "fill-outline-color") {
        Error error;
        optional<PropertyValue<Color>> typedValue = convert<PropertyValue<Color>>(value, error, true, false);
        if (!typedValue) {
            return error;
        }

        setFillOutlineColor(*typedValue);
        return nullopt;
    }
    if (name == "fill-outline-color-transition") {
        Error error;
        optional<TransitionOptions> transition = convert<TransitionOptions>(value, error);
        if (!transition) {
            return error;
        }

        setFillOutlineColorTransition(*transition);
        return nullopt;
    }
    
    if (name == "fill-translate") {
        Error error;
        optional<PropertyValue<std::array<float, 2>>> typedValue = convert<PropertyValue<std::array<float, 2>>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }

        setFillTranslate(*typedValue);
        return nullopt;
    }
    if (name == "fill-translate-transition") {
        Error error;
        optional<TransitionOptions> transition = convert<TransitionOptions>(value, error);
        if (!transition) {
            return error;
        }

        setFillTranslateTransition(*transition);
        return nullopt;
    }
    
    if (name == "fill-translate-anchor") {
        Error error;
        optional<PropertyValue<TranslateAnchorType>> typedValue = convert<PropertyValue<TranslateAnchorType>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }

        setFillTranslateAnchor(*typedValue);
        return nullopt;
    }
    if (name == "fill-translate-anchor-transition") {
        Error error;
        optional<TransitionOptions> transition = convert<TransitionOptions>(value, error);
        if (!transition) {
            return error;
        }

        setFillTranslateAnchorTransition(*transition);
        return nullopt;
    }
    
    if (name == "fill-pattern") {
        Error error;
        optional<PropertyValue<std::string>> typedValue = convert<PropertyValue<std::string>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }

        setFillPattern(*typedValue);
        return nullopt;
    }
    if (name == "fill-pattern-transition") {
        Error error;
        optional<TransitionOptions> transition = convert<TransitionOptions>(value, error);
        if (!transition) {
            return error;
        }

        setFillPatternTransition(*transition);
        return nullopt;
    }
    
    return Error { "layer doesn't support this property" };
}

optional<Error> FillLayer::setLayoutProperty(const std::string& name, const Convertible& value) {
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
