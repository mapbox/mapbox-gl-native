// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/layer/raster_layer.hpp>
#include <mbgl/layer/raster_layer_impl.hpp>

namespace mbgl {

RasterLayer::RasterLayer(const std::string& layerID)
    : Layer(Type::Raster, std::make_unique<Impl>())
    , impl(static_cast<Impl*>(baseImpl.get())) {
    impl->id = layerID;
}

RasterLayer::RasterLayer(const Impl& other)
    : Layer(Type::Raster, std::make_unique<Impl>(other))
    , impl(static_cast<Impl*>(baseImpl.get())) {
}

RasterLayer::~RasterLayer() = default;

std::unique_ptr<Layer> RasterLayer::Impl::clone() const {
    return std::make_unique<RasterLayer>(*this);
}

// Source

void RasterLayer::setSource(const std::string& sourceID) {
    impl->source = sourceID;
}

const std::string& RasterLayer::getSourceID() const {
    return impl->source;
}

// Layout properties


// Paint properties

PropertyValue<float> RasterLayer::getRasterOpacity() const {
    return impl->paint.rasterOpacity.get();
}

void RasterLayer::setRasterOpacity(PropertyValue<float> value) {
    impl->paint.rasterOpacity.set(value);
}

PropertyValue<float> RasterLayer::getRasterHueRotate() const {
    return impl->paint.rasterHueRotate.get();
}

void RasterLayer::setRasterHueRotate(PropertyValue<float> value) {
    impl->paint.rasterHueRotate.set(value);
}

PropertyValue<float> RasterLayer::getRasterBrightnessMin() const {
    return impl->paint.rasterBrightnessMin.get();
}

void RasterLayer::setRasterBrightnessMin(PropertyValue<float> value) {
    impl->paint.rasterBrightnessMin.set(value);
}

PropertyValue<float> RasterLayer::getRasterBrightnessMax() const {
    return impl->paint.rasterBrightnessMax.get();
}

void RasterLayer::setRasterBrightnessMax(PropertyValue<float> value) {
    impl->paint.rasterBrightnessMax.set(value);
}

PropertyValue<float> RasterLayer::getRasterSaturation() const {
    return impl->paint.rasterSaturation.get();
}

void RasterLayer::setRasterSaturation(PropertyValue<float> value) {
    impl->paint.rasterSaturation.set(value);
}

PropertyValue<float> RasterLayer::getRasterContrast() const {
    return impl->paint.rasterContrast.get();
}

void RasterLayer::setRasterContrast(PropertyValue<float> value) {
    impl->paint.rasterContrast.set(value);
}

PropertyValue<float> RasterLayer::getRasterFadeDuration() const {
    return impl->paint.rasterFadeDuration.get();
}

void RasterLayer::setRasterFadeDuration(PropertyValue<float> value) {
    impl->paint.rasterFadeDuration.set(value);
}

} // namespace mbgl
