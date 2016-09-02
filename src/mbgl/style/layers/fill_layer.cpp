// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/fill_layer.hpp>
#include <mbgl/style/layers/fill_layer_impl.hpp>

namespace mbgl {
namespace style {

FillLayer::FillLayer(const std::string& layerID, const std::string& sourceID)
    : Layer(Type::Fill, std::make_unique<Impl>())
    , impl(static_cast<Impl*>(baseImpl.get())) {
    impl->id = layerID;
    impl->source = sourceID;
}

FillLayer::FillLayer(const Impl& other)
    : Layer(Type::Fill, std::make_unique<Impl>(other))
    , impl(static_cast<Impl*>(baseImpl.get())) {
}

FillLayer::~FillLayer() = default;

std::unique_ptr<Layer> FillLayer::Impl::clone() const {
    return std::make_unique<FillLayer>(*this);
}

std::unique_ptr<Layer> FillLayer::Impl::cloneRef(const std::string& id_) const {
    auto result = std::make_unique<FillLayer>(*this);
    result->impl->id = id_;
    result->impl->ref = this->id;
    result->impl->paint = FillPaintProperties();
    return std::move(result);
}

// Source

const std::string& FillLayer::getSourceID() const {
    return impl->source;
}

void FillLayer::setSourceLayer(const std::string& sourceLayer) {
    impl->sourceLayer = sourceLayer;
}

const std::string& FillLayer::getSourceLayer() const {
    return impl->sourceLayer;
}

// Filter

void FillLayer::setFilter(const Filter& filter) {
    impl->filter = filter;
}

const Filter& FillLayer::getFilter() const {
    return impl->filter;
}

// Layout properties


// Paint properties

PropertyValue<bool> FillLayer::getDefaultFillAntialias() {
    return { true };
}

PropertyValue<bool> FillLayer::getFillAntialias() const {
    return impl->paint.fillAntialias.get();
}

void FillLayer::setFillAntialias(PropertyValue<bool> value, const optional<std::string>& klass) {
    impl->paint.fillAntialias.set(value, klass);
}

PropertyValue<float> FillLayer::getDefaultFillOpacity() {
    return { 1 };
}

PropertyValue<float> FillLayer::getFillOpacity() const {
    return impl->paint.fillOpacity.get();
}

void FillLayer::setFillOpacity(PropertyValue<float> value, const optional<std::string>& klass) {
    impl->paint.fillOpacity.set(value, klass);
}

PropertyValue<Color> FillLayer::getDefaultFillColor() {
    return { Color::black() };
}

PropertyValue<Color> FillLayer::getFillColor() const {
    return impl->paint.fillColor.get();
}

void FillLayer::setFillColor(PropertyValue<Color> value, const optional<std::string>& klass) {
    impl->paint.fillColor.set(value, klass);
}

PropertyValue<Color> FillLayer::getDefaultFillOutlineColor() {
    return { {} };
}

PropertyValue<Color> FillLayer::getFillOutlineColor() const {
    return impl->paint.fillOutlineColor.get();
}

void FillLayer::setFillOutlineColor(PropertyValue<Color> value, const optional<std::string>& klass) {
    impl->paint.fillOutlineColor.set(value, klass);
}

PropertyValue<std::array<float, 2>> FillLayer::getDefaultFillTranslate() {
    return { {{ 0, 0 }} };
}

PropertyValue<std::array<float, 2>> FillLayer::getFillTranslate() const {
    return impl->paint.fillTranslate.get();
}

void FillLayer::setFillTranslate(PropertyValue<std::array<float, 2>> value, const optional<std::string>& klass) {
    impl->paint.fillTranslate.set(value, klass);
}

PropertyValue<TranslateAnchorType> FillLayer::getDefaultFillTranslateAnchor() {
    return { TranslateAnchorType::Map };
}

PropertyValue<TranslateAnchorType> FillLayer::getFillTranslateAnchor() const {
    return impl->paint.fillTranslateAnchor.get();
}

void FillLayer::setFillTranslateAnchor(PropertyValue<TranslateAnchorType> value, const optional<std::string>& klass) {
    impl->paint.fillTranslateAnchor.set(value, klass);
}

PropertyValue<std::string> FillLayer::getDefaultFillPattern() {
    return { "" };
}

PropertyValue<std::string> FillLayer::getFillPattern() const {
    return impl->paint.fillPattern.get();
}

void FillLayer::setFillPattern(PropertyValue<std::string> value, const optional<std::string>& klass) {
    impl->paint.fillPattern.set(value, klass);
}

} // namespace style
} // namespace mbgl
