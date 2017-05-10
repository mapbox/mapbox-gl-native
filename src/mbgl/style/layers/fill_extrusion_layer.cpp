// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/fill_extrusion_layer.hpp>
#include <mbgl/style/layers/fill_extrusion_layer_impl.hpp>
#include <mbgl/style/layer_observer.hpp>

namespace mbgl {
namespace style {

FillExtrusionLayer::FillExtrusionLayer(const std::string& layerID, const std::string& sourceID)
    : Layer(makeMutable<Impl>(LayerType::FillExtrusion, layerID, sourceID)) {
}

FillExtrusionLayer::FillExtrusionLayer(Immutable<Impl> impl_)
    : Layer(std::move(impl_)) {
}

FillExtrusionLayer::~FillExtrusionLayer() = default;

const FillExtrusionLayer::Impl& FillExtrusionLayer::impl() const {
    return static_cast<const Impl&>(*baseImpl);
}

Mutable<FillExtrusionLayer::Impl> FillExtrusionLayer::mutableImpl() const {
    return makeMutable<Impl>(impl());
}

std::unique_ptr<Layer> FillExtrusionLayer::cloneRef(const std::string& id_) const {
    auto impl_ = mutableImpl();
    impl_->id = id_;
    impl_->paint = FillExtrusionPaintProperties::Transitionable();
    return std::make_unique<FillExtrusionLayer>(std::move(impl_));
}

void FillExtrusionLayer::Impl::stringifyLayout(rapidjson::Writer<rapidjson::StringBuffer>&) const {
}

// Source

const std::string& FillExtrusionLayer::getSourceID() const {
    return impl().source;
}

void FillExtrusionLayer::setSourceLayer(const std::string& sourceLayer) {
    auto impl_ = mutableImpl();
    impl_->sourceLayer = sourceLayer;
    baseImpl = std::move(impl_);
}

const std::string& FillExtrusionLayer::getSourceLayer() const {
    return impl().sourceLayer;
}

// Filter

void FillExtrusionLayer::setFilter(const Filter& filter) {
    auto impl_ = mutableImpl();
    impl_->filter = filter;
    baseImpl = std::move(impl_);
    observer->onLayerFilterChanged(*this);
}

const Filter& FillExtrusionLayer::getFilter() const {
    return impl().filter;
}

// Visibility

void FillExtrusionLayer::setVisibility(VisibilityType value) {
    if (value == getVisibility())
        return;
    auto impl_ = mutableImpl();
    impl_->visibility = value;
    baseImpl = std::move(impl_);
    observer->onLayerVisibilityChanged(*this);
}

// Zoom range

void FillExtrusionLayer::setMinZoom(float minZoom) {
    auto impl_ = mutableImpl();
    impl_->minZoom = minZoom;
    baseImpl = std::move(impl_);
}

void FillExtrusionLayer::setMaxZoom(float maxZoom) {
    auto impl_ = mutableImpl();
    impl_->maxZoom = maxZoom;
    baseImpl = std::move(impl_);
}

// Layout properties


// Paint properties

PropertyValue<float> FillExtrusionLayer::getDefaultFillExtrusionOpacity() {
    return { 1 };
}

PropertyValue<float> FillExtrusionLayer::getFillExtrusionOpacity(const optional<std::string>& klass) const {
    return impl().paint.template get<FillExtrusionOpacity>().get(klass);
}

void FillExtrusionLayer::setFillExtrusionOpacity(PropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getFillExtrusionOpacity(klass))
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillExtrusionOpacity>().set(value, klass);
    baseImpl = std::move(impl_);
    observer->onLayerPaintPropertyChanged(*this);
}

void FillExtrusionLayer::setFillExtrusionOpacityTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillExtrusionOpacity>().setTransition(value, klass);
    baseImpl = std::move(impl_);
}

TransitionOptions FillExtrusionLayer::getFillExtrusionOpacityTransition(const optional<std::string>& klass) const {
    return impl().paint.template get<FillExtrusionOpacity>().getTransition(klass);
}

DataDrivenPropertyValue<Color> FillExtrusionLayer::getDefaultFillExtrusionColor() {
    return { Color::black() };
}

DataDrivenPropertyValue<Color> FillExtrusionLayer::getFillExtrusionColor(const optional<std::string>& klass) const {
    return impl().paint.template get<FillExtrusionColor>().get(klass);
}

void FillExtrusionLayer::setFillExtrusionColor(DataDrivenPropertyValue<Color> value, const optional<std::string>& klass) {
    if (value == getFillExtrusionColor(klass))
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillExtrusionColor>().set(value, klass);
    baseImpl = std::move(impl_);
    if (value.isDataDriven()) {
        observer->onLayerDataDrivenPaintPropertyChanged(*this);
    } else {
        observer->onLayerPaintPropertyChanged(*this);
    }
}

void FillExtrusionLayer::setFillExtrusionColorTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillExtrusionColor>().setTransition(value, klass);
    baseImpl = std::move(impl_);
}

TransitionOptions FillExtrusionLayer::getFillExtrusionColorTransition(const optional<std::string>& klass) const {
    return impl().paint.template get<FillExtrusionColor>().getTransition(klass);
}

PropertyValue<std::array<float, 2>> FillExtrusionLayer::getDefaultFillExtrusionTranslate() {
    return { {{ 0, 0 }} };
}

PropertyValue<std::array<float, 2>> FillExtrusionLayer::getFillExtrusionTranslate(const optional<std::string>& klass) const {
    return impl().paint.template get<FillExtrusionTranslate>().get(klass);
}

void FillExtrusionLayer::setFillExtrusionTranslate(PropertyValue<std::array<float, 2>> value, const optional<std::string>& klass) {
    if (value == getFillExtrusionTranslate(klass))
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillExtrusionTranslate>().set(value, klass);
    baseImpl = std::move(impl_);
    observer->onLayerPaintPropertyChanged(*this);
}

void FillExtrusionLayer::setFillExtrusionTranslateTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillExtrusionTranslate>().setTransition(value, klass);
    baseImpl = std::move(impl_);
}

TransitionOptions FillExtrusionLayer::getFillExtrusionTranslateTransition(const optional<std::string>& klass) const {
    return impl().paint.template get<FillExtrusionTranslate>().getTransition(klass);
}

PropertyValue<TranslateAnchorType> FillExtrusionLayer::getDefaultFillExtrusionTranslateAnchor() {
    return { TranslateAnchorType::Map };
}

PropertyValue<TranslateAnchorType> FillExtrusionLayer::getFillExtrusionTranslateAnchor(const optional<std::string>& klass) const {
    return impl().paint.template get<FillExtrusionTranslateAnchor>().get(klass);
}

void FillExtrusionLayer::setFillExtrusionTranslateAnchor(PropertyValue<TranslateAnchorType> value, const optional<std::string>& klass) {
    if (value == getFillExtrusionTranslateAnchor(klass))
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillExtrusionTranslateAnchor>().set(value, klass);
    baseImpl = std::move(impl_);
    observer->onLayerPaintPropertyChanged(*this);
}

void FillExtrusionLayer::setFillExtrusionTranslateAnchorTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillExtrusionTranslateAnchor>().setTransition(value, klass);
    baseImpl = std::move(impl_);
}

TransitionOptions FillExtrusionLayer::getFillExtrusionTranslateAnchorTransition(const optional<std::string>& klass) const {
    return impl().paint.template get<FillExtrusionTranslateAnchor>().getTransition(klass);
}

PropertyValue<std::string> FillExtrusionLayer::getDefaultFillExtrusionPattern() {
    return { "" };
}

PropertyValue<std::string> FillExtrusionLayer::getFillExtrusionPattern(const optional<std::string>& klass) const {
    return impl().paint.template get<FillExtrusionPattern>().get(klass);
}

void FillExtrusionLayer::setFillExtrusionPattern(PropertyValue<std::string> value, const optional<std::string>& klass) {
    if (value == getFillExtrusionPattern(klass))
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillExtrusionPattern>().set(value, klass);
    baseImpl = std::move(impl_);
    observer->onLayerPaintPropertyChanged(*this);
}

void FillExtrusionLayer::setFillExtrusionPatternTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillExtrusionPattern>().setTransition(value, klass);
    baseImpl = std::move(impl_);
}

TransitionOptions FillExtrusionLayer::getFillExtrusionPatternTransition(const optional<std::string>& klass) const {
    return impl().paint.template get<FillExtrusionPattern>().getTransition(klass);
}

DataDrivenPropertyValue<float> FillExtrusionLayer::getDefaultFillExtrusionHeight() {
    return { 0 };
}

DataDrivenPropertyValue<float> FillExtrusionLayer::getFillExtrusionHeight(const optional<std::string>& klass) const {
    return impl().paint.template get<FillExtrusionHeight>().get(klass);
}

void FillExtrusionLayer::setFillExtrusionHeight(DataDrivenPropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getFillExtrusionHeight(klass))
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillExtrusionHeight>().set(value, klass);
    baseImpl = std::move(impl_);
    if (value.isDataDriven()) {
        observer->onLayerDataDrivenPaintPropertyChanged(*this);
    } else {
        observer->onLayerPaintPropertyChanged(*this);
    }
}

void FillExtrusionLayer::setFillExtrusionHeightTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillExtrusionHeight>().setTransition(value, klass);
    baseImpl = std::move(impl_);
}

TransitionOptions FillExtrusionLayer::getFillExtrusionHeightTransition(const optional<std::string>& klass) const {
    return impl().paint.template get<FillExtrusionHeight>().getTransition(klass);
}

DataDrivenPropertyValue<float> FillExtrusionLayer::getDefaultFillExtrusionBase() {
    return { 0 };
}

DataDrivenPropertyValue<float> FillExtrusionLayer::getFillExtrusionBase(const optional<std::string>& klass) const {
    return impl().paint.template get<FillExtrusionBase>().get(klass);
}

void FillExtrusionLayer::setFillExtrusionBase(DataDrivenPropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getFillExtrusionBase(klass))
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillExtrusionBase>().set(value, klass);
    baseImpl = std::move(impl_);
    if (value.isDataDriven()) {
        observer->onLayerDataDrivenPaintPropertyChanged(*this);
    } else {
        observer->onLayerPaintPropertyChanged(*this);
    }
}

void FillExtrusionLayer::setFillExtrusionBaseTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillExtrusionBase>().setTransition(value, klass);
    baseImpl = std::move(impl_);
}

TransitionOptions FillExtrusionLayer::getFillExtrusionBaseTransition(const optional<std::string>& klass) const {
    return impl().paint.template get<FillExtrusionBase>().getTransition(klass);
}

} // namespace style
} // namespace mbgl
