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
    impl->observer->onLayerFilterChanged(*this);
}

const Filter& FillLayer::getFilter() const {
    return impl->filter;
}

// Layout properties


// Paint properties

PropertyValue<bool> FillLayer::getDefaultFillAntialias() {
    return { true };
}

PropertyValue<bool> FillLayer::getFillAntialias(const optional<std::string>& klass) const {
    return impl->paint.get<FillAntialias>(klass);
}

void FillLayer::setFillAntialias(PropertyValue<bool> value, const optional<std::string>& klass) {
    if (value == getFillAntialias(klass))
        return;
    impl->paint.set<FillAntialias>(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<float> FillLayer::getDefaultFillOpacity() {
    return { 1 };
}

PropertyValue<float> FillLayer::getFillOpacity(const optional<std::string>& klass) const {
    return impl->paint.get<FillOpacity>(klass);
}

void FillLayer::setFillOpacity(PropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getFillOpacity(klass))
        return;
    impl->paint.set<FillOpacity>(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<Color> FillLayer::getDefaultFillColor() {
    return { Color::black() };
}

PropertyValue<Color> FillLayer::getFillColor(const optional<std::string>& klass) const {
    return impl->paint.get<FillColor>(klass);
}

void FillLayer::setFillColor(PropertyValue<Color> value, const optional<std::string>& klass) {
    if (value == getFillColor(klass))
        return;
    impl->paint.set<FillColor>(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<Color> FillLayer::getDefaultFillOutlineColor() {
    return { {} };
}

PropertyValue<Color> FillLayer::getFillOutlineColor(const optional<std::string>& klass) const {
    return impl->paint.get<FillOutlineColor>(klass);
}

void FillLayer::setFillOutlineColor(PropertyValue<Color> value, const optional<std::string>& klass) {
    if (value == getFillOutlineColor(klass))
        return;
    impl->paint.set<FillOutlineColor>(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<std::array<float, 2>> FillLayer::getDefaultFillTranslate() {
    return { {{ 0, 0 }} };
}

PropertyValue<std::array<float, 2>> FillLayer::getFillTranslate(const optional<std::string>& klass) const {
    return impl->paint.get<FillTranslate>(klass);
}

void FillLayer::setFillTranslate(PropertyValue<std::array<float, 2>> value, const optional<std::string>& klass) {
    if (value == getFillTranslate(klass))
        return;
    impl->paint.set<FillTranslate>(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<TranslateAnchorType> FillLayer::getDefaultFillTranslateAnchor() {
    return { TranslateAnchorType::Map };
}

PropertyValue<TranslateAnchorType> FillLayer::getFillTranslateAnchor(const optional<std::string>& klass) const {
    return impl->paint.get<FillTranslateAnchor>(klass);
}

void FillLayer::setFillTranslateAnchor(PropertyValue<TranslateAnchorType> value, const optional<std::string>& klass) {
    if (value == getFillTranslateAnchor(klass))
        return;
    impl->paint.set<FillTranslateAnchor>(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<std::string> FillLayer::getDefaultFillPattern() {
    return { "" };
}

PropertyValue<std::string> FillLayer::getFillPattern(const optional<std::string>& klass) const {
    return impl->paint.get<FillPattern>(klass);
}

void FillLayer::setFillPattern(PropertyValue<std::string> value, const optional<std::string>& klass) {
    if (value == getFillPattern(klass))
        return;
    impl->paint.set<FillPattern>(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

} // namespace style
} // namespace mbgl
