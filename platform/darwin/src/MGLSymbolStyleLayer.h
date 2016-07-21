// This file is generated. Do not edit.

#import <Mapbox/Mapbox.h>

#import "MGLTypes.h"

typedef NS_ENUM(NSUInteger, MGLSymbolStyleLayerSymbolPlacement) {
    MGLSymbolStyleLayerSymbolPlacementPoint,
    MGLSymbolStyleLayerSymbolPlacementLine,
};
typedef NS_ENUM(NSUInteger, MGLSymbolStyleLayerIconRotationAlignment) {
    MGLSymbolStyleLayerIconRotationAlignmentMap,
    MGLSymbolStyleLayerIconRotationAlignmentViewport,
};
typedef NS_ENUM(NSUInteger, MGLSymbolStyleLayerIconTextFit) {
    MGLSymbolStyleLayerIconTextFitNone,
    MGLSymbolStyleLayerIconTextFitBoth,
    MGLSymbolStyleLayerIconTextFitWidth,
    MGLSymbolStyleLayerIconTextFitHeight,
};
typedef NS_ENUM(NSUInteger, MGLSymbolStyleLayerTextPitchAlignment) {
    MGLSymbolStyleLayerTextPitchAlignmentMap,
    MGLSymbolStyleLayerTextPitchAlignmentViewport,
};
typedef NS_ENUM(NSUInteger, MGLSymbolStyleLayerTextRotationAlignment) {
    MGLSymbolStyleLayerTextRotationAlignmentMap,
    MGLSymbolStyleLayerTextRotationAlignmentViewport,
};
typedef NS_ENUM(NSUInteger, MGLSymbolStyleLayerTextJustify) {
    MGLSymbolStyleLayerTextJustifyLeft,
    MGLSymbolStyleLayerTextJustifyCenter,
    MGLSymbolStyleLayerTextJustifyRight,
};
typedef NS_ENUM(NSUInteger, MGLSymbolStyleLayerTextAnchor) {
    MGLSymbolStyleLayerTextAnchorCenter,
    MGLSymbolStyleLayerTextAnchorLeft,
    MGLSymbolStyleLayerTextAnchorRight,
    MGLSymbolStyleLayerTextAnchorTop,
    MGLSymbolStyleLayerTextAnchorBottom,
    MGLSymbolStyleLayerTextAnchorTopLeft,
    MGLSymbolStyleLayerTextAnchorTopRight,
    MGLSymbolStyleLayerTextAnchorBottomLeft,
    MGLSymbolStyleLayerTextAnchorBottomRight,
};
typedef NS_ENUM(NSUInteger, MGLSymbolStyleLayerTextTransform) {
    MGLSymbolStyleLayerTextTransformNone,
    MGLSymbolStyleLayerTextTransformUppercase,
    MGLSymbolStyleLayerTextTransformLowercase,
};
typedef NS_ENUM(NSUInteger, MGLSymbolStyleLayerIconTranslateAnchor) {
    MGLSymbolStyleLayerIconTranslateAnchorMap,
    MGLSymbolStyleLayerIconTranslateAnchorViewport,
};
typedef NS_ENUM(NSUInteger, MGLSymbolStyleLayerTextTranslateAnchor) {
    MGLSymbolStyleLayerTextTranslateAnchorMap,
    MGLSymbolStyleLayerTextTranslateAnchorViewport,
};

@interface MGLSymbolStyleLayer : MGLStyleLayer

// Layout properties

@property (nonatomic) MGLSymbolStyleLayerSymbolPlacement symbolPlacement;

@property (nonatomic) CGFloat symbolSpacing;

@property (nonatomic) BOOL symbolAvoidEdges;

@property (nonatomic) BOOL iconAllowOverlap;

@property (nonatomic) BOOL iconIgnorePlacement;

@property (nonatomic) BOOL iconOptional;

@property (nonatomic) MGLSymbolStyleLayerIconRotationAlignment iconRotationAlignment;

@property (nonatomic) CGFloat iconSize;

@property (nonatomic) MGLSymbolStyleLayerIconTextFit iconTextFit;

@property (nonatomic) NSArray *iconTextFitPadding;

@property (nonatomic) NSString *iconImage;

@property (nonatomic) CGFloat iconRotate;

@property (nonatomic) CGFloat iconPadding;

@property (nonatomic) BOOL iconKeepUpright;

@property (nonatomic) NSArray *iconOffset;

@property (nonatomic) MGLSymbolStyleLayerTextPitchAlignment textPitchAlignment;

@property (nonatomic) MGLSymbolStyleLayerTextRotationAlignment textRotationAlignment;

@property (nonatomic) NSString *textField;

@property (nonatomic) NSArray *textFont;

@property (nonatomic) CGFloat textSize;

@property (nonatomic) CGFloat textMaxWidth;

@property (nonatomic) CGFloat textLineHeight;

@property (nonatomic) CGFloat textLetterSpacing;

@property (nonatomic) MGLSymbolStyleLayerTextJustify textJustify;

@property (nonatomic) MGLSymbolStyleLayerTextAnchor textAnchor;

@property (nonatomic) CGFloat textMaxAngle;

@property (nonatomic) CGFloat textRotate;

@property (nonatomic) CGFloat textPadding;

@property (nonatomic) BOOL textKeepUpright;

@property (nonatomic) MGLSymbolStyleLayerTextTransform textTransform;

@property (nonatomic) NSArray *textOffset;

@property (nonatomic) BOOL textAllowOverlap;

@property (nonatomic) BOOL textIgnorePlacement;

@property (nonatomic) BOOL textOptional;

// Paint properties

@property (nonatomic) CGFloat iconOpacity;

@property (nonatomic) MGLColor *iconColor;

@property (nonatomic) MGLColor *iconHaloColor;

@property (nonatomic) CGFloat iconHaloWidth;

@property (nonatomic) CGFloat iconHaloBlur;

@property (nonatomic) NSArray *iconTranslate;

@property (nonatomic) MGLSymbolStyleLayerIconTranslateAnchor iconTranslateAnchor;

@property (nonatomic) CGFloat textOpacity;

@property (nonatomic) MGLColor *textColor;

@property (nonatomic) MGLColor *textHaloColor;

@property (nonatomic) CGFloat textHaloWidth;

@property (nonatomic) CGFloat textHaloBlur;

@property (nonatomic) NSArray *textTranslate;

@property (nonatomic) MGLSymbolStyleLayerTextTranslateAnchor textTranslateAnchor;

@end

