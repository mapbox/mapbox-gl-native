// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/background_layer.hpp>
#include <mbgl/style/layers/background_layer_impl.hpp>
#include <mbgl/style/conversion/stringify.hpp>

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
    result->impl->paint = BackgroundPaintProperties();
    return std::move(result);
}

void BackgroundLayer::Impl::stringifyLayout(rapidjson::Writer<rapidjson::StringBuffer>&) const {
}


// Layout properties


// Paint properties

PropertyValue<Color> BackgroundLayer::getDefaultBackgroundColor() {
    return { Color::black() };
}

PropertyValue<Color> BackgroundLayer::getBackgroundColor(const optional<std::string>& klass) const {
    return impl->paint.get<BackgroundColor>(klass);
}

void BackgroundLayer::setBackgroundColor(PropertyValue<Color> value, const optional<std::string>& klass) {
    if (value == getBackgroundColor(klass))
        return;
    impl->paint.set<BackgroundColor>(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<std::string> BackgroundLayer::getDefaultBackgroundPattern() {
    return { "" };
}

PropertyValue<std::string> BackgroundLayer::getBackgroundPattern(const optional<std::string>& klass) const {
    return impl->paint.get<BackgroundPattern>(klass);
}

void BackgroundLayer::setBackgroundPattern(PropertyValue<std::string> value, const optional<std::string>& klass) {
    if (value == getBackgroundPattern(klass))
        return;
    impl->paint.set<BackgroundPattern>(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<float> BackgroundLayer::getDefaultBackgroundOpacity() {
    return { 1 };
}

PropertyValue<float> BackgroundLayer::getBackgroundOpacity(const optional<std::string>& klass) const {
    return impl->paint.get<BackgroundOpacity>(klass);
}

void BackgroundLayer::setBackgroundOpacity(PropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getBackgroundOpacity(klass))
        return;
    impl->paint.set<BackgroundOpacity>(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

} // namespace style
} // namespace mbgl
