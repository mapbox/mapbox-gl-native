// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/circle_layer.hpp>
#include <mbgl/style/layers/circle_layer_impl.hpp>

namespace mbgl {
namespace style {

CircleLayer::CircleLayer(const std::string& layerID, const std::string& sourceID)
    : Layer(Type::Circle, std::make_unique<Impl>())
    , impl(static_cast<Impl*>(baseImpl.get())) {
    impl->id = layerID;
    impl->source = sourceID;
}

CircleLayer::CircleLayer(const Impl& other)
    : Layer(Type::Circle, std::make_unique<Impl>(other))
    , impl(static_cast<Impl*>(baseImpl.get())) {
}

CircleLayer::~CircleLayer() = default;

std::unique_ptr<Layer> CircleLayer::Impl::clone() const {
    return std::make_unique<CircleLayer>(*this);
}

std::unique_ptr<Layer> CircleLayer::Impl::cloneRef(const std::string& id_) const {
    auto result = std::make_unique<CircleLayer>(*this);
    result->impl->id = id_;
    result->impl->ref = this->id;
    result->impl->paint = CirclePaintProperties();
    return std::move(result);
}

// Source

const std::string& CircleLayer::getSourceID() const {
    return impl->source;
}

void CircleLayer::setSourceLayer(const std::string& sourceLayer) {
    impl->sourceLayer = sourceLayer;
}

const std::string& CircleLayer::getSourceLayer() const {
    return impl->sourceLayer;
}

// Filter

void CircleLayer::setFilter(const Filter& filter) {
    impl->filter = filter;
    impl->observer->onLayerFilterChanged(*this);
}

const Filter& CircleLayer::getFilter() const {
    return impl->filter;
}

// Layout properties


// Paint properties

PropertyValue<float> CircleLayer::getDefaultCircleRadius() {
    return { 5 };
}

PropertyValue<float> CircleLayer::getCircleRadius(const optional<std::string>& klass) const {
    return impl->paint.get<CircleRadius>(klass);
}

void CircleLayer::setCircleRadius(PropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getCircleRadius(klass))
        return;
    impl->paint.set<CircleRadius>(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<Color> CircleLayer::getDefaultCircleColor() {
    return { Color::black() };
}

PropertyValue<Color> CircleLayer::getCircleColor(const optional<std::string>& klass) const {
    return impl->paint.get<CircleColor>(klass);
}

void CircleLayer::setCircleColor(PropertyValue<Color> value, const optional<std::string>& klass) {
    if (value == getCircleColor(klass))
        return;
    impl->paint.set<CircleColor>(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<float> CircleLayer::getDefaultCircleBlur() {
    return { 0 };
}

PropertyValue<float> CircleLayer::getCircleBlur(const optional<std::string>& klass) const {
    return impl->paint.get<CircleBlur>(klass);
}

void CircleLayer::setCircleBlur(PropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getCircleBlur(klass))
        return;
    impl->paint.set<CircleBlur>(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<float> CircleLayer::getDefaultCircleOpacity() {
    return { 1 };
}

PropertyValue<float> CircleLayer::getCircleOpacity(const optional<std::string>& klass) const {
    return impl->paint.get<CircleOpacity>(klass);
}

void CircleLayer::setCircleOpacity(PropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getCircleOpacity(klass))
        return;
    impl->paint.set<CircleOpacity>(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<std::array<float, 2>> CircleLayer::getDefaultCircleTranslate() {
    return { {{ 0, 0 }} };
}

PropertyValue<std::array<float, 2>> CircleLayer::getCircleTranslate(const optional<std::string>& klass) const {
    return impl->paint.get<CircleTranslate>(klass);
}

void CircleLayer::setCircleTranslate(PropertyValue<std::array<float, 2>> value, const optional<std::string>& klass) {
    if (value == getCircleTranslate(klass))
        return;
    impl->paint.set<CircleTranslate>(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<TranslateAnchorType> CircleLayer::getDefaultCircleTranslateAnchor() {
    return { TranslateAnchorType::Map };
}

PropertyValue<TranslateAnchorType> CircleLayer::getCircleTranslateAnchor(const optional<std::string>& klass) const {
    return impl->paint.get<CircleTranslateAnchor>(klass);
}

void CircleLayer::setCircleTranslateAnchor(PropertyValue<TranslateAnchorType> value, const optional<std::string>& klass) {
    if (value == getCircleTranslateAnchor(klass))
        return;
    impl->paint.set<CircleTranslateAnchor>(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<CirclePitchScaleType> CircleLayer::getDefaultCirclePitchScale() {
    return { CirclePitchScaleType::Map };
}

PropertyValue<CirclePitchScaleType> CircleLayer::getCirclePitchScale(const optional<std::string>& klass) const {
    return impl->paint.get<CirclePitchScale>(klass);
}

void CircleLayer::setCirclePitchScale(PropertyValue<CirclePitchScaleType> value, const optional<std::string>& klass) {
    if (value == getCirclePitchScale(klass))
        return;
    impl->paint.set<CirclePitchScale>(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

} // namespace style
} // namespace mbgl
