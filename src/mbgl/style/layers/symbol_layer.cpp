// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/style/layers/symbol_layer_impl.hpp>
#include <mbgl/style/layer_observer.hpp>

namespace mbgl {
namespace style {

SymbolLayer::SymbolLayer(const std::string& layerID, const std::string& sourceID)
    : Layer(makeMutable<Impl>(LayerType::Symbol, layerID, sourceID)) {
}

SymbolLayer::SymbolLayer(Immutable<Impl> impl_)
    : Layer(std::move(impl_)) {
}

SymbolLayer::~SymbolLayer() = default;

const SymbolLayer::Impl& SymbolLayer::impl() const {
    return static_cast<const Impl&>(*baseImpl);
}

Mutable<SymbolLayer::Impl> SymbolLayer::mutableImpl() const {
    return makeMutable<Impl>(impl());
}

std::unique_ptr<Layer> SymbolLayer::cloneRef(const std::string& id_) const {
    auto impl_ = mutableImpl();
    impl_->id = id_;
    impl_->paint = SymbolPaintProperties::Transitionable();
    return std::make_unique<SymbolLayer>(std::move(impl_));
}

void SymbolLayer::Impl::stringifyLayout(rapidjson::Writer<rapidjson::StringBuffer>& writer) const {
    layout.stringify(writer);
}

// Source

const std::string& SymbolLayer::getSourceID() const {
    return impl().source;
}

void SymbolLayer::setSourceLayer(const std::string& sourceLayer) {
    auto impl_ = mutableImpl();
    impl_->sourceLayer = sourceLayer;
    baseImpl = std::move(impl_);
}

const std::string& SymbolLayer::getSourceLayer() const {
    return impl().sourceLayer;
}

// Filter

void SymbolLayer::setFilter(const Filter& filter) {
    auto impl_ = mutableImpl();
    impl_->filter = filter;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

const Filter& SymbolLayer::getFilter() const {
    return impl().filter;
}

// Visibility

void SymbolLayer::setVisibility(VisibilityType value) {
    if (value == getVisibility())
        return;
    auto impl_ = mutableImpl();
    impl_->visibility = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

// Zoom range

void SymbolLayer::setMinZoom(float minZoom) {
    auto impl_ = mutableImpl();
    impl_->minZoom = minZoom;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void SymbolLayer::setMaxZoom(float maxZoom) {
    auto impl_ = mutableImpl();
    impl_->maxZoom = maxZoom;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

// Layout properties

PropertyValue<SymbolPlacementType> SymbolLayer::getDefaultSymbolPlacement() {
    return SymbolPlacement::defaultValue();
}

PropertyValue<SymbolPlacementType> SymbolLayer::getSymbolPlacement() const {
    return impl().layout.get<SymbolPlacement>();
}

void SymbolLayer::setSymbolPlacement(PropertyValue<SymbolPlacementType> value) {
    if (value == getSymbolPlacement())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.get<SymbolPlacement>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
PropertyValue<float> SymbolLayer::getDefaultSymbolSpacing() {
    return SymbolSpacing::defaultValue();
}

PropertyValue<float> SymbolLayer::getSymbolSpacing() const {
    return impl().layout.get<SymbolSpacing>();
}

void SymbolLayer::setSymbolSpacing(PropertyValue<float> value) {
    if (value == getSymbolSpacing())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.get<SymbolSpacing>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
PropertyValue<bool> SymbolLayer::getDefaultSymbolAvoidEdges() {
    return SymbolAvoidEdges::defaultValue();
}

PropertyValue<bool> SymbolLayer::getSymbolAvoidEdges() const {
    return impl().layout.get<SymbolAvoidEdges>();
}

void SymbolLayer::setSymbolAvoidEdges(PropertyValue<bool> value) {
    if (value == getSymbolAvoidEdges())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.get<SymbolAvoidEdges>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
PropertyValue<bool> SymbolLayer::getDefaultIconAllowOverlap() {
    return IconAllowOverlap::defaultValue();
}

PropertyValue<bool> SymbolLayer::getIconAllowOverlap() const {
    return impl().layout.get<IconAllowOverlap>();
}

void SymbolLayer::setIconAllowOverlap(PropertyValue<bool> value) {
    if (value == getIconAllowOverlap())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.get<IconAllowOverlap>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
PropertyValue<bool> SymbolLayer::getDefaultIconIgnorePlacement() {
    return IconIgnorePlacement::defaultValue();
}

PropertyValue<bool> SymbolLayer::getIconIgnorePlacement() const {
    return impl().layout.get<IconIgnorePlacement>();
}

void SymbolLayer::setIconIgnorePlacement(PropertyValue<bool> value) {
    if (value == getIconIgnorePlacement())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.get<IconIgnorePlacement>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
PropertyValue<bool> SymbolLayer::getDefaultIconOptional() {
    return IconOptional::defaultValue();
}

PropertyValue<bool> SymbolLayer::getIconOptional() const {
    return impl().layout.get<IconOptional>();
}

void SymbolLayer::setIconOptional(PropertyValue<bool> value) {
    if (value == getIconOptional())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.get<IconOptional>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
PropertyValue<AlignmentType> SymbolLayer::getDefaultIconRotationAlignment() {
    return IconRotationAlignment::defaultValue();
}

PropertyValue<AlignmentType> SymbolLayer::getIconRotationAlignment() const {
    return impl().layout.get<IconRotationAlignment>();
}

void SymbolLayer::setIconRotationAlignment(PropertyValue<AlignmentType> value) {
    if (value == getIconRotationAlignment())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.get<IconRotationAlignment>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
DataDrivenPropertyValue<float> SymbolLayer::getDefaultIconSize() {
    return IconSize::defaultValue();
}

DataDrivenPropertyValue<float> SymbolLayer::getIconSize() const {
    return impl().layout.get<IconSize>();
}

void SymbolLayer::setIconSize(DataDrivenPropertyValue<float> value) {
    if (value == getIconSize())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.get<IconSize>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
PropertyValue<IconTextFitType> SymbolLayer::getDefaultIconTextFit() {
    return IconTextFit::defaultValue();
}

PropertyValue<IconTextFitType> SymbolLayer::getIconTextFit() const {
    return impl().layout.get<IconTextFit>();
}

void SymbolLayer::setIconTextFit(PropertyValue<IconTextFitType> value) {
    if (value == getIconTextFit())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.get<IconTextFit>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
PropertyValue<std::array<float, 4>> SymbolLayer::getDefaultIconTextFitPadding() {
    return IconTextFitPadding::defaultValue();
}

PropertyValue<std::array<float, 4>> SymbolLayer::getIconTextFitPadding() const {
    return impl().layout.get<IconTextFitPadding>();
}

void SymbolLayer::setIconTextFitPadding(PropertyValue<std::array<float, 4>> value) {
    if (value == getIconTextFitPadding())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.get<IconTextFitPadding>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
DataDrivenPropertyValue<std::string> SymbolLayer::getDefaultIconImage() {
    return IconImage::defaultValue();
}

DataDrivenPropertyValue<std::string> SymbolLayer::getIconImage() const {
    return impl().layout.get<IconImage>();
}

void SymbolLayer::setIconImage(DataDrivenPropertyValue<std::string> value) {
    if (value == getIconImage())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.get<IconImage>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
DataDrivenPropertyValue<float> SymbolLayer::getDefaultIconRotate() {
    return IconRotate::defaultValue();
}

DataDrivenPropertyValue<float> SymbolLayer::getIconRotate() const {
    return impl().layout.get<IconRotate>();
}

void SymbolLayer::setIconRotate(DataDrivenPropertyValue<float> value) {
    if (value == getIconRotate())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.get<IconRotate>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
PropertyValue<float> SymbolLayer::getDefaultIconPadding() {
    return IconPadding::defaultValue();
}

PropertyValue<float> SymbolLayer::getIconPadding() const {
    return impl().layout.get<IconPadding>();
}

void SymbolLayer::setIconPadding(PropertyValue<float> value) {
    if (value == getIconPadding())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.get<IconPadding>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
PropertyValue<bool> SymbolLayer::getDefaultIconKeepUpright() {
    return IconKeepUpright::defaultValue();
}

PropertyValue<bool> SymbolLayer::getIconKeepUpright() const {
    return impl().layout.get<IconKeepUpright>();
}

void SymbolLayer::setIconKeepUpright(PropertyValue<bool> value) {
    if (value == getIconKeepUpright())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.get<IconKeepUpright>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
DataDrivenPropertyValue<std::array<float, 2>> SymbolLayer::getDefaultIconOffset() {
    return IconOffset::defaultValue();
}

DataDrivenPropertyValue<std::array<float, 2>> SymbolLayer::getIconOffset() const {
    return impl().layout.get<IconOffset>();
}

void SymbolLayer::setIconOffset(DataDrivenPropertyValue<std::array<float, 2>> value) {
    if (value == getIconOffset())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.get<IconOffset>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
DataDrivenPropertyValue<SymbolAnchorType> SymbolLayer::getDefaultIconAnchor() {
    return IconAnchor::defaultValue();
}

DataDrivenPropertyValue<SymbolAnchorType> SymbolLayer::getIconAnchor() const {
    return impl().layout.get<IconAnchor>();
}

void SymbolLayer::setIconAnchor(DataDrivenPropertyValue<SymbolAnchorType> value) {
    if (value == getIconAnchor())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.get<IconAnchor>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
PropertyValue<AlignmentType> SymbolLayer::getDefaultIconPitchAlignment() {
    return IconPitchAlignment::defaultValue();
}

PropertyValue<AlignmentType> SymbolLayer::getIconPitchAlignment() const {
    return impl().layout.get<IconPitchAlignment>();
}

void SymbolLayer::setIconPitchAlignment(PropertyValue<AlignmentType> value) {
    if (value == getIconPitchAlignment())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.get<IconPitchAlignment>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
PropertyValue<AlignmentType> SymbolLayer::getDefaultTextPitchAlignment() {
    return TextPitchAlignment::defaultValue();
}

PropertyValue<AlignmentType> SymbolLayer::getTextPitchAlignment() const {
    return impl().layout.get<TextPitchAlignment>();
}

void SymbolLayer::setTextPitchAlignment(PropertyValue<AlignmentType> value) {
    if (value == getTextPitchAlignment())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.get<TextPitchAlignment>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
PropertyValue<AlignmentType> SymbolLayer::getDefaultTextRotationAlignment() {
    return TextRotationAlignment::defaultValue();
}

PropertyValue<AlignmentType> SymbolLayer::getTextRotationAlignment() const {
    return impl().layout.get<TextRotationAlignment>();
}

void SymbolLayer::setTextRotationAlignment(PropertyValue<AlignmentType> value) {
    if (value == getTextRotationAlignment())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.get<TextRotationAlignment>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
DataDrivenPropertyValue<std::string> SymbolLayer::getDefaultTextField() {
    return TextField::defaultValue();
}

DataDrivenPropertyValue<std::string> SymbolLayer::getTextField() const {
    return impl().layout.get<TextField>();
}

void SymbolLayer::setTextField(DataDrivenPropertyValue<std::string> value) {
    if (value == getTextField())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.get<TextField>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
DataDrivenPropertyValue<std::vector<std::string>> SymbolLayer::getDefaultTextFont() {
    return TextFont::defaultValue();
}

DataDrivenPropertyValue<std::vector<std::string>> SymbolLayer::getTextFont() const {
    return impl().layout.get<TextFont>();
}

void SymbolLayer::setTextFont(DataDrivenPropertyValue<std::vector<std::string>> value) {
    if (value == getTextFont())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.get<TextFont>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
DataDrivenPropertyValue<float> SymbolLayer::getDefaultTextSize() {
    return TextSize::defaultValue();
}

DataDrivenPropertyValue<float> SymbolLayer::getTextSize() const {
    return impl().layout.get<TextSize>();
}

void SymbolLayer::setTextSize(DataDrivenPropertyValue<float> value) {
    if (value == getTextSize())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.get<TextSize>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
DataDrivenPropertyValue<float> SymbolLayer::getDefaultTextMaxWidth() {
    return TextMaxWidth::defaultValue();
}

DataDrivenPropertyValue<float> SymbolLayer::getTextMaxWidth() const {
    return impl().layout.get<TextMaxWidth>();
}

void SymbolLayer::setTextMaxWidth(DataDrivenPropertyValue<float> value) {
    if (value == getTextMaxWidth())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.get<TextMaxWidth>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
PropertyValue<float> SymbolLayer::getDefaultTextLineHeight() {
    return TextLineHeight::defaultValue();
}

PropertyValue<float> SymbolLayer::getTextLineHeight() const {
    return impl().layout.get<TextLineHeight>();
}

void SymbolLayer::setTextLineHeight(PropertyValue<float> value) {
    if (value == getTextLineHeight())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.get<TextLineHeight>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
DataDrivenPropertyValue<float> SymbolLayer::getDefaultTextLetterSpacing() {
    return TextLetterSpacing::defaultValue();
}

DataDrivenPropertyValue<float> SymbolLayer::getTextLetterSpacing() const {
    return impl().layout.get<TextLetterSpacing>();
}

void SymbolLayer::setTextLetterSpacing(DataDrivenPropertyValue<float> value) {
    if (value == getTextLetterSpacing())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.get<TextLetterSpacing>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
DataDrivenPropertyValue<TextJustifyType> SymbolLayer::getDefaultTextJustify() {
    return TextJustify::defaultValue();
}

DataDrivenPropertyValue<TextJustifyType> SymbolLayer::getTextJustify() const {
    return impl().layout.get<TextJustify>();
}

void SymbolLayer::setTextJustify(DataDrivenPropertyValue<TextJustifyType> value) {
    if (value == getTextJustify())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.get<TextJustify>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
DataDrivenPropertyValue<SymbolAnchorType> SymbolLayer::getDefaultTextAnchor() {
    return TextAnchor::defaultValue();
}

DataDrivenPropertyValue<SymbolAnchorType> SymbolLayer::getTextAnchor() const {
    return impl().layout.get<TextAnchor>();
}

void SymbolLayer::setTextAnchor(DataDrivenPropertyValue<SymbolAnchorType> value) {
    if (value == getTextAnchor())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.get<TextAnchor>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
PropertyValue<float> SymbolLayer::getDefaultTextMaxAngle() {
    return TextMaxAngle::defaultValue();
}

PropertyValue<float> SymbolLayer::getTextMaxAngle() const {
    return impl().layout.get<TextMaxAngle>();
}

void SymbolLayer::setTextMaxAngle(PropertyValue<float> value) {
    if (value == getTextMaxAngle())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.get<TextMaxAngle>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
DataDrivenPropertyValue<float> SymbolLayer::getDefaultTextRotate() {
    return TextRotate::defaultValue();
}

DataDrivenPropertyValue<float> SymbolLayer::getTextRotate() const {
    return impl().layout.get<TextRotate>();
}

void SymbolLayer::setTextRotate(DataDrivenPropertyValue<float> value) {
    if (value == getTextRotate())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.get<TextRotate>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
PropertyValue<float> SymbolLayer::getDefaultTextPadding() {
    return TextPadding::defaultValue();
}

PropertyValue<float> SymbolLayer::getTextPadding() const {
    return impl().layout.get<TextPadding>();
}

void SymbolLayer::setTextPadding(PropertyValue<float> value) {
    if (value == getTextPadding())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.get<TextPadding>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
PropertyValue<bool> SymbolLayer::getDefaultTextKeepUpright() {
    return TextKeepUpright::defaultValue();
}

PropertyValue<bool> SymbolLayer::getTextKeepUpright() const {
    return impl().layout.get<TextKeepUpright>();
}

void SymbolLayer::setTextKeepUpright(PropertyValue<bool> value) {
    if (value == getTextKeepUpright())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.get<TextKeepUpright>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
DataDrivenPropertyValue<TextTransformType> SymbolLayer::getDefaultTextTransform() {
    return TextTransform::defaultValue();
}

DataDrivenPropertyValue<TextTransformType> SymbolLayer::getTextTransform() const {
    return impl().layout.get<TextTransform>();
}

void SymbolLayer::setTextTransform(DataDrivenPropertyValue<TextTransformType> value) {
    if (value == getTextTransform())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.get<TextTransform>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
DataDrivenPropertyValue<std::array<float, 2>> SymbolLayer::getDefaultTextOffset() {
    return TextOffset::defaultValue();
}

DataDrivenPropertyValue<std::array<float, 2>> SymbolLayer::getTextOffset() const {
    return impl().layout.get<TextOffset>();
}

void SymbolLayer::setTextOffset(DataDrivenPropertyValue<std::array<float, 2>> value) {
    if (value == getTextOffset())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.get<TextOffset>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
PropertyValue<bool> SymbolLayer::getDefaultTextAllowOverlap() {
    return TextAllowOverlap::defaultValue();
}

PropertyValue<bool> SymbolLayer::getTextAllowOverlap() const {
    return impl().layout.get<TextAllowOverlap>();
}

void SymbolLayer::setTextAllowOverlap(PropertyValue<bool> value) {
    if (value == getTextAllowOverlap())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.get<TextAllowOverlap>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
PropertyValue<bool> SymbolLayer::getDefaultTextIgnorePlacement() {
    return TextIgnorePlacement::defaultValue();
}

PropertyValue<bool> SymbolLayer::getTextIgnorePlacement() const {
    return impl().layout.get<TextIgnorePlacement>();
}

void SymbolLayer::setTextIgnorePlacement(PropertyValue<bool> value) {
    if (value == getTextIgnorePlacement())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.get<TextIgnorePlacement>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
PropertyValue<bool> SymbolLayer::getDefaultTextOptional() {
    return TextOptional::defaultValue();
}

PropertyValue<bool> SymbolLayer::getTextOptional() const {
    return impl().layout.get<TextOptional>();
}

void SymbolLayer::setTextOptional(PropertyValue<bool> value) {
    if (value == getTextOptional())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.get<TextOptional>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

// Paint properties

DataDrivenPropertyValue<float> SymbolLayer::getDefaultIconOpacity() {
    return { 1 };
}

DataDrivenPropertyValue<float> SymbolLayer::getIconOpacity() const {
    return impl().paint.template get<IconOpacity>().value;
}

void SymbolLayer::setIconOpacity(DataDrivenPropertyValue<float> value) {
    if (value == getIconOpacity())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<IconOpacity>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void SymbolLayer::setIconOpacityTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<IconOpacity>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions SymbolLayer::getIconOpacityTransition() const {
    return impl().paint.template get<IconOpacity>().options;
}

DataDrivenPropertyValue<Color> SymbolLayer::getDefaultIconColor() {
    return { Color::black() };
}

DataDrivenPropertyValue<Color> SymbolLayer::getIconColor() const {
    return impl().paint.template get<IconColor>().value;
}

void SymbolLayer::setIconColor(DataDrivenPropertyValue<Color> value) {
    if (value == getIconColor())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<IconColor>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void SymbolLayer::setIconColorTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<IconColor>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions SymbolLayer::getIconColorTransition() const {
    return impl().paint.template get<IconColor>().options;
}

DataDrivenPropertyValue<Color> SymbolLayer::getDefaultIconHaloColor() {
    return { {} };
}

DataDrivenPropertyValue<Color> SymbolLayer::getIconHaloColor() const {
    return impl().paint.template get<IconHaloColor>().value;
}

void SymbolLayer::setIconHaloColor(DataDrivenPropertyValue<Color> value) {
    if (value == getIconHaloColor())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<IconHaloColor>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void SymbolLayer::setIconHaloColorTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<IconHaloColor>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions SymbolLayer::getIconHaloColorTransition() const {
    return impl().paint.template get<IconHaloColor>().options;
}

DataDrivenPropertyValue<float> SymbolLayer::getDefaultIconHaloWidth() {
    return { 0 };
}

DataDrivenPropertyValue<float> SymbolLayer::getIconHaloWidth() const {
    return impl().paint.template get<IconHaloWidth>().value;
}

void SymbolLayer::setIconHaloWidth(DataDrivenPropertyValue<float> value) {
    if (value == getIconHaloWidth())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<IconHaloWidth>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void SymbolLayer::setIconHaloWidthTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<IconHaloWidth>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions SymbolLayer::getIconHaloWidthTransition() const {
    return impl().paint.template get<IconHaloWidth>().options;
}

DataDrivenPropertyValue<float> SymbolLayer::getDefaultIconHaloBlur() {
    return { 0 };
}

DataDrivenPropertyValue<float> SymbolLayer::getIconHaloBlur() const {
    return impl().paint.template get<IconHaloBlur>().value;
}

void SymbolLayer::setIconHaloBlur(DataDrivenPropertyValue<float> value) {
    if (value == getIconHaloBlur())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<IconHaloBlur>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void SymbolLayer::setIconHaloBlurTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<IconHaloBlur>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions SymbolLayer::getIconHaloBlurTransition() const {
    return impl().paint.template get<IconHaloBlur>().options;
}

PropertyValue<std::array<float, 2>> SymbolLayer::getDefaultIconTranslate() {
    return { {{ 0, 0 }} };
}

PropertyValue<std::array<float, 2>> SymbolLayer::getIconTranslate() const {
    return impl().paint.template get<IconTranslate>().value;
}

void SymbolLayer::setIconTranslate(PropertyValue<std::array<float, 2>> value) {
    if (value == getIconTranslate())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<IconTranslate>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void SymbolLayer::setIconTranslateTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<IconTranslate>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions SymbolLayer::getIconTranslateTransition() const {
    return impl().paint.template get<IconTranslate>().options;
}

PropertyValue<TranslateAnchorType> SymbolLayer::getDefaultIconTranslateAnchor() {
    return { TranslateAnchorType::Map };
}

PropertyValue<TranslateAnchorType> SymbolLayer::getIconTranslateAnchor() const {
    return impl().paint.template get<IconTranslateAnchor>().value;
}

void SymbolLayer::setIconTranslateAnchor(PropertyValue<TranslateAnchorType> value) {
    if (value == getIconTranslateAnchor())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<IconTranslateAnchor>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void SymbolLayer::setIconTranslateAnchorTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<IconTranslateAnchor>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions SymbolLayer::getIconTranslateAnchorTransition() const {
    return impl().paint.template get<IconTranslateAnchor>().options;
}

DataDrivenPropertyValue<float> SymbolLayer::getDefaultTextOpacity() {
    return { 1 };
}

DataDrivenPropertyValue<float> SymbolLayer::getTextOpacity() const {
    return impl().paint.template get<TextOpacity>().value;
}

void SymbolLayer::setTextOpacity(DataDrivenPropertyValue<float> value) {
    if (value == getTextOpacity())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<TextOpacity>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void SymbolLayer::setTextOpacityTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<TextOpacity>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions SymbolLayer::getTextOpacityTransition() const {
    return impl().paint.template get<TextOpacity>().options;
}

DataDrivenPropertyValue<Color> SymbolLayer::getDefaultTextColor() {
    return { Color::black() };
}

DataDrivenPropertyValue<Color> SymbolLayer::getTextColor() const {
    return impl().paint.template get<TextColor>().value;
}

void SymbolLayer::setTextColor(DataDrivenPropertyValue<Color> value) {
    if (value == getTextColor())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<TextColor>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void SymbolLayer::setTextColorTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<TextColor>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions SymbolLayer::getTextColorTransition() const {
    return impl().paint.template get<TextColor>().options;
}

DataDrivenPropertyValue<Color> SymbolLayer::getDefaultTextHaloColor() {
    return { {} };
}

DataDrivenPropertyValue<Color> SymbolLayer::getTextHaloColor() const {
    return impl().paint.template get<TextHaloColor>().value;
}

void SymbolLayer::setTextHaloColor(DataDrivenPropertyValue<Color> value) {
    if (value == getTextHaloColor())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<TextHaloColor>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void SymbolLayer::setTextHaloColorTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<TextHaloColor>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions SymbolLayer::getTextHaloColorTransition() const {
    return impl().paint.template get<TextHaloColor>().options;
}

DataDrivenPropertyValue<float> SymbolLayer::getDefaultTextHaloWidth() {
    return { 0 };
}

DataDrivenPropertyValue<float> SymbolLayer::getTextHaloWidth() const {
    return impl().paint.template get<TextHaloWidth>().value;
}

void SymbolLayer::setTextHaloWidth(DataDrivenPropertyValue<float> value) {
    if (value == getTextHaloWidth())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<TextHaloWidth>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void SymbolLayer::setTextHaloWidthTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<TextHaloWidth>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions SymbolLayer::getTextHaloWidthTransition() const {
    return impl().paint.template get<TextHaloWidth>().options;
}

DataDrivenPropertyValue<float> SymbolLayer::getDefaultTextHaloBlur() {
    return { 0 };
}

DataDrivenPropertyValue<float> SymbolLayer::getTextHaloBlur() const {
    return impl().paint.template get<TextHaloBlur>().value;
}

void SymbolLayer::setTextHaloBlur(DataDrivenPropertyValue<float> value) {
    if (value == getTextHaloBlur())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<TextHaloBlur>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void SymbolLayer::setTextHaloBlurTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<TextHaloBlur>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions SymbolLayer::getTextHaloBlurTransition() const {
    return impl().paint.template get<TextHaloBlur>().options;
}

PropertyValue<std::array<float, 2>> SymbolLayer::getDefaultTextTranslate() {
    return { {{ 0, 0 }} };
}

PropertyValue<std::array<float, 2>> SymbolLayer::getTextTranslate() const {
    return impl().paint.template get<TextTranslate>().value;
}

void SymbolLayer::setTextTranslate(PropertyValue<std::array<float, 2>> value) {
    if (value == getTextTranslate())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<TextTranslate>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void SymbolLayer::setTextTranslateTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<TextTranslate>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions SymbolLayer::getTextTranslateTransition() const {
    return impl().paint.template get<TextTranslate>().options;
}

PropertyValue<TranslateAnchorType> SymbolLayer::getDefaultTextTranslateAnchor() {
    return { TranslateAnchorType::Map };
}

PropertyValue<TranslateAnchorType> SymbolLayer::getTextTranslateAnchor() const {
    return impl().paint.template get<TextTranslateAnchor>().value;
}

void SymbolLayer::setTextTranslateAnchor(PropertyValue<TranslateAnchorType> value) {
    if (value == getTextTranslateAnchor())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<TextTranslateAnchor>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void SymbolLayer::setTextTranslateAnchorTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<TextTranslateAnchor>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions SymbolLayer::getTextTranslateAnchorTransition() const {
    return impl().paint.template get<TextTranslateAnchor>().options;
}

} // namespace style
} // namespace mbgl
