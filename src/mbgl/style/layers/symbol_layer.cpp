// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/style/layers/symbol_layer_impl.hpp>
#include <mbgl/style/conversion/stringify.hpp>

namespace mbgl {
namespace style {

SymbolLayer::SymbolLayer(const std::string& layerID, const std::string& sourceID)
    : Layer(Type::Symbol, std::make_unique<Impl>())
    , impl(static_cast<Impl*>(baseImpl.get())) {
    impl->id = layerID;
    impl->source = sourceID;
}

SymbolLayer::SymbolLayer(const Impl& other)
    : Layer(Type::Symbol, std::make_unique<Impl>(other))
    , impl(static_cast<Impl*>(baseImpl.get())) {
}

SymbolLayer::~SymbolLayer() = default;

std::unique_ptr<Layer> SymbolLayer::Impl::clone() const {
    return std::make_unique<SymbolLayer>(*this);
}

std::unique_ptr<Layer> SymbolLayer::Impl::cloneRef(const std::string& id_) const {
    auto result = std::make_unique<SymbolLayer>(*this);
    result->impl->id = id_;
    result->impl->paint = SymbolPaintProperties();
    return std::move(result);
}

void SymbolLayer::Impl::stringifyLayout(rapidjson::Writer<rapidjson::StringBuffer>& writer) const {
    conversion::stringify(writer, layout);
}

// Source

const std::string& SymbolLayer::getSourceID() const {
    return impl->source;
}

void SymbolLayer::setSourceLayer(const std::string& sourceLayer) {
    impl->sourceLayer = sourceLayer;
}

const std::string& SymbolLayer::getSourceLayer() const {
    return impl->sourceLayer;
}

// Filter

void SymbolLayer::setFilter(const Filter& filter) {
    impl->filter = filter;
    impl->observer->onLayerFilterChanged(*this);
}

const Filter& SymbolLayer::getFilter() const {
    return impl->filter;
}

// Layout properties

PropertyValue<SymbolPlacementType> SymbolLayer::getDefaultSymbolPlacement() {
    return SymbolPlacement::defaultValue();
}

PropertyValue<SymbolPlacementType> SymbolLayer::getSymbolPlacement() const {
    return impl->layout.unevaluated.get<SymbolPlacement>();
}

void SymbolLayer::setSymbolPlacement(PropertyValue<SymbolPlacementType> value) {
    if (value == getSymbolPlacement())
        return;
    impl->layout.unevaluated.get<SymbolPlacement>() = value;
    impl->observer->onLayerLayoutPropertyChanged(*this, "symbol-placement");
}
PropertyValue<float> SymbolLayer::getDefaultSymbolSpacing() {
    return SymbolSpacing::defaultValue();
}

PropertyValue<float> SymbolLayer::getSymbolSpacing() const {
    return impl->layout.unevaluated.get<SymbolSpacing>();
}

void SymbolLayer::setSymbolSpacing(PropertyValue<float> value) {
    if (value == getSymbolSpacing())
        return;
    impl->layout.unevaluated.get<SymbolSpacing>() = value;
    impl->observer->onLayerLayoutPropertyChanged(*this, "symbol-spacing");
}
PropertyValue<bool> SymbolLayer::getDefaultSymbolAvoidEdges() {
    return SymbolAvoidEdges::defaultValue();
}

PropertyValue<bool> SymbolLayer::getSymbolAvoidEdges() const {
    return impl->layout.unevaluated.get<SymbolAvoidEdges>();
}

void SymbolLayer::setSymbolAvoidEdges(PropertyValue<bool> value) {
    if (value == getSymbolAvoidEdges())
        return;
    impl->layout.unevaluated.get<SymbolAvoidEdges>() = value;
    impl->observer->onLayerLayoutPropertyChanged(*this, "symbol-avoid-edges");
}
PropertyValue<bool> SymbolLayer::getDefaultIconAllowOverlap() {
    return IconAllowOverlap::defaultValue();
}

PropertyValue<bool> SymbolLayer::getIconAllowOverlap() const {
    return impl->layout.unevaluated.get<IconAllowOverlap>();
}

void SymbolLayer::setIconAllowOverlap(PropertyValue<bool> value) {
    if (value == getIconAllowOverlap())
        return;
    impl->layout.unevaluated.get<IconAllowOverlap>() = value;
    impl->observer->onLayerLayoutPropertyChanged(*this, "icon-allow-overlap");
}
PropertyValue<bool> SymbolLayer::getDefaultIconIgnorePlacement() {
    return IconIgnorePlacement::defaultValue();
}

PropertyValue<bool> SymbolLayer::getIconIgnorePlacement() const {
    return impl->layout.unevaluated.get<IconIgnorePlacement>();
}

void SymbolLayer::setIconIgnorePlacement(PropertyValue<bool> value) {
    if (value == getIconIgnorePlacement())
        return;
    impl->layout.unevaluated.get<IconIgnorePlacement>() = value;
    impl->observer->onLayerLayoutPropertyChanged(*this, "icon-ignore-placement");
}
PropertyValue<bool> SymbolLayer::getDefaultIconOptional() {
    return IconOptional::defaultValue();
}

PropertyValue<bool> SymbolLayer::getIconOptional() const {
    return impl->layout.unevaluated.get<IconOptional>();
}

void SymbolLayer::setIconOptional(PropertyValue<bool> value) {
    if (value == getIconOptional())
        return;
    impl->layout.unevaluated.get<IconOptional>() = value;
    impl->observer->onLayerLayoutPropertyChanged(*this, "icon-optional");
}
PropertyValue<AlignmentType> SymbolLayer::getDefaultIconRotationAlignment() {
    return IconRotationAlignment::defaultValue();
}

PropertyValue<AlignmentType> SymbolLayer::getIconRotationAlignment() const {
    return impl->layout.unevaluated.get<IconRotationAlignment>();
}

void SymbolLayer::setIconRotationAlignment(PropertyValue<AlignmentType> value) {
    if (value == getIconRotationAlignment())
        return;
    impl->layout.unevaluated.get<IconRotationAlignment>() = value;
    impl->observer->onLayerLayoutPropertyChanged(*this, "icon-rotation-alignment");
}
PropertyValue<float> SymbolLayer::getDefaultIconSize() {
    return IconSize::defaultValue();
}

PropertyValue<float> SymbolLayer::getIconSize() const {
    return impl->layout.unevaluated.get<IconSize>();
}

void SymbolLayer::setIconSize(PropertyValue<float> value) {
    if (value == getIconSize())
        return;
    impl->layout.unevaluated.get<IconSize>() = value;
    impl->observer->onLayerLayoutPropertyChanged(*this, "icon-size");
}
PropertyValue<IconTextFitType> SymbolLayer::getDefaultIconTextFit() {
    return IconTextFit::defaultValue();
}

PropertyValue<IconTextFitType> SymbolLayer::getIconTextFit() const {
    return impl->layout.unevaluated.get<IconTextFit>();
}

void SymbolLayer::setIconTextFit(PropertyValue<IconTextFitType> value) {
    if (value == getIconTextFit())
        return;
    impl->layout.unevaluated.get<IconTextFit>() = value;
    impl->observer->onLayerLayoutPropertyChanged(*this, "icon-text-fit");
}
PropertyValue<std::array<float, 4>> SymbolLayer::getDefaultIconTextFitPadding() {
    return IconTextFitPadding::defaultValue();
}

PropertyValue<std::array<float, 4>> SymbolLayer::getIconTextFitPadding() const {
    return impl->layout.unevaluated.get<IconTextFitPadding>();
}

void SymbolLayer::setIconTextFitPadding(PropertyValue<std::array<float, 4>> value) {
    if (value == getIconTextFitPadding())
        return;
    impl->layout.unevaluated.get<IconTextFitPadding>() = value;
    impl->observer->onLayerLayoutPropertyChanged(*this, "icon-text-fit-padding");
}
PropertyValue<std::string> SymbolLayer::getDefaultIconImage() {
    return IconImage::defaultValue();
}

PropertyValue<std::string> SymbolLayer::getIconImage() const {
    return impl->layout.unevaluated.get<IconImage>();
}

void SymbolLayer::setIconImage(PropertyValue<std::string> value) {
    if (value == getIconImage())
        return;
    impl->layout.unevaluated.get<IconImage>() = value;
    impl->observer->onLayerLayoutPropertyChanged(*this, "icon-image");
}
PropertyValue<float> SymbolLayer::getDefaultIconRotate() {
    return IconRotate::defaultValue();
}

PropertyValue<float> SymbolLayer::getIconRotate() const {
    return impl->layout.unevaluated.get<IconRotate>();
}

void SymbolLayer::setIconRotate(PropertyValue<float> value) {
    if (value == getIconRotate())
        return;
    impl->layout.unevaluated.get<IconRotate>() = value;
    impl->observer->onLayerLayoutPropertyChanged(*this, "icon-rotate");
}
PropertyValue<float> SymbolLayer::getDefaultIconPadding() {
    return IconPadding::defaultValue();
}

PropertyValue<float> SymbolLayer::getIconPadding() const {
    return impl->layout.unevaluated.get<IconPadding>();
}

void SymbolLayer::setIconPadding(PropertyValue<float> value) {
    if (value == getIconPadding())
        return;
    impl->layout.unevaluated.get<IconPadding>() = value;
    impl->observer->onLayerLayoutPropertyChanged(*this, "icon-padding");
}
PropertyValue<bool> SymbolLayer::getDefaultIconKeepUpright() {
    return IconKeepUpright::defaultValue();
}

PropertyValue<bool> SymbolLayer::getIconKeepUpright() const {
    return impl->layout.unevaluated.get<IconKeepUpright>();
}

void SymbolLayer::setIconKeepUpright(PropertyValue<bool> value) {
    if (value == getIconKeepUpright())
        return;
    impl->layout.unevaluated.get<IconKeepUpright>() = value;
    impl->observer->onLayerLayoutPropertyChanged(*this, "icon-keep-upright");
}
PropertyValue<std::array<float, 2>> SymbolLayer::getDefaultIconOffset() {
    return IconOffset::defaultValue();
}

PropertyValue<std::array<float, 2>> SymbolLayer::getIconOffset() const {
    return impl->layout.unevaluated.get<IconOffset>();
}

void SymbolLayer::setIconOffset(PropertyValue<std::array<float, 2>> value) {
    if (value == getIconOffset())
        return;
    impl->layout.unevaluated.get<IconOffset>() = value;
    impl->observer->onLayerLayoutPropertyChanged(*this, "icon-offset");
}
PropertyValue<AlignmentType> SymbolLayer::getDefaultTextPitchAlignment() {
    return TextPitchAlignment::defaultValue();
}

PropertyValue<AlignmentType> SymbolLayer::getTextPitchAlignment() const {
    return impl->layout.unevaluated.get<TextPitchAlignment>();
}

void SymbolLayer::setTextPitchAlignment(PropertyValue<AlignmentType> value) {
    if (value == getTextPitchAlignment())
        return;
    impl->layout.unevaluated.get<TextPitchAlignment>() = value;
    impl->observer->onLayerLayoutPropertyChanged(*this, "text-pitch-alignment");
}
PropertyValue<AlignmentType> SymbolLayer::getDefaultTextRotationAlignment() {
    return TextRotationAlignment::defaultValue();
}

PropertyValue<AlignmentType> SymbolLayer::getTextRotationAlignment() const {
    return impl->layout.unevaluated.get<TextRotationAlignment>();
}

void SymbolLayer::setTextRotationAlignment(PropertyValue<AlignmentType> value) {
    if (value == getTextRotationAlignment())
        return;
    impl->layout.unevaluated.get<TextRotationAlignment>() = value;
    impl->observer->onLayerLayoutPropertyChanged(*this, "text-rotation-alignment");
}
PropertyValue<std::string> SymbolLayer::getDefaultTextField() {
    return TextField::defaultValue();
}

PropertyValue<std::string> SymbolLayer::getTextField() const {
    return impl->layout.unevaluated.get<TextField>();
}

void SymbolLayer::setTextField(PropertyValue<std::string> value) {
    if (value == getTextField())
        return;
    impl->layout.unevaluated.get<TextField>() = value;
    impl->observer->onLayerLayoutPropertyChanged(*this, "text-field");
}
PropertyValue<std::vector<std::string>> SymbolLayer::getDefaultTextFont() {
    return TextFont::defaultValue();
}

PropertyValue<std::vector<std::string>> SymbolLayer::getTextFont() const {
    return impl->layout.unevaluated.get<TextFont>();
}

void SymbolLayer::setTextFont(PropertyValue<std::vector<std::string>> value) {
    if (value == getTextFont())
        return;
    impl->layout.unevaluated.get<TextFont>() = value;
    impl->observer->onLayerLayoutPropertyChanged(*this, "text-font");
}
PropertyValue<float> SymbolLayer::getDefaultTextSize() {
    return TextSize::defaultValue();
}

PropertyValue<float> SymbolLayer::getTextSize() const {
    return impl->layout.unevaluated.get<TextSize>();
}

void SymbolLayer::setTextSize(PropertyValue<float> value) {
    if (value == getTextSize())
        return;
    impl->layout.unevaluated.get<TextSize>() = value;
    impl->observer->onLayerLayoutPropertyChanged(*this, "text-size");
}
PropertyValue<float> SymbolLayer::getDefaultTextMaxWidth() {
    return TextMaxWidth::defaultValue();
}

PropertyValue<float> SymbolLayer::getTextMaxWidth() const {
    return impl->layout.unevaluated.get<TextMaxWidth>();
}

void SymbolLayer::setTextMaxWidth(PropertyValue<float> value) {
    if (value == getTextMaxWidth())
        return;
    impl->layout.unevaluated.get<TextMaxWidth>() = value;
    impl->observer->onLayerLayoutPropertyChanged(*this, "text-max-width");
}
PropertyValue<float> SymbolLayer::getDefaultTextLineHeight() {
    return TextLineHeight::defaultValue();
}

PropertyValue<float> SymbolLayer::getTextLineHeight() const {
    return impl->layout.unevaluated.get<TextLineHeight>();
}

void SymbolLayer::setTextLineHeight(PropertyValue<float> value) {
    if (value == getTextLineHeight())
        return;
    impl->layout.unevaluated.get<TextLineHeight>() = value;
    impl->observer->onLayerLayoutPropertyChanged(*this, "text-line-height");
}
PropertyValue<float> SymbolLayer::getDefaultTextLetterSpacing() {
    return TextLetterSpacing::defaultValue();
}

PropertyValue<float> SymbolLayer::getTextLetterSpacing() const {
    return impl->layout.unevaluated.get<TextLetterSpacing>();
}

void SymbolLayer::setTextLetterSpacing(PropertyValue<float> value) {
    if (value == getTextLetterSpacing())
        return;
    impl->layout.unevaluated.get<TextLetterSpacing>() = value;
    impl->observer->onLayerLayoutPropertyChanged(*this, "text-letter-spacing");
}
PropertyValue<TextJustifyType> SymbolLayer::getDefaultTextJustify() {
    return TextJustify::defaultValue();
}

PropertyValue<TextJustifyType> SymbolLayer::getTextJustify() const {
    return impl->layout.unevaluated.get<TextJustify>();
}

void SymbolLayer::setTextJustify(PropertyValue<TextJustifyType> value) {
    if (value == getTextJustify())
        return;
    impl->layout.unevaluated.get<TextJustify>() = value;
    impl->observer->onLayerLayoutPropertyChanged(*this, "text-justify");
}
PropertyValue<TextAnchorType> SymbolLayer::getDefaultTextAnchor() {
    return TextAnchor::defaultValue();
}

PropertyValue<TextAnchorType> SymbolLayer::getTextAnchor() const {
    return impl->layout.unevaluated.get<TextAnchor>();
}

void SymbolLayer::setTextAnchor(PropertyValue<TextAnchorType> value) {
    if (value == getTextAnchor())
        return;
    impl->layout.unevaluated.get<TextAnchor>() = value;
    impl->observer->onLayerLayoutPropertyChanged(*this, "text-anchor");
}
PropertyValue<float> SymbolLayer::getDefaultTextMaxAngle() {
    return TextMaxAngle::defaultValue();
}

PropertyValue<float> SymbolLayer::getTextMaxAngle() const {
    return impl->layout.unevaluated.get<TextMaxAngle>();
}

void SymbolLayer::setTextMaxAngle(PropertyValue<float> value) {
    if (value == getTextMaxAngle())
        return;
    impl->layout.unevaluated.get<TextMaxAngle>() = value;
    impl->observer->onLayerLayoutPropertyChanged(*this, "text-max-angle");
}
PropertyValue<float> SymbolLayer::getDefaultTextRotate() {
    return TextRotate::defaultValue();
}

PropertyValue<float> SymbolLayer::getTextRotate() const {
    return impl->layout.unevaluated.get<TextRotate>();
}

void SymbolLayer::setTextRotate(PropertyValue<float> value) {
    if (value == getTextRotate())
        return;
    impl->layout.unevaluated.get<TextRotate>() = value;
    impl->observer->onLayerLayoutPropertyChanged(*this, "text-rotate");
}
PropertyValue<float> SymbolLayer::getDefaultTextPadding() {
    return TextPadding::defaultValue();
}

PropertyValue<float> SymbolLayer::getTextPadding() const {
    return impl->layout.unevaluated.get<TextPadding>();
}

void SymbolLayer::setTextPadding(PropertyValue<float> value) {
    if (value == getTextPadding())
        return;
    impl->layout.unevaluated.get<TextPadding>() = value;
    impl->observer->onLayerLayoutPropertyChanged(*this, "text-padding");
}
PropertyValue<bool> SymbolLayer::getDefaultTextKeepUpright() {
    return TextKeepUpright::defaultValue();
}

PropertyValue<bool> SymbolLayer::getTextKeepUpright() const {
    return impl->layout.unevaluated.get<TextKeepUpright>();
}

void SymbolLayer::setTextKeepUpright(PropertyValue<bool> value) {
    if (value == getTextKeepUpright())
        return;
    impl->layout.unevaluated.get<TextKeepUpright>() = value;
    impl->observer->onLayerLayoutPropertyChanged(*this, "text-keep-upright");
}
PropertyValue<TextTransformType> SymbolLayer::getDefaultTextTransform() {
    return TextTransform::defaultValue();
}

PropertyValue<TextTransformType> SymbolLayer::getTextTransform() const {
    return impl->layout.unevaluated.get<TextTransform>();
}

void SymbolLayer::setTextTransform(PropertyValue<TextTransformType> value) {
    if (value == getTextTransform())
        return;
    impl->layout.unevaluated.get<TextTransform>() = value;
    impl->observer->onLayerLayoutPropertyChanged(*this, "text-transform");
}
PropertyValue<std::array<float, 2>> SymbolLayer::getDefaultTextOffset() {
    return TextOffset::defaultValue();
}

PropertyValue<std::array<float, 2>> SymbolLayer::getTextOffset() const {
    return impl->layout.unevaluated.get<TextOffset>();
}

void SymbolLayer::setTextOffset(PropertyValue<std::array<float, 2>> value) {
    if (value == getTextOffset())
        return;
    impl->layout.unevaluated.get<TextOffset>() = value;
    impl->observer->onLayerLayoutPropertyChanged(*this, "text-offset");
}
PropertyValue<bool> SymbolLayer::getDefaultTextAllowOverlap() {
    return TextAllowOverlap::defaultValue();
}

PropertyValue<bool> SymbolLayer::getTextAllowOverlap() const {
    return impl->layout.unevaluated.get<TextAllowOverlap>();
}

void SymbolLayer::setTextAllowOverlap(PropertyValue<bool> value) {
    if (value == getTextAllowOverlap())
        return;
    impl->layout.unevaluated.get<TextAllowOverlap>() = value;
    impl->observer->onLayerLayoutPropertyChanged(*this, "text-allow-overlap");
}
PropertyValue<bool> SymbolLayer::getDefaultTextIgnorePlacement() {
    return TextIgnorePlacement::defaultValue();
}

PropertyValue<bool> SymbolLayer::getTextIgnorePlacement() const {
    return impl->layout.unevaluated.get<TextIgnorePlacement>();
}

void SymbolLayer::setTextIgnorePlacement(PropertyValue<bool> value) {
    if (value == getTextIgnorePlacement())
        return;
    impl->layout.unevaluated.get<TextIgnorePlacement>() = value;
    impl->observer->onLayerLayoutPropertyChanged(*this, "text-ignore-placement");
}
PropertyValue<bool> SymbolLayer::getDefaultTextOptional() {
    return TextOptional::defaultValue();
}

PropertyValue<bool> SymbolLayer::getTextOptional() const {
    return impl->layout.unevaluated.get<TextOptional>();
}

void SymbolLayer::setTextOptional(PropertyValue<bool> value) {
    if (value == getTextOptional())
        return;
    impl->layout.unevaluated.get<TextOptional>() = value;
    impl->observer->onLayerLayoutPropertyChanged(*this, "text-optional");
}

// Paint properties

PropertyValue<float> SymbolLayer::getDefaultIconOpacity() {
    return { 1 };
}

PropertyValue<float> SymbolLayer::getIconOpacity(const optional<std::string>& klass) const {
    return impl->paint.get<IconOpacity>(klass);
}

void SymbolLayer::setIconOpacity(PropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getIconOpacity(klass))
        return;
    impl->paint.set<IconOpacity>(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<Color> SymbolLayer::getDefaultIconColor() {
    return { Color::black() };
}

PropertyValue<Color> SymbolLayer::getIconColor(const optional<std::string>& klass) const {
    return impl->paint.get<IconColor>(klass);
}

void SymbolLayer::setIconColor(PropertyValue<Color> value, const optional<std::string>& klass) {
    if (value == getIconColor(klass))
        return;
    impl->paint.set<IconColor>(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<Color> SymbolLayer::getDefaultIconHaloColor() {
    return { {} };
}

PropertyValue<Color> SymbolLayer::getIconHaloColor(const optional<std::string>& klass) const {
    return impl->paint.get<IconHaloColor>(klass);
}

void SymbolLayer::setIconHaloColor(PropertyValue<Color> value, const optional<std::string>& klass) {
    if (value == getIconHaloColor(klass))
        return;
    impl->paint.set<IconHaloColor>(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<float> SymbolLayer::getDefaultIconHaloWidth() {
    return { 0 };
}

PropertyValue<float> SymbolLayer::getIconHaloWidth(const optional<std::string>& klass) const {
    return impl->paint.get<IconHaloWidth>(klass);
}

void SymbolLayer::setIconHaloWidth(PropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getIconHaloWidth(klass))
        return;
    impl->paint.set<IconHaloWidth>(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<float> SymbolLayer::getDefaultIconHaloBlur() {
    return { 0 };
}

PropertyValue<float> SymbolLayer::getIconHaloBlur(const optional<std::string>& klass) const {
    return impl->paint.get<IconHaloBlur>(klass);
}

void SymbolLayer::setIconHaloBlur(PropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getIconHaloBlur(klass))
        return;
    impl->paint.set<IconHaloBlur>(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<std::array<float, 2>> SymbolLayer::getDefaultIconTranslate() {
    return { {{ 0, 0 }} };
}

PropertyValue<std::array<float, 2>> SymbolLayer::getIconTranslate(const optional<std::string>& klass) const {
    return impl->paint.get<IconTranslate>(klass);
}

void SymbolLayer::setIconTranslate(PropertyValue<std::array<float, 2>> value, const optional<std::string>& klass) {
    if (value == getIconTranslate(klass))
        return;
    impl->paint.set<IconTranslate>(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<TranslateAnchorType> SymbolLayer::getDefaultIconTranslateAnchor() {
    return { TranslateAnchorType::Map };
}

PropertyValue<TranslateAnchorType> SymbolLayer::getIconTranslateAnchor(const optional<std::string>& klass) const {
    return impl->paint.get<IconTranslateAnchor>(klass);
}

void SymbolLayer::setIconTranslateAnchor(PropertyValue<TranslateAnchorType> value, const optional<std::string>& klass) {
    if (value == getIconTranslateAnchor(klass))
        return;
    impl->paint.set<IconTranslateAnchor>(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<float> SymbolLayer::getDefaultTextOpacity() {
    return { 1 };
}

PropertyValue<float> SymbolLayer::getTextOpacity(const optional<std::string>& klass) const {
    return impl->paint.get<TextOpacity>(klass);
}

void SymbolLayer::setTextOpacity(PropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getTextOpacity(klass))
        return;
    impl->paint.set<TextOpacity>(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<Color> SymbolLayer::getDefaultTextColor() {
    return { Color::black() };
}

PropertyValue<Color> SymbolLayer::getTextColor(const optional<std::string>& klass) const {
    return impl->paint.get<TextColor>(klass);
}

void SymbolLayer::setTextColor(PropertyValue<Color> value, const optional<std::string>& klass) {
    if (value == getTextColor(klass))
        return;
    impl->paint.set<TextColor>(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<Color> SymbolLayer::getDefaultTextHaloColor() {
    return { {} };
}

PropertyValue<Color> SymbolLayer::getTextHaloColor(const optional<std::string>& klass) const {
    return impl->paint.get<TextHaloColor>(klass);
}

void SymbolLayer::setTextHaloColor(PropertyValue<Color> value, const optional<std::string>& klass) {
    if (value == getTextHaloColor(klass))
        return;
    impl->paint.set<TextHaloColor>(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<float> SymbolLayer::getDefaultTextHaloWidth() {
    return { 0 };
}

PropertyValue<float> SymbolLayer::getTextHaloWidth(const optional<std::string>& klass) const {
    return impl->paint.get<TextHaloWidth>(klass);
}

void SymbolLayer::setTextHaloWidth(PropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getTextHaloWidth(klass))
        return;
    impl->paint.set<TextHaloWidth>(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<float> SymbolLayer::getDefaultTextHaloBlur() {
    return { 0 };
}

PropertyValue<float> SymbolLayer::getTextHaloBlur(const optional<std::string>& klass) const {
    return impl->paint.get<TextHaloBlur>(klass);
}

void SymbolLayer::setTextHaloBlur(PropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getTextHaloBlur(klass))
        return;
    impl->paint.set<TextHaloBlur>(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<std::array<float, 2>> SymbolLayer::getDefaultTextTranslate() {
    return { {{ 0, 0 }} };
}

PropertyValue<std::array<float, 2>> SymbolLayer::getTextTranslate(const optional<std::string>& klass) const {
    return impl->paint.get<TextTranslate>(klass);
}

void SymbolLayer::setTextTranslate(PropertyValue<std::array<float, 2>> value, const optional<std::string>& klass) {
    if (value == getTextTranslate(klass))
        return;
    impl->paint.set<TextTranslate>(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

PropertyValue<TranslateAnchorType> SymbolLayer::getDefaultTextTranslateAnchor() {
    return { TranslateAnchorType::Map };
}

PropertyValue<TranslateAnchorType> SymbolLayer::getTextTranslateAnchor(const optional<std::string>& klass) const {
    return impl->paint.get<TextTranslateAnchor>(klass);
}

void SymbolLayer::setTextTranslateAnchor(PropertyValue<TranslateAnchorType> value, const optional<std::string>& klass) {
    if (value == getTextTranslateAnchor(klass))
        return;
    impl->paint.set<TextTranslateAnchor>(value, klass);
    impl->observer->onLayerPaintPropertyChanged(*this);
}

} // namespace style
} // namespace mbgl
