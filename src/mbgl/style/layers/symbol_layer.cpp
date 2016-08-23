// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/style/layers/symbol_layer_impl.hpp>

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
    result->impl->ref = this->id;
    result->impl->paint = SymbolPaintProperties();
    return std::move(result);
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
}

const Filter& SymbolLayer::getFilter() const {
    return impl->filter;
}

// Layout properties

PropertyValue<SymbolPlacementType> SymbolLayer::getDefaultSymbolPlacement() {
    return { SymbolPlacementType::Point };
}

PropertyValue<SymbolPlacementType> SymbolLayer::getSymbolPlacement() const {
    return impl->layout.symbolPlacement.get();
}

void SymbolLayer::setSymbolPlacement(PropertyValue<SymbolPlacementType> value) {
    impl->layout.symbolPlacement.set(value);
}
PropertyValue<float> SymbolLayer::getDefaultSymbolSpacing() {
    return { 250 };
}

PropertyValue<float> SymbolLayer::getSymbolSpacing() const {
    return impl->layout.symbolSpacing.get();
}

void SymbolLayer::setSymbolSpacing(PropertyValue<float> value) {
    impl->layout.symbolSpacing.set(value);
}
PropertyValue<bool> SymbolLayer::getDefaultSymbolAvoidEdges() {
    return { false };
}

PropertyValue<bool> SymbolLayer::getSymbolAvoidEdges() const {
    return impl->layout.symbolAvoidEdges.get();
}

void SymbolLayer::setSymbolAvoidEdges(PropertyValue<bool> value) {
    impl->layout.symbolAvoidEdges.set(value);
}
PropertyValue<bool> SymbolLayer::getDefaultIconAllowOverlap() {
    return { false };
}

PropertyValue<bool> SymbolLayer::getIconAllowOverlap() const {
    return impl->layout.iconAllowOverlap.get();
}

void SymbolLayer::setIconAllowOverlap(PropertyValue<bool> value) {
    impl->layout.iconAllowOverlap.set(value);
}
PropertyValue<bool> SymbolLayer::getDefaultIconIgnorePlacement() {
    return { false };
}

PropertyValue<bool> SymbolLayer::getIconIgnorePlacement() const {
    return impl->layout.iconIgnorePlacement.get();
}

void SymbolLayer::setIconIgnorePlacement(PropertyValue<bool> value) {
    impl->layout.iconIgnorePlacement.set(value);
}
PropertyValue<bool> SymbolLayer::getDefaultIconOptional() {
    return { false };
}

PropertyValue<bool> SymbolLayer::getIconOptional() const {
    return impl->layout.iconOptional.get();
}

void SymbolLayer::setIconOptional(PropertyValue<bool> value) {
    impl->layout.iconOptional.set(value);
}
PropertyValue<AlignmentType> SymbolLayer::getDefaultIconRotationAlignment() {
    return { AlignmentType::Viewport };
}

PropertyValue<AlignmentType> SymbolLayer::getIconRotationAlignment() const {
    return impl->layout.iconRotationAlignment.get();
}

void SymbolLayer::setIconRotationAlignment(PropertyValue<AlignmentType> value) {
    impl->layout.iconRotationAlignment.set(value);
}
PropertyValue<float> SymbolLayer::getDefaultIconSize() {
    return { 1 };
}

PropertyValue<float> SymbolLayer::getIconSize() const {
    return impl->layout.iconSize.get();
}

void SymbolLayer::setIconSize(PropertyValue<float> value) {
    impl->layout.iconSize.set(value);
}
PropertyValue<IconTextFitType> SymbolLayer::getDefaultIconTextFit() {
    return { IconTextFitType::None };
}

PropertyValue<IconTextFitType> SymbolLayer::getIconTextFit() const {
    return impl->layout.iconTextFit.get();
}

void SymbolLayer::setIconTextFit(PropertyValue<IconTextFitType> value) {
    impl->layout.iconTextFit.set(value);
}
PropertyValue<std::array<float, 4>> SymbolLayer::getDefaultIconTextFitPadding() {
    return { {{ 0, 0, 0, 0 }} };
}

PropertyValue<std::array<float, 4>> SymbolLayer::getIconTextFitPadding() const {
    return impl->layout.iconTextFitPadding.get();
}

void SymbolLayer::setIconTextFitPadding(PropertyValue<std::array<float, 4>> value) {
    impl->layout.iconTextFitPadding.set(value);
}
PropertyValue<std::string> SymbolLayer::getDefaultIconImage() {
    return { "" };
}

PropertyValue<std::string> SymbolLayer::getIconImage() const {
    return impl->layout.iconImage.get();
}

void SymbolLayer::setIconImage(PropertyValue<std::string> value) {
    impl->layout.iconImage.set(value);
}
PropertyValue<float> SymbolLayer::getDefaultIconRotate() {
    return { 0 };
}

PropertyValue<float> SymbolLayer::getIconRotate() const {
    return impl->layout.iconRotate.get();
}

void SymbolLayer::setIconRotate(PropertyValue<float> value) {
    impl->layout.iconRotate.set(value);
}
PropertyValue<float> SymbolLayer::getDefaultIconPadding() {
    return { 2 };
}

PropertyValue<float> SymbolLayer::getIconPadding() const {
    return impl->layout.iconPadding.get();
}

void SymbolLayer::setIconPadding(PropertyValue<float> value) {
    impl->layout.iconPadding.set(value);
}
PropertyValue<bool> SymbolLayer::getDefaultIconKeepUpright() {
    return { false };
}

PropertyValue<bool> SymbolLayer::getIconKeepUpright() const {
    return impl->layout.iconKeepUpright.get();
}

void SymbolLayer::setIconKeepUpright(PropertyValue<bool> value) {
    impl->layout.iconKeepUpright.set(value);
}
PropertyValue<std::array<float, 2>> SymbolLayer::getDefaultIconOffset() {
    return { {{ 0, 0 }} };
}

PropertyValue<std::array<float, 2>> SymbolLayer::getIconOffset() const {
    return impl->layout.iconOffset.get();
}

void SymbolLayer::setIconOffset(PropertyValue<std::array<float, 2>> value) {
    impl->layout.iconOffset.set(value);
}
PropertyValue<AlignmentType> SymbolLayer::getDefaultTextPitchAlignment() {
    return { AlignmentType::Undefined };
}

PropertyValue<AlignmentType> SymbolLayer::getTextPitchAlignment() const {
    return impl->layout.textPitchAlignment.get();
}

void SymbolLayer::setTextPitchAlignment(PropertyValue<AlignmentType> value) {
    impl->layout.textPitchAlignment.set(value);
}
PropertyValue<AlignmentType> SymbolLayer::getDefaultTextRotationAlignment() {
    return { AlignmentType::Viewport };
}

PropertyValue<AlignmentType> SymbolLayer::getTextRotationAlignment() const {
    return impl->layout.textRotationAlignment.get();
}

void SymbolLayer::setTextRotationAlignment(PropertyValue<AlignmentType> value) {
    impl->layout.textRotationAlignment.set(value);
}
PropertyValue<std::string> SymbolLayer::getDefaultTextField() {
    return { "" };
}

PropertyValue<std::string> SymbolLayer::getTextField() const {
    return impl->layout.textField.get();
}

void SymbolLayer::setTextField(PropertyValue<std::string> value) {
    impl->layout.textField.set(value);
}
PropertyValue<std::vector<std::string>> SymbolLayer::getDefaultTextFont() {
    return { { "Open Sans Regular", "Arial Unicode MS Regular" } };
}

PropertyValue<std::vector<std::string>> SymbolLayer::getTextFont() const {
    return impl->layout.textFont.get();
}

void SymbolLayer::setTextFont(PropertyValue<std::vector<std::string>> value) {
    impl->layout.textFont.set(value);
}
PropertyValue<float> SymbolLayer::getDefaultTextSize() {
    return { 16 };
}

PropertyValue<float> SymbolLayer::getTextSize() const {
    return impl->layout.textSize.get();
}

void SymbolLayer::setTextSize(PropertyValue<float> value) {
    impl->layout.textSize.set(value);
}
PropertyValue<float> SymbolLayer::getDefaultTextMaxWidth() {
    return { 10 };
}

PropertyValue<float> SymbolLayer::getTextMaxWidth() const {
    return impl->layout.textMaxWidth.get();
}

void SymbolLayer::setTextMaxWidth(PropertyValue<float> value) {
    impl->layout.textMaxWidth.set(value);
}
PropertyValue<float> SymbolLayer::getDefaultTextLineHeight() {
    return { 1.2 };
}

PropertyValue<float> SymbolLayer::getTextLineHeight() const {
    return impl->layout.textLineHeight.get();
}

void SymbolLayer::setTextLineHeight(PropertyValue<float> value) {
    impl->layout.textLineHeight.set(value);
}
PropertyValue<float> SymbolLayer::getDefaultTextLetterSpacing() {
    return { 0 };
}

PropertyValue<float> SymbolLayer::getTextLetterSpacing() const {
    return impl->layout.textLetterSpacing.get();
}

void SymbolLayer::setTextLetterSpacing(PropertyValue<float> value) {
    impl->layout.textLetterSpacing.set(value);
}
PropertyValue<TextJustifyType> SymbolLayer::getDefaultTextJustify() {
    return { TextJustifyType::Center };
}

PropertyValue<TextJustifyType> SymbolLayer::getTextJustify() const {
    return impl->layout.textJustify.get();
}

void SymbolLayer::setTextJustify(PropertyValue<TextJustifyType> value) {
    impl->layout.textJustify.set(value);
}
PropertyValue<TextAnchorType> SymbolLayer::getDefaultTextAnchor() {
    return { TextAnchorType::Center };
}

PropertyValue<TextAnchorType> SymbolLayer::getTextAnchor() const {
    return impl->layout.textAnchor.get();
}

void SymbolLayer::setTextAnchor(PropertyValue<TextAnchorType> value) {
    impl->layout.textAnchor.set(value);
}
PropertyValue<float> SymbolLayer::getDefaultTextMaxAngle() {
    return { 45 };
}

PropertyValue<float> SymbolLayer::getTextMaxAngle() const {
    return impl->layout.textMaxAngle.get();
}

void SymbolLayer::setTextMaxAngle(PropertyValue<float> value) {
    impl->layout.textMaxAngle.set(value);
}
PropertyValue<float> SymbolLayer::getDefaultTextRotate() {
    return { 0 };
}

PropertyValue<float> SymbolLayer::getTextRotate() const {
    return impl->layout.textRotate.get();
}

void SymbolLayer::setTextRotate(PropertyValue<float> value) {
    impl->layout.textRotate.set(value);
}
PropertyValue<float> SymbolLayer::getDefaultTextPadding() {
    return { 2 };
}

PropertyValue<float> SymbolLayer::getTextPadding() const {
    return impl->layout.textPadding.get();
}

void SymbolLayer::setTextPadding(PropertyValue<float> value) {
    impl->layout.textPadding.set(value);
}
PropertyValue<bool> SymbolLayer::getDefaultTextKeepUpright() {
    return { true };
}

PropertyValue<bool> SymbolLayer::getTextKeepUpright() const {
    return impl->layout.textKeepUpright.get();
}

void SymbolLayer::setTextKeepUpright(PropertyValue<bool> value) {
    impl->layout.textKeepUpright.set(value);
}
PropertyValue<TextTransformType> SymbolLayer::getDefaultTextTransform() {
    return { TextTransformType::None };
}

PropertyValue<TextTransformType> SymbolLayer::getTextTransform() const {
    return impl->layout.textTransform.get();
}

void SymbolLayer::setTextTransform(PropertyValue<TextTransformType> value) {
    impl->layout.textTransform.set(value);
}
PropertyValue<std::array<float, 2>> SymbolLayer::getDefaultTextOffset() {
    return { {{ 0, 0 }} };
}

PropertyValue<std::array<float, 2>> SymbolLayer::getTextOffset() const {
    return impl->layout.textOffset.get();
}

void SymbolLayer::setTextOffset(PropertyValue<std::array<float, 2>> value) {
    impl->layout.textOffset.set(value);
}
PropertyValue<bool> SymbolLayer::getDefaultTextAllowOverlap() {
    return { false };
}

PropertyValue<bool> SymbolLayer::getTextAllowOverlap() const {
    return impl->layout.textAllowOverlap.get();
}

void SymbolLayer::setTextAllowOverlap(PropertyValue<bool> value) {
    impl->layout.textAllowOverlap.set(value);
}
PropertyValue<bool> SymbolLayer::getDefaultTextIgnorePlacement() {
    return { false };
}

PropertyValue<bool> SymbolLayer::getTextIgnorePlacement() const {
    return impl->layout.textIgnorePlacement.get();
}

void SymbolLayer::setTextIgnorePlacement(PropertyValue<bool> value) {
    impl->layout.textIgnorePlacement.set(value);
}
PropertyValue<bool> SymbolLayer::getDefaultTextOptional() {
    return { false };
}

PropertyValue<bool> SymbolLayer::getTextOptional() const {
    return impl->layout.textOptional.get();
}

void SymbolLayer::setTextOptional(PropertyValue<bool> value) {
    impl->layout.textOptional.set(value);
}

// Paint properties

PropertyValue<float> SymbolLayer::getDefaultIconOpacity() {
    return { 1 };
}

PropertyValue<float> SymbolLayer::getIconOpacity() const {
    return impl->paint.iconOpacity.get();
}

void SymbolLayer::setIconOpacity(PropertyValue<float> value, const optional<std::string>& klass) {
    impl->paint.iconOpacity.set(value, klass);
}

PropertyValue<Color> SymbolLayer::getDefaultIconColor() {
    return { Color::black() };
}

PropertyValue<Color> SymbolLayer::getIconColor() const {
    return impl->paint.iconColor.get();
}

void SymbolLayer::setIconColor(PropertyValue<Color> value, const optional<std::string>& klass) {
    impl->paint.iconColor.set(value, klass);
}

PropertyValue<Color> SymbolLayer::getDefaultIconHaloColor() {
    return { {} };
}

PropertyValue<Color> SymbolLayer::getIconHaloColor() const {
    return impl->paint.iconHaloColor.get();
}

void SymbolLayer::setIconHaloColor(PropertyValue<Color> value, const optional<std::string>& klass) {
    impl->paint.iconHaloColor.set(value, klass);
}

PropertyValue<float> SymbolLayer::getDefaultIconHaloWidth() {
    return { 0 };
}

PropertyValue<float> SymbolLayer::getIconHaloWidth() const {
    return impl->paint.iconHaloWidth.get();
}

void SymbolLayer::setIconHaloWidth(PropertyValue<float> value, const optional<std::string>& klass) {
    impl->paint.iconHaloWidth.set(value, klass);
}

PropertyValue<float> SymbolLayer::getDefaultIconHaloBlur() {
    return { 0 };
}

PropertyValue<float> SymbolLayer::getIconHaloBlur() const {
    return impl->paint.iconHaloBlur.get();
}

void SymbolLayer::setIconHaloBlur(PropertyValue<float> value, const optional<std::string>& klass) {
    impl->paint.iconHaloBlur.set(value, klass);
}

PropertyValue<std::array<float, 2>> SymbolLayer::getDefaultIconTranslate() {
    return { {{ 0, 0 }} };
}

PropertyValue<std::array<float, 2>> SymbolLayer::getIconTranslate() const {
    return impl->paint.iconTranslate.get();
}

void SymbolLayer::setIconTranslate(PropertyValue<std::array<float, 2>> value, const optional<std::string>& klass) {
    impl->paint.iconTranslate.set(value, klass);
}

PropertyValue<TranslateAnchorType> SymbolLayer::getDefaultIconTranslateAnchor() {
    return { TranslateAnchorType::Map };
}

PropertyValue<TranslateAnchorType> SymbolLayer::getIconTranslateAnchor() const {
    return impl->paint.iconTranslateAnchor.get();
}

void SymbolLayer::setIconTranslateAnchor(PropertyValue<TranslateAnchorType> value, const optional<std::string>& klass) {
    impl->paint.iconTranslateAnchor.set(value, klass);
}

PropertyValue<float> SymbolLayer::getDefaultTextOpacity() {
    return { 1 };
}

PropertyValue<float> SymbolLayer::getTextOpacity() const {
    return impl->paint.textOpacity.get();
}

void SymbolLayer::setTextOpacity(PropertyValue<float> value, const optional<std::string>& klass) {
    impl->paint.textOpacity.set(value, klass);
}

PropertyValue<Color> SymbolLayer::getDefaultTextColor() {
    return { Color::black() };
}

PropertyValue<Color> SymbolLayer::getTextColor() const {
    return impl->paint.textColor.get();
}

void SymbolLayer::setTextColor(PropertyValue<Color> value, const optional<std::string>& klass) {
    impl->paint.textColor.set(value, klass);
}

PropertyValue<Color> SymbolLayer::getDefaultTextHaloColor() {
    return { {} };
}

PropertyValue<Color> SymbolLayer::getTextHaloColor() const {
    return impl->paint.textHaloColor.get();
}

void SymbolLayer::setTextHaloColor(PropertyValue<Color> value, const optional<std::string>& klass) {
    impl->paint.textHaloColor.set(value, klass);
}

PropertyValue<float> SymbolLayer::getDefaultTextHaloWidth() {
    return { 0 };
}

PropertyValue<float> SymbolLayer::getTextHaloWidth() const {
    return impl->paint.textHaloWidth.get();
}

void SymbolLayer::setTextHaloWidth(PropertyValue<float> value, const optional<std::string>& klass) {
    impl->paint.textHaloWidth.set(value, klass);
}

PropertyValue<float> SymbolLayer::getDefaultTextHaloBlur() {
    return { 0 };
}

PropertyValue<float> SymbolLayer::getTextHaloBlur() const {
    return impl->paint.textHaloBlur.get();
}

void SymbolLayer::setTextHaloBlur(PropertyValue<float> value, const optional<std::string>& klass) {
    impl->paint.textHaloBlur.set(value, klass);
}

PropertyValue<std::array<float, 2>> SymbolLayer::getDefaultTextTranslate() {
    return { {{ 0, 0 }} };
}

PropertyValue<std::array<float, 2>> SymbolLayer::getTextTranslate() const {
    return impl->paint.textTranslate.get();
}

void SymbolLayer::setTextTranslate(PropertyValue<std::array<float, 2>> value, const optional<std::string>& klass) {
    impl->paint.textTranslate.set(value, klass);
}

PropertyValue<TranslateAnchorType> SymbolLayer::getDefaultTextTranslateAnchor() {
    return { TranslateAnchorType::Map };
}

PropertyValue<TranslateAnchorType> SymbolLayer::getTextTranslateAnchor() const {
    return impl->paint.textTranslateAnchor.get();
}

void SymbolLayer::setTextTranslateAnchor(PropertyValue<TranslateAnchorType> value, const optional<std::string>& klass) {
    impl->paint.textTranslateAnchor.set(value, klass);
}

} // namespace style
} // namespace mbgl
