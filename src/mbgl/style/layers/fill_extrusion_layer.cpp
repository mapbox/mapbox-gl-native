// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/fill_extrusion_layer.hpp>
#include <mbgl/style/layers/fill_extrusion_layer_impl.hpp>
#include <mbgl/style/conversion/stringify.hpp>

namespace mbgl {
namespace style {

FillExtrusionLayer::FillExtrusionLayer(const std::string& layerID, const std::string& sourceID)
    : Layer(LayerType::FillExtrusion, std::make_unique<Impl>())
    , impl(static_cast<Impl*>(baseImpl.get())) {
    impl->id = layerID;
    impl->source = sourceID;
}

FillExtrusionLayer::FillExtrusionLayer(const Impl& other)
    : Layer(LayerType::FillExtrusion, std::make_unique<Impl>(other))
    , impl(static_cast<Impl*>(baseImpl.get())) {
}

FillExtrusionLayer::~FillExtrusionLayer() = default;

std::unique_ptr<Layer> FillExtrusionLayer::Impl::clone() const {
    return std::make_unique<FillExtrusionLayer>(*this);
}

std::unique_ptr<Layer> FillExtrusionLayer::Impl::cloneRef(const std::string& id_) const {
    auto result = std::make_unique<FillExtrusionLayer>(*this);
    result->impl->id = id_;
    result->impl->cascading = FillExtrusionPaintProperties::Cascading();
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
    return impl->cascading.template get<FillExtrusionOpacity>().get(klass);
}

void FillExtrusionLayer::setFillExtrusionOpacity(PropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getFillExtrusionOpacity(klass))
        return;
    impl->cascading.template get<FillExtrusionOpacity>().set(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

void FillExtrusionLayer::setFillExtrusionOpacityTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    impl->cascading.template get<FillExtrusionOpacity>().setTransition(value, klass);
}

TransitionOptions FillExtrusionLayer::getFillExtrusionOpacityTransition(const optional<std::string>& klass) const {
    return impl->cascading.template get<FillExtrusionOpacity>().getTransition(klass);
}

DataDrivenPropertyValue<Color> FillExtrusionLayer::getDefaultFillExtrusionColor() {
    return { Color::black() };
}

DataDrivenPropertyValue<Color> FillExtrusionLayer::getFillExtrusionColor(const optional<std::string>& klass) const {
    return impl->cascading.template get<FillExtrusionColor>().get(klass);
}

void FillExtrusionLayer::setFillExtrusionColor(DataDrivenPropertyValue<Color> value, const optional<std::string>& klass) {
    if (value == getFillExtrusionColor(klass))
        return;
    impl->cascading.template get<FillExtrusionColor>().set(value, klass);
    if (value.isDataDriven()) {
        impl->observer->onLayerDataDrivenPaintPropertyChanged(*this);
    } else {
        impl->observer->onLayerPaintPropertyChanged(*this);
    }
}

void FillExtrusionLayer::setFillExtrusionColorTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    impl->cascading.template get<FillExtrusionColor>().setTransition(value, klass);
}

TransitionOptions FillExtrusionLayer::getFillExtrusionColorTransition(const optional<std::string>& klass) const {
    return impl->cascading.template get<FillExtrusionColor>().getTransition(klass);
}

PropertyValue<std::array<float, 2>> FillExtrusionLayer::getDefaultFillExtrusionTranslate() {
    return { {{ 0, 0 }} };
}

PropertyValue<std::array<float, 2>> FillExtrusionLayer::getFillExtrusionTranslate(const optional<std::string>& klass) const {
    return impl->cascading.template get<FillExtrusionTranslate>().get(klass);
}

void FillExtrusionLayer::setFillExtrusionTranslate(PropertyValue<std::array<float, 2>> value, const optional<std::string>& klass) {
    if (value == getFillExtrusionTranslate(klass))
        return;
    impl->cascading.template get<FillExtrusionTranslate>().set(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

void FillExtrusionLayer::setFillExtrusionTranslateTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    impl->cascading.template get<FillExtrusionTranslate>().setTransition(value, klass);
}

TransitionOptions FillExtrusionLayer::getFillExtrusionTranslateTransition(const optional<std::string>& klass) const {
    return impl->cascading.template get<FillExtrusionTranslate>().getTransition(klass);
}

PropertyValue<TranslateAnchorType> FillExtrusionLayer::getDefaultFillExtrusionTranslateAnchor() {
    return { TranslateAnchorType::Map };
}

PropertyValue<TranslateAnchorType> FillExtrusionLayer::getFillExtrusionTranslateAnchor(const optional<std::string>& klass) const {
    return impl->cascading.template get<FillExtrusionTranslateAnchor>().get(klass);
}

void FillExtrusionLayer::setFillExtrusionTranslateAnchor(PropertyValue<TranslateAnchorType> value, const optional<std::string>& klass) {
    if (value == getFillExtrusionTranslateAnchor(klass))
        return;
    impl->cascading.template get<FillExtrusionTranslateAnchor>().set(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

void FillExtrusionLayer::setFillExtrusionTranslateAnchorTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    impl->cascading.template get<FillExtrusionTranslateAnchor>().setTransition(value, klass);
}

TransitionOptions FillExtrusionLayer::getFillExtrusionTranslateAnchorTransition(const optional<std::string>& klass) const {
    return impl->cascading.template get<FillExtrusionTranslateAnchor>().getTransition(klass);
}

PropertyValue<std::string> FillExtrusionLayer::getDefaultFillExtrusionPattern() {
    return { "" };
}

PropertyValue<std::string> FillExtrusionLayer::getFillExtrusionPattern(const optional<std::string>& klass) const {
    return impl->cascading.template get<FillExtrusionPattern>().get(klass);
}

void FillExtrusionLayer::setFillExtrusionPattern(PropertyValue<std::string> value, const optional<std::string>& klass) {
    if (value == getFillExtrusionPattern(klass))
        return;
    impl->cascading.template get<FillExtrusionPattern>().set(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

void FillExtrusionLayer::setFillExtrusionPatternTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    impl->cascading.template get<FillExtrusionPattern>().setTransition(value, klass);
}

TransitionOptions FillExtrusionLayer::getFillExtrusionPatternTransition(const optional<std::string>& klass) const {
    return impl->cascading.template get<FillExtrusionPattern>().getTransition(klass);
}

DataDrivenPropertyValue<float> FillExtrusionLayer::getDefaultFillExtrusionHeight() {
    return { 0 };
}

DataDrivenPropertyValue<float> FillExtrusionLayer::getFillExtrusionHeight(const optional<std::string>& klass) const {
    return impl->cascading.template get<FillExtrusionHeight>().get(klass);
}

void FillExtrusionLayer::setFillExtrusionHeight(DataDrivenPropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getFillExtrusionHeight(klass))
        return;
    impl->cascading.template get<FillExtrusionHeight>().set(value, klass);
    if (value.isDataDriven()) {
        impl->observer->onLayerDataDrivenPaintPropertyChanged(*this);
    } else {
        impl->observer->onLayerPaintPropertyChanged(*this);
    }
}

void FillExtrusionLayer::setFillExtrusionHeightTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    impl->cascading.template get<FillExtrusionHeight>().setTransition(value, klass);
}

TransitionOptions FillExtrusionLayer::getFillExtrusionHeightTransition(const optional<std::string>& klass) const {
    return impl->cascading.template get<FillExtrusionHeight>().getTransition(klass);
}

DataDrivenPropertyValue<float> FillExtrusionLayer::getDefaultFillExtrusionBase() {
    return { 0 };
}

DataDrivenPropertyValue<float> FillExtrusionLayer::getFillExtrusionBase(const optional<std::string>& klass) const {
    return impl->cascading.template get<FillExtrusionBase>().get(klass);
}

void FillExtrusionLayer::setFillExtrusionBase(DataDrivenPropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getFillExtrusionBase(klass))
        return;
    impl->cascading.template get<FillExtrusionBase>().set(value, klass);
    if (value.isDataDriven()) {
        impl->observer->onLayerDataDrivenPaintPropertyChanged(*this);
    } else {
        impl->observer->onLayerPaintPropertyChanged(*this);
    }
}

void FillExtrusionLayer::setFillExtrusionBaseTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    impl->cascading.template get<FillExtrusionBase>().setTransition(value, klass);
}

TransitionOptions FillExtrusionLayer::getFillExtrusionBaseTransition(const optional<std::string>& klass) const {
    return impl->cascading.template get<FillExtrusionBase>().getTransition(klass);
}

} // namespace style
} // namespace mbgl
