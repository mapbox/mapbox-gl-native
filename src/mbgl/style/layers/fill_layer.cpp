// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/fill_layer.hpp>
#include <mbgl/style/layers/fill_layer_impl.hpp>
#include <mbgl/style/conversion/stringify.hpp>
#include <mbgl/style/layer_observer.hpp>

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
    impl_->paint = FillPaintProperties::Cascading();
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
    observer->onLayerFilterChanged(*this);
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
    observer->onLayerVisibilityChanged(*this);
}

// Zoom range

void FillLayer::setMinZoom(float minZoom) {
    auto impl_ = mutableImpl();
    impl_->minZoom = minZoom;
    baseImpl = std::move(impl_);
}

void FillLayer::setMaxZoom(float maxZoom) {
    auto impl_ = mutableImpl();
    impl_->maxZoom = maxZoom;
    baseImpl = std::move(impl_);
}

// Layout properties


// Paint properties

PropertyValue<bool> FillLayer::getDefaultFillAntialias() {
    return { true };
}

PropertyValue<bool> FillLayer::getFillAntialias(const optional<std::string>& klass) const {
    return impl().paint.template get<FillAntialias>().get(klass);
}

void FillLayer::setFillAntialias(PropertyValue<bool> value, const optional<std::string>& klass) {
    if (value == getFillAntialias(klass))
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillAntialias>().set(value, klass);
    baseImpl = std::move(impl_);
    observer->onLayerPaintPropertyChanged(*this);
}

void FillLayer::setFillAntialiasTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillAntialias>().setTransition(value, klass);
    baseImpl = std::move(impl_);
}

TransitionOptions FillLayer::getFillAntialiasTransition(const optional<std::string>& klass) const {
    return impl().paint.template get<FillAntialias>().getTransition(klass);
}

DataDrivenPropertyValue<float> FillLayer::getDefaultFillOpacity() {
    return { 1 };
}

DataDrivenPropertyValue<float> FillLayer::getFillOpacity(const optional<std::string>& klass) const {
    return impl().paint.template get<FillOpacity>().get(klass);
}

void FillLayer::setFillOpacity(DataDrivenPropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getFillOpacity(klass))
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillOpacity>().set(value, klass);
    baseImpl = std::move(impl_);
    if (value.isDataDriven()) {
        observer->onLayerDataDrivenPaintPropertyChanged(*this);
    } else {
        observer->onLayerPaintPropertyChanged(*this);
    }
}

void FillLayer::setFillOpacityTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillOpacity>().setTransition(value, klass);
    baseImpl = std::move(impl_);
}

TransitionOptions FillLayer::getFillOpacityTransition(const optional<std::string>& klass) const {
    return impl().paint.template get<FillOpacity>().getTransition(klass);
}

DataDrivenPropertyValue<Color> FillLayer::getDefaultFillColor() {
    return { Color::black() };
}

DataDrivenPropertyValue<Color> FillLayer::getFillColor(const optional<std::string>& klass) const {
    return impl().paint.template get<FillColor>().get(klass);
}

void FillLayer::setFillColor(DataDrivenPropertyValue<Color> value, const optional<std::string>& klass) {
    if (value == getFillColor(klass))
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillColor>().set(value, klass);
    baseImpl = std::move(impl_);
    if (value.isDataDriven()) {
        observer->onLayerDataDrivenPaintPropertyChanged(*this);
    } else {
        observer->onLayerPaintPropertyChanged(*this);
    }
}

void FillLayer::setFillColorTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillColor>().setTransition(value, klass);
    baseImpl = std::move(impl_);
}

TransitionOptions FillLayer::getFillColorTransition(const optional<std::string>& klass) const {
    return impl().paint.template get<FillColor>().getTransition(klass);
}

DataDrivenPropertyValue<Color> FillLayer::getDefaultFillOutlineColor() {
    return { {} };
}

DataDrivenPropertyValue<Color> FillLayer::getFillOutlineColor(const optional<std::string>& klass) const {
    return impl().paint.template get<FillOutlineColor>().get(klass);
}

void FillLayer::setFillOutlineColor(DataDrivenPropertyValue<Color> value, const optional<std::string>& klass) {
    if (value == getFillOutlineColor(klass))
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillOutlineColor>().set(value, klass);
    baseImpl = std::move(impl_);
    if (value.isDataDriven()) {
        observer->onLayerDataDrivenPaintPropertyChanged(*this);
    } else {
        observer->onLayerPaintPropertyChanged(*this);
    }
}

void FillLayer::setFillOutlineColorTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillOutlineColor>().setTransition(value, klass);
    baseImpl = std::move(impl_);
}

TransitionOptions FillLayer::getFillOutlineColorTransition(const optional<std::string>& klass) const {
    return impl().paint.template get<FillOutlineColor>().getTransition(klass);
}

PropertyValue<std::array<float, 2>> FillLayer::getDefaultFillTranslate() {
    return { {{ 0, 0 }} };
}

PropertyValue<std::array<float, 2>> FillLayer::getFillTranslate(const optional<std::string>& klass) const {
    return impl().paint.template get<FillTranslate>().get(klass);
}

void FillLayer::setFillTranslate(PropertyValue<std::array<float, 2>> value, const optional<std::string>& klass) {
    if (value == getFillTranslate(klass))
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillTranslate>().set(value, klass);
    baseImpl = std::move(impl_);
    observer->onLayerPaintPropertyChanged(*this);
}

void FillLayer::setFillTranslateTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillTranslate>().setTransition(value, klass);
    baseImpl = std::move(impl_);
}

TransitionOptions FillLayer::getFillTranslateTransition(const optional<std::string>& klass) const {
    return impl().paint.template get<FillTranslate>().getTransition(klass);
}

PropertyValue<TranslateAnchorType> FillLayer::getDefaultFillTranslateAnchor() {
    return { TranslateAnchorType::Map };
}

PropertyValue<TranslateAnchorType> FillLayer::getFillTranslateAnchor(const optional<std::string>& klass) const {
    return impl().paint.template get<FillTranslateAnchor>().get(klass);
}

void FillLayer::setFillTranslateAnchor(PropertyValue<TranslateAnchorType> value, const optional<std::string>& klass) {
    if (value == getFillTranslateAnchor(klass))
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillTranslateAnchor>().set(value, klass);
    baseImpl = std::move(impl_);
    observer->onLayerPaintPropertyChanged(*this);
}

void FillLayer::setFillTranslateAnchorTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillTranslateAnchor>().setTransition(value, klass);
    baseImpl = std::move(impl_);
}

TransitionOptions FillLayer::getFillTranslateAnchorTransition(const optional<std::string>& klass) const {
    return impl().paint.template get<FillTranslateAnchor>().getTransition(klass);
}

PropertyValue<std::string> FillLayer::getDefaultFillPattern() {
    return { "" };
}

PropertyValue<std::string> FillLayer::getFillPattern(const optional<std::string>& klass) const {
    return impl().paint.template get<FillPattern>().get(klass);
}

void FillLayer::setFillPattern(PropertyValue<std::string> value, const optional<std::string>& klass) {
    if (value == getFillPattern(klass))
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillPattern>().set(value, klass);
    baseImpl = std::move(impl_);
    observer->onLayerPaintPropertyChanged(*this);
}

void FillLayer::setFillPatternTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillPattern>().setTransition(value, klass);
    baseImpl = std::move(impl_);
}

TransitionOptions FillLayer::getFillPatternTransition(const optional<std::string>& klass) const {
    return impl().paint.template get<FillPattern>().getTransition(klass);
}

} // namespace style
} // namespace mbgl
