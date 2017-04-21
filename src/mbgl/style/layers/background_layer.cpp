// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/background_layer.hpp>
#include <mbgl/style/layers/background_layer_impl.hpp>
#include <mbgl/style/conversion/stringify.hpp>

namespace mbgl {
namespace style {

BackgroundLayer::BackgroundLayer(const std::string& layerID)
    : Layer(LayerType::Background, std::make_unique<Impl>())
    , impl(static_cast<Impl*>(baseImpl.get())) {
    impl->id = layerID;
}

BackgroundLayer::BackgroundLayer(const Impl& other)
    : Layer(LayerType::Background, std::make_unique<Impl>(other))
    , impl(static_cast<Impl*>(baseImpl.get())) {
}

BackgroundLayer::~BackgroundLayer() = default;

std::unique_ptr<Layer> BackgroundLayer::Impl::clone() const {
    return std::make_unique<BackgroundLayer>(*this);
}

std::unique_ptr<Layer> BackgroundLayer::Impl::cloneRef(const std::string& id_) const {
    auto result = std::make_unique<BackgroundLayer>(*this);
    result->impl->id = id_;
    result->impl->cascading = BackgroundPaintProperties::Cascading();
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
    return impl->cascading.template get<BackgroundColor>().get(klass);
}

void BackgroundLayer::setBackgroundColor(PropertyValue<Color> value, const optional<std::string>& klass) {
    if (value == getBackgroundColor(klass))
        return;
    impl->cascading.template get<BackgroundColor>().set(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

void BackgroundLayer::setBackgroundColorTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    impl->cascading.template get<BackgroundColor>().setTransition(value, klass);
}

TransitionOptions BackgroundLayer::getBackgroundColorTransition(const optional<std::string>& klass) const {
    return impl->cascading.template get<BackgroundColor>().getTransition(klass);
}

PropertyValue<std::string> BackgroundLayer::getDefaultBackgroundPattern() {
    return { "" };
}

PropertyValue<std::string> BackgroundLayer::getBackgroundPattern(const optional<std::string>& klass) const {
    return impl->cascading.template get<BackgroundPattern>().get(klass);
}

void BackgroundLayer::setBackgroundPattern(PropertyValue<std::string> value, const optional<std::string>& klass) {
    if (value == getBackgroundPattern(klass))
        return;
    impl->cascading.template get<BackgroundPattern>().set(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

void BackgroundLayer::setBackgroundPatternTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    impl->cascading.template get<BackgroundPattern>().setTransition(value, klass);
}

TransitionOptions BackgroundLayer::getBackgroundPatternTransition(const optional<std::string>& klass) const {
    return impl->cascading.template get<BackgroundPattern>().getTransition(klass);
}

PropertyValue<float> BackgroundLayer::getDefaultBackgroundOpacity() {
    return { 1 };
}

PropertyValue<float> BackgroundLayer::getBackgroundOpacity(const optional<std::string>& klass) const {
    return impl->cascading.template get<BackgroundOpacity>().get(klass);
}

void BackgroundLayer::setBackgroundOpacity(PropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getBackgroundOpacity(klass))
        return;
    impl->cascading.template get<BackgroundOpacity>().set(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

void BackgroundLayer::setBackgroundOpacityTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    impl->cascading.template get<BackgroundOpacity>().setTransition(value, klass);
}

TransitionOptions BackgroundLayer::getBackgroundOpacityTransition(const optional<std::string>& klass) const {
    return impl->cascading.template get<BackgroundOpacity>().getTransition(klass);
}

} // namespace style
} // namespace mbgl
