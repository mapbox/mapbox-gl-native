// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/heatmap_layer.hpp>
#include <mbgl/style/layers/heatmap_layer_impl.hpp>
#include <mbgl/style/layer_observer.hpp>
// for constructing default heatmap-color ramp expression from style JSON
#include <mbgl/style/conversion.hpp>
#include <mbgl/style/conversion/json.hpp>
#include <mbgl/style/conversion/color_ramp_property_value.hpp>

namespace mbgl {
namespace style {

HeatmapLayer::HeatmapLayer(const std::string& layerID, const std::string& sourceID)
    : Layer(makeMutable<Impl>(LayerType::Heatmap, layerID, sourceID)) {
}

HeatmapLayer::HeatmapLayer(Immutable<Impl> impl_)
    : Layer(std::move(impl_)) {
}

HeatmapLayer::~HeatmapLayer() = default;

const HeatmapLayer::Impl& HeatmapLayer::impl() const {
    return static_cast<const Impl&>(*baseImpl);
}

Mutable<HeatmapLayer::Impl> HeatmapLayer::mutableImpl() const {
    return makeMutable<Impl>(impl());
}

std::unique_ptr<Layer> HeatmapLayer::cloneRef(const std::string& id_) const {
    auto impl_ = mutableImpl();
    impl_->id = id_;
    impl_->paint = HeatmapPaintProperties::Transitionable();
    return std::make_unique<HeatmapLayer>(std::move(impl_));
}

void HeatmapLayer::Impl::stringifyLayout(rapidjson::Writer<rapidjson::StringBuffer>&) const {
}

// Source

const std::string& HeatmapLayer::getSourceID() const {
    return impl().source;
}

void HeatmapLayer::setSourceLayer(const std::string& sourceLayer) {
    auto impl_ = mutableImpl();
    impl_->sourceLayer = sourceLayer;
    baseImpl = std::move(impl_);
}

const std::string& HeatmapLayer::getSourceLayer() const {
    return impl().sourceLayer;
}

// Filter

void HeatmapLayer::setFilter(const Filter& filter) {
    auto impl_ = mutableImpl();
    impl_->filter = filter;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

const Filter& HeatmapLayer::getFilter() const {
    return impl().filter;
}

// Visibility

void HeatmapLayer::setVisibility(VisibilityType value) {
    if (value == getVisibility())
        return;
    auto impl_ = mutableImpl();
    impl_->visibility = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

// Zoom range

void HeatmapLayer::setMinZoom(float minZoom) {
    auto impl_ = mutableImpl();
    impl_->minZoom = minZoom;
    baseImpl = std::move(impl_);
}

void HeatmapLayer::setMaxZoom(float maxZoom) {
    auto impl_ = mutableImpl();
    impl_->maxZoom = maxZoom;
    baseImpl = std::move(impl_);
}

// Layout properties


// Paint properties

DataDrivenPropertyValue<float> HeatmapLayer::getDefaultHeatmapRadius() {
    return { 30 };
}

DataDrivenPropertyValue<float> HeatmapLayer::getHeatmapRadius() const {
    return impl().paint.template get<HeatmapRadius>().value;
}

void HeatmapLayer::setHeatmapRadius(DataDrivenPropertyValue<float> value) {
    if (value == getHeatmapRadius())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<HeatmapRadius>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void HeatmapLayer::setHeatmapRadiusTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<HeatmapRadius>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions HeatmapLayer::getHeatmapRadiusTransition() const {
    return impl().paint.template get<HeatmapRadius>().options;
}

DataDrivenPropertyValue<float> HeatmapLayer::getDefaultHeatmapWeight() {
    return { 1 };
}

DataDrivenPropertyValue<float> HeatmapLayer::getHeatmapWeight() const {
    return impl().paint.template get<HeatmapWeight>().value;
}

void HeatmapLayer::setHeatmapWeight(DataDrivenPropertyValue<float> value) {
    if (value == getHeatmapWeight())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<HeatmapWeight>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void HeatmapLayer::setHeatmapWeightTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<HeatmapWeight>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions HeatmapLayer::getHeatmapWeightTransition() const {
    return impl().paint.template get<HeatmapWeight>().options;
}

PropertyValue<float> HeatmapLayer::getDefaultHeatmapIntensity() {
    return { 1 };
}

PropertyValue<float> HeatmapLayer::getHeatmapIntensity() const {
    return impl().paint.template get<HeatmapIntensity>().value;
}

void HeatmapLayer::setHeatmapIntensity(PropertyValue<float> value) {
    if (value == getHeatmapIntensity())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<HeatmapIntensity>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void HeatmapLayer::setHeatmapIntensityTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<HeatmapIntensity>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions HeatmapLayer::getHeatmapIntensityTransition() const {
    return impl().paint.template get<HeatmapIntensity>().options;
}

ColorRampPropertyValue HeatmapLayer::getDefaultHeatmapColor() {
    conversion::Error error;
    std::string rawValue = R"JSON(["interpolate",["linear"],["heatmap-density"],0,"rgba(0, 0, 255, 0)",0.1,"royalblue",0.3,"cyan",0.5,"lime",0.7,"yellow",1,"red"])JSON";
    return *conversion::convertJSON<ColorRampPropertyValue>(rawValue, error);
}

ColorRampPropertyValue HeatmapLayer::getHeatmapColor() const {
    return impl().paint.template get<HeatmapColor>().value;
}

void HeatmapLayer::setHeatmapColor(ColorRampPropertyValue value) {
    if (value == getHeatmapColor())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<HeatmapColor>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void HeatmapLayer::setHeatmapColorTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<HeatmapColor>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions HeatmapLayer::getHeatmapColorTransition() const {
    return impl().paint.template get<HeatmapColor>().options;
}

PropertyValue<float> HeatmapLayer::getDefaultHeatmapOpacity() {
    return { 1 };
}

PropertyValue<float> HeatmapLayer::getHeatmapOpacity() const {
    return impl().paint.template get<HeatmapOpacity>().value;
}

void HeatmapLayer::setHeatmapOpacity(PropertyValue<float> value) {
    if (value == getHeatmapOpacity())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<HeatmapOpacity>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void HeatmapLayer::setHeatmapOpacityTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<HeatmapOpacity>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions HeatmapLayer::getHeatmapOpacityTransition() const {
    return impl().paint.template get<HeatmapOpacity>().options;
}

} // namespace style
} // namespace mbgl
