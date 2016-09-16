// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/raster_layer.hpp>
#include <mbgl/style/layers/raster_layer_impl.hpp>

namespace mbgl {
namespace style {

RasterLayer::RasterLayer(const std::string& layerID, const std::string& sourceID)
    : Layer(Type::Raster, std::make_unique<Impl>()), impl(static_cast<Impl*>(baseImpl.get())) {
    impl->id = layerID;
    impl->source = sourceID;
}

RasterLayer::RasterLayer(const Impl& other)
    : Layer(Type::Raster, std::make_unique<Impl>(other)), impl(static_cast<Impl*>(baseImpl.get())) {
}

RasterLayer::~RasterLayer() = default;

std::unique_ptr<Layer> RasterLayer::Impl::clone() const {
    return std::make_unique<RasterLayer>(*this);
}

std::unique_ptr<Layer> RasterLayer::Impl::cloneRef(const std::string& id_) const {
    auto result = std::make_unique<RasterLayer>(*this);
    result->impl->id = id_;
    result->impl->ref = this->id;
    result->impl->paint = RasterPaintProperties();
    return std::move(result);
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
    return impl->paint.rasterOpacity.get(klass);
}

void RasterLayer::setRasterOpacity(PropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getRasterOpacity(klass))
        return;
    impl->paint.rasterOpacity.set(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<float> RasterLayer::getDefaultRasterHueRotate() {
    return { 0 };
}

PropertyValue<float> RasterLayer::getRasterHueRotate(const optional<std::string>& klass) const {
    return impl->paint.rasterHueRotate.get(klass);
}

void RasterLayer::setRasterHueRotate(PropertyValue<float> value,
                                     const optional<std::string>& klass) {
    if (value == getRasterHueRotate(klass))
        return;
    impl->paint.rasterHueRotate.set(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<float> RasterLayer::getDefaultRasterBrightnessMin() {
    return { 0 };
}

PropertyValue<float> RasterLayer::getRasterBrightnessMin(const optional<std::string>& klass) const {
    return impl->paint.rasterBrightnessMin.get(klass);
}

void RasterLayer::setRasterBrightnessMin(PropertyValue<float> value,
                                         const optional<std::string>& klass) {
    if (value == getRasterBrightnessMin(klass))
        return;
    impl->paint.rasterBrightnessMin.set(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<float> RasterLayer::getDefaultRasterBrightnessMax() {
    return { 1 };
}

PropertyValue<float> RasterLayer::getRasterBrightnessMax(const optional<std::string>& klass) const {
    return impl->paint.rasterBrightnessMax.get(klass);
}

void RasterLayer::setRasterBrightnessMax(PropertyValue<float> value,
                                         const optional<std::string>& klass) {
    if (value == getRasterBrightnessMax(klass))
        return;
    impl->paint.rasterBrightnessMax.set(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<float> RasterLayer::getDefaultRasterSaturation() {
    return { 0 };
}

PropertyValue<float> RasterLayer::getRasterSaturation(const optional<std::string>& klass) const {
    return impl->paint.rasterSaturation.get(klass);
}

void RasterLayer::setRasterSaturation(PropertyValue<float> value,
                                      const optional<std::string>& klass) {
    if (value == getRasterSaturation(klass))
        return;
    impl->paint.rasterSaturation.set(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<float> RasterLayer::getDefaultRasterContrast() {
    return { 0 };
}

PropertyValue<float> RasterLayer::getRasterContrast(const optional<std::string>& klass) const {
    return impl->paint.rasterContrast.get(klass);
}

void RasterLayer::setRasterContrast(PropertyValue<float> value,
                                    const optional<std::string>& klass) {
    if (value == getRasterContrast(klass))
        return;
    impl->paint.rasterContrast.set(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<float> RasterLayer::getDefaultRasterFadeDuration() {
    return { 300 };
}

PropertyValue<float> RasterLayer::getRasterFadeDuration(const optional<std::string>& klass) const {
    return impl->paint.rasterFadeDuration.get(klass);
}

void RasterLayer::setRasterFadeDuration(PropertyValue<float> value,
                                        const optional<std::string>& klass) {
    if (value == getRasterFadeDuration(klass))
        return;
    impl->paint.rasterFadeDuration.set(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

} // namespace style
} // namespace mbgl
