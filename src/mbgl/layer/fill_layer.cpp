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

Function<bool> FillLayer::getFillAntialias() const {
    return impl->paint.fillAntialias.values.at(ClassID::Default);
}

void FillLayer::setFillAntialias(Function<bool> value) {
    impl->paint.fillAntialias.values.emplace(ClassID::Default, value);
}

Function<float> FillLayer::getFillOpacity() const {
    return impl->paint.fillOpacity.values.at(ClassID::Default);
}

void FillLayer::setFillOpacity(Function<float> value) {
    impl->paint.fillOpacity.values.emplace(ClassID::Default, value);
}

Function<Color> FillLayer::getFillColor() const {
    return impl->paint.fillColor.values.at(ClassID::Default);
}

void FillLayer::setFillColor(Function<Color> value) {
    impl->paint.fillColor.values.emplace(ClassID::Default, value);
}

Function<Color> FillLayer::getFillOutlineColor() const {
    return impl->paint.fillOutlineColor.values.at(ClassID::Default);
}

void FillLayer::setFillOutlineColor(Function<Color> value) {
    impl->paint.fillOutlineColor.values.emplace(ClassID::Default, value);
}

Function<std::array<float, 2>> FillLayer::getFillTranslate() const {
    return impl->paint.fillTranslate.values.at(ClassID::Default);
}

void FillLayer::setFillTranslate(Function<std::array<float, 2>> value) {
    impl->paint.fillTranslate.values.emplace(ClassID::Default, value);
}

Function<TranslateAnchorType> FillLayer::getFillTranslateAnchor() const {
    return impl->paint.fillTranslateAnchor.values.at(ClassID::Default);
}

void FillLayer::setFillTranslateAnchor(Function<TranslateAnchorType> value) {
    impl->paint.fillTranslateAnchor.values.emplace(ClassID::Default, value);
}

Function<std::string> FillLayer::getFillPattern() const {
    return impl->paint.fillPattern.values.at(ClassID::Default);
}

void FillLayer::setFillPattern(Function<std::string> value) {
    impl->paint.fillPattern.values.emplace(ClassID::Default, value);
}

} // namespace mbgl
