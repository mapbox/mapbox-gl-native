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


// Layout properties


// Paint properties

PropertyValue<Color> BackgroundLayer::getBackgroundColor() const {
    return impl->paint.backgroundColor.get();
}

void BackgroundLayer::setBackgroundColor(PropertyValue<Color> value) {
    impl->paint.backgroundColor.set(value);
}

PropertyValue<std::string> BackgroundLayer::getBackgroundPattern() const {
    return impl->paint.backgroundPattern.get();
}

void BackgroundLayer::setBackgroundPattern(PropertyValue<std::string> value) {
    impl->paint.backgroundPattern.set(value);
}

PropertyValue<float> BackgroundLayer::getBackgroundOpacity() const {
    return impl->paint.backgroundOpacity.get();
}

void BackgroundLayer::setBackgroundOpacity(PropertyValue<float> value) {
    impl->paint.backgroundOpacity.set(value);
}

} // namespace style
} // namespace mbgl
