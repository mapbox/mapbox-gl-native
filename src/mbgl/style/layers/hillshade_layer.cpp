// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/hillshade_layer.hpp>
#include <mbgl/style/layers/hillshade_layer_impl.hpp>
#include <mbgl/style/layer_observer.hpp>

namespace mbgl {
namespace style {

HillshadeLayer::HillshadeLayer(const std::string& layerID, const std::string& sourceID)
    : Layer(makeMutable<Impl>(LayerType::Hillshade, layerID, sourceID)) {
}

HillshadeLayer::HillshadeLayer(Immutable<Impl> impl_)
    : Layer(std::move(impl_)) {
}

HillshadeLayer::~HillshadeLayer() = default;

const HillshadeLayer::Impl& HillshadeLayer::impl() const {
    return static_cast<const Impl&>(*baseImpl);
}

Mutable<HillshadeLayer::Impl> HillshadeLayer::mutableImpl() const {
    return makeMutable<Impl>(impl());
}

std::unique_ptr<Layer> HillshadeLayer::cloneRef(const std::string& id_) const {
    auto impl_ = mutableImpl();
    impl_->id = id_;
    impl_->paint = HillshadePaintProperties::Transitionable();
    return std::make_unique<HillshadeLayer>(std::move(impl_));
}

void HillshadeLayer::Impl::stringifyLayout(rapidjson::Writer<rapidjson::StringBuffer>&) const {
}

// Source

const std::string& HillshadeLayer::getSourceID() const {
    return impl().source;
}


// Visibility

void HillshadeLayer::setVisibility(VisibilityType value) {
    if (value == getVisibility())
        return;
    auto impl_ = mutableImpl();
    impl_->visibility = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

// Zoom range

void HillshadeLayer::setMinZoom(float minZoom) {
    auto impl_ = mutableImpl();
    impl_->minZoom = minZoom;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void HillshadeLayer::setMaxZoom(float maxZoom) {
    auto impl_ = mutableImpl();
    impl_->maxZoom = maxZoom;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

// Layout properties


// Paint properties

PropertyValue<float> HillshadeLayer::getDefaultHillshadeIlluminationDirection() {
    return { 335 };
}

PropertyValue<float> HillshadeLayer::getHillshadeIlluminationDirection() const {
    return impl().paint.template get<HillshadeIlluminationDirection>().value;
}

void HillshadeLayer::setHillshadeIlluminationDirection(PropertyValue<float> value) {
    if (value == getHillshadeIlluminationDirection())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<HillshadeIlluminationDirection>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void HillshadeLayer::setHillshadeIlluminationDirectionTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<HillshadeIlluminationDirection>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions HillshadeLayer::getHillshadeIlluminationDirectionTransition() const {
    return impl().paint.template get<HillshadeIlluminationDirection>().options;
}

PropertyValue<HillshadeIlluminationAnchorType> HillshadeLayer::getDefaultHillshadeIlluminationAnchor() {
    return { HillshadeIlluminationAnchorType::Viewport };
}

PropertyValue<HillshadeIlluminationAnchorType> HillshadeLayer::getHillshadeIlluminationAnchor() const {
    return impl().paint.template get<HillshadeIlluminationAnchor>().value;
}

void HillshadeLayer::setHillshadeIlluminationAnchor(PropertyValue<HillshadeIlluminationAnchorType> value) {
    if (value == getHillshadeIlluminationAnchor())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<HillshadeIlluminationAnchor>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void HillshadeLayer::setHillshadeIlluminationAnchorTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<HillshadeIlluminationAnchor>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions HillshadeLayer::getHillshadeIlluminationAnchorTransition() const {
    return impl().paint.template get<HillshadeIlluminationAnchor>().options;
}

PropertyValue<float> HillshadeLayer::getDefaultHillshadeExaggeration() {
    return { 0.5 };
}

PropertyValue<float> HillshadeLayer::getHillshadeExaggeration() const {
    return impl().paint.template get<HillshadeExaggeration>().value;
}

void HillshadeLayer::setHillshadeExaggeration(PropertyValue<float> value) {
    if (value == getHillshadeExaggeration())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<HillshadeExaggeration>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void HillshadeLayer::setHillshadeExaggerationTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<HillshadeExaggeration>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions HillshadeLayer::getHillshadeExaggerationTransition() const {
    return impl().paint.template get<HillshadeExaggeration>().options;
}

PropertyValue<Color> HillshadeLayer::getDefaultHillshadeShadowColor() {
    return { Color::black() };
}

PropertyValue<Color> HillshadeLayer::getHillshadeShadowColor() const {
    return impl().paint.template get<HillshadeShadowColor>().value;
}

void HillshadeLayer::setHillshadeShadowColor(PropertyValue<Color> value) {
    if (value == getHillshadeShadowColor())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<HillshadeShadowColor>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void HillshadeLayer::setHillshadeShadowColorTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<HillshadeShadowColor>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions HillshadeLayer::getHillshadeShadowColorTransition() const {
    return impl().paint.template get<HillshadeShadowColor>().options;
}

PropertyValue<Color> HillshadeLayer::getDefaultHillshadeHighlightColor() {
    return { Color::white() };
}

PropertyValue<Color> HillshadeLayer::getHillshadeHighlightColor() const {
    return impl().paint.template get<HillshadeHighlightColor>().value;
}

void HillshadeLayer::setHillshadeHighlightColor(PropertyValue<Color> value) {
    if (value == getHillshadeHighlightColor())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<HillshadeHighlightColor>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void HillshadeLayer::setHillshadeHighlightColorTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<HillshadeHighlightColor>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions HillshadeLayer::getHillshadeHighlightColorTransition() const {
    return impl().paint.template get<HillshadeHighlightColor>().options;
}

PropertyValue<Color> HillshadeLayer::getDefaultHillshadeAccentColor() {
    return { Color::black() };
}

PropertyValue<Color> HillshadeLayer::getHillshadeAccentColor() const {
    return impl().paint.template get<HillshadeAccentColor>().value;
}

void HillshadeLayer::setHillshadeAccentColor(PropertyValue<Color> value) {
    if (value == getHillshadeAccentColor())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<HillshadeAccentColor>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void HillshadeLayer::setHillshadeAccentColorTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<HillshadeAccentColor>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions HillshadeLayer::getHillshadeAccentColorTransition() const {
    return impl().paint.template get<HillshadeAccentColor>().options;
}

} // namespace style
} // namespace mbgl
