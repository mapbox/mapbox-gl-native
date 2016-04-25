// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/layer/fill_layer.hpp>
#include <mbgl/layer/fill_layer_impl.hpp>

namespace mbgl {

FillLayer::FillLayer(const std::string& layerID)
    : Layer(Type::Fill, std::make_unique<Impl>())
    , impl(static_cast<Impl*>(baseImpl.get())) {
    impl->id = layerID;
}

FillLayer::FillLayer(const Impl& other)
    : Layer(Type::Fill, std::make_unique<Impl>(other))
    , impl(static_cast<Impl*>(baseImpl.get())) {
}

FillLayer::~FillLayer() = default;

std::unique_ptr<Layer> FillLayer::Impl::clone() const {
    return std::make_unique<FillLayer>(*this);
}

// Source

void FillLayer::setSource(const std::string& sourceID, const std::string& sourceLayer) {
    impl->source = sourceID;
    impl->sourceLayer = sourceLayer;
}

const std::string& FillLayer::getSourceID() const {
    return impl->source;
}

const std::string& FillLayer::getSourceLayer() const {
    return impl->sourceLayer;
}

// Layout properties


// Paint properties

PropertyValue<bool> FillLayer::getFillAntialias() const {
    return impl->paint.fillAntialias.get();
}

void FillLayer::setFillAntialias(PropertyValue<bool> value) {
    impl->paint.fillAntialias.set(value);
}

PropertyValue<float> FillLayer::getFillOpacity() const {
    return impl->paint.fillOpacity.get();
}

void FillLayer::setFillOpacity(PropertyValue<float> value) {
    impl->paint.fillOpacity.set(value);
}

PropertyValue<Color> FillLayer::getFillColor() const {
    return impl->paint.fillColor.get();
}

void FillLayer::setFillColor(PropertyValue<Color> value) {
    impl->paint.fillColor.set(value);
}

PropertyValue<Color> FillLayer::getFillOutlineColor() const {
    return impl->paint.fillOutlineColor.get();
}

void FillLayer::setFillOutlineColor(PropertyValue<Color> value) {
    impl->paint.fillOutlineColor.set(value);
}

PropertyValue<std::array<float, 2>> FillLayer::getFillTranslate() const {
    return impl->paint.fillTranslate.get();
}

void FillLayer::setFillTranslate(PropertyValue<std::array<float, 2>> value) {
    impl->paint.fillTranslate.set(value);
}

PropertyValue<TranslateAnchorType> FillLayer::getFillTranslateAnchor() const {
    return impl->paint.fillTranslateAnchor.get();
}

void FillLayer::setFillTranslateAnchor(PropertyValue<TranslateAnchorType> value) {
    impl->paint.fillTranslateAnchor.set(value);
}

PropertyValue<std::string> FillLayer::getFillPattern() const {
    return impl->paint.fillPattern.get();
}

void FillLayer::setFillPattern(PropertyValue<std::string> value) {
    impl->paint.fillPattern.set(value);
}

} // namespace mbgl
