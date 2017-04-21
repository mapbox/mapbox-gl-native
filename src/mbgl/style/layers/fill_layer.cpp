// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/fill_layer.hpp>
#include <mbgl/style/layers/fill_layer_impl.hpp>
#include <mbgl/style/conversion/stringify.hpp>

namespace mbgl {
namespace style {

FillLayer::FillLayer(const std::string& layerID, const std::string& sourceID)
    : Layer(LayerType::Fill, std::make_unique<Impl>())
    , impl(static_cast<Impl*>(baseImpl.get())) {
    impl->id = layerID;
    impl->source = sourceID;
}

FillLayer::FillLayer(const Impl& other)
    : Layer(LayerType::Fill, std::make_unique<Impl>(other))
    , impl(static_cast<Impl*>(baseImpl.get())) {
}

FillLayer::~FillLayer() = default;

std::unique_ptr<Layer> FillLayer::Impl::clone() const {
    return std::make_unique<FillLayer>(*this);
}

std::unique_ptr<Layer> FillLayer::Impl::cloneRef(const std::string& id_) const {
    auto result = std::make_unique<FillLayer>(*this);
    result->impl->id = id_;
    result->impl->cascading = FillPaintProperties::Cascading();
    return std::move(result);
}

void FillLayer::Impl::stringifyLayout(rapidjson::Writer<rapidjson::StringBuffer>&) const {
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
    return impl->cascading.template get<FillAntialias>().get(klass);
}

void FillLayer::setFillAntialias(PropertyValue<bool> value, const optional<std::string>& klass) {
    if (value == getFillAntialias(klass))
        return;
    impl->cascading.template get<FillAntialias>().set(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

void FillLayer::setFillAntialiasTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    impl->cascading.template get<FillAntialias>().setTransition(value, klass);
}

TransitionOptions FillLayer::getFillAntialiasTransition(const optional<std::string>& klass) const {
    return impl->cascading.template get<FillAntialias>().getTransition(klass);
}

DataDrivenPropertyValue<float> FillLayer::getDefaultFillOpacity() {
    return { 1 };
}

DataDrivenPropertyValue<float> FillLayer::getFillOpacity(const optional<std::string>& klass) const {
    return impl->cascading.template get<FillOpacity>().get(klass);
}

void FillLayer::setFillOpacity(DataDrivenPropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getFillOpacity(klass))
        return;
    impl->cascading.template get<FillOpacity>().set(value, klass);
    if (value.isDataDriven()) {
        impl->observer->onLayerDataDrivenPaintPropertyChanged(*this);
    } else {
        impl->observer->onLayerPaintPropertyChanged(*this);
    }
}

void FillLayer::setFillOpacityTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    impl->cascading.template get<FillOpacity>().setTransition(value, klass);
}

TransitionOptions FillLayer::getFillOpacityTransition(const optional<std::string>& klass) const {
    return impl->cascading.template get<FillOpacity>().getTransition(klass);
}

DataDrivenPropertyValue<Color> FillLayer::getDefaultFillColor() {
    return { Color::black() };
}

DataDrivenPropertyValue<Color> FillLayer::getFillColor(const optional<std::string>& klass) const {
    return impl->cascading.template get<FillColor>().get(klass);
}

void FillLayer::setFillColor(DataDrivenPropertyValue<Color> value, const optional<std::string>& klass) {
    if (value == getFillColor(klass))
        return;
    impl->cascading.template get<FillColor>().set(value, klass);
    if (value.isDataDriven()) {
        impl->observer->onLayerDataDrivenPaintPropertyChanged(*this);
    } else {
        impl->observer->onLayerPaintPropertyChanged(*this);
    }
}

void FillLayer::setFillColorTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    impl->cascading.template get<FillColor>().setTransition(value, klass);
}

TransitionOptions FillLayer::getFillColorTransition(const optional<std::string>& klass) const {
    return impl->cascading.template get<FillColor>().getTransition(klass);
}

DataDrivenPropertyValue<Color> FillLayer::getDefaultFillOutlineColor() {
    return { {} };
}

DataDrivenPropertyValue<Color> FillLayer::getFillOutlineColor(const optional<std::string>& klass) const {
    return impl->cascading.template get<FillOutlineColor>().get(klass);
}

void FillLayer::setFillOutlineColor(DataDrivenPropertyValue<Color> value, const optional<std::string>& klass) {
    if (value == getFillOutlineColor(klass))
        return;
    impl->cascading.template get<FillOutlineColor>().set(value, klass);
    if (value.isDataDriven()) {
        impl->observer->onLayerDataDrivenPaintPropertyChanged(*this);
    } else {
        impl->observer->onLayerPaintPropertyChanged(*this);
    }
}

void FillLayer::setFillOutlineColorTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    impl->cascading.template get<FillOutlineColor>().setTransition(value, klass);
}

TransitionOptions FillLayer::getFillOutlineColorTransition(const optional<std::string>& klass) const {
    return impl->cascading.template get<FillOutlineColor>().getTransition(klass);
}

PropertyValue<std::array<float, 2>> FillLayer::getDefaultFillTranslate() {
    return { {{ 0, 0 }} };
}

PropertyValue<std::array<float, 2>> FillLayer::getFillTranslate(const optional<std::string>& klass) const {
    return impl->cascading.template get<FillTranslate>().get(klass);
}

void FillLayer::setFillTranslate(PropertyValue<std::array<float, 2>> value, const optional<std::string>& klass) {
    if (value == getFillTranslate(klass))
        return;
    impl->cascading.template get<FillTranslate>().set(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

void FillLayer::setFillTranslateTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    impl->cascading.template get<FillTranslate>().setTransition(value, klass);
}

TransitionOptions FillLayer::getFillTranslateTransition(const optional<std::string>& klass) const {
    return impl->cascading.template get<FillTranslate>().getTransition(klass);
}

PropertyValue<TranslateAnchorType> FillLayer::getDefaultFillTranslateAnchor() {
    return { TranslateAnchorType::Map };
}

PropertyValue<TranslateAnchorType> FillLayer::getFillTranslateAnchor(const optional<std::string>& klass) const {
    return impl->cascading.template get<FillTranslateAnchor>().get(klass);
}

void FillLayer::setFillTranslateAnchor(PropertyValue<TranslateAnchorType> value, const optional<std::string>& klass) {
    if (value == getFillTranslateAnchor(klass))
        return;
    impl->cascading.template get<FillTranslateAnchor>().set(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

void FillLayer::setFillTranslateAnchorTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    impl->cascading.template get<FillTranslateAnchor>().setTransition(value, klass);
}

TransitionOptions FillLayer::getFillTranslateAnchorTransition(const optional<std::string>& klass) const {
    return impl->cascading.template get<FillTranslateAnchor>().getTransition(klass);
}

PropertyValue<std::string> FillLayer::getDefaultFillPattern() {
    return { "" };
}

PropertyValue<std::string> FillLayer::getFillPattern(const optional<std::string>& klass) const {
    return impl->cascading.template get<FillPattern>().get(klass);
}

void FillLayer::setFillPattern(PropertyValue<std::string> value, const optional<std::string>& klass) {
    if (value == getFillPattern(klass))
        return;
    impl->cascading.template get<FillPattern>().set(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

void FillLayer::setFillPatternTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    impl->cascading.template get<FillPattern>().setTransition(value, klass);
}

TransitionOptions FillLayer::getFillPatternTransition(const optional<std::string>& klass) const {
    return impl->cascading.template get<FillPattern>().getTransition(klass);
}

} // namespace style
} // namespace mbgl
