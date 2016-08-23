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
}

const Filter& CircleLayer::getFilter() const {
    return impl->filter;
}

// Layout properties


// Paint properties

PropertyValue<float> CircleLayer::getDefaultCircleRadius() {
    return { 5 };
}

PropertyValue<float> CircleLayer::getCircleRadius() const {
    return impl->paint.circleRadius.get();
}

void CircleLayer::setCircleRadius(PropertyValue<float> value, const optional<std::string>& klass) {
    impl->paint.circleRadius.set(value, klass);
}

PropertyValue<Color> CircleLayer::getDefaultCircleColor() {
    return { Color::black() };
}

PropertyValue<Color> CircleLayer::getCircleColor() const {
    return impl->paint.circleColor.get();
}

void CircleLayer::setCircleColor(PropertyValue<Color> value, const optional<std::string>& klass) {
    impl->paint.circleColor.set(value, klass);
}

PropertyValue<float> CircleLayer::getDefaultCircleBlur() {
    return { 0 };
}

PropertyValue<float> CircleLayer::getCircleBlur() const {
    return impl->paint.circleBlur.get();
}

void CircleLayer::setCircleBlur(PropertyValue<float> value, const optional<std::string>& klass) {
    impl->paint.circleBlur.set(value, klass);
}

PropertyValue<float> CircleLayer::getDefaultCircleOpacity() {
    return { 1 };
}

PropertyValue<float> CircleLayer::getCircleOpacity() const {
    return impl->paint.circleOpacity.get();
}

void CircleLayer::setCircleOpacity(PropertyValue<float> value, const optional<std::string>& klass) {
    impl->paint.circleOpacity.set(value, klass);
}

PropertyValue<std::array<float, 2>> CircleLayer::getDefaultCircleTranslate() {
    return { {{ 0, 0 }} };
}

PropertyValue<std::array<float, 2>> CircleLayer::getCircleTranslate() const {
    return impl->paint.circleTranslate.get();
}

void CircleLayer::setCircleTranslate(PropertyValue<std::array<float, 2>> value, const optional<std::string>& klass) {
    impl->paint.circleTranslate.set(value, klass);
}

PropertyValue<TranslateAnchorType> CircleLayer::getDefaultCircleTranslateAnchor() {
    return { TranslateAnchorType::Map };
}

PropertyValue<TranslateAnchorType> CircleLayer::getCircleTranslateAnchor() const {
    return impl->paint.circleTranslateAnchor.get();
}

void CircleLayer::setCircleTranslateAnchor(PropertyValue<TranslateAnchorType> value, const optional<std::string>& klass) {
    impl->paint.circleTranslateAnchor.set(value, klass);
}

PropertyValue<CirclePitchScaleType> CircleLayer::getDefaultCirclePitchScale() {
    return { CirclePitchScaleType::Map };
}

PropertyValue<CirclePitchScaleType> CircleLayer::getCirclePitchScale() const {
    return impl->paint.circlePitchScale.get();
}

void CircleLayer::setCirclePitchScale(PropertyValue<CirclePitchScaleType> value, const optional<std::string>& klass) {
    impl->paint.circlePitchScale.set(value, klass);
}

} // namespace style
} // namespace mbgl
