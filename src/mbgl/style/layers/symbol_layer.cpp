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
DataDrivenPropertyValue<float> SymbolLayer::getDefaultIconSize() {
    return IconSize::defaultValue();
}

DataDrivenPropertyValue<float> SymbolLayer::getIconSize() const {
    return impl->layout.unevaluated.get<IconSize>();
}

void SymbolLayer::setIconSize(DataDrivenPropertyValue<float> value) {
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
DataDrivenPropertyValue<std::string> SymbolLayer::getDefaultIconImage() {
    return IconImage::defaultValue();
}

DataDrivenPropertyValue<std::string> SymbolLayer::getIconImage() const {
    return impl->layout.unevaluated.get<IconImage>();
}

void SymbolLayer::setIconImage(DataDrivenPropertyValue<std::string> value) {
    if (value == getIconImage())
        return;
    impl->layout.unevaluated.get<IconImage>() = value;
    impl->observer->onLayerLayoutPropertyChanged(*this, "icon-image");
}
DataDrivenPropertyValue<float> SymbolLayer::getDefaultIconRotate() {
    return IconRotate::defaultValue();
}

DataDrivenPropertyValue<float> SymbolLayer::getIconRotate() const {
    return impl->layout.unevaluated.get<IconRotate>();
}

void SymbolLayer::setIconRotate(DataDrivenPropertyValue<float> value) {
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
DataDrivenPropertyValue<std::array<float, 2>> SymbolLayer::getDefaultIconOffset() {
    return IconOffset::defaultValue();
}

DataDrivenPropertyValue<std::array<float, 2>> SymbolLayer::getIconOffset() const {
    return impl->layout.unevaluated.get<IconOffset>();
}

void SymbolLayer::setIconOffset(DataDrivenPropertyValue<std::array<float, 2>> value) {
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
DataDrivenPropertyValue<std::string> SymbolLayer::getDefaultTextField() {
    return TextField::defaultValue();
}

DataDrivenPropertyValue<std::string> SymbolLayer::getTextField() const {
    return impl->layout.unevaluated.get<TextField>();
}

void SymbolLayer::setTextField(DataDrivenPropertyValue<std::string> value) {
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
DataDrivenPropertyValue<float> SymbolLayer::getDefaultTextSize() {
    return TextSize::defaultValue();
}

DataDrivenPropertyValue<float> SymbolLayer::getTextSize() const {
    return impl->layout.unevaluated.get<TextSize>();
}

void SymbolLayer::setTextSize(DataDrivenPropertyValue<float> value) {
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
DataDrivenPropertyValue<float> SymbolLayer::getDefaultTextRotate() {
    return TextRotate::defaultValue();
}

DataDrivenPropertyValue<float> SymbolLayer::getTextRotate() const {
    return impl->layout.unevaluated.get<TextRotate>();
}

void SymbolLayer::setTextRotate(DataDrivenPropertyValue<float> value) {
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
DataDrivenPropertyValue<TextTransformType> SymbolLayer::getDefaultTextTransform() {
    return TextTransform::defaultValue();
}

DataDrivenPropertyValue<TextTransformType> SymbolLayer::getTextTransform() const {
    return impl->layout.unevaluated.get<TextTransform>();
}

void SymbolLayer::setTextTransform(DataDrivenPropertyValue<TextTransformType> value) {
    if (value == getTextTransform())
        return;
    impl->layout.unevaluated.get<TextTransform>() = value;
    impl->observer->onLayerLayoutPropertyChanged(*this, "text-transform");
}
DataDrivenPropertyValue<std::array<float, 2>> SymbolLayer::getDefaultTextOffset() {
    return TextOffset::defaultValue();
}

DataDrivenPropertyValue<std::array<float, 2>> SymbolLayer::getTextOffset() const {
    return impl->layout.unevaluated.get<TextOffset>();
}

void SymbolLayer::setTextOffset(DataDrivenPropertyValue<std::array<float, 2>> value) {
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

DataDrivenPropertyValue<float> SymbolLayer::getDefaultIconOpacity() {
    return { 1 };
}

DataDrivenPropertyValue<float> SymbolLayer::getIconOpacity(const optional<std::string>& klass) const {
    return impl->paint.get<IconOpacity>(klass);
}

void SymbolLayer::setIconOpacity(DataDrivenPropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getIconOpacity(klass))
        return;
    impl->paint.set<IconOpacity>(value, klass);
    if (value.isDataDriven()) {
        impl->observer->onLayerDataDrivenPaintPropertyChanged(*this);
    } else {
        impl->observer->onLayerPaintPropertyChanged(*this);
    }
}

void SymbolLayer::setIconOpacityTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    impl->paint.setTransition<IconOpacity>(value, klass);
}

TransitionOptions SymbolLayer::getIconOpacityTransition(const optional<std::string>& klass) const {
    return impl->paint.getTransition<IconOpacity>(klass);
}

DataDrivenPropertyValue<Color> SymbolLayer::getDefaultIconColor() {
    return { Color::black() };
}

DataDrivenPropertyValue<Color> SymbolLayer::getIconColor(const optional<std::string>& klass) const {
    return impl->paint.get<IconColor>(klass);
}

void SymbolLayer::setIconColor(DataDrivenPropertyValue<Color> value, const optional<std::string>& klass) {
    if (value == getIconColor(klass))
        return;
    impl->paint.set<IconColor>(value, klass);
    if (value.isDataDriven()) {
        impl->observer->onLayerDataDrivenPaintPropertyChanged(*this);
    } else {
        impl->observer->onLayerPaintPropertyChanged(*this);
    }
}

void SymbolLayer::setIconColorTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    impl->paint.setTransition<IconColor>(value, klass);
}

TransitionOptions SymbolLayer::getIconColorTransition(const optional<std::string>& klass) const {
    return impl->paint.getTransition<IconColor>(klass);
}

DataDrivenPropertyValue<Color> SymbolLayer::getDefaultIconHaloColor() {
    return { {} };
}

DataDrivenPropertyValue<Color> SymbolLayer::getIconHaloColor(const optional<std::string>& klass) const {
    return impl->paint.get<IconHaloColor>(klass);
}

void SymbolLayer::setIconHaloColor(DataDrivenPropertyValue<Color> value, const optional<std::string>& klass) {
    if (value == getIconHaloColor(klass))
        return;
    impl->paint.set<IconHaloColor>(value, klass);
    if (value.isDataDriven()) {
        impl->observer->onLayerDataDrivenPaintPropertyChanged(*this);
    } else {
        impl->observer->onLayerPaintPropertyChanged(*this);
    }
}

void SymbolLayer::setIconHaloColorTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    impl->paint.setTransition<IconHaloColor>(value, klass);
}

TransitionOptions SymbolLayer::getIconHaloColorTransition(const optional<std::string>& klass) const {
    return impl->paint.getTransition<IconHaloColor>(klass);
}

DataDrivenPropertyValue<float> SymbolLayer::getDefaultIconHaloWidth() {
    return { 0 };
}

DataDrivenPropertyValue<float> SymbolLayer::getIconHaloWidth(const optional<std::string>& klass) const {
    return impl->paint.get<IconHaloWidth>(klass);
}

void SymbolLayer::setIconHaloWidth(DataDrivenPropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getIconHaloWidth(klass))
        return;
    impl->paint.set<IconHaloWidth>(value, klass);
    if (value.isDataDriven()) {
        impl->observer->onLayerDataDrivenPaintPropertyChanged(*this);
    } else {
        impl->observer->onLayerPaintPropertyChanged(*this);
    }
}

void SymbolLayer::setIconHaloWidthTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    impl->paint.setTransition<IconHaloWidth>(value, klass);
}

TransitionOptions SymbolLayer::getIconHaloWidthTransition(const optional<std::string>& klass) const {
    return impl->paint.getTransition<IconHaloWidth>(klass);
}

DataDrivenPropertyValue<float> SymbolLayer::getDefaultIconHaloBlur() {
    return { 0 };
}

DataDrivenPropertyValue<float> SymbolLayer::getIconHaloBlur(const optional<std::string>& klass) const {
    return impl->paint.get<IconHaloBlur>(klass);
}

void SymbolLayer::setIconHaloBlur(DataDrivenPropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getIconHaloBlur(klass))
        return;
    impl->paint.set<IconHaloBlur>(value, klass);
    if (value.isDataDriven()) {
        impl->observer->onLayerDataDrivenPaintPropertyChanged(*this);
    } else {
        impl->observer->onLayerPaintPropertyChanged(*this);
    }
}

void SymbolLayer::setIconHaloBlurTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    impl->paint.setTransition<IconHaloBlur>(value, klass);
}

TransitionOptions SymbolLayer::getIconHaloBlurTransition(const optional<std::string>& klass) const {
    return impl->paint.getTransition<IconHaloBlur>(klass);
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

void SymbolLayer::setIconTranslateTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    impl->paint.setTransition<IconTranslate>(value, klass);
}

TransitionOptions SymbolLayer::getIconTranslateTransition(const optional<std::string>& klass) const {
    return impl->paint.getTransition<IconTranslate>(klass);
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

void SymbolLayer::setIconTranslateAnchorTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    impl->paint.setTransition<IconTranslateAnchor>(value, klass);
}

TransitionOptions SymbolLayer::getIconTranslateAnchorTransition(const optional<std::string>& klass) const {
    return impl->paint.getTransition<IconTranslateAnchor>(klass);
}

DataDrivenPropertyValue<float> SymbolLayer::getDefaultTextOpacity() {
    return { 1 };
}

DataDrivenPropertyValue<float> SymbolLayer::getTextOpacity(const optional<std::string>& klass) const {
    return impl->paint.get<TextOpacity>(klass);
}

void SymbolLayer::setTextOpacity(DataDrivenPropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getTextOpacity(klass))
        return;
    impl->paint.set<TextOpacity>(value, klass);
    if (value.isDataDriven()) {
        impl->observer->onLayerDataDrivenPaintPropertyChanged(*this);
    } else {
        impl->observer->onLayerPaintPropertyChanged(*this);
    }
}

void SymbolLayer::setTextOpacityTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    impl->paint.setTransition<TextOpacity>(value, klass);
}

TransitionOptions SymbolLayer::getTextOpacityTransition(const optional<std::string>& klass) const {
    return impl->paint.getTransition<TextOpacity>(klass);
}

DataDrivenPropertyValue<Color> SymbolLayer::getDefaultTextColor() {
    return { Color::black() };
}

DataDrivenPropertyValue<Color> SymbolLayer::getTextColor(const optional<std::string>& klass) const {
    return impl->paint.get<TextColor>(klass);
}

void SymbolLayer::setTextColor(DataDrivenPropertyValue<Color> value, const optional<std::string>& klass) {
    if (value == getTextColor(klass))
        return;
    impl->paint.set<TextColor>(value, klass);
    if (value.isDataDriven()) {
        impl->observer->onLayerDataDrivenPaintPropertyChanged(*this);
    } else {
        impl->observer->onLayerPaintPropertyChanged(*this);
    }
}

void SymbolLayer::setTextColorTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    impl->paint.setTransition<TextColor>(value, klass);
}

TransitionOptions SymbolLayer::getTextColorTransition(const optional<std::string>& klass) const {
    return impl->paint.getTransition<TextColor>(klass);
}

DataDrivenPropertyValue<Color> SymbolLayer::getDefaultTextHaloColor() {
    return { {} };
}

DataDrivenPropertyValue<Color> SymbolLayer::getTextHaloColor(const optional<std::string>& klass) const {
    return impl->paint.get<TextHaloColor>(klass);
}

void SymbolLayer::setTextHaloColor(DataDrivenPropertyValue<Color> value, const optional<std::string>& klass) {
    if (value == getTextHaloColor(klass))
        return;
    impl->paint.set<TextHaloColor>(value, klass);
    if (value.isDataDriven()) {
        impl->observer->onLayerDataDrivenPaintPropertyChanged(*this);
    } else {
        impl->observer->onLayerPaintPropertyChanged(*this);
    }
}

void SymbolLayer::setTextHaloColorTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    impl->paint.setTransition<TextHaloColor>(value, klass);
}

TransitionOptions SymbolLayer::getTextHaloColorTransition(const optional<std::string>& klass) const {
    return impl->paint.getTransition<TextHaloColor>(klass);
}

DataDrivenPropertyValue<float> SymbolLayer::getDefaultTextHaloWidth() {
    return { 0 };
}

DataDrivenPropertyValue<float> SymbolLayer::getTextHaloWidth(const optional<std::string>& klass) const {
    return impl->paint.get<TextHaloWidth>(klass);
}

void SymbolLayer::setTextHaloWidth(DataDrivenPropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getTextHaloWidth(klass))
        return;
    impl->paint.set<TextHaloWidth>(value, klass);
    if (value.isDataDriven()) {
        impl->observer->onLayerDataDrivenPaintPropertyChanged(*this);
    } else {
        impl->observer->onLayerPaintPropertyChanged(*this);
    }
}

void SymbolLayer::setTextHaloWidthTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    impl->paint.setTransition<TextHaloWidth>(value, klass);
}

TransitionOptions SymbolLayer::getTextHaloWidthTransition(const optional<std::string>& klass) const {
    return impl->paint.getTransition<TextHaloWidth>(klass);
}

DataDrivenPropertyValue<float> SymbolLayer::getDefaultTextHaloBlur() {
    return { 0 };
}

DataDrivenPropertyValue<float> SymbolLayer::getTextHaloBlur(const optional<std::string>& klass) const {
    return impl->paint.get<TextHaloBlur>(klass);
}

void SymbolLayer::setTextHaloBlur(DataDrivenPropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getTextHaloBlur(klass))
        return;
    impl->paint.set<TextHaloBlur>(value, klass);
    if (value.isDataDriven()) {
        impl->observer->onLayerDataDrivenPaintPropertyChanged(*this);
    } else {
        impl->observer->onLayerPaintPropertyChanged(*this);
    }
}

void SymbolLayer::setTextHaloBlurTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    impl->paint.setTransition<TextHaloBlur>(value, klass);
}

TransitionOptions SymbolLayer::getTextHaloBlurTransition(const optional<std::string>& klass) const {
    return impl->paint.getTransition<TextHaloBlur>(klass);
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

void SymbolLayer::setTextTranslateTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    impl->paint.setTransition<TextTranslate>(value, klass);
}

TransitionOptions SymbolLayer::getTextTranslateTransition(const optional<std::string>& klass) const {
    return impl->paint.getTransition<TextTranslate>(klass);
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

void SymbolLayer::setTextTranslateAnchorTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    impl->paint.setTransition<TextTranslateAnchor>(value, klass);
}

TransitionOptions SymbolLayer::getTextTranslateAnchorTransition(const optional<std::string>& klass) const {
    return impl->paint.getTransition<TextTranslateAnchor>(klass);
}

} // namespace style
} // namespace mbgl
