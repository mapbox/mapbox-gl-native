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

Function<float> CircleLayer::getCircleRadius() const {
    return impl->paint.circleRadius.values.at(ClassID::Default);
}

void CircleLayer::setCircleRadius(Function<float> value) {
    impl->paint.circleRadius.values.emplace(ClassID::Default, value);
}

Function<Color> CircleLayer::getCircleColor() const {
    return impl->paint.circleColor.values.at(ClassID::Default);
}

void CircleLayer::setCircleColor(Function<Color> value) {
    impl->paint.circleColor.values.emplace(ClassID::Default, value);
}

Function<float> CircleLayer::getCircleBlur() const {
    return impl->paint.circleBlur.values.at(ClassID::Default);
}

void CircleLayer::setCircleBlur(Function<float> value) {
    impl->paint.circleBlur.values.emplace(ClassID::Default, value);
}

Function<float> CircleLayer::getCircleOpacity() const {
    return impl->paint.circleOpacity.values.at(ClassID::Default);
}

void CircleLayer::setCircleOpacity(Function<float> value) {
    impl->paint.circleOpacity.values.emplace(ClassID::Default, value);
}

Function<std::array<float, 2>> CircleLayer::getCircleTranslate() const {
    return impl->paint.circleTranslate.values.at(ClassID::Default);
}

void CircleLayer::setCircleTranslate(Function<std::array<float, 2>> value) {
    impl->paint.circleTranslate.values.emplace(ClassID::Default, value);
}

Function<TranslateAnchorType> CircleLayer::getCircleTranslateAnchor() const {
    return impl->paint.circleTranslateAnchor.values.at(ClassID::Default);
}

void CircleLayer::setCircleTranslateAnchor(Function<TranslateAnchorType> value) {
    impl->paint.circleTranslateAnchor.values.emplace(ClassID::Default, value);
}

} // namespace mbgl
