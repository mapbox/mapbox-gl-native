// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/style/layers/symbol_layer_impl.hpp>
#include <mbgl/style/conversion/stringify.hpp>
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
    impl_->paint = SymbolPaintProperties::Cascading();
    return std::make_unique<SymbolLayer>(std::move(impl_));
}

void SymbolLayer::Impl::stringifyLayout(rapidjson::Writer<rapidjson::StringBuffer>& writer) const {
    conversion::stringify(writer, layout);
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
    observer->onLayerFilterChanged(*this);
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
    observer->onLayerVisibilityChanged(*this);
}

// Zoom range

void SymbolLayer::setMinZoom(float minZoom) {
    auto impl_ = mutableImpl();
    impl_->minZoom = minZoom;
    baseImpl = std::move(impl_);
}

void SymbolLayer::setMaxZoom(float maxZoom) {
    auto impl_ = mutableImpl();
    impl_->maxZoom = maxZoom;
    baseImpl = std::move(impl_);
}

// Layout properties

PropertyValue<SymbolPlacementType> SymbolLayer::getDefaultSymbolPlacement() {
    return SymbolPlacement::defaultValue();
}

PropertyValue<SymbolPlacementType> SymbolLayer::getSymbolPlacement() const {
    return impl().layout.unevaluated.get<SymbolPlacement>();
}

void SymbolLayer::setSymbolPlacement(PropertyValue<SymbolPlacementType> value) {
    if (value == getSymbolPlacement())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.unevaluated.get<SymbolPlacement>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerLayoutPropertyChanged(*this, "symbol-placement");
}
PropertyValue<float> SymbolLayer::getDefaultSymbolSpacing() {
    return SymbolSpacing::defaultValue();
}

PropertyValue<float> SymbolLayer::getSymbolSpacing() const {
    return impl().layout.unevaluated.get<SymbolSpacing>();
}

void SymbolLayer::setSymbolSpacing(PropertyValue<float> value) {
    if (value == getSymbolSpacing())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.unevaluated.get<SymbolSpacing>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerLayoutPropertyChanged(*this, "symbol-spacing");
}
PropertyValue<bool> SymbolLayer::getDefaultSymbolAvoidEdges() {
    return SymbolAvoidEdges::defaultValue();
}

PropertyValue<bool> SymbolLayer::getSymbolAvoidEdges() const {
    return impl().layout.unevaluated.get<SymbolAvoidEdges>();
}

void SymbolLayer::setSymbolAvoidEdges(PropertyValue<bool> value) {
    if (value == getSymbolAvoidEdges())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.unevaluated.get<SymbolAvoidEdges>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerLayoutPropertyChanged(*this, "symbol-avoid-edges");
}
PropertyValue<bool> SymbolLayer::getDefaultIconAllowOverlap() {
    return IconAllowOverlap::defaultValue();
}

PropertyValue<bool> SymbolLayer::getIconAllowOverlap() const {
    return impl().layout.unevaluated.get<IconAllowOverlap>();
}

void SymbolLayer::setIconAllowOverlap(PropertyValue<bool> value) {
    if (value == getIconAllowOverlap())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.unevaluated.get<IconAllowOverlap>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerLayoutPropertyChanged(*this, "icon-allow-overlap");
}
PropertyValue<bool> SymbolLayer::getDefaultIconIgnorePlacement() {
    return IconIgnorePlacement::defaultValue();
}

PropertyValue<bool> SymbolLayer::getIconIgnorePlacement() const {
    return impl().layout.unevaluated.get<IconIgnorePlacement>();
}

void SymbolLayer::setIconIgnorePlacement(PropertyValue<bool> value) {
    if (value == getIconIgnorePlacement())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.unevaluated.get<IconIgnorePlacement>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerLayoutPropertyChanged(*this, "icon-ignore-placement");
}
PropertyValue<bool> SymbolLayer::getDefaultIconOptional() {
    return IconOptional::defaultValue();
}

PropertyValue<bool> SymbolLayer::getIconOptional() const {
    return impl().layout.unevaluated.get<IconOptional>();
}

void SymbolLayer::setIconOptional(PropertyValue<bool> value) {
    if (value == getIconOptional())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.unevaluated.get<IconOptional>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerLayoutPropertyChanged(*this, "icon-optional");
}
PropertyValue<AlignmentType> SymbolLayer::getDefaultIconRotationAlignment() {
    return IconRotationAlignment::defaultValue();
}

PropertyValue<AlignmentType> SymbolLayer::getIconRotationAlignment() const {
    return impl().layout.unevaluated.get<IconRotationAlignment>();
}

void SymbolLayer::setIconRotationAlignment(PropertyValue<AlignmentType> value) {
    if (value == getIconRotationAlignment())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.unevaluated.get<IconRotationAlignment>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerLayoutPropertyChanged(*this, "icon-rotation-alignment");
}
DataDrivenPropertyValue<float> SymbolLayer::getDefaultIconSize() {
    return IconSize::defaultValue();
}

DataDrivenPropertyValue<float> SymbolLayer::getIconSize() const {
    return impl().layout.unevaluated.get<IconSize>();
}

void SymbolLayer::setIconSize(DataDrivenPropertyValue<float> value) {
    if (value == getIconSize())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.unevaluated.get<IconSize>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerLayoutPropertyChanged(*this, "icon-size");
}
PropertyValue<IconTextFitType> SymbolLayer::getDefaultIconTextFit() {
    return IconTextFit::defaultValue();
}

PropertyValue<IconTextFitType> SymbolLayer::getIconTextFit() const {
    return impl().layout.unevaluated.get<IconTextFit>();
}

void SymbolLayer::setIconTextFit(PropertyValue<IconTextFitType> value) {
    if (value == getIconTextFit())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.unevaluated.get<IconTextFit>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerLayoutPropertyChanged(*this, "icon-text-fit");
}
PropertyValue<std::array<float, 4>> SymbolLayer::getDefaultIconTextFitPadding() {
    return IconTextFitPadding::defaultValue();
}

PropertyValue<std::array<float, 4>> SymbolLayer::getIconTextFitPadding() const {
    return impl().layout.unevaluated.get<IconTextFitPadding>();
}

void SymbolLayer::setIconTextFitPadding(PropertyValue<std::array<float, 4>> value) {
    if (value == getIconTextFitPadding())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.unevaluated.get<IconTextFitPadding>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerLayoutPropertyChanged(*this, "icon-text-fit-padding");
}
DataDrivenPropertyValue<std::string> SymbolLayer::getDefaultIconImage() {
    return IconImage::defaultValue();
}

DataDrivenPropertyValue<std::string> SymbolLayer::getIconImage() const {
    return impl().layout.unevaluated.get<IconImage>();
}

void SymbolLayer::setIconImage(DataDrivenPropertyValue<std::string> value) {
    if (value == getIconImage())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.unevaluated.get<IconImage>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerLayoutPropertyChanged(*this, "icon-image");
}
DataDrivenPropertyValue<float> SymbolLayer::getDefaultIconRotate() {
    return IconRotate::defaultValue();
}

DataDrivenPropertyValue<float> SymbolLayer::getIconRotate() const {
    return impl().layout.unevaluated.get<IconRotate>();
}

void SymbolLayer::setIconRotate(DataDrivenPropertyValue<float> value) {
    if (value == getIconRotate())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.unevaluated.get<IconRotate>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerLayoutPropertyChanged(*this, "icon-rotate");
}
PropertyValue<float> SymbolLayer::getDefaultIconPadding() {
    return IconPadding::defaultValue();
}

PropertyValue<float> SymbolLayer::getIconPadding() const {
    return impl().layout.unevaluated.get<IconPadding>();
}

void SymbolLayer::setIconPadding(PropertyValue<float> value) {
    if (value == getIconPadding())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.unevaluated.get<IconPadding>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerLayoutPropertyChanged(*this, "icon-padding");
}
PropertyValue<bool> SymbolLayer::getDefaultIconKeepUpright() {
    return IconKeepUpright::defaultValue();
}

PropertyValue<bool> SymbolLayer::getIconKeepUpright() const {
    return impl().layout.unevaluated.get<IconKeepUpright>();
}

void SymbolLayer::setIconKeepUpright(PropertyValue<bool> value) {
    if (value == getIconKeepUpright())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.unevaluated.get<IconKeepUpright>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerLayoutPropertyChanged(*this, "icon-keep-upright");
}
DataDrivenPropertyValue<std::array<float, 2>> SymbolLayer::getDefaultIconOffset() {
    return IconOffset::defaultValue();
}

DataDrivenPropertyValue<std::array<float, 2>> SymbolLayer::getIconOffset() const {
    return impl().layout.unevaluated.get<IconOffset>();
}

void SymbolLayer::setIconOffset(DataDrivenPropertyValue<std::array<float, 2>> value) {
    if (value == getIconOffset())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.unevaluated.get<IconOffset>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerLayoutPropertyChanged(*this, "icon-offset");
}
PropertyValue<AlignmentType> SymbolLayer::getDefaultTextPitchAlignment() {
    return TextPitchAlignment::defaultValue();
}

PropertyValue<AlignmentType> SymbolLayer::getTextPitchAlignment() const {
    return impl().layout.unevaluated.get<TextPitchAlignment>();
}

void SymbolLayer::setTextPitchAlignment(PropertyValue<AlignmentType> value) {
    if (value == getTextPitchAlignment())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.unevaluated.get<TextPitchAlignment>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerLayoutPropertyChanged(*this, "text-pitch-alignment");
}
PropertyValue<AlignmentType> SymbolLayer::getDefaultTextRotationAlignment() {
    return TextRotationAlignment::defaultValue();
}

PropertyValue<AlignmentType> SymbolLayer::getTextRotationAlignment() const {
    return impl().layout.unevaluated.get<TextRotationAlignment>();
}

void SymbolLayer::setTextRotationAlignment(PropertyValue<AlignmentType> value) {
    if (value == getTextRotationAlignment())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.unevaluated.get<TextRotationAlignment>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerLayoutPropertyChanged(*this, "text-rotation-alignment");
}
DataDrivenPropertyValue<std::string> SymbolLayer::getDefaultTextField() {
    return TextField::defaultValue();
}

DataDrivenPropertyValue<std::string> SymbolLayer::getTextField() const {
    return impl().layout.unevaluated.get<TextField>();
}

void SymbolLayer::setTextField(DataDrivenPropertyValue<std::string> value) {
    if (value == getTextField())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.unevaluated.get<TextField>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerLayoutPropertyChanged(*this, "text-field");
}
PropertyValue<std::vector<std::string>> SymbolLayer::getDefaultTextFont() {
    return TextFont::defaultValue();
}

PropertyValue<std::vector<std::string>> SymbolLayer::getTextFont() const {
    return impl().layout.unevaluated.get<TextFont>();
}

void SymbolLayer::setTextFont(PropertyValue<std::vector<std::string>> value) {
    if (value == getTextFont())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.unevaluated.get<TextFont>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerLayoutPropertyChanged(*this, "text-font");
}
DataDrivenPropertyValue<float> SymbolLayer::getDefaultTextSize() {
    return TextSize::defaultValue();
}

DataDrivenPropertyValue<float> SymbolLayer::getTextSize() const {
    return impl().layout.unevaluated.get<TextSize>();
}

void SymbolLayer::setTextSize(DataDrivenPropertyValue<float> value) {
    if (value == getTextSize())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.unevaluated.get<TextSize>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerLayoutPropertyChanged(*this, "text-size");
}
PropertyValue<float> SymbolLayer::getDefaultTextMaxWidth() {
    return TextMaxWidth::defaultValue();
}

PropertyValue<float> SymbolLayer::getTextMaxWidth() const {
    return impl().layout.unevaluated.get<TextMaxWidth>();
}

void SymbolLayer::setTextMaxWidth(PropertyValue<float> value) {
    if (value == getTextMaxWidth())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.unevaluated.get<TextMaxWidth>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerLayoutPropertyChanged(*this, "text-max-width");
}
PropertyValue<float> SymbolLayer::getDefaultTextLineHeight() {
    return TextLineHeight::defaultValue();
}

PropertyValue<float> SymbolLayer::getTextLineHeight() const {
    return impl().layout.unevaluated.get<TextLineHeight>();
}

void SymbolLayer::setTextLineHeight(PropertyValue<float> value) {
    if (value == getTextLineHeight())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.unevaluated.get<TextLineHeight>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerLayoutPropertyChanged(*this, "text-line-height");
}
PropertyValue<float> SymbolLayer::getDefaultTextLetterSpacing() {
    return TextLetterSpacing::defaultValue();
}

PropertyValue<float> SymbolLayer::getTextLetterSpacing() const {
    return impl().layout.unevaluated.get<TextLetterSpacing>();
}

void SymbolLayer::setTextLetterSpacing(PropertyValue<float> value) {
    if (value == getTextLetterSpacing())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.unevaluated.get<TextLetterSpacing>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerLayoutPropertyChanged(*this, "text-letter-spacing");
}
PropertyValue<TextJustifyType> SymbolLayer::getDefaultTextJustify() {
    return TextJustify::defaultValue();
}

PropertyValue<TextJustifyType> SymbolLayer::getTextJustify() const {
    return impl().layout.unevaluated.get<TextJustify>();
}

void SymbolLayer::setTextJustify(PropertyValue<TextJustifyType> value) {
    if (value == getTextJustify())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.unevaluated.get<TextJustify>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerLayoutPropertyChanged(*this, "text-justify");
}
PropertyValue<TextAnchorType> SymbolLayer::getDefaultTextAnchor() {
    return TextAnchor::defaultValue();
}

PropertyValue<TextAnchorType> SymbolLayer::getTextAnchor() const {
    return impl().layout.unevaluated.get<TextAnchor>();
}

void SymbolLayer::setTextAnchor(PropertyValue<TextAnchorType> value) {
    if (value == getTextAnchor())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.unevaluated.get<TextAnchor>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerLayoutPropertyChanged(*this, "text-anchor");
}
PropertyValue<float> SymbolLayer::getDefaultTextMaxAngle() {
    return TextMaxAngle::defaultValue();
}

PropertyValue<float> SymbolLayer::getTextMaxAngle() const {
    return impl().layout.unevaluated.get<TextMaxAngle>();
}

void SymbolLayer::setTextMaxAngle(PropertyValue<float> value) {
    if (value == getTextMaxAngle())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.unevaluated.get<TextMaxAngle>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerLayoutPropertyChanged(*this, "text-max-angle");
}
DataDrivenPropertyValue<float> SymbolLayer::getDefaultTextRotate() {
    return TextRotate::defaultValue();
}

DataDrivenPropertyValue<float> SymbolLayer::getTextRotate() const {
    return impl().layout.unevaluated.get<TextRotate>();
}

void SymbolLayer::setTextRotate(DataDrivenPropertyValue<float> value) {
    if (value == getTextRotate())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.unevaluated.get<TextRotate>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerLayoutPropertyChanged(*this, "text-rotate");
}
PropertyValue<float> SymbolLayer::getDefaultTextPadding() {
    return TextPadding::defaultValue();
}

PropertyValue<float> SymbolLayer::getTextPadding() const {
    return impl().layout.unevaluated.get<TextPadding>();
}

void SymbolLayer::setTextPadding(PropertyValue<float> value) {
    if (value == getTextPadding())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.unevaluated.get<TextPadding>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerLayoutPropertyChanged(*this, "text-padding");
}
PropertyValue<bool> SymbolLayer::getDefaultTextKeepUpright() {
    return TextKeepUpright::defaultValue();
}

PropertyValue<bool> SymbolLayer::getTextKeepUpright() const {
    return impl().layout.unevaluated.get<TextKeepUpright>();
}

void SymbolLayer::setTextKeepUpright(PropertyValue<bool> value) {
    if (value == getTextKeepUpright())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.unevaluated.get<TextKeepUpright>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerLayoutPropertyChanged(*this, "text-keep-upright");
}
DataDrivenPropertyValue<TextTransformType> SymbolLayer::getDefaultTextTransform() {
    return TextTransform::defaultValue();
}

DataDrivenPropertyValue<TextTransformType> SymbolLayer::getTextTransform() const {
    return impl().layout.unevaluated.get<TextTransform>();
}

void SymbolLayer::setTextTransform(DataDrivenPropertyValue<TextTransformType> value) {
    if (value == getTextTransform())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.unevaluated.get<TextTransform>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerLayoutPropertyChanged(*this, "text-transform");
}
DataDrivenPropertyValue<std::array<float, 2>> SymbolLayer::getDefaultTextOffset() {
    return TextOffset::defaultValue();
}

DataDrivenPropertyValue<std::array<float, 2>> SymbolLayer::getTextOffset() const {
    return impl().layout.unevaluated.get<TextOffset>();
}

void SymbolLayer::setTextOffset(DataDrivenPropertyValue<std::array<float, 2>> value) {
    if (value == getTextOffset())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.unevaluated.get<TextOffset>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerLayoutPropertyChanged(*this, "text-offset");
}
PropertyValue<bool> SymbolLayer::getDefaultTextAllowOverlap() {
    return TextAllowOverlap::defaultValue();
}

PropertyValue<bool> SymbolLayer::getTextAllowOverlap() const {
    return impl().layout.unevaluated.get<TextAllowOverlap>();
}

void SymbolLayer::setTextAllowOverlap(PropertyValue<bool> value) {
    if (value == getTextAllowOverlap())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.unevaluated.get<TextAllowOverlap>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerLayoutPropertyChanged(*this, "text-allow-overlap");
}
PropertyValue<bool> SymbolLayer::getDefaultTextIgnorePlacement() {
    return TextIgnorePlacement::defaultValue();
}

PropertyValue<bool> SymbolLayer::getTextIgnorePlacement() const {
    return impl().layout.unevaluated.get<TextIgnorePlacement>();
}

void SymbolLayer::setTextIgnorePlacement(PropertyValue<bool> value) {
    if (value == getTextIgnorePlacement())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.unevaluated.get<TextIgnorePlacement>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerLayoutPropertyChanged(*this, "text-ignore-placement");
}
PropertyValue<bool> SymbolLayer::getDefaultTextOptional() {
    return TextOptional::defaultValue();
}

PropertyValue<bool> SymbolLayer::getTextOptional() const {
    return impl().layout.unevaluated.get<TextOptional>();
}

void SymbolLayer::setTextOptional(PropertyValue<bool> value) {
    if (value == getTextOptional())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.unevaluated.get<TextOptional>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerLayoutPropertyChanged(*this, "text-optional");
}

// Paint properties

DataDrivenPropertyValue<float> SymbolLayer::getDefaultIconOpacity() {
    return { 1 };
}

DataDrivenPropertyValue<float> SymbolLayer::getIconOpacity(const optional<std::string>& klass) const {
    return impl().paint.template get<IconOpacity>().get(klass);
}

void SymbolLayer::setIconOpacity(DataDrivenPropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getIconOpacity(klass))
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<IconOpacity>().set(value, klass);
    baseImpl = std::move(impl_);
    if (value.isDataDriven()) {
        observer->onLayerDataDrivenPaintPropertyChanged(*this);
    } else {
        observer->onLayerPaintPropertyChanged(*this);
    }
}

void SymbolLayer::setIconOpacityTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<IconOpacity>().setTransition(value, klass);
    baseImpl = std::move(impl_);
}

TransitionOptions SymbolLayer::getIconOpacityTransition(const optional<std::string>& klass) const {
    return impl().paint.template get<IconOpacity>().getTransition(klass);
}

DataDrivenPropertyValue<Color> SymbolLayer::getDefaultIconColor() {
    return { Color::black() };
}

DataDrivenPropertyValue<Color> SymbolLayer::getIconColor(const optional<std::string>& klass) const {
    return impl().paint.template get<IconColor>().get(klass);
}

void SymbolLayer::setIconColor(DataDrivenPropertyValue<Color> value, const optional<std::string>& klass) {
    if (value == getIconColor(klass))
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<IconColor>().set(value, klass);
    baseImpl = std::move(impl_);
    if (value.isDataDriven()) {
        observer->onLayerDataDrivenPaintPropertyChanged(*this);
    } else {
        observer->onLayerPaintPropertyChanged(*this);
    }
}

void SymbolLayer::setIconColorTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<IconColor>().setTransition(value, klass);
    baseImpl = std::move(impl_);
}

TransitionOptions SymbolLayer::getIconColorTransition(const optional<std::string>& klass) const {
    return impl().paint.template get<IconColor>().getTransition(klass);
}

DataDrivenPropertyValue<Color> SymbolLayer::getDefaultIconHaloColor() {
    return { {} };
}

DataDrivenPropertyValue<Color> SymbolLayer::getIconHaloColor(const optional<std::string>& klass) const {
    return impl().paint.template get<IconHaloColor>().get(klass);
}

void SymbolLayer::setIconHaloColor(DataDrivenPropertyValue<Color> value, const optional<std::string>& klass) {
    if (value == getIconHaloColor(klass))
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<IconHaloColor>().set(value, klass);
    baseImpl = std::move(impl_);
    if (value.isDataDriven()) {
        observer->onLayerDataDrivenPaintPropertyChanged(*this);
    } else {
        observer->onLayerPaintPropertyChanged(*this);
    }
}

void SymbolLayer::setIconHaloColorTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<IconHaloColor>().setTransition(value, klass);
    baseImpl = std::move(impl_);
}

TransitionOptions SymbolLayer::getIconHaloColorTransition(const optional<std::string>& klass) const {
    return impl().paint.template get<IconHaloColor>().getTransition(klass);
}

DataDrivenPropertyValue<float> SymbolLayer::getDefaultIconHaloWidth() {
    return { 0 };
}

DataDrivenPropertyValue<float> SymbolLayer::getIconHaloWidth(const optional<std::string>& klass) const {
    return impl().paint.template get<IconHaloWidth>().get(klass);
}

void SymbolLayer::setIconHaloWidth(DataDrivenPropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getIconHaloWidth(klass))
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<IconHaloWidth>().set(value, klass);
    baseImpl = std::move(impl_);
    if (value.isDataDriven()) {
        observer->onLayerDataDrivenPaintPropertyChanged(*this);
    } else {
        observer->onLayerPaintPropertyChanged(*this);
    }
}

void SymbolLayer::setIconHaloWidthTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<IconHaloWidth>().setTransition(value, klass);
    baseImpl = std::move(impl_);
}

TransitionOptions SymbolLayer::getIconHaloWidthTransition(const optional<std::string>& klass) const {
    return impl().paint.template get<IconHaloWidth>().getTransition(klass);
}

DataDrivenPropertyValue<float> SymbolLayer::getDefaultIconHaloBlur() {
    return { 0 };
}

DataDrivenPropertyValue<float> SymbolLayer::getIconHaloBlur(const optional<std::string>& klass) const {
    return impl().paint.template get<IconHaloBlur>().get(klass);
}

void SymbolLayer::setIconHaloBlur(DataDrivenPropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getIconHaloBlur(klass))
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<IconHaloBlur>().set(value, klass);
    baseImpl = std::move(impl_);
    if (value.isDataDriven()) {
        observer->onLayerDataDrivenPaintPropertyChanged(*this);
    } else {
        observer->onLayerPaintPropertyChanged(*this);
    }
}

void SymbolLayer::setIconHaloBlurTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<IconHaloBlur>().setTransition(value, klass);
    baseImpl = std::move(impl_);
}

TransitionOptions SymbolLayer::getIconHaloBlurTransition(const optional<std::string>& klass) const {
    return impl().paint.template get<IconHaloBlur>().getTransition(klass);
}

PropertyValue<std::array<float, 2>> SymbolLayer::getDefaultIconTranslate() {
    return { {{ 0, 0 }} };
}

PropertyValue<std::array<float, 2>> SymbolLayer::getIconTranslate(const optional<std::string>& klass) const {
    return impl().paint.template get<IconTranslate>().get(klass);
}

void SymbolLayer::setIconTranslate(PropertyValue<std::array<float, 2>> value, const optional<std::string>& klass) {
    if (value == getIconTranslate(klass))
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<IconTranslate>().set(value, klass);
    baseImpl = std::move(impl_);
    observer->onLayerPaintPropertyChanged(*this);
}

void SymbolLayer::setIconTranslateTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<IconTranslate>().setTransition(value, klass);
    baseImpl = std::move(impl_);
}

TransitionOptions SymbolLayer::getIconTranslateTransition(const optional<std::string>& klass) const {
    return impl().paint.template get<IconTranslate>().getTransition(klass);
}

PropertyValue<TranslateAnchorType> SymbolLayer::getDefaultIconTranslateAnchor() {
    return { TranslateAnchorType::Map };
}

PropertyValue<TranslateAnchorType> SymbolLayer::getIconTranslateAnchor(const optional<std::string>& klass) const {
    return impl().paint.template get<IconTranslateAnchor>().get(klass);
}

void SymbolLayer::setIconTranslateAnchor(PropertyValue<TranslateAnchorType> value, const optional<std::string>& klass) {
    if (value == getIconTranslateAnchor(klass))
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<IconTranslateAnchor>().set(value, klass);
    baseImpl = std::move(impl_);
    observer->onLayerPaintPropertyChanged(*this);
}

void SymbolLayer::setIconTranslateAnchorTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<IconTranslateAnchor>().setTransition(value, klass);
    baseImpl = std::move(impl_);
}

TransitionOptions SymbolLayer::getIconTranslateAnchorTransition(const optional<std::string>& klass) const {
    return impl().paint.template get<IconTranslateAnchor>().getTransition(klass);
}

DataDrivenPropertyValue<float> SymbolLayer::getDefaultTextOpacity() {
    return { 1 };
}

DataDrivenPropertyValue<float> SymbolLayer::getTextOpacity(const optional<std::string>& klass) const {
    return impl().paint.template get<TextOpacity>().get(klass);
}

void SymbolLayer::setTextOpacity(DataDrivenPropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getTextOpacity(klass))
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<TextOpacity>().set(value, klass);
    baseImpl = std::move(impl_);
    if (value.isDataDriven()) {
        observer->onLayerDataDrivenPaintPropertyChanged(*this);
    } else {
        observer->onLayerPaintPropertyChanged(*this);
    }
}

void SymbolLayer::setTextOpacityTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<TextOpacity>().setTransition(value, klass);
    baseImpl = std::move(impl_);
}

TransitionOptions SymbolLayer::getTextOpacityTransition(const optional<std::string>& klass) const {
    return impl().paint.template get<TextOpacity>().getTransition(klass);
}

DataDrivenPropertyValue<Color> SymbolLayer::getDefaultTextColor() {
    return { Color::black() };
}

DataDrivenPropertyValue<Color> SymbolLayer::getTextColor(const optional<std::string>& klass) const {
    return impl().paint.template get<TextColor>().get(klass);
}

void SymbolLayer::setTextColor(DataDrivenPropertyValue<Color> value, const optional<std::string>& klass) {
    if (value == getTextColor(klass))
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<TextColor>().set(value, klass);
    baseImpl = std::move(impl_);
    if (value.isDataDriven()) {
        observer->onLayerDataDrivenPaintPropertyChanged(*this);
    } else {
        observer->onLayerPaintPropertyChanged(*this);
    }
}

void SymbolLayer::setTextColorTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<TextColor>().setTransition(value, klass);
    baseImpl = std::move(impl_);
}

TransitionOptions SymbolLayer::getTextColorTransition(const optional<std::string>& klass) const {
    return impl().paint.template get<TextColor>().getTransition(klass);
}

DataDrivenPropertyValue<Color> SymbolLayer::getDefaultTextHaloColor() {
    return { {} };
}

DataDrivenPropertyValue<Color> SymbolLayer::getTextHaloColor(const optional<std::string>& klass) const {
    return impl().paint.template get<TextHaloColor>().get(klass);
}

void SymbolLayer::setTextHaloColor(DataDrivenPropertyValue<Color> value, const optional<std::string>& klass) {
    if (value == getTextHaloColor(klass))
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<TextHaloColor>().set(value, klass);
    baseImpl = std::move(impl_);
    if (value.isDataDriven()) {
        observer->onLayerDataDrivenPaintPropertyChanged(*this);
    } else {
        observer->onLayerPaintPropertyChanged(*this);
    }
}

void SymbolLayer::setTextHaloColorTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<TextHaloColor>().setTransition(value, klass);
    baseImpl = std::move(impl_);
}

TransitionOptions SymbolLayer::getTextHaloColorTransition(const optional<std::string>& klass) const {
    return impl().paint.template get<TextHaloColor>().getTransition(klass);
}

DataDrivenPropertyValue<float> SymbolLayer::getDefaultTextHaloWidth() {
    return { 0 };
}

DataDrivenPropertyValue<float> SymbolLayer::getTextHaloWidth(const optional<std::string>& klass) const {
    return impl().paint.template get<TextHaloWidth>().get(klass);
}

void SymbolLayer::setTextHaloWidth(DataDrivenPropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getTextHaloWidth(klass))
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<TextHaloWidth>().set(value, klass);
    baseImpl = std::move(impl_);
    if (value.isDataDriven()) {
        observer->onLayerDataDrivenPaintPropertyChanged(*this);
    } else {
        observer->onLayerPaintPropertyChanged(*this);
    }
}

void SymbolLayer::setTextHaloWidthTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<TextHaloWidth>().setTransition(value, klass);
    baseImpl = std::move(impl_);
}

TransitionOptions SymbolLayer::getTextHaloWidthTransition(const optional<std::string>& klass) const {
    return impl().paint.template get<TextHaloWidth>().getTransition(klass);
}

DataDrivenPropertyValue<float> SymbolLayer::getDefaultTextHaloBlur() {
    return { 0 };
}

DataDrivenPropertyValue<float> SymbolLayer::getTextHaloBlur(const optional<std::string>& klass) const {
    return impl().paint.template get<TextHaloBlur>().get(klass);
}

void SymbolLayer::setTextHaloBlur(DataDrivenPropertyValue<float> value, const optional<std::string>& klass) {
    if (value == getTextHaloBlur(klass))
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<TextHaloBlur>().set(value, klass);
    baseImpl = std::move(impl_);
    if (value.isDataDriven()) {
        observer->onLayerDataDrivenPaintPropertyChanged(*this);
    } else {
        observer->onLayerPaintPropertyChanged(*this);
    }
}

void SymbolLayer::setTextHaloBlurTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<TextHaloBlur>().setTransition(value, klass);
    baseImpl = std::move(impl_);
}

TransitionOptions SymbolLayer::getTextHaloBlurTransition(const optional<std::string>& klass) const {
    return impl().paint.template get<TextHaloBlur>().getTransition(klass);
}

PropertyValue<std::array<float, 2>> SymbolLayer::getDefaultTextTranslate() {
    return { {{ 0, 0 }} };
}

PropertyValue<std::array<float, 2>> SymbolLayer::getTextTranslate(const optional<std::string>& klass) const {
    return impl().paint.template get<TextTranslate>().get(klass);
}

void SymbolLayer::setTextTranslate(PropertyValue<std::array<float, 2>> value, const optional<std::string>& klass) {
    if (value == getTextTranslate(klass))
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<TextTranslate>().set(value, klass);
    baseImpl = std::move(impl_);
    observer->onLayerPaintPropertyChanged(*this);
}

void SymbolLayer::setTextTranslateTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<TextTranslate>().setTransition(value, klass);
    baseImpl = std::move(impl_);
}

TransitionOptions SymbolLayer::getTextTranslateTransition(const optional<std::string>& klass) const {
    return impl().paint.template get<TextTranslate>().getTransition(klass);
}

PropertyValue<TranslateAnchorType> SymbolLayer::getDefaultTextTranslateAnchor() {
    return { TranslateAnchorType::Map };
}

PropertyValue<TranslateAnchorType> SymbolLayer::getTextTranslateAnchor(const optional<std::string>& klass) const {
    return impl().paint.template get<TextTranslateAnchor>().get(klass);
}

void SymbolLayer::setTextTranslateAnchor(PropertyValue<TranslateAnchorType> value, const optional<std::string>& klass) {
    if (value == getTextTranslateAnchor(klass))
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<TextTranslateAnchor>().set(value, klass);
    baseImpl = std::move(impl_);
    observer->onLayerPaintPropertyChanged(*this);
}

void SymbolLayer::setTextTranslateAnchorTransition(const TransitionOptions& value, const optional<std::string>& klass) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<TextTranslateAnchor>().setTransition(value, klass);
    baseImpl = std::move(impl_);
}

TransitionOptions SymbolLayer::getTextTranslateAnchorTransition(const optional<std::string>& klass) const {
    return impl().paint.template get<TextTranslateAnchor>().getTransition(klass);
}

} // namespace style
} // namespace mbgl
