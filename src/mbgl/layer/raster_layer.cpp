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

Function<float> RasterLayer::getRasterOpacity() const {
    return impl->paint.rasterOpacity.values.at(ClassID::Default);
}

void RasterLayer::setRasterOpacity(Function<float> value) {
    impl->paint.rasterOpacity.values.emplace(ClassID::Default, value);
}

Function<float> RasterLayer::getRasterHueRotate() const {
    return impl->paint.rasterHueRotate.values.at(ClassID::Default);
}

void RasterLayer::setRasterHueRotate(Function<float> value) {
    impl->paint.rasterHueRotate.values.emplace(ClassID::Default, value);
}

Function<float> RasterLayer::getRasterBrightnessMin() const {
    return impl->paint.rasterBrightnessMin.values.at(ClassID::Default);
}

void RasterLayer::setRasterBrightnessMin(Function<float> value) {
    impl->paint.rasterBrightnessMin.values.emplace(ClassID::Default, value);
}

Function<float> RasterLayer::getRasterBrightnessMax() const {
    return impl->paint.rasterBrightnessMax.values.at(ClassID::Default);
}

void RasterLayer::setRasterBrightnessMax(Function<float> value) {
    impl->paint.rasterBrightnessMax.values.emplace(ClassID::Default, value);
}

Function<float> RasterLayer::getRasterSaturation() const {
    return impl->paint.rasterSaturation.values.at(ClassID::Default);
}

void RasterLayer::setRasterSaturation(Function<float> value) {
    impl->paint.rasterSaturation.values.emplace(ClassID::Default, value);
}

Function<float> RasterLayer::getRasterContrast() const {
    return impl->paint.rasterContrast.values.at(ClassID::Default);
}

void RasterLayer::setRasterContrast(Function<float> value) {
    impl->paint.rasterContrast.values.emplace(ClassID::Default, value);
}

Function<float> RasterLayer::getRasterFadeDuration() const {
    return impl->paint.rasterFadeDuration.values.at(ClassID::Default);
}

void RasterLayer::setRasterFadeDuration(Function<float> value) {
    impl->paint.rasterFadeDuration.values.emplace(ClassID::Default, value);
}

} // namespace mbgl
