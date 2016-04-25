// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/layer/circle_layer.hpp>
#include <mbgl/layer/circle_layer_impl.hpp>

namespace mbgl {

CircleLayer::CircleLayer(const std::string& layerID)
    : Layer(Type::Circle, std::make_unique<Impl>())
    , impl(static_cast<Impl*>(baseImpl.get())) {
    impl->id = layerID;
}

CircleLayer::CircleLayer(const Impl& other)
    : Layer(Type::Circle, std::make_unique<Impl>(other))
    , impl(static_cast<Impl*>(baseImpl.get())) {
}

CircleLayer::~CircleLayer() = default;

std::unique_ptr<Layer> CircleLayer::Impl::clone() const {
    return std::make_unique<CircleLayer>(*this);
}

// Source

void CircleLayer::setSource(const std::string& sourceID, const std::string& sourceLayer) {
    impl->source = sourceID;
    impl->sourceLayer = sourceLayer;
}

const std::string& CircleLayer::getSourceID() const {
    return impl->source;
}

const std::string& CircleLayer::getSourceLayer() const {
    return impl->sourceLayer;
}

// Layout properties


// Paint properties

PropertyValue<float> CircleLayer::getCircleRadius() const {
    return impl->paint.circleRadius.get();
}

void CircleLayer::setCircleRadius(PropertyValue<float> value) {
    impl->paint.circleRadius.set(value);
}

PropertyValue<Color> CircleLayer::getCircleColor() const {
    return impl->paint.circleColor.get();
}

void CircleLayer::setCircleColor(PropertyValue<Color> value) {
    impl->paint.circleColor.set(value);
}

PropertyValue<float> CircleLayer::getCircleBlur() const {
    return impl->paint.circleBlur.get();
}

void CircleLayer::setCircleBlur(PropertyValue<float> value) {
    impl->paint.circleBlur.set(value);
}

PropertyValue<float> CircleLayer::getCircleOpacity() const {
    return impl->paint.circleOpacity.get();
}

void CircleLayer::setCircleOpacity(PropertyValue<float> value) {
    impl->paint.circleOpacity.set(value);
}

PropertyValue<std::array<float, 2>> CircleLayer::getCircleTranslate() const {
    return impl->paint.circleTranslate.get();
}

void CircleLayer::setCircleTranslate(PropertyValue<std::array<float, 2>> value) {
    impl->paint.circleTranslate.set(value);
}

PropertyValue<TranslateAnchorType> CircleLayer::getCircleTranslateAnchor() const {
    return impl->paint.circleTranslateAnchor.get();
}

void CircleLayer::setCircleTranslateAnchor(PropertyValue<TranslateAnchorType> value) {
    impl->paint.circleTranslateAnchor.set(value);
}

} // namespace mbgl
