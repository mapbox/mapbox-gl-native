// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/layer/symbol_layer.hpp>
#include <mbgl/layer/symbol_layer_impl.hpp>

namespace mbgl {

SymbolLayer::SymbolLayer(const std::string& layerID)
    : Layer(Type::Symbol, std::make_unique<Impl>())
    , impl(static_cast<Impl*>(baseImpl.get())) {
    impl->id = layerID;
}

SymbolLayer::SymbolLayer(const Impl& other)
    : Layer(Type::Symbol, std::make_unique<Impl>(other))
    , impl(static_cast<Impl*>(baseImpl.get())) {
}

SymbolLayer::~SymbolLayer() = default;

std::unique_ptr<Layer> SymbolLayer::Impl::clone() const {
    return std::make_unique<SymbolLayer>(*this);
}

// Source

void SymbolLayer::setSource(const std::string& sourceID, const std::string& sourceLayer) {
    impl->source = sourceID;
    impl->sourceLayer = sourceLayer;
}

const std::string& SymbolLayer::getSourceID() const {
    return impl->source;
}

const std::string& SymbolLayer::getSourceLayer() const {
    return impl->sourceLayer;
}

// Layout properties

Function<SymbolPlacementType> SymbolLayer::getSymbolPlacement() const {
    return *impl->layout.symbolPlacement.parsedValue;
}

void SymbolLayer::setSymbolPlacement(Function<SymbolPlacementType> value) {
    impl->layout.symbolPlacement.parsedValue = value;
}
Function<float> SymbolLayer::getSymbolSpacing() const {
    return *impl->layout.symbolSpacing.parsedValue;
}

void SymbolLayer::setSymbolSpacing(Function<float> value) {
    impl->layout.symbolSpacing.parsedValue = value;
}
Function<bool> SymbolLayer::getSymbolAvoidEdges() const {
    return *impl->layout.symbolAvoidEdges.parsedValue;
}

void SymbolLayer::setSymbolAvoidEdges(Function<bool> value) {
    impl->layout.symbolAvoidEdges.parsedValue = value;
}
Function<bool> SymbolLayer::getIconAllowOverlap() const {
    return *impl->layout.iconAllowOverlap.parsedValue;
}

void SymbolLayer::setIconAllowOverlap(Function<bool> value) {
    impl->layout.iconAllowOverlap.parsedValue = value;
}
Function<bool> SymbolLayer::getIconIgnorePlacement() const {
    return *impl->layout.iconIgnorePlacement.parsedValue;
}

void SymbolLayer::setIconIgnorePlacement(Function<bool> value) {
    impl->layout.iconIgnorePlacement.parsedValue = value;
}
Function<bool> SymbolLayer::getIconOptional() const {
    return *impl->layout.iconOptional.parsedValue;
}

void SymbolLayer::setIconOptional(Function<bool> value) {
    impl->layout.iconOptional.parsedValue = value;
}
Function<RotationAlignmentType> SymbolLayer::getIconRotationAlignment() const {
    return *impl->layout.iconRotationAlignment.parsedValue;
}

void SymbolLayer::setIconRotationAlignment(Function<RotationAlignmentType> value) {
    impl->layout.iconRotationAlignment.parsedValue = value;
}
Function<float> SymbolLayer::getIconSize() const {
    return *impl->layout.iconSize.parsedValue;
}

void SymbolLayer::setIconSize(Function<float> value) {
    impl->layout.iconSize.parsedValue = value;
}
Function<std::string> SymbolLayer::getIconImage() const {
    return *impl->layout.iconImage.parsedValue;
}

void SymbolLayer::setIconImage(Function<std::string> value) {
    impl->layout.iconImage.parsedValue = value;
}
Function<float> SymbolLayer::getIconRotate() const {
    return *impl->layout.iconRotate.parsedValue;
}

void SymbolLayer::setIconRotate(Function<float> value) {
    impl->layout.iconRotate.parsedValue = value;
}
Function<float> SymbolLayer::getIconPadding() const {
    return *impl->layout.iconPadding.parsedValue;
}

void SymbolLayer::setIconPadding(Function<float> value) {
    impl->layout.iconPadding.parsedValue = value;
}
Function<bool> SymbolLayer::getIconKeepUpright() const {
    return *impl->layout.iconKeepUpright.parsedValue;
}

void SymbolLayer::setIconKeepUpright(Function<bool> value) {
    impl->layout.iconKeepUpright.parsedValue = value;
}
Function<std::array<float, 2>> SymbolLayer::getIconOffset() const {
    return *impl->layout.iconOffset.parsedValue;
}

void SymbolLayer::setIconOffset(Function<std::array<float, 2>> value) {
    impl->layout.iconOffset.parsedValue = value;
}
Function<RotationAlignmentType> SymbolLayer::getTextRotationAlignment() const {
    return *impl->layout.textRotationAlignment.parsedValue;
}

void SymbolLayer::setTextRotationAlignment(Function<RotationAlignmentType> value) {
    impl->layout.textRotationAlignment.parsedValue = value;
}
Function<std::string> SymbolLayer::getTextField() const {
    return *impl->layout.textField.parsedValue;
}

void SymbolLayer::setTextField(Function<std::string> value) {
    impl->layout.textField.parsedValue = value;
}
Function<std::vector<std::string>> SymbolLayer::getTextFont() const {
    return *impl->layout.textFont.parsedValue;
}

void SymbolLayer::setTextFont(Function<std::vector<std::string>> value) {
    impl->layout.textFont.parsedValue = value;
}
Function<float> SymbolLayer::getTextSize() const {
    return *impl->layout.textSize.parsedValue;
}

void SymbolLayer::setTextSize(Function<float> value) {
    impl->layout.textSize.parsedValue = value;
}
Function<float> SymbolLayer::getTextMaxWidth() const {
    return *impl->layout.textMaxWidth.parsedValue;
}

void SymbolLayer::setTextMaxWidth(Function<float> value) {
    impl->layout.textMaxWidth.parsedValue = value;
}
Function<float> SymbolLayer::getTextLineHeight() const {
    return *impl->layout.textLineHeight.parsedValue;
}

void SymbolLayer::setTextLineHeight(Function<float> value) {
    impl->layout.textLineHeight.parsedValue = value;
}
Function<float> SymbolLayer::getTextLetterSpacing() const {
    return *impl->layout.textLetterSpacing.parsedValue;
}

void SymbolLayer::setTextLetterSpacing(Function<float> value) {
    impl->layout.textLetterSpacing.parsedValue = value;
}
Function<TextJustifyType> SymbolLayer::getTextJustify() const {
    return *impl->layout.textJustify.parsedValue;
}

void SymbolLayer::setTextJustify(Function<TextJustifyType> value) {
    impl->layout.textJustify.parsedValue = value;
}
Function<TextAnchorType> SymbolLayer::getTextAnchor() const {
    return *impl->layout.textAnchor.parsedValue;
}

void SymbolLayer::setTextAnchor(Function<TextAnchorType> value) {
    impl->layout.textAnchor.parsedValue = value;
}
Function<float> SymbolLayer::getTextMaxAngle() const {
    return *impl->layout.textMaxAngle.parsedValue;
}

void SymbolLayer::setTextMaxAngle(Function<float> value) {
    impl->layout.textMaxAngle.parsedValue = value;
}
Function<float> SymbolLayer::getTextRotate() const {
    return *impl->layout.textRotate.parsedValue;
}

void SymbolLayer::setTextRotate(Function<float> value) {
    impl->layout.textRotate.parsedValue = value;
}
Function<float> SymbolLayer::getTextPadding() const {
    return *impl->layout.textPadding.parsedValue;
}

void SymbolLayer::setTextPadding(Function<float> value) {
    impl->layout.textPadding.parsedValue = value;
}
Function<bool> SymbolLayer::getTextKeepUpright() const {
    return *impl->layout.textKeepUpright.parsedValue;
}

void SymbolLayer::setTextKeepUpright(Function<bool> value) {
    impl->layout.textKeepUpright.parsedValue = value;
}
Function<TextTransformType> SymbolLayer::getTextTransform() const {
    return *impl->layout.textTransform.parsedValue;
}

void SymbolLayer::setTextTransform(Function<TextTransformType> value) {
    impl->layout.textTransform.parsedValue = value;
}
Function<std::array<float, 2>> SymbolLayer::getTextOffset() const {
    return *impl->layout.textOffset.parsedValue;
}

void SymbolLayer::setTextOffset(Function<std::array<float, 2>> value) {
    impl->layout.textOffset.parsedValue = value;
}
Function<bool> SymbolLayer::getTextAllowOverlap() const {
    return *impl->layout.textAllowOverlap.parsedValue;
}

void SymbolLayer::setTextAllowOverlap(Function<bool> value) {
    impl->layout.textAllowOverlap.parsedValue = value;
}
Function<bool> SymbolLayer::getTextIgnorePlacement() const {
    return *impl->layout.textIgnorePlacement.parsedValue;
}

void SymbolLayer::setTextIgnorePlacement(Function<bool> value) {
    impl->layout.textIgnorePlacement.parsedValue = value;
}
Function<bool> SymbolLayer::getTextOptional() const {
    return *impl->layout.textOptional.parsedValue;
}

void SymbolLayer::setTextOptional(Function<bool> value) {
    impl->layout.textOptional.parsedValue = value;
}

// Paint properties

Function<float> SymbolLayer::getIconOpacity() const {
    return impl->paint.iconOpacity.values.at(ClassID::Default);
}

void SymbolLayer::setIconOpacity(Function<float> value) {
    impl->paint.iconOpacity.values.emplace(ClassID::Default, value);
}

Function<Color> SymbolLayer::getIconColor() const {
    return impl->paint.iconColor.values.at(ClassID::Default);
}

void SymbolLayer::setIconColor(Function<Color> value) {
    impl->paint.iconColor.values.emplace(ClassID::Default, value);
}

Function<Color> SymbolLayer::getIconHaloColor() const {
    return impl->paint.iconHaloColor.values.at(ClassID::Default);
}

void SymbolLayer::setIconHaloColor(Function<Color> value) {
    impl->paint.iconHaloColor.values.emplace(ClassID::Default, value);
}

Function<float> SymbolLayer::getIconHaloWidth() const {
    return impl->paint.iconHaloWidth.values.at(ClassID::Default);
}

void SymbolLayer::setIconHaloWidth(Function<float> value) {
    impl->paint.iconHaloWidth.values.emplace(ClassID::Default, value);
}

Function<float> SymbolLayer::getIconHaloBlur() const {
    return impl->paint.iconHaloBlur.values.at(ClassID::Default);
}

void SymbolLayer::setIconHaloBlur(Function<float> value) {
    impl->paint.iconHaloBlur.values.emplace(ClassID::Default, value);
}

Function<std::array<float, 2>> SymbolLayer::getIconTranslate() const {
    return impl->paint.iconTranslate.values.at(ClassID::Default);
}

void SymbolLayer::setIconTranslate(Function<std::array<float, 2>> value) {
    impl->paint.iconTranslate.values.emplace(ClassID::Default, value);
}

Function<TranslateAnchorType> SymbolLayer::getIconTranslateAnchor() const {
    return impl->paint.iconTranslateAnchor.values.at(ClassID::Default);
}

void SymbolLayer::setIconTranslateAnchor(Function<TranslateAnchorType> value) {
    impl->paint.iconTranslateAnchor.values.emplace(ClassID::Default, value);
}

Function<float> SymbolLayer::getTextOpacity() const {
    return impl->paint.textOpacity.values.at(ClassID::Default);
}

void SymbolLayer::setTextOpacity(Function<float> value) {
    impl->paint.textOpacity.values.emplace(ClassID::Default, value);
}

Function<Color> SymbolLayer::getTextColor() const {
    return impl->paint.textColor.values.at(ClassID::Default);
}

void SymbolLayer::setTextColor(Function<Color> value) {
    impl->paint.textColor.values.emplace(ClassID::Default, value);
}

Function<Color> SymbolLayer::getTextHaloColor() const {
    return impl->paint.textHaloColor.values.at(ClassID::Default);
}

void SymbolLayer::setTextHaloColor(Function<Color> value) {
    impl->paint.textHaloColor.values.emplace(ClassID::Default, value);
}

Function<float> SymbolLayer::getTextHaloWidth() const {
    return impl->paint.textHaloWidth.values.at(ClassID::Default);
}

void SymbolLayer::setTextHaloWidth(Function<float> value) {
    impl->paint.textHaloWidth.values.emplace(ClassID::Default, value);
}

Function<float> SymbolLayer::getTextHaloBlur() const {
    return impl->paint.textHaloBlur.values.at(ClassID::Default);
}

void SymbolLayer::setTextHaloBlur(Function<float> value) {
    impl->paint.textHaloBlur.values.emplace(ClassID::Default, value);
}

Function<std::array<float, 2>> SymbolLayer::getTextTranslate() const {
    return impl->paint.textTranslate.values.at(ClassID::Default);
}

void SymbolLayer::setTextTranslate(Function<std::array<float, 2>> value) {
    impl->paint.textTranslate.values.emplace(ClassID::Default, value);
}

Function<TranslateAnchorType> SymbolLayer::getTextTranslateAnchor() const {
    return impl->paint.textTranslateAnchor.values.at(ClassID::Default);
}

void SymbolLayer::setTextTranslateAnchor(Function<TranslateAnchorType> value) {
    impl->paint.textTranslateAnchor.values.emplace(ClassID::Default, value);
}

} // namespace mbgl
