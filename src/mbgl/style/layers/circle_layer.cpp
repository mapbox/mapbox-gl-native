// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/circle_layer.hpp>
#include <mbgl/style/layers/circle_layer_impl.hpp>
#include <mbgl/style/conversion/stringify.hpp>

namespace mbgl {
namespace style {

CircleLayer::CircleLayer(const std::string& layerID, const std::string& sourceID)
    : Layer(LayerType::Circle, std::make_unique<Impl>())
    , impl(static_cast<Impl*>(baseImpl.get())) {
    impl->id = layerID;
    impl->source = sourceID;
}

CircleLayer::CircleLayer(const Impl& other)
    : Layer(LayerType::Circle, std::make_unique<Impl>(other))
    , impl(static_cast<Impl*>(baseImpl.get())) {
}

CircleLayer::~CircleLayer() = default;

std::unique_ptr<Layer> CircleLayer::Impl::clone() const {
    return std::make_unique<CircleLayer>(*this);
}

std::unique_ptr<Layer> CircleLayer::Impl::cloneRef(const std::string& id_) const {
    auto result = std::make_unique<CircleLayer>(*this);
    result->impl->id = id_;
    result->impl->cascading = CirclePaintProperties::Cascading();
    return std::move(result);
}

void CircleLayer::Impl::stringifyLayout(rapidjson::Writer<rapidjson::StringBuffer>&) const {
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
    impl->observer->onLayerFilterChanged(*this);
}

const Filter& CircleLayer::getFilter() const {
    return impl->filter;
}

// Layout properties


// Paint properties

DataDrivenPropertyValue<float> CircleLayer::getDefaultCircleRadius() {
    return { 5 };
}

DataDrivenPropertyValue<float> CircleLayer::getCircleRadius(const optional<std::string>& klass) const {
    return impl->cascading.template get<CircleRadius>().get(klass);
}

void CircleLayer::setCircleRadius(DataDrivenPropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getCircleRadius(klass))
        return;
    impl->cascading.template get<CircleRadius>().set(value, klass);
    if (value.isDataDriven()) {
        impl->observer->onLayerDataDrivenPaintPropertyChanged(*this);
    } else {
        impl->observer->onLayerPaintPropertyChanged(*this);
    }
}

void CircleLayer::setCircleRadiusTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    impl->cascading.template get<CircleRadius>().setTransition(value, klass);
}

TransitionOptions CircleLayer::getCircleRadiusTransition(const optional<std::string>& klass) const {
    return impl->cascading.template get<CircleRadius>().getTransition(klass);
}

DataDrivenPropertyValue<Color> CircleLayer::getDefaultCircleColor() {
    return { Color::black() };
}

DataDrivenPropertyValue<Color> CircleLayer::getCircleColor(const optional<std::string>& klass) const {
    return impl->cascading.template get<CircleColor>().get(klass);
}

void CircleLayer::setCircleColor(DataDrivenPropertyValue<Color> value, const optional<std::string>& klass) {
    if (value == getCircleColor(klass))
        return;
    impl->cascading.template get<CircleColor>().set(value, klass);
    if (value.isDataDriven()) {
        impl->observer->onLayerDataDrivenPaintPropertyChanged(*this);
    } else {
        impl->observer->onLayerPaintPropertyChanged(*this);
    }
}

void CircleLayer::setCircleColorTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    impl->cascading.template get<CircleColor>().setTransition(value, klass);
}

TransitionOptions CircleLayer::getCircleColorTransition(const optional<std::string>& klass) const {
    return impl->cascading.template get<CircleColor>().getTransition(klass);
}

DataDrivenPropertyValue<float> CircleLayer::getDefaultCircleBlur() {
    return { 0 };
}

DataDrivenPropertyValue<float> CircleLayer::getCircleBlur(const optional<std::string>& klass) const {
    return impl->cascading.template get<CircleBlur>().get(klass);
}

void CircleLayer::setCircleBlur(DataDrivenPropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getCircleBlur(klass))
        return;
    impl->cascading.template get<CircleBlur>().set(value, klass);
    if (value.isDataDriven()) {
        impl->observer->onLayerDataDrivenPaintPropertyChanged(*this);
    } else {
        impl->observer->onLayerPaintPropertyChanged(*this);
    }
}

void CircleLayer::setCircleBlurTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    impl->cascading.template get<CircleBlur>().setTransition(value, klass);
}

TransitionOptions CircleLayer::getCircleBlurTransition(const optional<std::string>& klass) const {
    return impl->cascading.template get<CircleBlur>().getTransition(klass);
}

DataDrivenPropertyValue<float> CircleLayer::getDefaultCircleOpacity() {
    return { 1 };
}

DataDrivenPropertyValue<float> CircleLayer::getCircleOpacity(const optional<std::string>& klass) const {
    return impl->cascading.template get<CircleOpacity>().get(klass);
}

void CircleLayer::setCircleOpacity(DataDrivenPropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getCircleOpacity(klass))
        return;
    impl->cascading.template get<CircleOpacity>().set(value, klass);
    if (value.isDataDriven()) {
        impl->observer->onLayerDataDrivenPaintPropertyChanged(*this);
    } else {
        impl->observer->onLayerPaintPropertyChanged(*this);
    }
}

void CircleLayer::setCircleOpacityTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    impl->cascading.template get<CircleOpacity>().setTransition(value, klass);
}

TransitionOptions CircleLayer::getCircleOpacityTransition(const optional<std::string>& klass) const {
    return impl->cascading.template get<CircleOpacity>().getTransition(klass);
}

PropertyValue<std::array<float, 2>> CircleLayer::getDefaultCircleTranslate() {
    return { {{ 0, 0 }} };
}

PropertyValue<std::array<float, 2>> CircleLayer::getCircleTranslate(const optional<std::string>& klass) const {
    return impl->cascading.template get<CircleTranslate>().get(klass);
}

void CircleLayer::setCircleTranslate(PropertyValue<std::array<float, 2>> value, const optional<std::string>& klass) {
    if (value == getCircleTranslate(klass))
        return;
    impl->cascading.template get<CircleTranslate>().set(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

void CircleLayer::setCircleTranslateTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    impl->cascading.template get<CircleTranslate>().setTransition(value, klass);
}

TransitionOptions CircleLayer::getCircleTranslateTransition(const optional<std::string>& klass) const {
    return impl->cascading.template get<CircleTranslate>().getTransition(klass);
}

PropertyValue<TranslateAnchorType> CircleLayer::getDefaultCircleTranslateAnchor() {
    return { TranslateAnchorType::Map };
}

PropertyValue<TranslateAnchorType> CircleLayer::getCircleTranslateAnchor(const optional<std::string>& klass) const {
    return impl->cascading.template get<CircleTranslateAnchor>().get(klass);
}

void CircleLayer::setCircleTranslateAnchor(PropertyValue<TranslateAnchorType> value, const optional<std::string>& klass) {
    if (value == getCircleTranslateAnchor(klass))
        return;
    impl->cascading.template get<CircleTranslateAnchor>().set(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

void CircleLayer::setCircleTranslateAnchorTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    impl->cascading.template get<CircleTranslateAnchor>().setTransition(value, klass);
}

TransitionOptions CircleLayer::getCircleTranslateAnchorTransition(const optional<std::string>& klass) const {
    return impl->cascading.template get<CircleTranslateAnchor>().getTransition(klass);
}

PropertyValue<CirclePitchScaleType> CircleLayer::getDefaultCirclePitchScale() {
    return { CirclePitchScaleType::Map };
}

PropertyValue<CirclePitchScaleType> CircleLayer::getCirclePitchScale(const optional<std::string>& klass) const {
    return impl->cascading.template get<CirclePitchScale>().get(klass);
}

void CircleLayer::setCirclePitchScale(PropertyValue<CirclePitchScaleType> value, const optional<std::string>& klass) {
    if (value == getCirclePitchScale(klass))
        return;
    impl->cascading.template get<CirclePitchScale>().set(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

void CircleLayer::setCirclePitchScaleTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    impl->cascading.template get<CirclePitchScale>().setTransition(value, klass);
}

TransitionOptions CircleLayer::getCirclePitchScaleTransition(const optional<std::string>& klass) const {
    return impl->cascading.template get<CirclePitchScale>().getTransition(klass);
}

DataDrivenPropertyValue<float> CircleLayer::getDefaultCircleStrokeWidth() {
    return { 0 };
}

DataDrivenPropertyValue<float> CircleLayer::getCircleStrokeWidth(const optional<std::string>& klass) const {
    return impl->cascading.template get<CircleStrokeWidth>().get(klass);
}

void CircleLayer::setCircleStrokeWidth(DataDrivenPropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getCircleStrokeWidth(klass))
        return;
    impl->cascading.template get<CircleStrokeWidth>().set(value, klass);
    if (value.isDataDriven()) {
        impl->observer->onLayerDataDrivenPaintPropertyChanged(*this);
    } else {
        impl->observer->onLayerPaintPropertyChanged(*this);
    }
}

void CircleLayer::setCircleStrokeWidthTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    impl->cascading.template get<CircleStrokeWidth>().setTransition(value, klass);
}

TransitionOptions CircleLayer::getCircleStrokeWidthTransition(const optional<std::string>& klass) const {
    return impl->cascading.template get<CircleStrokeWidth>().getTransition(klass);
}

DataDrivenPropertyValue<Color> CircleLayer::getDefaultCircleStrokeColor() {
    return { Color::black() };
}

DataDrivenPropertyValue<Color> CircleLayer::getCircleStrokeColor(const optional<std::string>& klass) const {
    return impl->cascading.template get<CircleStrokeColor>().get(klass);
}

void CircleLayer::setCircleStrokeColor(DataDrivenPropertyValue<Color> value, const optional<std::string>& klass) {
    if (value == getCircleStrokeColor(klass))
        return;
    impl->cascading.template get<CircleStrokeColor>().set(value, klass);
    if (value.isDataDriven()) {
        impl->observer->onLayerDataDrivenPaintPropertyChanged(*this);
    } else {
        impl->observer->onLayerPaintPropertyChanged(*this);
    }
}

void CircleLayer::setCircleStrokeColorTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    impl->cascading.template get<CircleStrokeColor>().setTransition(value, klass);
}

TransitionOptions CircleLayer::getCircleStrokeColorTransition(const optional<std::string>& klass) const {
    return impl->cascading.template get<CircleStrokeColor>().getTransition(klass);
}

DataDrivenPropertyValue<float> CircleLayer::getDefaultCircleStrokeOpacity() {
    return { 1 };
}

DataDrivenPropertyValue<float> CircleLayer::getCircleStrokeOpacity(const optional<std::string>& klass) const {
    return impl->cascading.template get<CircleStrokeOpacity>().get(klass);
}

void CircleLayer::setCircleStrokeOpacity(DataDrivenPropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getCircleStrokeOpacity(klass))
        return;
    impl->cascading.template get<CircleStrokeOpacity>().set(value, klass);
    if (value.isDataDriven()) {
        impl->observer->onLayerDataDrivenPaintPropertyChanged(*this);
    } else {
        impl->observer->onLayerPaintPropertyChanged(*this);
    }
}

void CircleLayer::setCircleStrokeOpacityTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    impl->cascading.template get<CircleStrokeOpacity>().setTransition(value, klass);
}

TransitionOptions CircleLayer::getCircleStrokeOpacityTransition(const optional<std::string>& klass) const {
    return impl->cascading.template get<CircleStrokeOpacity>().getTransition(klass);
}

} // namespace style
} // namespace mbgl
