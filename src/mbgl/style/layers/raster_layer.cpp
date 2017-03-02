// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/raster_layer.hpp>
#include <mbgl/style/layers/raster_layer_impl.hpp>
#include <mbgl/style/conversion/stringify.hpp>

namespace mbgl {
namespace style {

RasterLayer::RasterLayer(const std::string& layerID, const std::string& sourceID)
    : Layer(Type::Raster, std::make_unique<Impl>())
    , impl(static_cast<Impl*>(baseImpl.get())) {
    impl->id = layerID;
    impl->source = sourceID;
}

RasterLayer::RasterLayer(const Impl& other)
    : Layer(Type::Raster, std::make_unique<Impl>(other))
    , impl(static_cast<Impl*>(baseImpl.get())) {
}

RasterLayer::~RasterLayer() = default;

std::unique_ptr<Layer> RasterLayer::Impl::clone() const {
    return std::make_unique<RasterLayer>(*this);
}

std::unique_ptr<Layer> RasterLayer::Impl::cloneRef(const std::string& id_) const {
    auto result = std::make_unique<RasterLayer>(*this);
    result->impl->id = id_;
    result->impl->paint = RasterPaintProperties();
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
    return impl->paint.get<RasterOpacity>(klass);
}

void RasterLayer::setRasterOpacity(PropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getRasterOpacity(klass))
        return;
    impl->paint.set<RasterOpacity>(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

void RasterLayer::setRasterOpacityTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    impl->paint.setTransition<RasterOpacity>(value, klass);
}

style::TransitionOptions RasterLayer::getRasterOpacityTransition(const optional<std::string>& klass) const {
    return impl->paint.getTransition<RasterOpacity>(klass);
}

PropertyValue<float> RasterLayer::getDefaultRasterHueRotate() {
    return { 0 };
}

PropertyValue<float> RasterLayer::getRasterHueRotate(const optional<std::string>& klass) const {
    return impl->paint.get<RasterHueRotate>(klass);
}

void RasterLayer::setRasterHueRotate(PropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getRasterHueRotate(klass))
        return;
    impl->paint.set<RasterHueRotate>(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

void RasterLayer::setRasterHueRotateTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    impl->paint.setTransition<RasterHueRotate>(value, klass);
}

style::TransitionOptions RasterLayer::getRasterHueRotateTransition(const optional<std::string>& klass) const {
    return impl->paint.getTransition<RasterHueRotate>(klass);
}

PropertyValue<float> RasterLayer::getDefaultRasterBrightnessMin() {
    return { 0 };
}

PropertyValue<float> RasterLayer::getRasterBrightnessMin(const optional<std::string>& klass) const {
    return impl->paint.get<RasterBrightnessMin>(klass);
}

void RasterLayer::setRasterBrightnessMin(PropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getRasterBrightnessMin(klass))
        return;
    impl->paint.set<RasterBrightnessMin>(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

void RasterLayer::setRasterBrightnessMinTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    impl->paint.setTransition<RasterBrightnessMin>(value, klass);
}

style::TransitionOptions RasterLayer::getRasterBrightnessMinTransition(const optional<std::string>& klass) const {
    return impl->paint.getTransition<RasterBrightnessMin>(klass);
}

PropertyValue<float> RasterLayer::getDefaultRasterBrightnessMax() {
    return { 1 };
}

PropertyValue<float> RasterLayer::getRasterBrightnessMax(const optional<std::string>& klass) const {
    return impl->paint.get<RasterBrightnessMax>(klass);
}

void RasterLayer::setRasterBrightnessMax(PropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getRasterBrightnessMax(klass))
        return;
    impl->paint.set<RasterBrightnessMax>(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

void RasterLayer::setRasterBrightnessMaxTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    impl->paint.setTransition<RasterBrightnessMax>(value, klass);
}

style::TransitionOptions RasterLayer::getRasterBrightnessMaxTransition(const optional<std::string>& klass) const {
    return impl->paint.getTransition<RasterBrightnessMax>(klass);
}

PropertyValue<float> RasterLayer::getDefaultRasterSaturation() {
    return { 0 };
}

PropertyValue<float> RasterLayer::getRasterSaturation(const optional<std::string>& klass) const {
    return impl->paint.get<RasterSaturation>(klass);
}

void RasterLayer::setRasterSaturation(PropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getRasterSaturation(klass))
        return;
    impl->paint.set<RasterSaturation>(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

void RasterLayer::setRasterSaturationTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    impl->paint.setTransition<RasterSaturation>(value, klass);
}

style::TransitionOptions RasterLayer::getRasterSaturationTransition(const optional<std::string>& klass) const {
    return impl->paint.getTransition<RasterSaturation>(klass);
}

PropertyValue<float> RasterLayer::getDefaultRasterContrast() {
    return { 0 };
}

PropertyValue<float> RasterLayer::getRasterContrast(const optional<std::string>& klass) const {
    return impl->paint.get<RasterContrast>(klass);
}

void RasterLayer::setRasterContrast(PropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getRasterContrast(klass))
        return;
    impl->paint.set<RasterContrast>(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

void RasterLayer::setRasterContrastTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    impl->paint.setTransition<RasterContrast>(value, klass);
}

style::TransitionOptions RasterLayer::getRasterContrastTransition(const optional<std::string>& klass) const {
    return impl->paint.getTransition<RasterContrast>(klass);
}

PropertyValue<float> RasterLayer::getDefaultRasterFadeDuration() {
    return { 300 };
}

PropertyValue<float> RasterLayer::getRasterFadeDuration(const optional<std::string>& klass) const {
    return impl->paint.get<RasterFadeDuration>(klass);
}

void RasterLayer::setRasterFadeDuration(PropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getRasterFadeDuration(klass))
        return;
    impl->paint.set<RasterFadeDuration>(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

void RasterLayer::setRasterFadeDurationTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    impl->paint.setTransition<RasterFadeDuration>(value, klass);
}

style::TransitionOptions RasterLayer::getRasterFadeDurationTransition(const optional<std::string>& klass) const {
    return impl->paint.getTransition<RasterFadeDuration>(klass);
}

} // namespace style
} // namespace mbgl
