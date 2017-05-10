// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/raster_layer.hpp>
#include <mbgl/style/layers/raster_layer_impl.hpp>
#include <mbgl/style/layer_observer.hpp>

namespace mbgl {
namespace style {

RasterLayer::RasterLayer(const std::string& layerID, const std::string& sourceID)
    : Layer(makeMutable<Impl>(LayerType::Raster, layerID, sourceID)) {
}

RasterLayer::RasterLayer(Immutable<Impl> impl_)
    : Layer(std::move(impl_)) {
}

RasterLayer::~RasterLayer() = default;

const RasterLayer::Impl& RasterLayer::impl() const {
    return static_cast<const Impl&>(*baseImpl);
}

Mutable<RasterLayer::Impl> RasterLayer::mutableImpl() const {
    return makeMutable<Impl>(impl());
}

std::unique_ptr<Layer> RasterLayer::cloneRef(const std::string& id_) const {
    auto impl_ = mutableImpl();
    impl_->id = id_;
    impl_->paint = RasterPaintProperties::Cascading();
    return std::make_unique<RasterLayer>(std::move(impl_));
}

void RasterLayer::Impl::stringifyLayout(rapidjson::Writer<rapidjson::StringBuffer>&) const {
}

// Source

const std::string& RasterLayer::getSourceID() const {
    return impl().source;
}


// Visibility

void RasterLayer::setVisibility(VisibilityType value) {
    if (value == getVisibility())
        return;
    auto impl_ = mutableImpl();
    impl_->visibility = value;
    baseImpl = std::move(impl_);
    observer->onLayerVisibilityChanged(*this);
}

// Zoom range

void RasterLayer::setMinZoom(float minZoom) {
    auto impl_ = mutableImpl();
    impl_->minZoom = minZoom;
    baseImpl = std::move(impl_);
}

void RasterLayer::setMaxZoom(float maxZoom) {
    auto impl_ = mutableImpl();
    impl_->maxZoom = maxZoom;
    baseImpl = std::move(impl_);
}

// Layout properties


// Paint properties

PropertyValue<float> RasterLayer::getDefaultRasterOpacity() {
    return { 1 };
}

PropertyValue<float> RasterLayer::getRasterOpacity(const optional<std::string>& klass) const {
    return impl().paint.template get<RasterOpacity>().get(klass);
}

void RasterLayer::setRasterOpacity(PropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getRasterOpacity(klass))
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<RasterOpacity>().set(value, klass);
    baseImpl = std::move(impl_);
    observer->onLayerPaintPropertyChanged(*this);
}

void RasterLayer::setRasterOpacityTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<RasterOpacity>().setTransition(value, klass);
    baseImpl = std::move(impl_);
}

TransitionOptions RasterLayer::getRasterOpacityTransition(const optional<std::string>& klass) const {
    return impl().paint.template get<RasterOpacity>().getTransition(klass);
}

PropertyValue<float> RasterLayer::getDefaultRasterHueRotate() {
    return { 0 };
}

PropertyValue<float> RasterLayer::getRasterHueRotate(const optional<std::string>& klass) const {
    return impl().paint.template get<RasterHueRotate>().get(klass);
}

void RasterLayer::setRasterHueRotate(PropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getRasterHueRotate(klass))
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<RasterHueRotate>().set(value, klass);
    baseImpl = std::move(impl_);
    observer->onLayerPaintPropertyChanged(*this);
}

void RasterLayer::setRasterHueRotateTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<RasterHueRotate>().setTransition(value, klass);
    baseImpl = std::move(impl_);
}

TransitionOptions RasterLayer::getRasterHueRotateTransition(const optional<std::string>& klass) const {
    return impl().paint.template get<RasterHueRotate>().getTransition(klass);
}

PropertyValue<float> RasterLayer::getDefaultRasterBrightnessMin() {
    return { 0 };
}

PropertyValue<float> RasterLayer::getRasterBrightnessMin(const optional<std::string>& klass) const {
    return impl().paint.template get<RasterBrightnessMin>().get(klass);
}

void RasterLayer::setRasterBrightnessMin(PropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getRasterBrightnessMin(klass))
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<RasterBrightnessMin>().set(value, klass);
    baseImpl = std::move(impl_);
    observer->onLayerPaintPropertyChanged(*this);
}

void RasterLayer::setRasterBrightnessMinTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<RasterBrightnessMin>().setTransition(value, klass);
    baseImpl = std::move(impl_);
}

TransitionOptions RasterLayer::getRasterBrightnessMinTransition(const optional<std::string>& klass) const {
    return impl().paint.template get<RasterBrightnessMin>().getTransition(klass);
}

PropertyValue<float> RasterLayer::getDefaultRasterBrightnessMax() {
    return { 1 };
}

PropertyValue<float> RasterLayer::getRasterBrightnessMax(const optional<std::string>& klass) const {
    return impl().paint.template get<RasterBrightnessMax>().get(klass);
}

void RasterLayer::setRasterBrightnessMax(PropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getRasterBrightnessMax(klass))
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<RasterBrightnessMax>().set(value, klass);
    baseImpl = std::move(impl_);
    observer->onLayerPaintPropertyChanged(*this);
}

void RasterLayer::setRasterBrightnessMaxTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<RasterBrightnessMax>().setTransition(value, klass);
    baseImpl = std::move(impl_);
}

TransitionOptions RasterLayer::getRasterBrightnessMaxTransition(const optional<std::string>& klass) const {
    return impl().paint.template get<RasterBrightnessMax>().getTransition(klass);
}

PropertyValue<float> RasterLayer::getDefaultRasterSaturation() {
    return { 0 };
}

PropertyValue<float> RasterLayer::getRasterSaturation(const optional<std::string>& klass) const {
    return impl().paint.template get<RasterSaturation>().get(klass);
}

void RasterLayer::setRasterSaturation(PropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getRasterSaturation(klass))
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<RasterSaturation>().set(value, klass);
    baseImpl = std::move(impl_);
    observer->onLayerPaintPropertyChanged(*this);
}

void RasterLayer::setRasterSaturationTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<RasterSaturation>().setTransition(value, klass);
    baseImpl = std::move(impl_);
}

TransitionOptions RasterLayer::getRasterSaturationTransition(const optional<std::string>& klass) const {
    return impl().paint.template get<RasterSaturation>().getTransition(klass);
}

PropertyValue<float> RasterLayer::getDefaultRasterContrast() {
    return { 0 };
}

PropertyValue<float> RasterLayer::getRasterContrast(const optional<std::string>& klass) const {
    return impl().paint.template get<RasterContrast>().get(klass);
}

void RasterLayer::setRasterContrast(PropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getRasterContrast(klass))
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<RasterContrast>().set(value, klass);
    baseImpl = std::move(impl_);
    observer->onLayerPaintPropertyChanged(*this);
}

void RasterLayer::setRasterContrastTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<RasterContrast>().setTransition(value, klass);
    baseImpl = std::move(impl_);
}

TransitionOptions RasterLayer::getRasterContrastTransition(const optional<std::string>& klass) const {
    return impl().paint.template get<RasterContrast>().getTransition(klass);
}

PropertyValue<float> RasterLayer::getDefaultRasterFadeDuration() {
    return { 300 };
}

PropertyValue<float> RasterLayer::getRasterFadeDuration(const optional<std::string>& klass) const {
    return impl().paint.template get<RasterFadeDuration>().get(klass);
}

void RasterLayer::setRasterFadeDuration(PropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getRasterFadeDuration(klass))
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<RasterFadeDuration>().set(value, klass);
    baseImpl = std::move(impl_);
    observer->onLayerPaintPropertyChanged(*this);
}

void RasterLayer::setRasterFadeDurationTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<RasterFadeDuration>().setTransition(value, klass);
    baseImpl = std::move(impl_);
}

TransitionOptions RasterLayer::getRasterFadeDurationTransition(const optional<std::string>& klass) const {
    return impl().paint.template get<RasterFadeDuration>().getTransition(klass);
}

} // namespace style
} // namespace mbgl
