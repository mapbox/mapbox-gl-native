// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/line_layer.hpp>
#include <mbgl/style/layers/line_layer_impl.hpp>
#include <mbgl/style/conversion/stringify.hpp>

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
    result->impl->paint = LinePaintProperties();
    return std::move(result);
}

void LineLayer::Impl::stringifyLayout(rapidjson::Writer<rapidjson::StringBuffer>& writer) const {
    conversion::stringify(writer, layout);
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
    return LineCap::defaultValue();
}

PropertyValue<LineCapType> LineLayer::getLineCap() const {
    return impl->layout.unevaluated.get<LineCap>();
}

void LineLayer::setLineCap(PropertyValue<LineCapType> value) {
    if (value == getLineCap())
        return;
    impl->layout.unevaluated.get<LineCap>() = value;
    impl->observer->onLayerLayoutPropertyChanged(*this, "line-cap");
}
PropertyValue<LineJoinType> LineLayer::getDefaultLineJoin() {
    return LineJoin::defaultValue();
}

PropertyValue<LineJoinType> LineLayer::getLineJoin() const {
    return impl->layout.unevaluated.get<LineJoin>();
}

void LineLayer::setLineJoin(PropertyValue<LineJoinType> value) {
    if (value == getLineJoin())
        return;
    impl->layout.unevaluated.get<LineJoin>() = value;
    impl->observer->onLayerLayoutPropertyChanged(*this, "line-join");
}
PropertyValue<float> LineLayer::getDefaultLineMiterLimit() {
    return LineMiterLimit::defaultValue();
}

PropertyValue<float> LineLayer::getLineMiterLimit() const {
    return impl->layout.unevaluated.get<LineMiterLimit>();
}

void LineLayer::setLineMiterLimit(PropertyValue<float> value) {
    if (value == getLineMiterLimit())
        return;
    impl->layout.unevaluated.get<LineMiterLimit>() = value;
    impl->observer->onLayerLayoutPropertyChanged(*this, "line-miter-limit");
}
PropertyValue<float> LineLayer::getDefaultLineRoundLimit() {
    return LineRoundLimit::defaultValue();
}

PropertyValue<float> LineLayer::getLineRoundLimit() const {
    return impl->layout.unevaluated.get<LineRoundLimit>();
}

void LineLayer::setLineRoundLimit(PropertyValue<float> value) {
    if (value == getLineRoundLimit())
        return;
    impl->layout.unevaluated.get<LineRoundLimit>() = value;
    impl->observer->onLayerLayoutPropertyChanged(*this, "line-round-limit");
}

// Paint properties

PropertyValue<float> LineLayer::getDefaultLineOpacity() {
    return { 1 };
}

PropertyValue<float> LineLayer::getLineOpacity(const optional<std::string>& klass) const {
    return impl->paint.get<LineOpacity>(klass);
}

void LineLayer::setLineOpacity(PropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getLineOpacity(klass))
        return;
    impl->paint.set<LineOpacity>(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<Color> LineLayer::getDefaultLineColor() {
    return { Color::black() };
}

PropertyValue<Color> LineLayer::getLineColor(const optional<std::string>& klass) const {
    return impl->paint.get<LineColor>(klass);
}

void LineLayer::setLineColor(PropertyValue<Color> value, const optional<std::string>& klass) {
    if (value == getLineColor(klass))
        return;
    impl->paint.set<LineColor>(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<std::array<float, 2>> LineLayer::getDefaultLineTranslate() {
    return { {{ 0, 0 }} };
}

PropertyValue<std::array<float, 2>> LineLayer::getLineTranslate(const optional<std::string>& klass) const {
    return impl->paint.get<LineTranslate>(klass);
}

void LineLayer::setLineTranslate(PropertyValue<std::array<float, 2>> value, const optional<std::string>& klass) {
    if (value == getLineTranslate(klass))
        return;
    impl->paint.set<LineTranslate>(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<TranslateAnchorType> LineLayer::getDefaultLineTranslateAnchor() {
    return { TranslateAnchorType::Map };
}

PropertyValue<TranslateAnchorType> LineLayer::getLineTranslateAnchor(const optional<std::string>& klass) const {
    return impl->paint.get<LineTranslateAnchor>(klass);
}

void LineLayer::setLineTranslateAnchor(PropertyValue<TranslateAnchorType> value, const optional<std::string>& klass) {
    if (value == getLineTranslateAnchor(klass))
        return;
    impl->paint.set<LineTranslateAnchor>(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<float> LineLayer::getDefaultLineWidth() {
    return { 1 };
}

PropertyValue<float> LineLayer::getLineWidth(const optional<std::string>& klass) const {
    return impl->paint.get<LineWidth>(klass);
}

void LineLayer::setLineWidth(PropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getLineWidth(klass))
        return;
    impl->paint.set<LineWidth>(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<float> LineLayer::getDefaultLineGapWidth() {
    return { 0 };
}

PropertyValue<float> LineLayer::getLineGapWidth(const optional<std::string>& klass) const {
    return impl->paint.get<LineGapWidth>(klass);
}

void LineLayer::setLineGapWidth(PropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getLineGapWidth(klass))
        return;
    impl->paint.set<LineGapWidth>(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<float> LineLayer::getDefaultLineOffset() {
    return { 0 };
}

PropertyValue<float> LineLayer::getLineOffset(const optional<std::string>& klass) const {
    return impl->paint.get<LineOffset>(klass);
}

void LineLayer::setLineOffset(PropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getLineOffset(klass))
        return;
    impl->paint.set<LineOffset>(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<float> LineLayer::getDefaultLineBlur() {
    return { 0 };
}

PropertyValue<float> LineLayer::getLineBlur(const optional<std::string>& klass) const {
    return impl->paint.get<LineBlur>(klass);
}

void LineLayer::setLineBlur(PropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getLineBlur(klass))
        return;
    impl->paint.set<LineBlur>(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<std::vector<float>> LineLayer::getDefaultLineDasharray() {
    return { {  } };
}

PropertyValue<std::vector<float>> LineLayer::getLineDasharray(const optional<std::string>& klass) const {
    return impl->paint.get<LineDasharray>(klass);
}

void LineLayer::setLineDasharray(PropertyValue<std::vector<float>> value, const optional<std::string>& klass) {
    if (value == getLineDasharray(klass))
        return;
    impl->paint.set<LineDasharray>(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<std::string> LineLayer::getDefaultLinePattern() {
    return { "" };
}

PropertyValue<std::string> LineLayer::getLinePattern(const optional<std::string>& klass) const {
    return impl->paint.get<LinePattern>(klass);
}

void LineLayer::setLinePattern(PropertyValue<std::string> value, const optional<std::string>& klass) {
    if (value == getLinePattern(klass))
        return;
    impl->paint.set<LinePattern>(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

} // namespace style
} // namespace mbgl
