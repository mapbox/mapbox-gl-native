// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/style/layers/symbol_layer_impl.hpp>
#include <mbgl/style/layer_observer.hpp>
#include <mbgl/style/conversion/color_ramp_property_value.hpp>
#include <mbgl/style/conversion/constant.hpp>
#include <mbgl/style/conversion/property_value.hpp>
#include <mbgl/style/conversion/transition_options.hpp>
#include <mbgl/style/conversion/json.hpp>
#include <mbgl/style/conversion_impl.hpp>

#include <mapbox/eternal.hpp>

namespace mbgl {
namespace style {


// static
const LayerTypeInfo* SymbolLayer::Impl::staticTypeInfo() noexcept {
    const static LayerTypeInfo typeInfo
        {"symbol",
          LayerTypeInfo::Source::Required,
          LayerTypeInfo::Pass3D::NotRequired,
          LayerTypeInfo::Layout::Required,
          LayerTypeInfo::Clipping::NotRequired
        };
    return &typeInfo;
}


SymbolLayer::SymbolLayer(const std::string& layerID, const std::string& sourceID)
    : Layer(makeMutable<Impl>(layerID, sourceID)) {
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
PropertyValue<SymbolZOrderType> SymbolLayer::getDefaultSymbolZOrder() {
    return SymbolZOrder::defaultValue();
}

PropertyValue<SymbolZOrderType> SymbolLayer::getSymbolZOrder() const {
    return impl().layout.get<SymbolZOrder>();
}

void SymbolLayer::setSymbolZOrder(PropertyValue<SymbolZOrderType> value) {
    if (value == getSymbolZOrder())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.get<SymbolZOrder>() = value;
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
PropertyValue<float> SymbolLayer::getDefaultIconSize() {
    return IconSize::defaultValue();
}

PropertyValue<float> SymbolLayer::getIconSize() const {
    return impl().layout.get<IconSize>();
}

void SymbolLayer::setIconSize(PropertyValue<float> value) {
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
PropertyValue<std::string> SymbolLayer::getDefaultIconImage() {
    return IconImage::defaultValue();
}

PropertyValue<std::string> SymbolLayer::getIconImage() const {
    return impl().layout.get<IconImage>();
}

void SymbolLayer::setIconImage(PropertyValue<std::string> value) {
    if (value == getIconImage())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.get<IconImage>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
PropertyValue<float> SymbolLayer::getDefaultIconRotate() {
    return IconRotate::defaultValue();
}

PropertyValue<float> SymbolLayer::getIconRotate() const {
    return impl().layout.get<IconRotate>();
}

void SymbolLayer::setIconRotate(PropertyValue<float> value) {
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
PropertyValue<std::array<float, 2>> SymbolLayer::getDefaultIconOffset() {
    return IconOffset::defaultValue();
}

PropertyValue<std::array<float, 2>> SymbolLayer::getIconOffset() const {
    return impl().layout.get<IconOffset>();
}

void SymbolLayer::setIconOffset(PropertyValue<std::array<float, 2>> value) {
    if (value == getIconOffset())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.get<IconOffset>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
PropertyValue<SymbolAnchorType> SymbolLayer::getDefaultIconAnchor() {
    return IconAnchor::defaultValue();
}

PropertyValue<SymbolAnchorType> SymbolLayer::getIconAnchor() const {
    return impl().layout.get<IconAnchor>();
}

void SymbolLayer::setIconAnchor(PropertyValue<SymbolAnchorType> value) {
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
PropertyValue<expression::Formatted> SymbolLayer::getDefaultTextField() {
    return TextField::defaultValue();
}

PropertyValue<expression::Formatted> SymbolLayer::getTextField() const {
    return impl().layout.get<TextField>();
}

void SymbolLayer::setTextField(PropertyValue<expression::Formatted> value) {
    if (value == getTextField())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.get<TextField>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
PropertyValue<std::vector<std::string>> SymbolLayer::getDefaultTextFont() {
    return TextFont::defaultValue();
}

PropertyValue<std::vector<std::string>> SymbolLayer::getTextFont() const {
    return impl().layout.get<TextFont>();
}

void SymbolLayer::setTextFont(PropertyValue<std::vector<std::string>> value) {
    if (value == getTextFont())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.get<TextFont>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
PropertyValue<float> SymbolLayer::getDefaultTextSize() {
    return TextSize::defaultValue();
}

PropertyValue<float> SymbolLayer::getTextSize() const {
    return impl().layout.get<TextSize>();
}

void SymbolLayer::setTextSize(PropertyValue<float> value) {
    if (value == getTextSize())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.get<TextSize>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
PropertyValue<float> SymbolLayer::getDefaultTextMaxWidth() {
    return TextMaxWidth::defaultValue();
}

PropertyValue<float> SymbolLayer::getTextMaxWidth() const {
    return impl().layout.get<TextMaxWidth>();
}

void SymbolLayer::setTextMaxWidth(PropertyValue<float> value) {
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
PropertyValue<float> SymbolLayer::getDefaultTextLetterSpacing() {
    return TextLetterSpacing::defaultValue();
}

PropertyValue<float> SymbolLayer::getTextLetterSpacing() const {
    return impl().layout.get<TextLetterSpacing>();
}

void SymbolLayer::setTextLetterSpacing(PropertyValue<float> value) {
    if (value == getTextLetterSpacing())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.get<TextLetterSpacing>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
PropertyValue<TextJustifyType> SymbolLayer::getDefaultTextJustify() {
    return TextJustify::defaultValue();
}

PropertyValue<TextJustifyType> SymbolLayer::getTextJustify() const {
    return impl().layout.get<TextJustify>();
}

void SymbolLayer::setTextJustify(PropertyValue<TextJustifyType> value) {
    if (value == getTextJustify())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.get<TextJustify>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
PropertyValue<float> SymbolLayer::getDefaultTextRadialOffset() {
    return TextRadialOffset::defaultValue();
}

PropertyValue<float> SymbolLayer::getTextRadialOffset() const {
    return impl().layout.get<TextRadialOffset>();
}

void SymbolLayer::setTextRadialOffset(PropertyValue<float> value) {
    if (value == getTextRadialOffset())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.get<TextRadialOffset>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
PropertyValue<std::vector<TextVariableAnchorType>> SymbolLayer::getDefaultTextVariableAnchor() {
    return TextVariableAnchor::defaultValue();
}

PropertyValue<std::vector<TextVariableAnchorType>> SymbolLayer::getTextVariableAnchor() const {
    return impl().layout.get<TextVariableAnchor>();
}

void SymbolLayer::setTextVariableAnchor(PropertyValue<std::vector<TextVariableAnchorType>> value) {
    if (value == getTextVariableAnchor())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.get<TextVariableAnchor>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
PropertyValue<SymbolAnchorType> SymbolLayer::getDefaultTextAnchor() {
    return TextAnchor::defaultValue();
}

PropertyValue<SymbolAnchorType> SymbolLayer::getTextAnchor() const {
    return impl().layout.get<TextAnchor>();
}

void SymbolLayer::setTextAnchor(PropertyValue<SymbolAnchorType> value) {
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
PropertyValue<float> SymbolLayer::getDefaultTextRotate() {
    return TextRotate::defaultValue();
}

PropertyValue<float> SymbolLayer::getTextRotate() const {
    return impl().layout.get<TextRotate>();
}

void SymbolLayer::setTextRotate(PropertyValue<float> value) {
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
PropertyValue<TextTransformType> SymbolLayer::getDefaultTextTransform() {
    return TextTransform::defaultValue();
}

PropertyValue<TextTransformType> SymbolLayer::getTextTransform() const {
    return impl().layout.get<TextTransform>();
}

void SymbolLayer::setTextTransform(PropertyValue<TextTransformType> value) {
    if (value == getTextTransform())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.get<TextTransform>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
PropertyValue<std::array<float, 2>> SymbolLayer::getDefaultTextOffset() {
    return TextOffset::defaultValue();
}

PropertyValue<std::array<float, 2>> SymbolLayer::getTextOffset() const {
    return impl().layout.get<TextOffset>();
}

void SymbolLayer::setTextOffset(PropertyValue<std::array<float, 2>> value) {
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

PropertyValue<float> SymbolLayer::getDefaultIconOpacity() {
    return { 1 };
}

PropertyValue<float> SymbolLayer::getIconOpacity() const {
    return impl().paint.template get<IconOpacity>().value;
}

void SymbolLayer::setIconOpacity(PropertyValue<float> value) {
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

PropertyValue<Color> SymbolLayer::getDefaultIconColor() {
    return { Color::black() };
}

PropertyValue<Color> SymbolLayer::getIconColor() const {
    return impl().paint.template get<IconColor>().value;
}

void SymbolLayer::setIconColor(PropertyValue<Color> value) {
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

PropertyValue<Color> SymbolLayer::getDefaultIconHaloColor() {
    return { {} };
}

PropertyValue<Color> SymbolLayer::getIconHaloColor() const {
    return impl().paint.template get<IconHaloColor>().value;
}

void SymbolLayer::setIconHaloColor(PropertyValue<Color> value) {
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

PropertyValue<float> SymbolLayer::getDefaultIconHaloWidth() {
    return { 0 };
}

PropertyValue<float> SymbolLayer::getIconHaloWidth() const {
    return impl().paint.template get<IconHaloWidth>().value;
}

void SymbolLayer::setIconHaloWidth(PropertyValue<float> value) {
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

PropertyValue<float> SymbolLayer::getDefaultIconHaloBlur() {
    return { 0 };
}

PropertyValue<float> SymbolLayer::getIconHaloBlur() const {
    return impl().paint.template get<IconHaloBlur>().value;
}

void SymbolLayer::setIconHaloBlur(PropertyValue<float> value) {
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

PropertyValue<float> SymbolLayer::getDefaultTextOpacity() {
    return { 1 };
}

PropertyValue<float> SymbolLayer::getTextOpacity() const {
    return impl().paint.template get<TextOpacity>().value;
}

void SymbolLayer::setTextOpacity(PropertyValue<float> value) {
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

PropertyValue<Color> SymbolLayer::getDefaultTextColor() {
    return { Color::black() };
}

PropertyValue<Color> SymbolLayer::getTextColor() const {
    return impl().paint.template get<TextColor>().value;
}

void SymbolLayer::setTextColor(PropertyValue<Color> value) {
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

PropertyValue<Color> SymbolLayer::getDefaultTextHaloColor() {
    return { {} };
}

PropertyValue<Color> SymbolLayer::getTextHaloColor() const {
    return impl().paint.template get<TextHaloColor>().value;
}

void SymbolLayer::setTextHaloColor(PropertyValue<Color> value) {
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

PropertyValue<float> SymbolLayer::getDefaultTextHaloWidth() {
    return { 0 };
}

PropertyValue<float> SymbolLayer::getTextHaloWidth() const {
    return impl().paint.template get<TextHaloWidth>().value;
}

void SymbolLayer::setTextHaloWidth(PropertyValue<float> value) {
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

PropertyValue<float> SymbolLayer::getDefaultTextHaloBlur() {
    return { 0 };
}

PropertyValue<float> SymbolLayer::getTextHaloBlur() const {
    return impl().paint.template get<TextHaloBlur>().value;
}

void SymbolLayer::setTextHaloBlur(PropertyValue<float> value) {
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

using namespace conversion;

optional<Error> SymbolLayer::setPaintProperty(const std::string& name, const Convertible& value) {
    enum class Property : uint8_t {
        IconOpacity,
        IconColor,
        IconHaloColor,
        IconHaloWidth,
        IconHaloBlur,
        IconTranslate,
        IconTranslateAnchor,
        TextOpacity,
        TextColor,
        TextHaloColor,
        TextHaloWidth,
        TextHaloBlur,
        TextTranslate,
        TextTranslateAnchor,
        IconOpacityTransition,
        IconColorTransition,
        IconHaloColorTransition,
        IconHaloWidthTransition,
        IconHaloBlurTransition,
        IconTranslateTransition,
        IconTranslateAnchorTransition,
        TextOpacityTransition,
        TextColorTransition,
        TextHaloColorTransition,
        TextHaloWidthTransition,
        TextHaloBlurTransition,
        TextTranslateTransition,
        TextTranslateAnchorTransition,
    };

    MAPBOX_ETERNAL_CONSTEXPR const auto properties = mapbox::eternal::hash_map<mapbox::eternal::string, uint8_t>({
        { "icon-opacity", static_cast<uint8_t>(Property::IconOpacity) },
        { "icon-color", static_cast<uint8_t>(Property::IconColor) },
        { "icon-halo-color", static_cast<uint8_t>(Property::IconHaloColor) },
        { "icon-halo-width", static_cast<uint8_t>(Property::IconHaloWidth) },
        { "icon-halo-blur", static_cast<uint8_t>(Property::IconHaloBlur) },
        { "icon-translate", static_cast<uint8_t>(Property::IconTranslate) },
        { "icon-translate-anchor", static_cast<uint8_t>(Property::IconTranslateAnchor) },
        { "text-opacity", static_cast<uint8_t>(Property::TextOpacity) },
        { "text-color", static_cast<uint8_t>(Property::TextColor) },
        { "text-halo-color", static_cast<uint8_t>(Property::TextHaloColor) },
        { "text-halo-width", static_cast<uint8_t>(Property::TextHaloWidth) },
        { "text-halo-blur", static_cast<uint8_t>(Property::TextHaloBlur) },
        { "text-translate", static_cast<uint8_t>(Property::TextTranslate) },
        { "text-translate-anchor", static_cast<uint8_t>(Property::TextTranslateAnchor) },
        { "icon-opacity-transition", static_cast<uint8_t>(Property::IconOpacityTransition) },
        { "icon-color-transition", static_cast<uint8_t>(Property::IconColorTransition) },
        { "icon-halo-color-transition", static_cast<uint8_t>(Property::IconHaloColorTransition) },
        { "icon-halo-width-transition", static_cast<uint8_t>(Property::IconHaloWidthTransition) },
        { "icon-halo-blur-transition", static_cast<uint8_t>(Property::IconHaloBlurTransition) },
        { "icon-translate-transition", static_cast<uint8_t>(Property::IconTranslateTransition) },
        { "icon-translate-anchor-transition", static_cast<uint8_t>(Property::IconTranslateAnchorTransition) },
        { "text-opacity-transition", static_cast<uint8_t>(Property::TextOpacityTransition) },
        { "text-color-transition", static_cast<uint8_t>(Property::TextColorTransition) },
        { "text-halo-color-transition", static_cast<uint8_t>(Property::TextHaloColorTransition) },
        { "text-halo-width-transition", static_cast<uint8_t>(Property::TextHaloWidthTransition) },
        { "text-halo-blur-transition", static_cast<uint8_t>(Property::TextHaloBlurTransition) },
        { "text-translate-transition", static_cast<uint8_t>(Property::TextTranslateTransition) },
        { "text-translate-anchor-transition", static_cast<uint8_t>(Property::TextTranslateAnchorTransition) }
    });

    const auto it = properties.find(name.c_str());
    if (it == properties.end()) {
        return Error { "layer doesn't support this property" };
    }

    Property property = static_cast<Property>(it->second);

        
    if (property == Property::IconOpacity || property == Property::IconHaloWidth || property == Property::IconHaloBlur || property == Property::TextOpacity || property == Property::TextHaloWidth || property == Property::TextHaloBlur) {
        Error error;
        optional<PropertyValue<float>> typedValue = convert<PropertyValue<float>>(value, error, true, false);
        if (!typedValue) {
            return error;
        }
        
        if (property == Property::IconOpacity) {
            setIconOpacity(*typedValue);
            return nullopt;
        }
        
        if (property == Property::IconHaloWidth) {
            setIconHaloWidth(*typedValue);
            return nullopt;
        }
        
        if (property == Property::IconHaloBlur) {
            setIconHaloBlur(*typedValue);
            return nullopt;
        }
        
        if (property == Property::TextOpacity) {
            setTextOpacity(*typedValue);
            return nullopt;
        }
        
        if (property == Property::TextHaloWidth) {
            setTextHaloWidth(*typedValue);
            return nullopt;
        }
        
        if (property == Property::TextHaloBlur) {
            setTextHaloBlur(*typedValue);
            return nullopt;
        }
        
    }
    
    if (property == Property::IconColor || property == Property::IconHaloColor || property == Property::TextColor || property == Property::TextHaloColor) {
        Error error;
        optional<PropertyValue<Color>> typedValue = convert<PropertyValue<Color>>(value, error, true, false);
        if (!typedValue) {
            return error;
        }
        
        if (property == Property::IconColor) {
            setIconColor(*typedValue);
            return nullopt;
        }
        
        if (property == Property::IconHaloColor) {
            setIconHaloColor(*typedValue);
            return nullopt;
        }
        
        if (property == Property::TextColor) {
            setTextColor(*typedValue);
            return nullopt;
        }
        
        if (property == Property::TextHaloColor) {
            setTextHaloColor(*typedValue);
            return nullopt;
        }
        
    }
    
    if (property == Property::IconTranslate || property == Property::TextTranslate) {
        Error error;
        optional<PropertyValue<std::array<float, 2>>> typedValue = convert<PropertyValue<std::array<float, 2>>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }
        
        if (property == Property::IconTranslate) {
            setIconTranslate(*typedValue);
            return nullopt;
        }
        
        if (property == Property::TextTranslate) {
            setTextTranslate(*typedValue);
            return nullopt;
        }
        
    }
    
    if (property == Property::IconTranslateAnchor || property == Property::TextTranslateAnchor) {
        Error error;
        optional<PropertyValue<TranslateAnchorType>> typedValue = convert<PropertyValue<TranslateAnchorType>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }
        
        if (property == Property::IconTranslateAnchor) {
            setIconTranslateAnchor(*typedValue);
            return nullopt;
        }
        
        if (property == Property::TextTranslateAnchor) {
            setTextTranslateAnchor(*typedValue);
            return nullopt;
        }
        
    }
    

    Error error;
    optional<TransitionOptions> transition = convert<TransitionOptions>(value, error);
    if (!transition) {
        return error;
    }
    
    if (property == Property::IconOpacityTransition) {
        setIconOpacityTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::IconColorTransition) {
        setIconColorTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::IconHaloColorTransition) {
        setIconHaloColorTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::IconHaloWidthTransition) {
        setIconHaloWidthTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::IconHaloBlurTransition) {
        setIconHaloBlurTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::IconTranslateTransition) {
        setIconTranslateTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::IconTranslateAnchorTransition) {
        setIconTranslateAnchorTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::TextOpacityTransition) {
        setTextOpacityTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::TextColorTransition) {
        setTextColorTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::TextHaloColorTransition) {
        setTextHaloColorTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::TextHaloWidthTransition) {
        setTextHaloWidthTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::TextHaloBlurTransition) {
        setTextHaloBlurTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::TextTranslateTransition) {
        setTextTranslateTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::TextTranslateAnchorTransition) {
        setTextTranslateAnchorTransition(*transition);
        return nullopt;
    }
    

    return Error { "layer doesn't support this property" };
}

optional<Error> SymbolLayer::setLayoutProperty(const std::string& name, const Convertible& value) {
    if (name == "visibility") {
        return Layer::setVisibility(value);
    }
    enum class Property {
        SymbolPlacement,
        SymbolSpacing,
        SymbolAvoidEdges,
        SymbolZOrder,
        IconAllowOverlap,
        IconIgnorePlacement,
        IconOptional,
        IconRotationAlignment,
        IconSize,
        IconTextFit,
        IconTextFitPadding,
        IconImage,
        IconRotate,
        IconPadding,
        IconKeepUpright,
        IconOffset,
        IconAnchor,
        IconPitchAlignment,
        TextPitchAlignment,
        TextRotationAlignment,
        TextField,
        TextFont,
        TextSize,
        TextMaxWidth,
        TextLineHeight,
        TextLetterSpacing,
        TextJustify,
        TextRadialOffset,
        TextVariableAnchor,
        TextAnchor,
        TextMaxAngle,
        TextRotate,
        TextPadding,
        TextKeepUpright,
        TextTransform,
        TextOffset,
        TextAllowOverlap,
        TextIgnorePlacement,
        TextOptional,
    };
    MAPBOX_ETERNAL_CONSTEXPR const auto properties = mapbox::eternal::hash_map<mapbox::eternal::string, uint8_t>({
        { "symbol-placement", static_cast<uint8_t>(Property::SymbolPlacement) },
        { "symbol-spacing", static_cast<uint8_t>(Property::SymbolSpacing) },
        { "symbol-avoid-edges", static_cast<uint8_t>(Property::SymbolAvoidEdges) },
        { "symbol-z-order", static_cast<uint8_t>(Property::SymbolZOrder) },
        { "icon-allow-overlap", static_cast<uint8_t>(Property::IconAllowOverlap) },
        { "icon-ignore-placement", static_cast<uint8_t>(Property::IconIgnorePlacement) },
        { "icon-optional", static_cast<uint8_t>(Property::IconOptional) },
        { "icon-rotation-alignment", static_cast<uint8_t>(Property::IconRotationAlignment) },
        { "icon-size", static_cast<uint8_t>(Property::IconSize) },
        { "icon-text-fit", static_cast<uint8_t>(Property::IconTextFit) },
        { "icon-text-fit-padding", static_cast<uint8_t>(Property::IconTextFitPadding) },
        { "icon-image", static_cast<uint8_t>(Property::IconImage) },
        { "icon-rotate", static_cast<uint8_t>(Property::IconRotate) },
        { "icon-padding", static_cast<uint8_t>(Property::IconPadding) },
        { "icon-keep-upright", static_cast<uint8_t>(Property::IconKeepUpright) },
        { "icon-offset", static_cast<uint8_t>(Property::IconOffset) },
        { "icon-anchor", static_cast<uint8_t>(Property::IconAnchor) },
        { "icon-pitch-alignment", static_cast<uint8_t>(Property::IconPitchAlignment) },
        { "text-pitch-alignment", static_cast<uint8_t>(Property::TextPitchAlignment) },
        { "text-rotation-alignment", static_cast<uint8_t>(Property::TextRotationAlignment) },
        { "text-field", static_cast<uint8_t>(Property::TextField) },
        { "text-font", static_cast<uint8_t>(Property::TextFont) },
        { "text-size", static_cast<uint8_t>(Property::TextSize) },
        { "text-max-width", static_cast<uint8_t>(Property::TextMaxWidth) },
        { "text-line-height", static_cast<uint8_t>(Property::TextLineHeight) },
        { "text-letter-spacing", static_cast<uint8_t>(Property::TextLetterSpacing) },
        { "text-justify", static_cast<uint8_t>(Property::TextJustify) },
        { "text-radial-offset", static_cast<uint8_t>(Property::TextRadialOffset) },
        { "text-variable-anchor", static_cast<uint8_t>(Property::TextVariableAnchor) },
        { "text-anchor", static_cast<uint8_t>(Property::TextAnchor) },
        { "text-max-angle", static_cast<uint8_t>(Property::TextMaxAngle) },
        { "text-rotate", static_cast<uint8_t>(Property::TextRotate) },
        { "text-padding", static_cast<uint8_t>(Property::TextPadding) },
        { "text-keep-upright", static_cast<uint8_t>(Property::TextKeepUpright) },
        { "text-transform", static_cast<uint8_t>(Property::TextTransform) },
        { "text-offset", static_cast<uint8_t>(Property::TextOffset) },
        { "text-allow-overlap", static_cast<uint8_t>(Property::TextAllowOverlap) },
        { "text-ignore-placement", static_cast<uint8_t>(Property::TextIgnorePlacement) },
        { "text-optional", static_cast<uint8_t>(Property::TextOptional) }
    });

    const auto it = properties.find(name.c_str());
    if (it == properties.end()) {
        return Error { "layer doesn't support this property" };
    }

    Property property = static_cast<Property>(it->second);

        
    if (property == Property::SymbolPlacement) {
        Error error;
        optional<PropertyValue<SymbolPlacementType>> typedValue = convert<PropertyValue<SymbolPlacementType>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }
        
        setSymbolPlacement(*typedValue);
        return nullopt;
        
    }
    
    if (property == Property::SymbolSpacing || property == Property::IconPadding || property == Property::TextLineHeight || property == Property::TextMaxAngle || property == Property::TextPadding) {
        Error error;
        optional<PropertyValue<float>> typedValue = convert<PropertyValue<float>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }
        
        if (property == Property::SymbolSpacing) {
            setSymbolSpacing(*typedValue);
            return nullopt;
        }
        
        if (property == Property::IconPadding) {
            setIconPadding(*typedValue);
            return nullopt;
        }
        
        if (property == Property::TextLineHeight) {
            setTextLineHeight(*typedValue);
            return nullopt;
        }
        
        if (property == Property::TextMaxAngle) {
            setTextMaxAngle(*typedValue);
            return nullopt;
        }
        
        if (property == Property::TextPadding) {
            setTextPadding(*typedValue);
            return nullopt;
        }
        
    }
    
    if (property == Property::SymbolAvoidEdges || property == Property::IconAllowOverlap || property == Property::IconIgnorePlacement || property == Property::IconOptional || property == Property::IconKeepUpright || property == Property::TextKeepUpright || property == Property::TextAllowOverlap || property == Property::TextIgnorePlacement || property == Property::TextOptional) {
        Error error;
        optional<PropertyValue<bool>> typedValue = convert<PropertyValue<bool>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }
        
        if (property == Property::SymbolAvoidEdges) {
            setSymbolAvoidEdges(*typedValue);
            return nullopt;
        }
        
        if (property == Property::IconAllowOverlap) {
            setIconAllowOverlap(*typedValue);
            return nullopt;
        }
        
        if (property == Property::IconIgnorePlacement) {
            setIconIgnorePlacement(*typedValue);
            return nullopt;
        }
        
        if (property == Property::IconOptional) {
            setIconOptional(*typedValue);
            return nullopt;
        }
        
        if (property == Property::IconKeepUpright) {
            setIconKeepUpright(*typedValue);
            return nullopt;
        }
        
        if (property == Property::TextKeepUpright) {
            setTextKeepUpright(*typedValue);
            return nullopt;
        }
        
        if (property == Property::TextAllowOverlap) {
            setTextAllowOverlap(*typedValue);
            return nullopt;
        }
        
        if (property == Property::TextIgnorePlacement) {
            setTextIgnorePlacement(*typedValue);
            return nullopt;
        }
        
        if (property == Property::TextOptional) {
            setTextOptional(*typedValue);
            return nullopt;
        }
        
    }
    
    if (property == Property::SymbolZOrder) {
        Error error;
        optional<PropertyValue<SymbolZOrderType>> typedValue = convert<PropertyValue<SymbolZOrderType>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }
        
        setSymbolZOrder(*typedValue);
        return nullopt;
        
    }
    
    if (property == Property::IconRotationAlignment || property == Property::IconPitchAlignment || property == Property::TextPitchAlignment || property == Property::TextRotationAlignment) {
        Error error;
        optional<PropertyValue<AlignmentType>> typedValue = convert<PropertyValue<AlignmentType>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }
        
        if (property == Property::IconRotationAlignment) {
            setIconRotationAlignment(*typedValue);
            return nullopt;
        }
        
        if (property == Property::IconPitchAlignment) {
            setIconPitchAlignment(*typedValue);
            return nullopt;
        }
        
        if (property == Property::TextPitchAlignment) {
            setTextPitchAlignment(*typedValue);
            return nullopt;
        }
        
        if (property == Property::TextRotationAlignment) {
            setTextRotationAlignment(*typedValue);
            return nullopt;
        }
        
    }
    
    if (property == Property::IconSize || property == Property::IconRotate || property == Property::TextSize || property == Property::TextMaxWidth || property == Property::TextLetterSpacing || property == Property::TextRadialOffset || property == Property::TextRotate) {
        Error error;
        optional<PropertyValue<float>> typedValue = convert<PropertyValue<float>>(value, error, true, false);
        if (!typedValue) {
            return error;
        }
        
        if (property == Property::IconSize) {
            setIconSize(*typedValue);
            return nullopt;
        }
        
        if (property == Property::IconRotate) {
            setIconRotate(*typedValue);
            return nullopt;
        }
        
        if (property == Property::TextSize) {
            setTextSize(*typedValue);
            return nullopt;
        }
        
        if (property == Property::TextMaxWidth) {
            setTextMaxWidth(*typedValue);
            return nullopt;
        }
        
        if (property == Property::TextLetterSpacing) {
            setTextLetterSpacing(*typedValue);
            return nullopt;
        }
        
        if (property == Property::TextRadialOffset) {
            setTextRadialOffset(*typedValue);
            return nullopt;
        }
        
        if (property == Property::TextRotate) {
            setTextRotate(*typedValue);
            return nullopt;
        }
        
    }
    
    if (property == Property::IconTextFit) {
        Error error;
        optional<PropertyValue<IconTextFitType>> typedValue = convert<PropertyValue<IconTextFitType>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }
        
        setIconTextFit(*typedValue);
        return nullopt;
        
    }
    
    if (property == Property::IconTextFitPadding) {
        Error error;
        optional<PropertyValue<std::array<float, 4>>> typedValue = convert<PropertyValue<std::array<float, 4>>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }
        
        setIconTextFitPadding(*typedValue);
        return nullopt;
        
    }
    
    if (property == Property::IconImage) {
        Error error;
        optional<PropertyValue<std::string>> typedValue = convert<PropertyValue<std::string>>(value, error, true, true);
        if (!typedValue) {
            return error;
        }
        
        setIconImage(*typedValue);
        return nullopt;
        
    }
    
    if (property == Property::IconOffset || property == Property::TextOffset) {
        Error error;
        optional<PropertyValue<std::array<float, 2>>> typedValue = convert<PropertyValue<std::array<float, 2>>>(value, error, true, false);
        if (!typedValue) {
            return error;
        }
        
        if (property == Property::IconOffset) {
            setIconOffset(*typedValue);
            return nullopt;
        }
        
        if (property == Property::TextOffset) {
            setTextOffset(*typedValue);
            return nullopt;
        }
        
    }
    
    if (property == Property::IconAnchor || property == Property::TextAnchor) {
        Error error;
        optional<PropertyValue<SymbolAnchorType>> typedValue = convert<PropertyValue<SymbolAnchorType>>(value, error, true, false);
        if (!typedValue) {
            return error;
        }
        
        if (property == Property::IconAnchor) {
            setIconAnchor(*typedValue);
            return nullopt;
        }
        
        if (property == Property::TextAnchor) {
            setTextAnchor(*typedValue);
            return nullopt;
        }
        
    }
    
    if (property == Property::TextField) {
        Error error;
        optional<PropertyValue<expression::Formatted>> typedValue = convert<PropertyValue<expression::Formatted>>(value, error, true, true);
        if (!typedValue) {
            return error;
        }
        
        setTextField(*typedValue);
        return nullopt;
        
    }
    
    if (property == Property::TextFont) {
        Error error;
        optional<PropertyValue<std::vector<std::string>>> typedValue = convert<PropertyValue<std::vector<std::string>>>(value, error, true, false);
        if (!typedValue) {
            return error;
        }
        
        setTextFont(*typedValue);
        return nullopt;
        
    }
    
    if (property == Property::TextJustify) {
        Error error;
        optional<PropertyValue<TextJustifyType>> typedValue = convert<PropertyValue<TextJustifyType>>(value, error, true, false);
        if (!typedValue) {
            return error;
        }
        
        setTextJustify(*typedValue);
        return nullopt;
        
    }
    
    if (property == Property::TextVariableAnchor) {
        Error error;
        optional<PropertyValue<std::vector<TextVariableAnchorType>>> typedValue = convert<PropertyValue<std::vector<TextVariableAnchorType>>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }
        
        setTextVariableAnchor(*typedValue);
        return nullopt;
        
    }
    
    if (property == Property::TextTransform) {
        Error error;
        optional<PropertyValue<TextTransformType>> typedValue = convert<PropertyValue<TextTransformType>>(value, error, true, false);
        if (!typedValue) {
            return error;
        }
        
        setTextTransform(*typedValue);
        return nullopt;
        
    }
    

    return Error { "layer doesn't support this property" };
}

Mutable<Layer::Impl> SymbolLayer::mutableBaseImpl() const {
    return staticMutableCast<Layer::Impl>(mutableImpl());
}

} // namespace style
} // namespace mbgl
