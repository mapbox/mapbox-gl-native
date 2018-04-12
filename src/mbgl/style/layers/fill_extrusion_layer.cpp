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
    observer->onLayerChanged(*this);
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
    observer->onLayerChanged(*this);
}

// Zoom range

void FillExtrusionLayer::setMinZoom(float minZoom) {
    auto impl_ = mutableImpl();
    impl_->minZoom = minZoom;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void FillExtrusionLayer::setMaxZoom(float maxZoom) {
    auto impl_ = mutableImpl();
    impl_->maxZoom = maxZoom;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

// Layout properties


// Paint properties

PropertyValue<float> FillExtrusionLayer::getDefaultFillExtrusionOpacity() {
    return { 1 };
}

PropertyValue<float> FillExtrusionLayer::getFillExtrusionOpacity() const {
    return impl().paint.template get<FillExtrusionOpacity>().value;
}

void FillExtrusionLayer::setFillExtrusionOpacity(PropertyValue<float> value) {
    if (value == getFillExtrusionOpacity())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillExtrusionOpacity>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void FillExtrusionLayer::setFillExtrusionOpacityTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillExtrusionOpacity>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions FillExtrusionLayer::getFillExtrusionOpacityTransition() const {
    return impl().paint.template get<FillExtrusionOpacity>().options;
}

DataDrivenPropertyValue<Color> FillExtrusionLayer::getDefaultFillExtrusionColor() {
    return { Color::black() };
}

DataDrivenPropertyValue<Color> FillExtrusionLayer::getFillExtrusionColor() const {
    return impl().paint.template get<FillExtrusionColor>().value;
}

void FillExtrusionLayer::setFillExtrusionColor(DataDrivenPropertyValue<Color> value) {
    if (value == getFillExtrusionColor())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillExtrusionColor>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void FillExtrusionLayer::setFillExtrusionColorTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillExtrusionColor>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions FillExtrusionLayer::getFillExtrusionColorTransition() const {
    return impl().paint.template get<FillExtrusionColor>().options;
}

PropertyValue<std::array<float, 2>> FillExtrusionLayer::getDefaultFillExtrusionTranslate() {
    return { {{ 0, 0 }} };
}

PropertyValue<std::array<float, 2>> FillExtrusionLayer::getFillExtrusionTranslate() const {
    return impl().paint.template get<FillExtrusionTranslate>().value;
}

void FillExtrusionLayer::setFillExtrusionTranslate(PropertyValue<std::array<float, 2>> value) {
    if (value == getFillExtrusionTranslate())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillExtrusionTranslate>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void FillExtrusionLayer::setFillExtrusionTranslateTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillExtrusionTranslate>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions FillExtrusionLayer::getFillExtrusionTranslateTransition() const {
    return impl().paint.template get<FillExtrusionTranslate>().options;
}

PropertyValue<TranslateAnchorType> FillExtrusionLayer::getDefaultFillExtrusionTranslateAnchor() {
    return { TranslateAnchorType::Map };
}

PropertyValue<TranslateAnchorType> FillExtrusionLayer::getFillExtrusionTranslateAnchor() const {
    return impl().paint.template get<FillExtrusionTranslateAnchor>().value;
}

void FillExtrusionLayer::setFillExtrusionTranslateAnchor(PropertyValue<TranslateAnchorType> value) {
    if (value == getFillExtrusionTranslateAnchor())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillExtrusionTranslateAnchor>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void FillExtrusionLayer::setFillExtrusionTranslateAnchorTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillExtrusionTranslateAnchor>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions FillExtrusionLayer::getFillExtrusionTranslateAnchorTransition() const {
    return impl().paint.template get<FillExtrusionTranslateAnchor>().options;
}

PropertyValue<std::string> FillExtrusionLayer::getDefaultFillExtrusionPattern() {
    return { "" };
}

PropertyValue<std::string> FillExtrusionLayer::getFillExtrusionPattern() const {
    return impl().paint.template get<FillExtrusionPattern>().value;
}

void FillExtrusionLayer::setFillExtrusionPattern(PropertyValue<std::string> value) {
    if (value == getFillExtrusionPattern())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillExtrusionPattern>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void FillExtrusionLayer::setFillExtrusionPatternTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillExtrusionPattern>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions FillExtrusionLayer::getFillExtrusionPatternTransition() const {
    return impl().paint.template get<FillExtrusionPattern>().options;
}

DataDrivenPropertyValue<float> FillExtrusionLayer::getDefaultFillExtrusionHeight() {
    return { 0 };
}

DataDrivenPropertyValue<float> FillExtrusionLayer::getFillExtrusionHeight() const {
    return impl().paint.template get<FillExtrusionHeight>().value;
}

void FillExtrusionLayer::setFillExtrusionHeight(DataDrivenPropertyValue<float> value) {
    if (value == getFillExtrusionHeight())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillExtrusionHeight>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void FillExtrusionLayer::setFillExtrusionHeightTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillExtrusionHeight>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions FillExtrusionLayer::getFillExtrusionHeightTransition() const {
    return impl().paint.template get<FillExtrusionHeight>().options;
}

DataDrivenPropertyValue<float> FillExtrusionLayer::getDefaultFillExtrusionBase() {
    return { 0 };
}

DataDrivenPropertyValue<float> FillExtrusionLayer::getFillExtrusionBase() const {
    return impl().paint.template get<FillExtrusionBase>().value;
}

void FillExtrusionLayer::setFillExtrusionBase(DataDrivenPropertyValue<float> value) {
    if (value == getFillExtrusionBase())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillExtrusionBase>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void FillExtrusionLayer::setFillExtrusionBaseTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillExtrusionBase>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions FillExtrusionLayer::getFillExtrusionBaseTransition() const {
    return impl().paint.template get<FillExtrusionBase>().options;
}

} // namespace style
} // namespace mbgl
