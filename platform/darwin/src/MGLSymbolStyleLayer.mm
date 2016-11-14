// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLSource.h"
#import "MGLMapView_Private.h"
#import "NSPredicate+MGLAdditions.h"
#import "MGLStyleLayer_Private.h"
#import "MGLStyleValue_Private.h"
#import "MGLSymbolStyleLayer.h"

#include <mbgl/style/layers/symbol_layer.hpp>

@interface MGLSymbolStyleLayer ()

@property (nonatomic) mbgl::style::SymbolLayer *rawLayer;

@end

@implementation MGLSymbolStyleLayer
{
    std::unique_ptr<mbgl::style::SymbolLayer> _pendingLayer;
}

- (instancetype)initWithIdentifier:(NSString *)identifier source:(MGLSource *)source
{
    if (self = [super initWithIdentifier:identifier source:source]) {
        auto layer = std::make_unique<mbgl::style::SymbolLayer>(identifier.UTF8String, source.identifier.UTF8String);
        _pendingLayer = std::move(layer);
        self.rawLayer = _pendingLayer.get();
    }
    return self;
}


- (NSString *)sourceLayerIdentifier
{
    auto layerID = self.rawLayer->getSourceLayer();
    return layerID.empty() ? nil : @(layerID.c_str());
}

- (void)setSourceLayerIdentifier:(NSString *)sourceLayerIdentifier
{
    self.rawLayer->setSourceLayer(sourceLayerIdentifier.UTF8String ?: "");
}

- (void)setPredicate:(NSPredicate *)predicate
{
    self.rawLayer->setFilter(predicate.mgl_filter);
}

- (NSPredicate *)predicate
{
    return [NSPredicate mgl_predicateWithFilter:self.rawLayer->getFilter()];
}


#pragma mark -  Adding to and removing from a map view

- (void)addToMapView:(MGLMapView *)mapView
{
    [self addToMapView:mapView belowLayer:nil];
}

- (void)addToMapView:(MGLMapView *)mapView belowLayer:(MGLStyleLayer *)otherLayer
{
    if (otherLayer) {
        const mbgl::optional<std::string> belowLayerId{otherLayer.identifier.UTF8String};
        mapView.mbglMap->addLayer(std::move(_pendingLayer), belowLayerId);
    } else {
        mapView.mbglMap->addLayer(std::move(_pendingLayer));
    }
}

- (void)removeFromMapView:(MGLMapView *)mapView
{
    auto removedLayer = mapView.mbglMap->removeLayer(self.identifier.UTF8String);
    _pendingLayer = std::move(reinterpret_cast<std::unique_ptr<mbgl::style::SymbolLayer> &>(removedLayer));
    self.rawLayer = _pendingLayer.get();
}

#pragma mark - Accessing the Layout Attributes

- (void)setSymbolPlacement:(MGLStyleValue<NSValue *> *)symbolPlacement {
    auto mbglValue = MGLStyleValueTransformer<mbgl::style::SymbolPlacementType, NSValue *>().toPropertyValue(symbolPlacement);
    self.rawLayer->setSymbolPlacement(mbglValue);
}

- (MGLStyleValue<NSValue *> *)symbolPlacement {
    auto propertyValue = self.rawLayer->getSymbolPlacement() ?: self.rawLayer->getDefaultSymbolPlacement();
    return MGLStyleValueTransformer<mbgl::style::SymbolPlacementType, NSValue *>().toStyleValue(propertyValue);
}

- (void)setSymbolSpacing:(MGLStyleValue<NSNumber *> *)symbolSpacing {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(symbolSpacing);
    self.rawLayer->setSymbolSpacing(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)symbolSpacing {
    auto propertyValue = self.rawLayer->getSymbolSpacing() ?: self.rawLayer->getDefaultSymbolSpacing();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setSymbolAvoidEdges:(MGLStyleValue<NSNumber *> *)symbolAvoidEdges {
    auto mbglValue = MGLStyleValueTransformer<bool, NSNumber *>().toPropertyValue(symbolAvoidEdges);
    self.rawLayer->setSymbolAvoidEdges(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)symbolAvoidEdges {
    auto propertyValue = self.rawLayer->getSymbolAvoidEdges() ?: self.rawLayer->getDefaultSymbolAvoidEdges();
    return MGLStyleValueTransformer<bool, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setIconAllowOverlap:(MGLStyleValue<NSNumber *> *)iconAllowOverlap {
    auto mbglValue = MGLStyleValueTransformer<bool, NSNumber *>().toPropertyValue(iconAllowOverlap);
    self.rawLayer->setIconAllowOverlap(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)iconAllowOverlap {
    auto propertyValue = self.rawLayer->getIconAllowOverlap() ?: self.rawLayer->getDefaultIconAllowOverlap();
    return MGLStyleValueTransformer<bool, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setIconIgnorePlacement:(MGLStyleValue<NSNumber *> *)iconIgnorePlacement {
    auto mbglValue = MGLStyleValueTransformer<bool, NSNumber *>().toPropertyValue(iconIgnorePlacement);
    self.rawLayer->setIconIgnorePlacement(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)iconIgnorePlacement {
    auto propertyValue = self.rawLayer->getIconIgnorePlacement() ?: self.rawLayer->getDefaultIconIgnorePlacement();
    return MGLStyleValueTransformer<bool, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setIconOptional:(MGLStyleValue<NSNumber *> *)iconOptional {
    auto mbglValue = MGLStyleValueTransformer<bool, NSNumber *>().toPropertyValue(iconOptional);
    self.rawLayer->setIconOptional(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)iconOptional {
    auto propertyValue = self.rawLayer->getIconOptional() ?: self.rawLayer->getDefaultIconOptional();
    return MGLStyleValueTransformer<bool, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setIconRotationAlignment:(MGLStyleValue<NSValue *> *)iconRotationAlignment {
    auto mbglValue = MGLStyleValueTransformer<mbgl::style::AlignmentType, NSValue *>().toPropertyValue(iconRotationAlignment);
    self.rawLayer->setIconRotationAlignment(mbglValue);
}

- (MGLStyleValue<NSValue *> *)iconRotationAlignment {
    auto propertyValue = self.rawLayer->getIconRotationAlignment() ?: self.rawLayer->getDefaultIconRotationAlignment();
    return MGLStyleValueTransformer<mbgl::style::AlignmentType, NSValue *>().toStyleValue(propertyValue);
}

- (void)setIconSize:(MGLStyleValue<NSNumber *> *)iconSize {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(iconSize);
    self.rawLayer->setIconSize(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)iconSize {
    auto propertyValue = self.rawLayer->getIconSize() ?: self.rawLayer->getDefaultIconSize();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setIconTextFit:(MGLStyleValue<NSValue *> *)iconTextFit {
    auto mbglValue = MGLStyleValueTransformer<mbgl::style::IconTextFitType, NSValue *>().toPropertyValue(iconTextFit);
    self.rawLayer->setIconTextFit(mbglValue);
}

- (MGLStyleValue<NSValue *> *)iconTextFit {
    auto propertyValue = self.rawLayer->getIconTextFit() ?: self.rawLayer->getDefaultIconTextFit();
    return MGLStyleValueTransformer<mbgl::style::IconTextFitType, NSValue *>().toStyleValue(propertyValue);
}

- (void)setIconTextFitPadding:(MGLStyleValue<NSValue *> *)iconTextFitPadding {
    auto mbglValue = MGLStyleValueTransformer<std::array<float, 4>, NSValue *>().toPropertyValue(iconTextFitPadding);
    self.rawLayer->setIconTextFitPadding(mbglValue);
}

- (MGLStyleValue<NSValue *> *)iconTextFitPadding {
    auto propertyValue = self.rawLayer->getIconTextFitPadding() ?: self.rawLayer->getDefaultIconTextFitPadding();
    return MGLStyleValueTransformer<std::array<float, 4>, NSValue *>().toStyleValue(propertyValue);
}

- (void)setIconImage:(MGLStyleValue<NSString *> *)iconImage {
    auto mbglValue = MGLStyleValueTransformer<std::string, NSString *>().toPropertyValue(iconImage);
    self.rawLayer->setIconImage(mbglValue);
}

- (MGLStyleValue<NSString *> *)iconImage {
    auto propertyValue = self.rawLayer->getIconImage() ?: self.rawLayer->getDefaultIconImage();
    return MGLStyleValueTransformer<std::string, NSString *>().toStyleValue(propertyValue);
}

- (void)setIconRotate:(MGLStyleValue<NSNumber *> *)iconRotate {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(iconRotate);
    self.rawLayer->setIconRotate(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)iconRotate {
    auto propertyValue = self.rawLayer->getIconRotate() ?: self.rawLayer->getDefaultIconRotate();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setIconPadding:(MGLStyleValue<NSNumber *> *)iconPadding {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(iconPadding);
    self.rawLayer->setIconPadding(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)iconPadding {
    auto propertyValue = self.rawLayer->getIconPadding() ?: self.rawLayer->getDefaultIconPadding();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setIconKeepUpright:(MGLStyleValue<NSNumber *> *)iconKeepUpright {
    auto mbglValue = MGLStyleValueTransformer<bool, NSNumber *>().toPropertyValue(iconKeepUpright);
    self.rawLayer->setIconKeepUpright(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)iconKeepUpright {
    auto propertyValue = self.rawLayer->getIconKeepUpright() ?: self.rawLayer->getDefaultIconKeepUpright();
    return MGLStyleValueTransformer<bool, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setIconOffset:(MGLStyleValue<NSValue *> *)iconOffset {
    auto mbglValue = MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toPropertyValue(iconOffset);
    self.rawLayer->setIconOffset(mbglValue);
}

- (MGLStyleValue<NSValue *> *)iconOffset {
    auto propertyValue = self.rawLayer->getIconOffset() ?: self.rawLayer->getDefaultIconOffset();
    return MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toStyleValue(propertyValue);
}

- (void)setTextPitchAlignment:(MGLStyleValue<NSValue *> *)textPitchAlignment {
    auto mbglValue = MGLStyleValueTransformer<mbgl::style::AlignmentType, NSValue *>().toPropertyValue(textPitchAlignment);
    self.rawLayer->setTextPitchAlignment(mbglValue);
}

- (MGLStyleValue<NSValue *> *)textPitchAlignment {
    auto propertyValue = self.rawLayer->getTextPitchAlignment() ?: self.rawLayer->getDefaultTextPitchAlignment();
    return MGLStyleValueTransformer<mbgl::style::AlignmentType, NSValue *>().toStyleValue(propertyValue);
}

- (void)setTextRotationAlignment:(MGLStyleValue<NSValue *> *)textRotationAlignment {
    auto mbglValue = MGLStyleValueTransformer<mbgl::style::AlignmentType, NSValue *>().toPropertyValue(textRotationAlignment);
    self.rawLayer->setTextRotationAlignment(mbglValue);
}

- (MGLStyleValue<NSValue *> *)textRotationAlignment {
    auto propertyValue = self.rawLayer->getTextRotationAlignment() ?: self.rawLayer->getDefaultTextRotationAlignment();
    return MGLStyleValueTransformer<mbgl::style::AlignmentType, NSValue *>().toStyleValue(propertyValue);
}

- (void)setTextField:(MGLStyleValue<NSString *> *)textField {
    auto mbglValue = MGLStyleValueTransformer<std::string, NSString *>().toPropertyValue(textField);
    self.rawLayer->setTextField(mbglValue);
}

- (MGLStyleValue<NSString *> *)textField {
    auto propertyValue = self.rawLayer->getTextField() ?: self.rawLayer->getDefaultTextField();
    return MGLStyleValueTransformer<std::string, NSString *>().toStyleValue(propertyValue);
}

- (void)setTextFont:(MGLStyleValue<NSArray<NSString *> *> *)textFont {
    auto mbglValue = MGLStyleValueTransformer<std::vector<std::string>, NSArray<NSString *> *, std::string>().toPropertyValue(textFont);
    self.rawLayer->setTextFont(mbglValue);
}

- (MGLStyleValue<NSArray<NSString *> *> *)textFont {
    auto propertyValue = self.rawLayer->getTextFont() ?: self.rawLayer->getDefaultTextFont();
    return MGLStyleValueTransformer<std::vector<std::string>, NSArray<NSString *> *, std::string>().toStyleValue(propertyValue);
}

- (void)setTextSize:(MGLStyleValue<NSNumber *> *)textSize {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(textSize);
    self.rawLayer->setTextSize(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)textSize {
    auto propertyValue = self.rawLayer->getTextSize() ?: self.rawLayer->getDefaultTextSize();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setTextMaxWidth:(MGLStyleValue<NSNumber *> *)textMaxWidth {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(textMaxWidth);
    self.rawLayer->setTextMaxWidth(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)textMaxWidth {
    auto propertyValue = self.rawLayer->getTextMaxWidth() ?: self.rawLayer->getDefaultTextMaxWidth();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setTextLineHeight:(MGLStyleValue<NSNumber *> *)textLineHeight {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(textLineHeight);
    self.rawLayer->setTextLineHeight(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)textLineHeight {
    auto propertyValue = self.rawLayer->getTextLineHeight() ?: self.rawLayer->getDefaultTextLineHeight();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setTextLetterSpacing:(MGLStyleValue<NSNumber *> *)textLetterSpacing {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(textLetterSpacing);
    self.rawLayer->setTextLetterSpacing(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)textLetterSpacing {
    auto propertyValue = self.rawLayer->getTextLetterSpacing() ?: self.rawLayer->getDefaultTextLetterSpacing();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setTextJustify:(MGLStyleValue<NSValue *> *)textJustify {
    auto mbglValue = MGLStyleValueTransformer<mbgl::style::TextJustifyType, NSValue *>().toPropertyValue(textJustify);
    self.rawLayer->setTextJustify(mbglValue);
}

- (MGLStyleValue<NSValue *> *)textJustify {
    auto propertyValue = self.rawLayer->getTextJustify() ?: self.rawLayer->getDefaultTextJustify();
    return MGLStyleValueTransformer<mbgl::style::TextJustifyType, NSValue *>().toStyleValue(propertyValue);
}

- (void)setTextAnchor:(MGLStyleValue<NSValue *> *)textAnchor {
    auto mbglValue = MGLStyleValueTransformer<mbgl::style::TextAnchorType, NSValue *>().toPropertyValue(textAnchor);
    self.rawLayer->setTextAnchor(mbglValue);
}

- (MGLStyleValue<NSValue *> *)textAnchor {
    auto propertyValue = self.rawLayer->getTextAnchor() ?: self.rawLayer->getDefaultTextAnchor();
    return MGLStyleValueTransformer<mbgl::style::TextAnchorType, NSValue *>().toStyleValue(propertyValue);
}

- (void)setTextMaxAngle:(MGLStyleValue<NSNumber *> *)textMaxAngle {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(textMaxAngle);
    self.rawLayer->setTextMaxAngle(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)textMaxAngle {
    auto propertyValue = self.rawLayer->getTextMaxAngle() ?: self.rawLayer->getDefaultTextMaxAngle();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setTextRotate:(MGLStyleValue<NSNumber *> *)textRotate {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(textRotate);
    self.rawLayer->setTextRotate(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)textRotate {
    auto propertyValue = self.rawLayer->getTextRotate() ?: self.rawLayer->getDefaultTextRotate();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setTextPadding:(MGLStyleValue<NSNumber *> *)textPadding {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(textPadding);
    self.rawLayer->setTextPadding(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)textPadding {
    auto propertyValue = self.rawLayer->getTextPadding() ?: self.rawLayer->getDefaultTextPadding();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setTextKeepUpright:(MGLStyleValue<NSNumber *> *)textKeepUpright {
    auto mbglValue = MGLStyleValueTransformer<bool, NSNumber *>().toPropertyValue(textKeepUpright);
    self.rawLayer->setTextKeepUpright(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)textKeepUpright {
    auto propertyValue = self.rawLayer->getTextKeepUpright() ?: self.rawLayer->getDefaultTextKeepUpright();
    return MGLStyleValueTransformer<bool, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setTextTransform:(MGLStyleValue<NSValue *> *)textTransform {
    auto mbglValue = MGLStyleValueTransformer<mbgl::style::TextTransformType, NSValue *>().toPropertyValue(textTransform);
    self.rawLayer->setTextTransform(mbglValue);
}

- (MGLStyleValue<NSValue *> *)textTransform {
    auto propertyValue = self.rawLayer->getTextTransform() ?: self.rawLayer->getDefaultTextTransform();
    return MGLStyleValueTransformer<mbgl::style::TextTransformType, NSValue *>().toStyleValue(propertyValue);
}

- (void)setTextOffset:(MGLStyleValue<NSValue *> *)textOffset {
    auto mbglValue = MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toPropertyValue(textOffset);
    self.rawLayer->setTextOffset(mbglValue);
}

- (MGLStyleValue<NSValue *> *)textOffset {
    auto propertyValue = self.rawLayer->getTextOffset() ?: self.rawLayer->getDefaultTextOffset();
    return MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toStyleValue(propertyValue);
}

- (void)setTextAllowOverlap:(MGLStyleValue<NSNumber *> *)textAllowOverlap {
    auto mbglValue = MGLStyleValueTransformer<bool, NSNumber *>().toPropertyValue(textAllowOverlap);
    self.rawLayer->setTextAllowOverlap(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)textAllowOverlap {
    auto propertyValue = self.rawLayer->getTextAllowOverlap() ?: self.rawLayer->getDefaultTextAllowOverlap();
    return MGLStyleValueTransformer<bool, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setTextIgnorePlacement:(MGLStyleValue<NSNumber *> *)textIgnorePlacement {
    auto mbglValue = MGLStyleValueTransformer<bool, NSNumber *>().toPropertyValue(textIgnorePlacement);
    self.rawLayer->setTextIgnorePlacement(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)textIgnorePlacement {
    auto propertyValue = self.rawLayer->getTextIgnorePlacement() ?: self.rawLayer->getDefaultTextIgnorePlacement();
    return MGLStyleValueTransformer<bool, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setTextOptional:(MGLStyleValue<NSNumber *> *)textOptional {
    auto mbglValue = MGLStyleValueTransformer<bool, NSNumber *>().toPropertyValue(textOptional);
    self.rawLayer->setTextOptional(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)textOptional {
    auto propertyValue = self.rawLayer->getTextOptional() ?: self.rawLayer->getDefaultTextOptional();
    return MGLStyleValueTransformer<bool, NSNumber *>().toStyleValue(propertyValue);
}

#pragma mark - Accessing the Paint Attributes

- (void)setIconOpacity:(MGLStyleValue<NSNumber *> *)iconOpacity {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(iconOpacity);
    self.rawLayer->setIconOpacity(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)iconOpacity {
    auto propertyValue = self.rawLayer->getIconOpacity() ?: self.rawLayer->getDefaultIconOpacity();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setIconColor:(MGLStyleValue<MGLColor *> *)iconColor {
    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toPropertyValue(iconColor);
    self.rawLayer->setIconColor(mbglValue);
}

- (MGLStyleValue<MGLColor *> *)iconColor {
    auto propertyValue = self.rawLayer->getIconColor() ?: self.rawLayer->getDefaultIconColor();
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toStyleValue(propertyValue);
}

- (void)setIconHaloColor:(MGLStyleValue<MGLColor *> *)iconHaloColor {
    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toPropertyValue(iconHaloColor);
    self.rawLayer->setIconHaloColor(mbglValue);
}

- (MGLStyleValue<MGLColor *> *)iconHaloColor {
    auto propertyValue = self.rawLayer->getIconHaloColor() ?: self.rawLayer->getDefaultIconHaloColor();
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toStyleValue(propertyValue);
}

- (void)setIconHaloWidth:(MGLStyleValue<NSNumber *> *)iconHaloWidth {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(iconHaloWidth);
    self.rawLayer->setIconHaloWidth(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)iconHaloWidth {
    auto propertyValue = self.rawLayer->getIconHaloWidth() ?: self.rawLayer->getDefaultIconHaloWidth();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setIconHaloBlur:(MGLStyleValue<NSNumber *> *)iconHaloBlur {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(iconHaloBlur);
    self.rawLayer->setIconHaloBlur(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)iconHaloBlur {
    auto propertyValue = self.rawLayer->getIconHaloBlur() ?: self.rawLayer->getDefaultIconHaloBlur();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setIconTranslate:(MGLStyleValue<NSValue *> *)iconTranslate {
    auto mbglValue = MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toPropertyValue(iconTranslate);
    self.rawLayer->setIconTranslate(mbglValue);
}

- (MGLStyleValue<NSValue *> *)iconTranslate {
    auto propertyValue = self.rawLayer->getIconTranslate() ?: self.rawLayer->getDefaultIconTranslate();
    return MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toStyleValue(propertyValue);
}

- (void)setIconTranslateAnchor:(MGLStyleValue<NSValue *> *)iconTranslateAnchor {
    auto mbglValue = MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *>().toPropertyValue(iconTranslateAnchor);
    self.rawLayer->setIconTranslateAnchor(mbglValue);
}

- (MGLStyleValue<NSValue *> *)iconTranslateAnchor {
    auto propertyValue = self.rawLayer->getIconTranslateAnchor() ?: self.rawLayer->getDefaultIconTranslateAnchor();
    return MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *>().toStyleValue(propertyValue);
}

- (void)setTextOpacity:(MGLStyleValue<NSNumber *> *)textOpacity {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(textOpacity);
    self.rawLayer->setTextOpacity(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)textOpacity {
    auto propertyValue = self.rawLayer->getTextOpacity() ?: self.rawLayer->getDefaultTextOpacity();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setTextColor:(MGLStyleValue<MGLColor *> *)textColor {
    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toPropertyValue(textColor);
    self.rawLayer->setTextColor(mbglValue);
}

- (MGLStyleValue<MGLColor *> *)textColor {
    auto propertyValue = self.rawLayer->getTextColor() ?: self.rawLayer->getDefaultTextColor();
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toStyleValue(propertyValue);
}

- (void)setTextHaloColor:(MGLStyleValue<MGLColor *> *)textHaloColor {
    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toPropertyValue(textHaloColor);
    self.rawLayer->setTextHaloColor(mbglValue);
}

- (MGLStyleValue<MGLColor *> *)textHaloColor {
    auto propertyValue = self.rawLayer->getTextHaloColor() ?: self.rawLayer->getDefaultTextHaloColor();
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toStyleValue(propertyValue);
}

- (void)setTextHaloWidth:(MGLStyleValue<NSNumber *> *)textHaloWidth {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(textHaloWidth);
    self.rawLayer->setTextHaloWidth(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)textHaloWidth {
    auto propertyValue = self.rawLayer->getTextHaloWidth() ?: self.rawLayer->getDefaultTextHaloWidth();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setTextHaloBlur:(MGLStyleValue<NSNumber *> *)textHaloBlur {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(textHaloBlur);
    self.rawLayer->setTextHaloBlur(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)textHaloBlur {
    auto propertyValue = self.rawLayer->getTextHaloBlur() ?: self.rawLayer->getDefaultTextHaloBlur();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setTextTranslate:(MGLStyleValue<NSValue *> *)textTranslate {
    auto mbglValue = MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toPropertyValue(textTranslate);
    self.rawLayer->setTextTranslate(mbglValue);
}

- (MGLStyleValue<NSValue *> *)textTranslate {
    auto propertyValue = self.rawLayer->getTextTranslate() ?: self.rawLayer->getDefaultTextTranslate();
    return MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toStyleValue(propertyValue);
}

- (void)setTextTranslateAnchor:(MGLStyleValue<NSValue *> *)textTranslateAnchor {
    auto mbglValue = MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *>().toPropertyValue(textTranslateAnchor);
    self.rawLayer->setTextTranslateAnchor(mbglValue);
}

- (MGLStyleValue<NSValue *> *)textTranslateAnchor {
    auto propertyValue = self.rawLayer->getTextTranslateAnchor() ?: self.rawLayer->getDefaultTextTranslateAnchor();
    return MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *>().toStyleValue(propertyValue);
}


@end
