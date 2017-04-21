// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/raster_layer.hpp>
#include <mbgl/style/layers/raster_layer_impl.hpp>
#include <mbgl/style/conversion/stringify.hpp>

namespace mbgl {
namespace style {

RasterLayer::RasterLayer(const std::string& layerID, const std::string& sourceID)
    : Layer(LayerType::Raster, std::make_unique<Impl>())
    , impl(static_cast<Impl*>(baseImpl.get())) {
    impl->id = layerID;
    impl->source = sourceID;
}

RasterLayer::RasterLayer(const Impl& other)
    : Layer(LayerType::Raster, std::make_unique<Impl>(other))
    , impl(static_cast<Impl*>(baseImpl.get())) {
}

RasterLayer::~RasterLayer() = default;

std::unique_ptr<Layer> RasterLayer::Impl::clone() const {
    return std::make_unique<RasterLayer>(*this);
}

std::unique_ptr<Layer> RasterLayer::Impl::cloneRef(const std::string& id_) const {
    auto result = std::make_unique<RasterLayer>(*this);
    result->impl->id = id_;
    result->impl->cascading = RasterPaintProperties::Cascading();
    return std::move(result);
}

void RasterLayer::Impl::stringifyLayout(rapidjson::Writer<rapidjson::StringBuffer>&) const {
}

// Source

const std::string& RasterLayer::getSourceID() const {
    return impl->source;
}


// Layout properties


// Paint properties

PropertyValue<float> RasterLayer::getDefaultRasterOpacity() {
    return { 1 };
}

PropertyValue<float> RasterLayer::getRasterOpacity(const optional<std::string>& klass) const {
    return impl->cascading.template get<RasterOpacity>().get(klass);
}

void RasterLayer::setRasterOpacity(PropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getRasterOpacity(klass))
        return;
    impl->cascading.template get<RasterOpacity>().set(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

void RasterLayer::setRasterOpacityTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    impl->cascading.template get<RasterOpacity>().setTransition(value, klass);
}

TransitionOptions RasterLayer::getRasterOpacityTransition(const optional<std::string>& klass) const {
    return impl->cascading.template get<RasterOpacity>().getTransition(klass);
}

PropertyValue<float> RasterLayer::getDefaultRasterHueRotate() {
    return { 0 };
}

PropertyValue<float> RasterLayer::getRasterHueRotate(const optional<std::string>& klass) const {
    return impl->cascading.template get<RasterHueRotate>().get(klass);
}

void RasterLayer::setRasterHueRotate(PropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getRasterHueRotate(klass))
        return;
    impl->cascading.template get<RasterHueRotate>().set(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

void RasterLayer::setRasterHueRotateTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    impl->cascading.template get<RasterHueRotate>().setTransition(value, klass);
}

TransitionOptions RasterLayer::getRasterHueRotateTransition(const optional<std::string>& klass) const {
    return impl->cascading.template get<RasterHueRotate>().getTransition(klass);
}

PropertyValue<float> RasterLayer::getDefaultRasterBrightnessMin() {
    return { 0 };
}

PropertyValue<float> RasterLayer::getRasterBrightnessMin(const optional<std::string>& klass) const {
    return impl->cascading.template get<RasterBrightnessMin>().get(klass);
}

void RasterLayer::setRasterBrightnessMin(PropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getRasterBrightnessMin(klass))
        return;
    impl->cascading.template get<RasterBrightnessMin>().set(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

void RasterLayer::setRasterBrightnessMinTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    impl->cascading.template get<RasterBrightnessMin>().setTransition(value, klass);
}

TransitionOptions RasterLayer::getRasterBrightnessMinTransition(const optional<std::string>& klass) const {
    return impl->cascading.template get<RasterBrightnessMin>().getTransition(klass);
}

PropertyValue<float> RasterLayer::getDefaultRasterBrightnessMax() {
    return { 1 };
}

PropertyValue<float> RasterLayer::getRasterBrightnessMax(const optional<std::string>& klass) const {
    return impl->cascading.template get<RasterBrightnessMax>().get(klass);
}

void RasterLayer::setRasterBrightnessMax(PropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getRasterBrightnessMax(klass))
        return;
    impl->cascading.template get<RasterBrightnessMax>().set(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

void RasterLayer::setRasterBrightnessMaxTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    impl->cascading.template get<RasterBrightnessMax>().setTransition(value, klass);
}

TransitionOptions RasterLayer::getRasterBrightnessMaxTransition(const optional<std::string>& klass) const {
    return impl->cascading.template get<RasterBrightnessMax>().getTransition(klass);
}

PropertyValue<float> RasterLayer::getDefaultRasterSaturation() {
    return { 0 };
}

PropertyValue<float> RasterLayer::getRasterSaturation(const optional<std::string>& klass) const {
    return impl->cascading.template get<RasterSaturation>().get(klass);
}

void RasterLayer::setRasterSaturation(PropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getRasterSaturation(klass))
        return;
    impl->cascading.template get<RasterSaturation>().set(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

void RasterLayer::setRasterSaturationTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    impl->cascading.template get<RasterSaturation>().setTransition(value, klass);
}

TransitionOptions RasterLayer::getRasterSaturationTransition(const optional<std::string>& klass) const {
    return impl->cascading.template get<RasterSaturation>().getTransition(klass);
}

PropertyValue<float> RasterLayer::getDefaultRasterContrast() {
    return { 0 };
}

PropertyValue<float> RasterLayer::getRasterContrast(const optional<std::string>& klass) const {
    return impl->cascading.template get<RasterContrast>().get(klass);
}

void RasterLayer::setRasterContrast(PropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getRasterContrast(klass))
        return;
    impl->cascading.template get<RasterContrast>().set(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

void RasterLayer::setRasterContrastTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    impl->cascading.template get<RasterContrast>().setTransition(value, klass);
}

TransitionOptions RasterLayer::getRasterContrastTransition(const optional<std::string>& klass) const {
    return impl->cascading.template get<RasterContrast>().getTransition(klass);
}

PropertyValue<float> RasterLayer::getDefaultRasterFadeDuration() {
    return { 300 };
}

PropertyValue<float> RasterLayer::getRasterFadeDuration(const optional<std::string>& klass) const {
    return impl->cascading.template get<RasterFadeDuration>().get(klass);
}

void RasterLayer::setRasterFadeDuration(PropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getRasterFadeDuration(klass))
        return;
    impl->cascading.template get<RasterFadeDuration>().set(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

void RasterLayer::setRasterFadeDurationTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    impl->cascading.template get<RasterFadeDuration>().setTransition(value, klass);
}

TransitionOptions RasterLayer::getRasterFadeDurationTransition(const optional<std::string>& klass) const {
    return impl->cascading.template get<RasterFadeDuration>().getTransition(klass);
}

} // namespace style
} // namespace mbgl
