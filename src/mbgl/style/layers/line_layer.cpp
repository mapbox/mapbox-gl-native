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
    impl->layout.lineCap.set(value);
}
PropertyValue<LineJoinType> LineLayer::getDefaultLineJoin() {
    return { LineJoinType::Miter };
}

PropertyValue<LineJoinType> LineLayer::getLineJoin() const {
    return impl->layout.lineJoin.get();
}

void LineLayer::setLineJoin(PropertyValue<LineJoinType> value) {
    impl->layout.lineJoin.set(value);
}
PropertyValue<float> LineLayer::getDefaultLineMiterLimit() {
    return { 2 };
}

PropertyValue<float> LineLayer::getLineMiterLimit() const {
    return impl->layout.lineMiterLimit.get();
}

void LineLayer::setLineMiterLimit(PropertyValue<float> value) {
    impl->layout.lineMiterLimit.set(value);
}
PropertyValue<float> LineLayer::getDefaultLineRoundLimit() {
    return { 1 };
}

PropertyValue<float> LineLayer::getLineRoundLimit() const {
    return impl->layout.lineRoundLimit.get();
}

void LineLayer::setLineRoundLimit(PropertyValue<float> value) {
    impl->layout.lineRoundLimit.set(value);
}

// Paint properties

PropertyValue<float> LineLayer::getDefaultLineOpacity() {
    return { 1 };
}

PropertyValue<float> LineLayer::getLineOpacity() const {
    return impl->paint.lineOpacity.get();
}

void LineLayer::setLineOpacity(PropertyValue<float> value, const optional<std::string>& klass) {
    impl->paint.lineOpacity.set(value, klass);
}

PropertyValue<Color> LineLayer::getDefaultLineColor() {
    return { Color::black() };
}

PropertyValue<Color> LineLayer::getLineColor() const {
    return impl->paint.lineColor.get();
}

void LineLayer::setLineColor(PropertyValue<Color> value, const optional<std::string>& klass) {
    impl->paint.lineColor.set(value, klass);
}

PropertyValue<std::array<float, 2>> LineLayer::getDefaultLineTranslate() {
    return { {{ 0, 0 }} };
}

PropertyValue<std::array<float, 2>> LineLayer::getLineTranslate() const {
    return impl->paint.lineTranslate.get();
}

void LineLayer::setLineTranslate(PropertyValue<std::array<float, 2>> value, const optional<std::string>& klass) {
    impl->paint.lineTranslate.set(value, klass);
}

PropertyValue<TranslateAnchorType> LineLayer::getDefaultLineTranslateAnchor() {
    return { TranslateAnchorType::Map };
}

PropertyValue<TranslateAnchorType> LineLayer::getLineTranslateAnchor() const {
    return impl->paint.lineTranslateAnchor.get();
}

void LineLayer::setLineTranslateAnchor(PropertyValue<TranslateAnchorType> value, const optional<std::string>& klass) {
    impl->paint.lineTranslateAnchor.set(value, klass);
}

PropertyValue<float> LineLayer::getDefaultLineWidth() {
    return { 1 };
}

PropertyValue<float> LineLayer::getLineWidth() const {
    return impl->paint.lineWidth.get();
}

void LineLayer::setLineWidth(PropertyValue<float> value, const optional<std::string>& klass) {
    impl->paint.lineWidth.set(value, klass);
}

PropertyValue<float> LineLayer::getDefaultLineGapWidth() {
    return { 0 };
}

PropertyValue<float> LineLayer::getLineGapWidth() const {
    return impl->paint.lineGapWidth.get();
}

void LineLayer::setLineGapWidth(PropertyValue<float> value, const optional<std::string>& klass) {
    impl->paint.lineGapWidth.set(value, klass);
}

PropertyValue<float> LineLayer::getDefaultLineOffset() {
    return { 0 };
}

PropertyValue<float> LineLayer::getLineOffset() const {
    return impl->paint.lineOffset.get();
}

void LineLayer::setLineOffset(PropertyValue<float> value, const optional<std::string>& klass) {
    impl->paint.lineOffset.set(value, klass);
}

PropertyValue<float> LineLayer::getDefaultLineBlur() {
    return { 0 };
}

PropertyValue<float> LineLayer::getLineBlur() const {
    return impl->paint.lineBlur.get();
}

void LineLayer::setLineBlur(PropertyValue<float> value, const optional<std::string>& klass) {
    impl->paint.lineBlur.set(value, klass);
}

PropertyValue<std::vector<float>> LineLayer::getDefaultLineDasharray() {
    return { {  } };
}

PropertyValue<std::vector<float>> LineLayer::getLineDasharray() const {
    return impl->paint.lineDasharray.get();
}

void LineLayer::setLineDasharray(PropertyValue<std::vector<float>> value, const optional<std::string>& klass) {
    impl->paint.lineDasharray.set(value, klass);
}

PropertyValue<std::string> LineLayer::getDefaultLinePattern() {
    return { "" };
}

PropertyValue<std::string> LineLayer::getLinePattern() const {
    return impl->paint.linePattern.get();
}

void LineLayer::setLinePattern(PropertyValue<std::string> value, const optional<std::string>& klass) {
    impl->paint.linePattern.set(value, klass);
}

} // namespace style
} // namespace mbgl
