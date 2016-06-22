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
}

const Filter& LineLayer::getFilter() const {
    return impl->filter;
}

// Layout properties

PropertyValue<LineCapType> LineLayer::getLineCap() const {
    return impl->layout.lineCap.get();
}

void LineLayer::setLineCap(PropertyValue<LineCapType> value) {
    impl->layout.lineCap.set(value);
}
PropertyValue<LineJoinType> LineLayer::getLineJoin() const {
    return impl->layout.lineJoin.get();
}

void LineLayer::setLineJoin(PropertyValue<LineJoinType> value) {
    impl->layout.lineJoin.set(value);
}
PropertyValue<float> LineLayer::getLineMiterLimit() const {
    return impl->layout.lineMiterLimit.get();
}

void LineLayer::setLineMiterLimit(PropertyValue<float> value) {
    impl->layout.lineMiterLimit.set(value);
}
PropertyValue<float> LineLayer::getLineRoundLimit() const {
    return impl->layout.lineRoundLimit.get();
}

void LineLayer::setLineRoundLimit(PropertyValue<float> value) {
    impl->layout.lineRoundLimit.set(value);
}

// Paint properties

PropertyValue<float> LineLayer::getLineOpacity() const {
    return impl->paint.lineOpacity.get();
}

void LineLayer::setLineOpacity(PropertyValue<float> value) {
    impl->paint.lineOpacity.set(value);
}

PropertyValue<Color> LineLayer::getLineColor() const {
    return impl->paint.lineColor.get();
}

void LineLayer::setLineColor(PropertyValue<Color> value) {
    impl->paint.lineColor.set(value);
}

PropertyValue<std::array<float, 2>> LineLayer::getLineTranslate() const {
    return impl->paint.lineTranslate.get();
}

void LineLayer::setLineTranslate(PropertyValue<std::array<float, 2>> value) {
    impl->paint.lineTranslate.set(value);
}

PropertyValue<TranslateAnchorType> LineLayer::getLineTranslateAnchor() const {
    return impl->paint.lineTranslateAnchor.get();
}

void LineLayer::setLineTranslateAnchor(PropertyValue<TranslateAnchorType> value) {
    impl->paint.lineTranslateAnchor.set(value);
}

PropertyValue<float> LineLayer::getLineWidth() const {
    return impl->paint.lineWidth.get();
}

void LineLayer::setLineWidth(PropertyValue<float> value) {
    impl->paint.lineWidth.set(value);
}

PropertyValue<float> LineLayer::getLineGapWidth() const {
    return impl->paint.lineGapWidth.get();
}

void LineLayer::setLineGapWidth(PropertyValue<float> value) {
    impl->paint.lineGapWidth.set(value);
}

PropertyValue<float> LineLayer::getLineOffset() const {
    return impl->paint.lineOffset.get();
}

void LineLayer::setLineOffset(PropertyValue<float> value) {
    impl->paint.lineOffset.set(value);
}

PropertyValue<float> LineLayer::getLineBlur() const {
    return impl->paint.lineBlur.get();
}

void LineLayer::setLineBlur(PropertyValue<float> value) {
    impl->paint.lineBlur.set(value);
}

PropertyValue<std::vector<float>> LineLayer::getLineDasharray() const {
    return impl->paint.lineDasharray.get();
}

void LineLayer::setLineDasharray(PropertyValue<std::vector<float>> value) {
    impl->paint.lineDasharray.set(value);
}

PropertyValue<std::string> LineLayer::getLinePattern() const {
    return impl->paint.linePattern.get();
}

void LineLayer::setLinePattern(PropertyValue<std::string> value) {
    impl->paint.linePattern.set(value);
}

} // namespace style
} // namespace mbgl
