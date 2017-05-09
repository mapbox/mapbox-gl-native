// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/circle_layer.hpp>
#include <mbgl/style/layers/circle_layer_impl.hpp>
#include <mbgl/style/conversion/stringify.hpp>
#include <mbgl/style/layer_observer.hpp>

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
    impl_->paint = CirclePaintProperties::Cascading();
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
    observer->onLayerFilterChanged(*this);
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
    observer->onLayerVisibilityChanged(*this);
}

// Zoom range

void CircleLayer::setMinZoom(float minZoom) {
    auto impl_ = mutableImpl();
    impl_->minZoom = minZoom;
    baseImpl = std::move(impl_);
}

void CircleLayer::setMaxZoom(float maxZoom) {
    auto impl_ = mutableImpl();
    impl_->maxZoom = maxZoom;
    baseImpl = std::move(impl_);
}

// Layout properties


// Paint properties

DataDrivenPropertyValue<float> CircleLayer::getDefaultCircleRadius() {
    return { 5 };
}

DataDrivenPropertyValue<float> CircleLayer::getCircleRadius(const optional<std::string>& klass) const {
    return impl().paint.template get<CircleRadius>().get(klass);
}

void CircleLayer::setCircleRadius(DataDrivenPropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getCircleRadius(klass))
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<CircleRadius>().set(value, klass);
    baseImpl = std::move(impl_);
    if (value.isDataDriven()) {
        observer->onLayerDataDrivenPaintPropertyChanged(*this);
    } else {
        observer->onLayerPaintPropertyChanged(*this);
    }
}

void CircleLayer::setCircleRadiusTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<CircleRadius>().setTransition(value, klass);
    baseImpl = std::move(impl_);
}

TransitionOptions CircleLayer::getCircleRadiusTransition(const optional<std::string>& klass) const {
    return impl().paint.template get<CircleRadius>().getTransition(klass);
}

DataDrivenPropertyValue<Color> CircleLayer::getDefaultCircleColor() {
    return { Color::black() };
}

DataDrivenPropertyValue<Color> CircleLayer::getCircleColor(const optional<std::string>& klass) const {
    return impl().paint.template get<CircleColor>().get(klass);
}

void CircleLayer::setCircleColor(DataDrivenPropertyValue<Color> value, const optional<std::string>& klass) {
    if (value == getCircleColor(klass))
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<CircleColor>().set(value, klass);
    baseImpl = std::move(impl_);
    if (value.isDataDriven()) {
        observer->onLayerDataDrivenPaintPropertyChanged(*this);
    } else {
        observer->onLayerPaintPropertyChanged(*this);
    }
}

void CircleLayer::setCircleColorTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<CircleColor>().setTransition(value, klass);
    baseImpl = std::move(impl_);
}

TransitionOptions CircleLayer::getCircleColorTransition(const optional<std::string>& klass) const {
    return impl().paint.template get<CircleColor>().getTransition(klass);
}

DataDrivenPropertyValue<float> CircleLayer::getDefaultCircleBlur() {
    return { 0 };
}

DataDrivenPropertyValue<float> CircleLayer::getCircleBlur(const optional<std::string>& klass) const {
    return impl().paint.template get<CircleBlur>().get(klass);
}

void CircleLayer::setCircleBlur(DataDrivenPropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getCircleBlur(klass))
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<CircleBlur>().set(value, klass);
    baseImpl = std::move(impl_);
    if (value.isDataDriven()) {
        observer->onLayerDataDrivenPaintPropertyChanged(*this);
    } else {
        observer->onLayerPaintPropertyChanged(*this);
    }
}

void CircleLayer::setCircleBlurTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<CircleBlur>().setTransition(value, klass);
    baseImpl = std::move(impl_);
}

TransitionOptions CircleLayer::getCircleBlurTransition(const optional<std::string>& klass) const {
    return impl().paint.template get<CircleBlur>().getTransition(klass);
}

DataDrivenPropertyValue<float> CircleLayer::getDefaultCircleOpacity() {
    return { 1 };
}

DataDrivenPropertyValue<float> CircleLayer::getCircleOpacity(const optional<std::string>& klass) const {
    return impl().paint.template get<CircleOpacity>().get(klass);
}

void CircleLayer::setCircleOpacity(DataDrivenPropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getCircleOpacity(klass))
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<CircleOpacity>().set(value, klass);
    baseImpl = std::move(impl_);
    if (value.isDataDriven()) {
        observer->onLayerDataDrivenPaintPropertyChanged(*this);
    } else {
        observer->onLayerPaintPropertyChanged(*this);
    }
}

void CircleLayer::setCircleOpacityTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<CircleOpacity>().setTransition(value, klass);
    baseImpl = std::move(impl_);
}

TransitionOptions CircleLayer::getCircleOpacityTransition(const optional<std::string>& klass) const {
    return impl().paint.template get<CircleOpacity>().getTransition(klass);
}

PropertyValue<std::array<float, 2>> CircleLayer::getDefaultCircleTranslate() {
    return { {{ 0, 0 }} };
}

PropertyValue<std::array<float, 2>> CircleLayer::getCircleTranslate(const optional<std::string>& klass) const {
    return impl().paint.template get<CircleTranslate>().get(klass);
}

void CircleLayer::setCircleTranslate(PropertyValue<std::array<float, 2>> value, const optional<std::string>& klass) {
    if (value == getCircleTranslate(klass))
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<CircleTranslate>().set(value, klass);
    baseImpl = std::move(impl_);
    observer->onLayerPaintPropertyChanged(*this);
}

void CircleLayer::setCircleTranslateTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<CircleTranslate>().setTransition(value, klass);
    baseImpl = std::move(impl_);
}

TransitionOptions CircleLayer::getCircleTranslateTransition(const optional<std::string>& klass) const {
    return impl().paint.template get<CircleTranslate>().getTransition(klass);
}

PropertyValue<TranslateAnchorType> CircleLayer::getDefaultCircleTranslateAnchor() {
    return { TranslateAnchorType::Map };
}

PropertyValue<TranslateAnchorType> CircleLayer::getCircleTranslateAnchor(const optional<std::string>& klass) const {
    return impl().paint.template get<CircleTranslateAnchor>().get(klass);
}

void CircleLayer::setCircleTranslateAnchor(PropertyValue<TranslateAnchorType> value, const optional<std::string>& klass) {
    if (value == getCircleTranslateAnchor(klass))
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<CircleTranslateAnchor>().set(value, klass);
    baseImpl = std::move(impl_);
    observer->onLayerPaintPropertyChanged(*this);
}

void CircleLayer::setCircleTranslateAnchorTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<CircleTranslateAnchor>().setTransition(value, klass);
    baseImpl = std::move(impl_);
}

TransitionOptions CircleLayer::getCircleTranslateAnchorTransition(const optional<std::string>& klass) const {
    return impl().paint.template get<CircleTranslateAnchor>().getTransition(klass);
}

PropertyValue<CirclePitchScaleType> CircleLayer::getDefaultCirclePitchScale() {
    return { CirclePitchScaleType::Map };
}

PropertyValue<CirclePitchScaleType> CircleLayer::getCirclePitchScale(const optional<std::string>& klass) const {
    return impl().paint.template get<CirclePitchScale>().get(klass);
}

void CircleLayer::setCirclePitchScale(PropertyValue<CirclePitchScaleType> value, const optional<std::string>& klass) {
    if (value == getCirclePitchScale(klass))
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<CirclePitchScale>().set(value, klass);
    baseImpl = std::move(impl_);
    observer->onLayerPaintPropertyChanged(*this);
}

void CircleLayer::setCirclePitchScaleTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<CirclePitchScale>().setTransition(value, klass);
    baseImpl = std::move(impl_);
}

TransitionOptions CircleLayer::getCirclePitchScaleTransition(const optional<std::string>& klass) const {
    return impl().paint.template get<CirclePitchScale>().getTransition(klass);
}

DataDrivenPropertyValue<float> CircleLayer::getDefaultCircleStrokeWidth() {
    return { 0 };
}

DataDrivenPropertyValue<float> CircleLayer::getCircleStrokeWidth(const optional<std::string>& klass) const {
    return impl().paint.template get<CircleStrokeWidth>().get(klass);
}

void CircleLayer::setCircleStrokeWidth(DataDrivenPropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getCircleStrokeWidth(klass))
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<CircleStrokeWidth>().set(value, klass);
    baseImpl = std::move(impl_);
    if (value.isDataDriven()) {
        observer->onLayerDataDrivenPaintPropertyChanged(*this);
    } else {
        observer->onLayerPaintPropertyChanged(*this);
    }
}

void CircleLayer::setCircleStrokeWidthTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<CircleStrokeWidth>().setTransition(value, klass);
    baseImpl = std::move(impl_);
}

TransitionOptions CircleLayer::getCircleStrokeWidthTransition(const optional<std::string>& klass) const {
    return impl().paint.template get<CircleStrokeWidth>().getTransition(klass);
}

DataDrivenPropertyValue<Color> CircleLayer::getDefaultCircleStrokeColor() {
    return { Color::black() };
}

DataDrivenPropertyValue<Color> CircleLayer::getCircleStrokeColor(const optional<std::string>& klass) const {
    return impl().paint.template get<CircleStrokeColor>().get(klass);
}

void CircleLayer::setCircleStrokeColor(DataDrivenPropertyValue<Color> value, const optional<std::string>& klass) {
    if (value == getCircleStrokeColor(klass))
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<CircleStrokeColor>().set(value, klass);
    baseImpl = std::move(impl_);
    if (value.isDataDriven()) {
        observer->onLayerDataDrivenPaintPropertyChanged(*this);
    } else {
        observer->onLayerPaintPropertyChanged(*this);
    }
}

void CircleLayer::setCircleStrokeColorTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<CircleStrokeColor>().setTransition(value, klass);
    baseImpl = std::move(impl_);
}

TransitionOptions CircleLayer::getCircleStrokeColorTransition(const optional<std::string>& klass) const {
    return impl().paint.template get<CircleStrokeColor>().getTransition(klass);
}

DataDrivenPropertyValue<float> CircleLayer::getDefaultCircleStrokeOpacity() {
    return { 1 };
}

DataDrivenPropertyValue<float> CircleLayer::getCircleStrokeOpacity(const optional<std::string>& klass) const {
    return impl().paint.template get<CircleStrokeOpacity>().get(klass);
}

void CircleLayer::setCircleStrokeOpacity(DataDrivenPropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getCircleStrokeOpacity(klass))
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<CircleStrokeOpacity>().set(value, klass);
    baseImpl = std::move(impl_);
    if (value.isDataDriven()) {
        observer->onLayerDataDrivenPaintPropertyChanged(*this);
    } else {
        observer->onLayerPaintPropertyChanged(*this);
    }
}

void CircleLayer::setCircleStrokeOpacityTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<CircleStrokeOpacity>().setTransition(value, klass);
    baseImpl = std::move(impl_);
}

TransitionOptions CircleLayer::getCircleStrokeOpacityTransition(const optional<std::string>& klass) const {
    return impl().paint.template get<CircleStrokeOpacity>().getTransition(klass);
}

} // namespace style
} // namespace mbgl
