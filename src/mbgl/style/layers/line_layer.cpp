// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/line_layer.hpp>
#include <mbgl/style/layers/line_layer_impl.hpp>
#include <mbgl/style/layer_observer.hpp>

namespace mbgl {
namespace style {

LineLayer::LineLayer(const std::string& layerID, const std::string& sourceID)
    : Layer(makeMutable<Impl>(LayerType::Line, layerID, sourceID)) {
}

LineLayer::LineLayer(Immutable<Impl> impl_)
    : Layer(std::move(impl_)) {
}

LineLayer::~LineLayer() = default;

const LineLayer::Impl& LineLayer::impl() const {
    return static_cast<const Impl&>(*baseImpl);
}

Mutable<LineLayer::Impl> LineLayer::mutableImpl() const {
    return makeMutable<Impl>(impl());
}

std::unique_ptr<Layer> LineLayer::cloneRef(const std::string& id_) const {
    auto impl_ = mutableImpl();
    impl_->id = id_;
    impl_->paint = LinePaintProperties::Transitionable();
    return std::make_unique<LineLayer>(std::move(impl_));
}

void LineLayer::Impl::stringifyLayout(rapidjson::Writer<rapidjson::StringBuffer>& writer) const {
    layout.stringify(writer);
}

// Source

const std::string& LineLayer::getSourceID() const {
    return impl().source;
}

void LineLayer::setSourceLayer(const std::string& sourceLayer) {
    auto impl_ = mutableImpl();
    impl_->sourceLayer = sourceLayer;
    baseImpl = std::move(impl_);
}

const std::string& LineLayer::getSourceLayer() const {
    return impl().sourceLayer;
}

// Filter

void LineLayer::setFilter(const Filter& filter) {
    auto impl_ = mutableImpl();
    impl_->filter = filter;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

const Filter& LineLayer::getFilter() const {
    return impl().filter;
}

// Visibility

void LineLayer::setVisibility(VisibilityType value) {
    if (value == getVisibility())
        return;
    auto impl_ = mutableImpl();
    impl_->visibility = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

// Zoom range

void LineLayer::setMinZoom(float minZoom) {
    auto impl_ = mutableImpl();
    impl_->minZoom = minZoom;
    baseImpl = std::move(impl_);
}

void LineLayer::setMaxZoom(float maxZoom) {
    auto impl_ = mutableImpl();
    impl_->maxZoom = maxZoom;
    baseImpl = std::move(impl_);
}

// Layout properties

PropertyValue<LineCapType> LineLayer::getDefaultLineCap() {
    return LineCap::defaultValue();
}

PropertyValue<LineCapType> LineLayer::getLineCap() const {
    return impl().layout.get<LineCap>();
}

void LineLayer::setLineCap(PropertyValue<LineCapType> value) {
    if (value == getLineCap())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.get<LineCap>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
DataDrivenPropertyValue<LineJoinType> LineLayer::getDefaultLineJoin() {
    return LineJoin::defaultValue();
}

DataDrivenPropertyValue<LineJoinType> LineLayer::getLineJoin() const {
    return impl().layout.get<LineJoin>();
}

void LineLayer::setLineJoin(DataDrivenPropertyValue<LineJoinType> value) {
    if (value == getLineJoin())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.get<LineJoin>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
PropertyValue<float> LineLayer::getDefaultLineMiterLimit() {
    return LineMiterLimit::defaultValue();
}

PropertyValue<float> LineLayer::getLineMiterLimit() const {
    return impl().layout.get<LineMiterLimit>();
}

void LineLayer::setLineMiterLimit(PropertyValue<float> value) {
    if (value == getLineMiterLimit())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.get<LineMiterLimit>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
PropertyValue<float> LineLayer::getDefaultLineRoundLimit() {
    return LineRoundLimit::defaultValue();
}

PropertyValue<float> LineLayer::getLineRoundLimit() const {
    return impl().layout.get<LineRoundLimit>();
}

void LineLayer::setLineRoundLimit(PropertyValue<float> value) {
    if (value == getLineRoundLimit())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.get<LineRoundLimit>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

// Paint properties

DataDrivenPropertyValue<float> LineLayer::getDefaultLineOpacity() {
    return { 1 };
}

DataDrivenPropertyValue<float> LineLayer::getLineOpacity() const {
    return impl().paint.template get<LineOpacity>().value;
}

void LineLayer::setLineOpacity(DataDrivenPropertyValue<float> value) {
    if (value == getLineOpacity())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<LineOpacity>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void LineLayer::setLineOpacityTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<LineOpacity>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions LineLayer::getLineOpacityTransition() const {
    return impl().paint.template get<LineOpacity>().options;
}

DataDrivenPropertyValue<Color> LineLayer::getDefaultLineColor() {
    return { Color::black() };
}

DataDrivenPropertyValue<Color> LineLayer::getLineColor() const {
    return impl().paint.template get<LineColor>().value;
}

void LineLayer::setLineColor(DataDrivenPropertyValue<Color> value) {
    if (value == getLineColor())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<LineColor>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void LineLayer::setLineColorTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<LineColor>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions LineLayer::getLineColorTransition() const {
    return impl().paint.template get<LineColor>().options;
}

PropertyValue<std::array<float, 2>> LineLayer::getDefaultLineTranslate() {
    return { {{ 0, 0 }} };
}

PropertyValue<std::array<float, 2>> LineLayer::getLineTranslate() const {
    return impl().paint.template get<LineTranslate>().value;
}

void LineLayer::setLineTranslate(PropertyValue<std::array<float, 2>> value) {
    if (value == getLineTranslate())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<LineTranslate>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void LineLayer::setLineTranslateTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<LineTranslate>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions LineLayer::getLineTranslateTransition() const {
    return impl().paint.template get<LineTranslate>().options;
}

PropertyValue<TranslateAnchorType> LineLayer::getDefaultLineTranslateAnchor() {
    return { TranslateAnchorType::Map };
}

PropertyValue<TranslateAnchorType> LineLayer::getLineTranslateAnchor() const {
    return impl().paint.template get<LineTranslateAnchor>().value;
}

void LineLayer::setLineTranslateAnchor(PropertyValue<TranslateAnchorType> value) {
    if (value == getLineTranslateAnchor())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<LineTranslateAnchor>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void LineLayer::setLineTranslateAnchorTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<LineTranslateAnchor>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions LineLayer::getLineTranslateAnchorTransition() const {
    return impl().paint.template get<LineTranslateAnchor>().options;
}

DataDrivenPropertyValue<float> LineLayer::getDefaultLineWidth() {
    return { 1 };
}

DataDrivenPropertyValue<float> LineLayer::getLineWidth() const {
    return impl().paint.template get<LineWidth>().value;
}

void LineLayer::setLineWidth(DataDrivenPropertyValue<float> value) {
    if (value == getLineWidth())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<LineWidth>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void LineLayer::setLineWidthTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<LineWidth>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions LineLayer::getLineWidthTransition() const {
    return impl().paint.template get<LineWidth>().options;
}

DataDrivenPropertyValue<float> LineLayer::getDefaultLineGapWidth() {
    return { 0 };
}

DataDrivenPropertyValue<float> LineLayer::getLineGapWidth() const {
    return impl().paint.template get<LineGapWidth>().value;
}

void LineLayer::setLineGapWidth(DataDrivenPropertyValue<float> value) {
    if (value == getLineGapWidth())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<LineGapWidth>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void LineLayer::setLineGapWidthTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<LineGapWidth>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions LineLayer::getLineGapWidthTransition() const {
    return impl().paint.template get<LineGapWidth>().options;
}

DataDrivenPropertyValue<float> LineLayer::getDefaultLineOffset() {
    return { 0 };
}

DataDrivenPropertyValue<float> LineLayer::getLineOffset() const {
    return impl().paint.template get<LineOffset>().value;
}

void LineLayer::setLineOffset(DataDrivenPropertyValue<float> value) {
    if (value == getLineOffset())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<LineOffset>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void LineLayer::setLineOffsetTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<LineOffset>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions LineLayer::getLineOffsetTransition() const {
    return impl().paint.template get<LineOffset>().options;
}

DataDrivenPropertyValue<float> LineLayer::getDefaultLineBlur() {
    return { 0 };
}

DataDrivenPropertyValue<float> LineLayer::getLineBlur() const {
    return impl().paint.template get<LineBlur>().value;
}

void LineLayer::setLineBlur(DataDrivenPropertyValue<float> value) {
    if (value == getLineBlur())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<LineBlur>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void LineLayer::setLineBlurTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<LineBlur>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions LineLayer::getLineBlurTransition() const {
    return impl().paint.template get<LineBlur>().options;
}

PropertyValue<std::vector<float>> LineLayer::getDefaultLineDasharray() {
    return { {  } };
}

PropertyValue<std::vector<float>> LineLayer::getLineDasharray() const {
    return impl().paint.template get<LineDasharray>().value;
}

void LineLayer::setLineDasharray(PropertyValue<std::vector<float>> value) {
    if (value == getLineDasharray())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<LineDasharray>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void LineLayer::setLineDasharrayTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<LineDasharray>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions LineLayer::getLineDasharrayTransition() const {
    return impl().paint.template get<LineDasharray>().options;
}

PropertyValue<std::string> LineLayer::getDefaultLinePattern() {
    return { "" };
}

PropertyValue<std::string> LineLayer::getLinePattern() const {
    return impl().paint.template get<LinePattern>().value;
}

void LineLayer::setLinePattern(PropertyValue<std::string> value) {
    if (value == getLinePattern())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<LinePattern>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void LineLayer::setLinePatternTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<LinePattern>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions LineLayer::getLinePatternTransition() const {
    return impl().paint.template get<LinePattern>().options;
}

} // namespace style
} // namespace mbgl
