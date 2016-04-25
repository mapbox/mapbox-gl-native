// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/layer/line_layer.hpp>
#include <mbgl/layer/line_layer_impl.hpp>

namespace mbgl {

LineLayer::LineLayer(const std::string& layerID)
    : Layer(Type::Line, std::make_unique<Impl>())
    , impl(static_cast<Impl*>(baseImpl.get())) {
    impl->id = layerID;
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

void LineLayer::setSource(const std::string& sourceID, const std::string& sourceLayer) {
    impl->source = sourceID;
    impl->sourceLayer = sourceLayer;
}

const std::string& LineLayer::getSourceID() const {
    return impl->source;
}

const std::string& LineLayer::getSourceLayer() const {
    return impl->sourceLayer;
}

// Layout properties

Function<LineCapType> LineLayer::getLineCap() const {
    return *impl->layout.lineCap.parsedValue;
}

void LineLayer::setLineCap(Function<LineCapType> value) {
    impl->layout.lineCap.parsedValue = value;
}
Function<LineJoinType> LineLayer::getLineJoin() const {
    return *impl->layout.lineJoin.parsedValue;
}

void LineLayer::setLineJoin(Function<LineJoinType> value) {
    impl->layout.lineJoin.parsedValue = value;
}
Function<float> LineLayer::getLineMiterLimit() const {
    return *impl->layout.lineMiterLimit.parsedValue;
}

void LineLayer::setLineMiterLimit(Function<float> value) {
    impl->layout.lineMiterLimit.parsedValue = value;
}
Function<float> LineLayer::getLineRoundLimit() const {
    return *impl->layout.lineRoundLimit.parsedValue;
}

void LineLayer::setLineRoundLimit(Function<float> value) {
    impl->layout.lineRoundLimit.parsedValue = value;
}

// Paint properties

Function<float> LineLayer::getLineOpacity() const {
    return impl->paint.lineOpacity.values.at(ClassID::Default);
}

void LineLayer::setLineOpacity(Function<float> value) {
    impl->paint.lineOpacity.values.emplace(ClassID::Default, value);
}

Function<Color> LineLayer::getLineColor() const {
    return impl->paint.lineColor.values.at(ClassID::Default);
}

void LineLayer::setLineColor(Function<Color> value) {
    impl->paint.lineColor.values.emplace(ClassID::Default, value);
}

Function<std::array<float, 2>> LineLayer::getLineTranslate() const {
    return impl->paint.lineTranslate.values.at(ClassID::Default);
}

void LineLayer::setLineTranslate(Function<std::array<float, 2>> value) {
    impl->paint.lineTranslate.values.emplace(ClassID::Default, value);
}

Function<TranslateAnchorType> LineLayer::getLineTranslateAnchor() const {
    return impl->paint.lineTranslateAnchor.values.at(ClassID::Default);
}

void LineLayer::setLineTranslateAnchor(Function<TranslateAnchorType> value) {
    impl->paint.lineTranslateAnchor.values.emplace(ClassID::Default, value);
}

Function<float> LineLayer::getLineWidth() const {
    return impl->paint.lineWidth.values.at(ClassID::Default);
}

void LineLayer::setLineWidth(Function<float> value) {
    impl->paint.lineWidth.values.emplace(ClassID::Default, value);
}

Function<float> LineLayer::getLineGapWidth() const {
    return impl->paint.lineGapWidth.values.at(ClassID::Default);
}

void LineLayer::setLineGapWidth(Function<float> value) {
    impl->paint.lineGapWidth.values.emplace(ClassID::Default, value);
}

Function<float> LineLayer::getLineOffset() const {
    return impl->paint.lineOffset.values.at(ClassID::Default);
}

void LineLayer::setLineOffset(Function<float> value) {
    impl->paint.lineOffset.values.emplace(ClassID::Default, value);
}

Function<float> LineLayer::getLineBlur() const {
    return impl->paint.lineBlur.values.at(ClassID::Default);
}

void LineLayer::setLineBlur(Function<float> value) {
    impl->paint.lineBlur.values.emplace(ClassID::Default, value);
}

Function<std::vector<float>> LineLayer::getLineDasharray() const {
    return impl->paint.lineDasharray.values.at(ClassID::Default);
}

void LineLayer::setLineDasharray(Function<std::vector<float>> value) {
    impl->paint.lineDasharray.values.emplace(ClassID::Default, value);
}

Function<std::string> LineLayer::getLinePattern() const {
    return impl->paint.linePattern.values.at(ClassID::Default);
}

void LineLayer::setLinePattern(Function<std::string> value) {
    impl->paint.linePattern.values.emplace(ClassID::Default, value);
}

} // namespace mbgl
