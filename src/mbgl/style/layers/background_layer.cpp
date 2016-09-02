// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/background_layer.hpp>
#include <mbgl/style/layers/background_layer_impl.hpp>

namespace mbgl {
namespace style {

BackgroundLayer::BackgroundLayer(const std::string& layerID)
    : Layer(Type::Background, std::make_unique<Impl>())
    , impl(static_cast<Impl*>(baseImpl.get())) {
    impl->id = layerID;
}

BackgroundLayer::BackgroundLayer(const Impl& other)
    : Layer(Type::Background, std::make_unique<Impl>(other))
    , impl(static_cast<Impl*>(baseImpl.get())) {
}

BackgroundLayer::~BackgroundLayer() = default;

std::unique_ptr<Layer> BackgroundLayer::Impl::clone() const {
    return std::make_unique<BackgroundLayer>(*this);
}

std::unique_ptr<Layer> BackgroundLayer::Impl::cloneRef(const std::string& id_) const {
    auto result = std::make_unique<BackgroundLayer>(*this);
    result->impl->id = id_;
    result->impl->ref = this->id;
    result->impl->paint = BackgroundPaintProperties();
    return std::move(result);
}


// Layout properties


// Paint properties

PropertyValue<Color> BackgroundLayer::getDefaultBackgroundColor() {
    return { Color::black() };
}

PropertyValue<Color> BackgroundLayer::getBackgroundColor() const {
    return impl->paint.backgroundColor.get();
}

void BackgroundLayer::setBackgroundColor(PropertyValue<Color> value, const optional<std::string>& klass) {
    impl->paint.backgroundColor.set(value, klass);
}

PropertyValue<std::string> BackgroundLayer::getDefaultBackgroundPattern() {
    return { "" };
}

PropertyValue<std::string> BackgroundLayer::getBackgroundPattern() const {
    return impl->paint.backgroundPattern.get();
}

void BackgroundLayer::setBackgroundPattern(PropertyValue<std::string> value, const optional<std::string>& klass) {
    impl->paint.backgroundPattern.set(value, klass);
}

PropertyValue<float> BackgroundLayer::getDefaultBackgroundOpacity() {
    return { 1 };
}

PropertyValue<float> BackgroundLayer::getBackgroundOpacity() const {
    return impl->paint.backgroundOpacity.get();
}

void BackgroundLayer::setBackgroundOpacity(PropertyValue<float> value, const optional<std::string>& klass) {
    impl->paint.backgroundOpacity.set(value, klass);
}

} // namespace style
} // namespace mbgl
