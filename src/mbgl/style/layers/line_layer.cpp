// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/line_layer.hpp>
#include <mbgl/style/layers/line_layer_impl.hpp>

namespace mbgl {
namespace style {

LineLayer::LineLayer(const std::string& layerID, const std::string& sourceID)
    : Layer(Type::Line, std::make_unique<Impl>())
    , impl(static_cast<Impl*>(baseImpl.get())) {
    impl->id = layerID;
    impl->source = sourceID;
}

LineLayer::LineLayer(const Impl& other)
    : Layer(Type::Line, std::make_unique<Impl>(other))
    , impl(static_cast<Impl*>(baseImpl.get())) {
}

LineLayer::~LineLayer() = default;

std::unique_ptr<Layer> LineLayer::Impl::clone() const {
    return std::make_unique<LineLayer>(*this);
}

std::unique_ptr<Layer> LineLayer::Impl::cloneRef(const std::string& id_) const {
    auto result = std::make_unique<LineLayer>(*this);
    result->impl->id = id_;
    result->impl->ref = this->id;
    result->impl->paint = LinePaintProperties();
    return std::move(result);
}

// Source

const std::string& LineLayer::getSourceID() const {
    return impl->source;
}

void LineLayer::setSourceLayer(const std::string& sourceLayer) {
    impl->sourceLayer = sourceLayer;
}

const std::string& LineLayer::getSourceLayer() const {
    return impl->sourceLayer;
}

// Filter

void LineLayer::setFilter(const Filter& filter) {
    impl->filter = filter;
    impl->observer->onLayerFilterChanged(*this);
}

const Filter& LineLayer::getFilter() const {
    return impl->filter;
}

// Layout properties

PropertyValue<LineCapType> LineLayer::getDefaultLineCap() {
    return { LineCapType::Butt };
}

PropertyValue<LineCapType> LineLayer::getLineCap() const {
    return impl->layout.lineCap.get();
}

void LineLayer::setLineCap(PropertyValue<LineCapType> value) {
    if (value == getLineCap())
        return;
    impl->layout.lineCap.set(value);
    impl->observer->onLayerLayoutPropertyChanged(*this);
}
PropertyValue<LineJoinType> LineLayer::getDefaultLineJoin() {
    return { LineJoinType::Miter };
}

PropertyValue<LineJoinType> LineLayer::getLineJoin() const {
    return impl->layout.lineJoin.get();
}

void LineLayer::setLineJoin(PropertyValue<LineJoinType> value) {
    if (value == getLineJoin())
        return;
    impl->layout.lineJoin.set(value);
    impl->observer->onLayerLayoutPropertyChanged(*this);
}
PropertyValue<float> LineLayer::getDefaultLineMiterLimit() {
    return { 2 };
}

PropertyValue<float> LineLayer::getLineMiterLimit() const {
    return impl->layout.lineMiterLimit.get();
}

void LineLayer::setLineMiterLimit(PropertyValue<float> value) {
    if (value == getLineMiterLimit())
        return;
    impl->layout.lineMiterLimit.set(value);
    impl->observer->onLayerLayoutPropertyChanged(*this);
}
PropertyValue<float> LineLayer::getDefaultLineRoundLimit() {
    return { 1 };
}

PropertyValue<float> LineLayer::getLineRoundLimit() const {
    return impl->layout.lineRoundLimit.get();
}

void LineLayer::setLineRoundLimit(PropertyValue<float> value) {
    if (value == getLineRoundLimit())
        return;
    impl->layout.lineRoundLimit.set(value);
    impl->observer->onLayerLayoutPropertyChanged(*this);
}

// Paint properties

PropertyValue<float> LineLayer::getDefaultLineOpacity() {
    return { 1 };
}

PropertyValue<float> LineLayer::getLineOpacity(const optional<std::string>& klass) const {
    return impl->paint.lineOpacity.get(klass);
}

void LineLayer::setLineOpacity(PropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getLineOpacity(klass))
        return;
    impl->paint.lineOpacity.set(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<Color> LineLayer::getDefaultLineColor() {
    return { Color::black() };
}

PropertyValue<Color> LineLayer::getLineColor(const optional<std::string>& klass) const {
    return impl->paint.lineColor.get(klass);
}

void LineLayer::setLineColor(PropertyValue<Color> value, const optional<std::string>& klass) {
    if (value == getLineColor(klass))
        return;
    impl->paint.lineColor.set(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<std::array<float, 2>> LineLayer::getDefaultLineTranslate() {
    return { {{ 0, 0 }} };
}

PropertyValue<std::array<float, 2>> LineLayer::getLineTranslate(const optional<std::string>& klass) const {
    return impl->paint.lineTranslate.get(klass);
}

void LineLayer::setLineTranslate(PropertyValue<std::array<float, 2>> value, const optional<std::string>& klass) {
    if (value == getLineTranslate(klass))
        return;
    impl->paint.lineTranslate.set(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<TranslateAnchorType> LineLayer::getDefaultLineTranslateAnchor() {
    return { TranslateAnchorType::Map };
}

PropertyValue<TranslateAnchorType> LineLayer::getLineTranslateAnchor(const optional<std::string>& klass) const {
    return impl->paint.lineTranslateAnchor.get(klass);
}

void LineLayer::setLineTranslateAnchor(PropertyValue<TranslateAnchorType> value, const optional<std::string>& klass) {
    if (value == getLineTranslateAnchor(klass))
        return;
    impl->paint.lineTranslateAnchor.set(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<float> LineLayer::getDefaultLineWidth() {
    return { 1 };
}

PropertyValue<float> LineLayer::getLineWidth(const optional<std::string>& klass) const {
    return impl->paint.lineWidth.get(klass);
}

void LineLayer::setLineWidth(PropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getLineWidth(klass))
        return;
    impl->paint.lineWidth.set(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<float> LineLayer::getDefaultLineGapWidth() {
    return { 0 };
}

PropertyValue<float> LineLayer::getLineGapWidth(const optional<std::string>& klass) const {
    return impl->paint.lineGapWidth.get(klass);
}

void LineLayer::setLineGapWidth(PropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getLineGapWidth(klass))
        return;
    impl->paint.lineGapWidth.set(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<float> LineLayer::getDefaultLineOffset() {
    return { 0 };
}

PropertyValue<float> LineLayer::getLineOffset(const optional<std::string>& klass) const {
    return impl->paint.lineOffset.get(klass);
}

void LineLayer::setLineOffset(PropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getLineOffset(klass))
        return;
    impl->paint.lineOffset.set(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<float> LineLayer::getDefaultLineBlur() {
    return { 0 };
}

PropertyValue<float> LineLayer::getLineBlur(const optional<std::string>& klass) const {
    return impl->paint.lineBlur.get(klass);
}

void LineLayer::setLineBlur(PropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getLineBlur(klass))
        return;
    impl->paint.lineBlur.set(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<std::vector<float>> LineLayer::getDefaultLineDasharray() {
    return { {  } };
}

PropertyValue<std::vector<float>> LineLayer::getLineDasharray(const optional<std::string>& klass) const {
    return impl->paint.lineDasharray.get(klass);
}

void LineLayer::setLineDasharray(PropertyValue<std::vector<float>> value, const optional<std::string>& klass) {
    if (value == getLineDasharray(klass))
        return;
    impl->paint.lineDasharray.set(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<std::string> LineLayer::getDefaultLinePattern() {
    return { "" };
}

PropertyValue<std::string> LineLayer::getLinePattern(const optional<std::string>& klass) const {
    return impl->paint.linePattern.get(klass);
}

void LineLayer::setLinePattern(PropertyValue<std::string> value, const optional<std::string>& klass) {
    if (value == getLinePattern(klass))
        return;
    impl->paint.linePattern.set(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

} // namespace style
} // namespace mbgl
