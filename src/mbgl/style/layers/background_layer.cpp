// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/background_layer.hpp>
#include <mbgl/style/layers/background_layer_impl.hpp>
#include <mbgl/style/layer_observer.hpp>

namespace mbgl {
namespace style {

BackgroundLayer::BackgroundLayer(const std::string& layerID)
    : Layer(makeMutable<Impl>(LayerType::Background, layerID, std::string())) {
}

BackgroundLayer::BackgroundLayer(Immutable<Impl> impl_)
    : Layer(std::move(impl_)) {
}

BackgroundLayer::~BackgroundLayer() = default;

const BackgroundLayer::Impl& BackgroundLayer::impl() const {
    return static_cast<const Impl&>(*baseImpl);
}

Mutable<BackgroundLayer::Impl> BackgroundLayer::mutableImpl() const {
    return makeMutable<Impl>(impl());
}

std::unique_ptr<Layer> BackgroundLayer::cloneRef(const std::string& id_) const {
    auto impl_ = mutableImpl();
    impl_->id = id_;
    impl_->paint = BackgroundPaintProperties::Transitionable();
    return std::make_unique<BackgroundLayer>(std::move(impl_));
}

void BackgroundLayer::Impl::stringifyLayout(rapidjson::Writer<rapidjson::StringBuffer>&) const {
}


// Visibility

void BackgroundLayer::setVisibility(VisibilityType value) {
    if (value == getVisibility())
        return;
    auto impl_ = mutableImpl();
    impl_->visibility = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

// Zoom range

void BackgroundLayer::setMinZoom(float minZoom) {
    auto impl_ = mutableImpl();
    impl_->minZoom = minZoom;
    baseImpl = std::move(impl_);
}

void BackgroundLayer::setMaxZoom(float maxZoom) {
    auto impl_ = mutableImpl();
    impl_->maxZoom = maxZoom;
    baseImpl = std::move(impl_);
}

// Layout properties


// Paint properties

PropertyValue<Color> BackgroundLayer::getDefaultBackgroundColor() {
    return { Color::black() };
}

PropertyValue<Color> BackgroundLayer::getBackgroundColor() const {
    return impl().paint.template get<BackgroundColor>().value;
}

void BackgroundLayer::setBackgroundColor(PropertyValue<Color> value) {
    if (value == getBackgroundColor())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<BackgroundColor>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void BackgroundLayer::setBackgroundColorTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<BackgroundColor>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions BackgroundLayer::getBackgroundColorTransition() const {
    return impl().paint.template get<BackgroundColor>().options;
}

PropertyValue<std::string> BackgroundLayer::getDefaultBackgroundPattern() {
    return { "" };
}

PropertyValue<std::string> BackgroundLayer::getBackgroundPattern() const {
    return impl().paint.template get<BackgroundPattern>().value;
}

void BackgroundLayer::setBackgroundPattern(PropertyValue<std::string> value) {
    if (value == getBackgroundPattern())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<BackgroundPattern>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void BackgroundLayer::setBackgroundPatternTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<BackgroundPattern>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions BackgroundLayer::getBackgroundPatternTransition() const {
    return impl().paint.template get<BackgroundPattern>().options;
}

PropertyValue<float> BackgroundLayer::getDefaultBackgroundOpacity() {
    return { 1 };
}

PropertyValue<float> BackgroundLayer::getBackgroundOpacity() const {
    return impl().paint.template get<BackgroundOpacity>().value;
}

void BackgroundLayer::setBackgroundOpacity(PropertyValue<float> value) {
    if (value == getBackgroundOpacity())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<BackgroundOpacity>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void BackgroundLayer::setBackgroundOpacityTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<BackgroundOpacity>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions BackgroundLayer::getBackgroundOpacityTransition() const {
    return impl().paint.template get<BackgroundOpacity>().options;
}

} // namespace style
} // namespace mbgl
