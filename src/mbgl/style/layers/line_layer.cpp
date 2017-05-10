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
    impl_->paint = LinePaintProperties::Cascading();
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
    observer->onLayerFilterChanged(*this);
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
    observer->onLayerVisibilityChanged(*this);
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
    observer->onLayerLayoutPropertyChanged(*this, "line-cap");
}
PropertyValue<LineJoinType> LineLayer::getDefaultLineJoin() {
    return LineJoin::defaultValue();
}

PropertyValue<LineJoinType> LineLayer::getLineJoin() const {
    return impl().layout.get<LineJoin>();
}

void LineLayer::setLineJoin(PropertyValue<LineJoinType> value) {
    if (value == getLineJoin())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.get<LineJoin>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerLayoutPropertyChanged(*this, "line-join");
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
    observer->onLayerLayoutPropertyChanged(*this, "line-miter-limit");
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
    observer->onLayerLayoutPropertyChanged(*this, "line-round-limit");
}

// Paint properties

DataDrivenPropertyValue<float> LineLayer::getDefaultLineOpacity() {
    return { 1 };
}

DataDrivenPropertyValue<float> LineLayer::getLineOpacity(const optional<std::string>& klass) const {
    return impl().paint.template get<LineOpacity>().get(klass);
}

void LineLayer::setLineOpacity(DataDrivenPropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getLineOpacity(klass))
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<LineOpacity>().set(value, klass);
    baseImpl = std::move(impl_);
    if (value.isDataDriven()) {
        observer->onLayerDataDrivenPaintPropertyChanged(*this);
    } else {
        observer->onLayerPaintPropertyChanged(*this);
    }
}

void LineLayer::setLineOpacityTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<LineOpacity>().setTransition(value, klass);
    baseImpl = std::move(impl_);
}

TransitionOptions LineLayer::getLineOpacityTransition(const optional<std::string>& klass) const {
    return impl().paint.template get<LineOpacity>().getTransition(klass);
}

DataDrivenPropertyValue<Color> LineLayer::getDefaultLineColor() {
    return { Color::black() };
}

DataDrivenPropertyValue<Color> LineLayer::getLineColor(const optional<std::string>& klass) const {
    return impl().paint.template get<LineColor>().get(klass);
}

void LineLayer::setLineColor(DataDrivenPropertyValue<Color> value, const optional<std::string>& klass) {
    if (value == getLineColor(klass))
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<LineColor>().set(value, klass);
    baseImpl = std::move(impl_);
    if (value.isDataDriven()) {
        observer->onLayerDataDrivenPaintPropertyChanged(*this);
    } else {
        observer->onLayerPaintPropertyChanged(*this);
    }
}

void LineLayer::setLineColorTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<LineColor>().setTransition(value, klass);
    baseImpl = std::move(impl_);
}

TransitionOptions LineLayer::getLineColorTransition(const optional<std::string>& klass) const {
    return impl().paint.template get<LineColor>().getTransition(klass);
}

PropertyValue<std::array<float, 2>> LineLayer::getDefaultLineTranslate() {
    return { {{ 0, 0 }} };
}

PropertyValue<std::array<float, 2>> LineLayer::getLineTranslate(const optional<std::string>& klass) const {
    return impl().paint.template get<LineTranslate>().get(klass);
}

void LineLayer::setLineTranslate(PropertyValue<std::array<float, 2>> value, const optional<std::string>& klass) {
    if (value == getLineTranslate(klass))
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<LineTranslate>().set(value, klass);
    baseImpl = std::move(impl_);
    observer->onLayerPaintPropertyChanged(*this);
}

void LineLayer::setLineTranslateTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<LineTranslate>().setTransition(value, klass);
    baseImpl = std::move(impl_);
}

TransitionOptions LineLayer::getLineTranslateTransition(const optional<std::string>& klass) const {
    return impl().paint.template get<LineTranslate>().getTransition(klass);
}

PropertyValue<TranslateAnchorType> LineLayer::getDefaultLineTranslateAnchor() {
    return { TranslateAnchorType::Map };
}

PropertyValue<TranslateAnchorType> LineLayer::getLineTranslateAnchor(const optional<std::string>& klass) const {
    return impl().paint.template get<LineTranslateAnchor>().get(klass);
}

void LineLayer::setLineTranslateAnchor(PropertyValue<TranslateAnchorType> value, const optional<std::string>& klass) {
    if (value == getLineTranslateAnchor(klass))
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<LineTranslateAnchor>().set(value, klass);
    baseImpl = std::move(impl_);
    observer->onLayerPaintPropertyChanged(*this);
}

void LineLayer::setLineTranslateAnchorTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<LineTranslateAnchor>().setTransition(value, klass);
    baseImpl = std::move(impl_);
}

TransitionOptions LineLayer::getLineTranslateAnchorTransition(const optional<std::string>& klass) const {
    return impl().paint.template get<LineTranslateAnchor>().getTransition(klass);
}

PropertyValue<float> LineLayer::getDefaultLineWidth() {
    return { 1 };
}

PropertyValue<float> LineLayer::getLineWidth(const optional<std::string>& klass) const {
    return impl().paint.template get<LineWidth>().get(klass);
}

void LineLayer::setLineWidth(PropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getLineWidth(klass))
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<LineWidth>().set(value, klass);
    baseImpl = std::move(impl_);
    observer->onLayerPaintPropertyChanged(*this);
}

void LineLayer::setLineWidthTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<LineWidth>().setTransition(value, klass);
    baseImpl = std::move(impl_);
}

TransitionOptions LineLayer::getLineWidthTransition(const optional<std::string>& klass) const {
    return impl().paint.template get<LineWidth>().getTransition(klass);
}

DataDrivenPropertyValue<float> LineLayer::getDefaultLineGapWidth() {
    return { 0 };
}

DataDrivenPropertyValue<float> LineLayer::getLineGapWidth(const optional<std::string>& klass) const {
    return impl().paint.template get<LineGapWidth>().get(klass);
}

void LineLayer::setLineGapWidth(DataDrivenPropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getLineGapWidth(klass))
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<LineGapWidth>().set(value, klass);
    baseImpl = std::move(impl_);
    if (value.isDataDriven()) {
        observer->onLayerDataDrivenPaintPropertyChanged(*this);
    } else {
        observer->onLayerPaintPropertyChanged(*this);
    }
}

void LineLayer::setLineGapWidthTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<LineGapWidth>().setTransition(value, klass);
    baseImpl = std::move(impl_);
}

TransitionOptions LineLayer::getLineGapWidthTransition(const optional<std::string>& klass) const {
    return impl().paint.template get<LineGapWidth>().getTransition(klass);
}

DataDrivenPropertyValue<float> LineLayer::getDefaultLineOffset() {
    return { 0 };
}

DataDrivenPropertyValue<float> LineLayer::getLineOffset(const optional<std::string>& klass) const {
    return impl().paint.template get<LineOffset>().get(klass);
}

void LineLayer::setLineOffset(DataDrivenPropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getLineOffset(klass))
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<LineOffset>().set(value, klass);
    baseImpl = std::move(impl_);
    if (value.isDataDriven()) {
        observer->onLayerDataDrivenPaintPropertyChanged(*this);
    } else {
        observer->onLayerPaintPropertyChanged(*this);
    }
}

void LineLayer::setLineOffsetTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<LineOffset>().setTransition(value, klass);
    baseImpl = std::move(impl_);
}

TransitionOptions LineLayer::getLineOffsetTransition(const optional<std::string>& klass) const {
    return impl().paint.template get<LineOffset>().getTransition(klass);
}

DataDrivenPropertyValue<float> LineLayer::getDefaultLineBlur() {
    return { 0 };
}

DataDrivenPropertyValue<float> LineLayer::getLineBlur(const optional<std::string>& klass) const {
    return impl().paint.template get<LineBlur>().get(klass);
}

void LineLayer::setLineBlur(DataDrivenPropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getLineBlur(klass))
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<LineBlur>().set(value, klass);
    baseImpl = std::move(impl_);
    if (value.isDataDriven()) {
        observer->onLayerDataDrivenPaintPropertyChanged(*this);
    } else {
        observer->onLayerPaintPropertyChanged(*this);
    }
}

void LineLayer::setLineBlurTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<LineBlur>().setTransition(value, klass);
    baseImpl = std::move(impl_);
}

TransitionOptions LineLayer::getLineBlurTransition(const optional<std::string>& klass) const {
    return impl().paint.template get<LineBlur>().getTransition(klass);
}

PropertyValue<std::vector<float>> LineLayer::getDefaultLineDasharray() {
    return { {  } };
}

PropertyValue<std::vector<float>> LineLayer::getLineDasharray(const optional<std::string>& klass) const {
    return impl().paint.template get<LineDasharray>().get(klass);
}

void LineLayer::setLineDasharray(PropertyValue<std::vector<float>> value, const optional<std::string>& klass) {
    if (value == getLineDasharray(klass))
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<LineDasharray>().set(value, klass);
    baseImpl = std::move(impl_);
    observer->onLayerPaintPropertyChanged(*this);
}

void LineLayer::setLineDasharrayTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<LineDasharray>().setTransition(value, klass);
    baseImpl = std::move(impl_);
}

TransitionOptions LineLayer::getLineDasharrayTransition(const optional<std::string>& klass) const {
    return impl().paint.template get<LineDasharray>().getTransition(klass);
}

PropertyValue<std::string> LineLayer::getDefaultLinePattern() {
    return { "" };
}

PropertyValue<std::string> LineLayer::getLinePattern(const optional<std::string>& klass) const {
    return impl().paint.template get<LinePattern>().get(klass);
}

void LineLayer::setLinePattern(PropertyValue<std::string> value, const optional<std::string>& klass) {
    if (value == getLinePattern(klass))
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<LinePattern>().set(value, klass);
    baseImpl = std::move(impl_);
    observer->onLayerPaintPropertyChanged(*this);
}

void LineLayer::setLinePatternTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<LinePattern>().setTransition(value, klass);
    baseImpl = std::move(impl_);
}

TransitionOptions LineLayer::getLinePatternTransition(const optional<std::string>& klass) const {
    return impl().paint.template get<LinePattern>().getTransition(klass);
}

} // namespace style
} // namespace mbgl
