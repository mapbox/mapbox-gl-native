// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/fill_extrusion_layer.hpp>
#include <mbgl/style/layers/fill_extrusion_layer_impl.hpp>
#include <mbgl/style/conversion/stringify.hpp>

namespace mbgl {
namespace style {

FillExtrusionLayer::FillExtrusionLayer(const std::string& layerID, const std::string& sourceID)
    : Layer(Type::FillExtrusion, std::make_unique<Impl>())
    , impl(static_cast<Impl*>(baseImpl.get())) {
    impl->id = layerID;
    impl->source = sourceID;
}

FillExtrusionLayer::FillExtrusionLayer(const Impl& other)
    : Layer(Type::FillExtrusion, std::make_unique<Impl>(other))
    , impl(static_cast<Impl*>(baseImpl.get())) {
}

FillExtrusionLayer::~FillExtrusionLayer() = default;

std::unique_ptr<Layer> FillExtrusionLayer::Impl::clone() const {
    return std::make_unique<FillExtrusionLayer>(*this);
}

std::unique_ptr<Layer> FillExtrusionLayer::Impl::cloneRef(const std::string& id_) const {
    auto result = std::make_unique<FillExtrusionLayer>(*this);
    result->impl->id = id_;
    result->impl->paint = FillExtrusionPaintProperties();
    return std::move(result);
}

void FillExtrusionLayer::Impl::stringifyLayout(rapidjson::Writer<rapidjson::StringBuffer>&) const {
}

// Source

const std::string& FillExtrusionLayer::getSourceID() const {
    return impl->source;
}

void FillExtrusionLayer::setSourceLayer(const std::string& sourceLayer) {
    impl->sourceLayer = sourceLayer;
}

const std::string& FillExtrusionLayer::getSourceLayer() const {
    return impl->sourceLayer;
}

// Filter

void FillExtrusionLayer::setFilter(const Filter& filter) {
    impl->filter = filter;
    impl->observer->onLayerFilterChanged(*this);
}

const Filter& FillExtrusionLayer::getFilter() const {
    return impl->filter;
}

// Layout properties


// Paint properties

PropertyValue<float> FillExtrusionLayer::getDefaultFillExtrusionOpacity() {
    return { 1 };
}

PropertyValue<float> FillExtrusionLayer::getFillExtrusionOpacity(const optional<std::string>& klass) const {
    return impl->paint.get<FillExtrusionOpacity>(klass);
}

void FillExtrusionLayer::setFillExtrusionOpacity(PropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getFillExtrusionOpacity(klass))
        return;
    impl->paint.set<FillExtrusionOpacity>(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<Color> FillExtrusionLayer::getDefaultFillExtrusionColor() {
    return { Color::black() };
}

PropertyValue<Color> FillExtrusionLayer::getFillExtrusionColor(const optional<std::string>& klass) const {
    return impl->paint.get<FillExtrusionColor>(klass);
}

void FillExtrusionLayer::setFillExtrusionColor(PropertyValue<Color> value, const optional<std::string>& klass) {
    if (value == getFillExtrusionColor(klass))
        return;
    impl->paint.set<FillExtrusionColor>(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<std::array<float, 2>> FillExtrusionLayer::getDefaultFillExtrusionTranslate() {
    return { {{ 0, 0 }} };
}

PropertyValue<std::array<float, 2>> FillExtrusionLayer::getFillExtrusionTranslate(const optional<std::string>& klass) const {
    return impl->paint.get<FillExtrusionTranslate>(klass);
}

void FillExtrusionLayer::setFillExtrusionTranslate(PropertyValue<std::array<float, 2>> value, const optional<std::string>& klass) {
    if (value == getFillExtrusionTranslate(klass))
        return;
    impl->paint.set<FillExtrusionTranslate>(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<TranslateAnchorType> FillExtrusionLayer::getDefaultFillExtrusionTranslateAnchor() {
    return { TranslateAnchorType::Map };
}

PropertyValue<TranslateAnchorType> FillExtrusionLayer::getFillExtrusionTranslateAnchor(const optional<std::string>& klass) const {
    return impl->paint.get<FillExtrusionTranslateAnchor>(klass);
}

void FillExtrusionLayer::setFillExtrusionTranslateAnchor(PropertyValue<TranslateAnchorType> value, const optional<std::string>& klass) {
    if (value == getFillExtrusionTranslateAnchor(klass))
        return;
    impl->paint.set<FillExtrusionTranslateAnchor>(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<std::string> FillExtrusionLayer::getDefaultFillExtrusionPattern() {
    return { "" };
}

PropertyValue<std::string> FillExtrusionLayer::getFillExtrusionPattern(const optional<std::string>& klass) const {
    return impl->paint.get<FillExtrusionPattern>(klass);
}

void FillExtrusionLayer::setFillExtrusionPattern(PropertyValue<std::string> value, const optional<std::string>& klass) {
    if (value == getFillExtrusionPattern(klass))
        return;
    impl->paint.set<FillExtrusionPattern>(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<float> FillExtrusionLayer::getDefaultFillExtrusionHeight() {
    return { 0 };
}

PropertyValue<float> FillExtrusionLayer::getFillExtrusionHeight(const optional<std::string>& klass) const {
    return impl->paint.get<FillExtrusionHeight>(klass);
}

void FillExtrusionLayer::setFillExtrusionHeight(PropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getFillExtrusionHeight(klass))
        return;
    impl->paint.set<FillExtrusionHeight>(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<float> FillExtrusionLayer::getDefaultFillExtrusionBase() {
    return { 0 };
}

PropertyValue<float> FillExtrusionLayer::getFillExtrusionBase(const optional<std::string>& klass) const {
    return impl->paint.get<FillExtrusionBase>(klass);
}

void FillExtrusionLayer::setFillExtrusionBase(PropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getFillExtrusionBase(klass))
        return;
    impl->paint.set<FillExtrusionBase>(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

} // namespace style
} // namespace mbgl
