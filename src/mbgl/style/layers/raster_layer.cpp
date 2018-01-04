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
    impl_->paint = RasterPaintProperties::Transitionable();
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
    observer->onLayerChanged(*this);
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

PropertyValue<float> RasterLayer::getRasterOpacity() const {
    return impl().paint.template get<RasterOpacity>().value;
}

void RasterLayer::setRasterOpacity(PropertyValue<float> value) {
    if (value == getRasterOpacity())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<RasterOpacity>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void RasterLayer::setRasterOpacityTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<RasterOpacity>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions RasterLayer::getRasterOpacityTransition() const {
    return impl().paint.template get<RasterOpacity>().options;
}

PropertyValue<float> RasterLayer::getDefaultRasterHueRotate() {
    return { 0 };
}

PropertyValue<float> RasterLayer::getRasterHueRotate() const {
    return impl().paint.template get<RasterHueRotate>().value;
}

void RasterLayer::setRasterHueRotate(PropertyValue<float> value) {
    if (value == getRasterHueRotate())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<RasterHueRotate>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void RasterLayer::setRasterHueRotateTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<RasterHueRotate>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions RasterLayer::getRasterHueRotateTransition() const {
    return impl().paint.template get<RasterHueRotate>().options;
}

PropertyValue<float> RasterLayer::getDefaultRasterBrightnessMin() {
    return { 0 };
}

PropertyValue<float> RasterLayer::getRasterBrightnessMin() const {
    return impl().paint.template get<RasterBrightnessMin>().value;
}

void RasterLayer::setRasterBrightnessMin(PropertyValue<float> value) {
    if (value == getRasterBrightnessMin())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<RasterBrightnessMin>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void RasterLayer::setRasterBrightnessMinTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<RasterBrightnessMin>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions RasterLayer::getRasterBrightnessMinTransition() const {
    return impl().paint.template get<RasterBrightnessMin>().options;
}

PropertyValue<float> RasterLayer::getDefaultRasterBrightnessMax() {
    return { 1 };
}

PropertyValue<float> RasterLayer::getRasterBrightnessMax() const {
    return impl().paint.template get<RasterBrightnessMax>().value;
}

void RasterLayer::setRasterBrightnessMax(PropertyValue<float> value) {
    if (value == getRasterBrightnessMax())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<RasterBrightnessMax>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void RasterLayer::setRasterBrightnessMaxTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<RasterBrightnessMax>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions RasterLayer::getRasterBrightnessMaxTransition() const {
    return impl().paint.template get<RasterBrightnessMax>().options;
}

PropertyValue<float> RasterLayer::getDefaultRasterSaturation() {
    return { 0 };
}

PropertyValue<float> RasterLayer::getRasterSaturation() const {
    return impl().paint.template get<RasterSaturation>().value;
}

void RasterLayer::setRasterSaturation(PropertyValue<float> value) {
    if (value == getRasterSaturation())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<RasterSaturation>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void RasterLayer::setRasterSaturationTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<RasterSaturation>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions RasterLayer::getRasterSaturationTransition() const {
    return impl().paint.template get<RasterSaturation>().options;
}

PropertyValue<float> RasterLayer::getDefaultRasterContrast() {
    return { 0 };
}

PropertyValue<float> RasterLayer::getRasterContrast() const {
    return impl().paint.template get<RasterContrast>().value;
}

void RasterLayer::setRasterContrast(PropertyValue<float> value) {
    if (value == getRasterContrast())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<RasterContrast>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void RasterLayer::setRasterContrastTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<RasterContrast>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions RasterLayer::getRasterContrastTransition() const {
    return impl().paint.template get<RasterContrast>().options;
}

PropertyValue<float> RasterLayer::getDefaultRasterFadeDuration() {
    return { 300 };
}

PropertyValue<float> RasterLayer::getRasterFadeDuration() const {
    return impl().paint.template get<RasterFadeDuration>().value;
}

void RasterLayer::setRasterFadeDuration(PropertyValue<float> value) {
    if (value == getRasterFadeDuration())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<RasterFadeDuration>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void RasterLayer::setRasterFadeDurationTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<RasterFadeDuration>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions RasterLayer::getRasterFadeDurationTransition() const {
    return impl().paint.template get<RasterFadeDuration>().options;
}

} // namespace style
} // namespace mbgl
