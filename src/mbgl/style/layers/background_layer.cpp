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
    observer->onLayerVisibilityChanged(*this);
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

PropertyValue<Color> BackgroundLayer::getBackgroundColor(const optional<std::string>& klass) const {
    return impl().paint.template get<BackgroundColor>().get(klass);
}

void BackgroundLayer::setBackgroundColor(PropertyValue<Color> value, const optional<std::string>& klass) {
    if (value == getBackgroundColor(klass))
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<BackgroundColor>().set(value, klass);
    baseImpl = std::move(impl_);
    observer->onLayerPaintPropertyChanged(*this);
}

void BackgroundLayer::setBackgroundColorTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<BackgroundColor>().setTransition(value, klass);
    baseImpl = std::move(impl_);
}

TransitionOptions BackgroundLayer::getBackgroundColorTransition(const optional<std::string>& klass) const {
    return impl().paint.template get<BackgroundColor>().getTransition(klass);
}

PropertyValue<std::string> BackgroundLayer::getDefaultBackgroundPattern() {
    return { "" };
}

PropertyValue<std::string> BackgroundLayer::getBackgroundPattern(const optional<std::string>& klass) const {
    return impl().paint.template get<BackgroundPattern>().get(klass);
}

void BackgroundLayer::setBackgroundPattern(PropertyValue<std::string> value, const optional<std::string>& klass) {
    if (value == getBackgroundPattern(klass))
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<BackgroundPattern>().set(value, klass);
    baseImpl = std::move(impl_);
    observer->onLayerPaintPropertyChanged(*this);
}

void BackgroundLayer::setBackgroundPatternTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<BackgroundPattern>().setTransition(value, klass);
    baseImpl = std::move(impl_);
}

TransitionOptions BackgroundLayer::getBackgroundPatternTransition(const optional<std::string>& klass) const {
    return impl().paint.template get<BackgroundPattern>().getTransition(klass);
}

PropertyValue<float> BackgroundLayer::getDefaultBackgroundOpacity() {
    return { 1 };
}

PropertyValue<float> BackgroundLayer::getBackgroundOpacity(const optional<std::string>& klass) const {
    return impl().paint.template get<BackgroundOpacity>().get(klass);
}

void BackgroundLayer::setBackgroundOpacity(PropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getBackgroundOpacity(klass))
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<BackgroundOpacity>().set(value, klass);
    baseImpl = std::move(impl_);
    observer->onLayerPaintPropertyChanged(*this);
}

void BackgroundLayer::setBackgroundOpacityTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<BackgroundOpacity>().setTransition(value, klass);
    baseImpl = std::move(impl_);
}

TransitionOptions BackgroundLayer::getBackgroundOpacityTransition(const optional<std::string>& klass) const {
    return impl().paint.template get<BackgroundOpacity>().getTransition(klass);
}

} // namespace style
} // namespace mbgl
