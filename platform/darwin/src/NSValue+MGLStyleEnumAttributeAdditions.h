// This file is generated.
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import <Foundation/Foundation.h>

#import "MGLFillStyleLayer.h"
#import "MGLLineStyleLayer.h"
#import "MGLSymbolStyleLayer.h"
#import "MGLCircleStyleLayer.h"
#import "MGLRasterStyleLayer.h"
#import "MGLBackgroundStyleLayer.h"

NS_ASSUME_NONNULL_BEGIN

/**
 Methods for round-tripping values for Mapbox style layer enumeration values.
*/
@interface NSValue (MGLStyleEnumAttributeAdditions)

#pragma mark Working with Style Layer Enumeration Attribute Values

/**
 Creates a new value object containing the given `MGLLineCap` enumeration.

 @param type The value for the new object.
 @return A new value object that contains the style enumeration type.
*/
+ (instancetype)valueWithMGLLineCap:(MGLLineCap)lineCap;

/**
 The `MGLLineCap` enumeration representation of the value.
*/
@property (readonly) MGLLineCap MGLLineCapValue;

/**
 Creates a new value object containing the given `MGLLineJoin` enumeration.

 @param type The value for the new object.
 @return A new value object that contains the style enumeration type.
*/
+ (instancetype)valueWithMGLLineJoin:(MGLLineJoin)lineJoin;

/**
 The `MGLLineJoin` enumeration representation of the value.
*/
@property (readonly) MGLLineJoin MGLLineJoinValue;

/**
 Creates a new value object containing the given `MGLIconRotationAlignment` enumeration.

 @param type The value for the new object.
 @return A new value object that contains the style enumeration type.
*/
+ (instancetype)valueWithMGLIconRotationAlignment:(MGLIconRotationAlignment)iconRotationAlignment;

/**
 The `MGLIconRotationAlignment` enumeration representation of the value.
*/
@property (readonly) MGLIconRotationAlignment MGLIconRotationAlignmentValue;

/**
 Creates a new value object containing the given `MGLIconTextFit` enumeration.

 @param type The value for the new object.
 @return A new value object that contains the style enumeration type.
*/
+ (instancetype)valueWithMGLIconTextFit:(MGLIconTextFit)iconTextFit;

/**
 The `MGLIconTextFit` enumeration representation of the value.
*/
@property (readonly) MGLIconTextFit MGLIconTextFitValue;

/**
 Creates a new value object containing the given `MGLSymbolPlacement` enumeration.

 @param type The value for the new object.
 @return A new value object that contains the style enumeration type.
*/
+ (instancetype)valueWithMGLSymbolPlacement:(MGLSymbolPlacement)symbolPlacement;

/**
 The `MGLSymbolPlacement` enumeration representation of the value.
*/
@property (readonly) MGLSymbolPlacement MGLSymbolPlacementValue;

/**
 Creates a new value object containing the given `MGLTextAnchor` enumeration.

 @param type The value for the new object.
 @return A new value object that contains the style enumeration type.
*/
+ (instancetype)valueWithMGLTextAnchor:(MGLTextAnchor)textAnchor;

/**
 The `MGLTextAnchor` enumeration representation of the value.
*/
@property (readonly) MGLTextAnchor MGLTextAnchorValue;

/**
 Creates a new value object containing the given `MGLTextJustify` enumeration.

 @param type The value for the new object.
 @return A new value object that contains the style enumeration type.
*/
+ (instancetype)valueWithMGLTextJustify:(MGLTextJustify)textJustify;

/**
 The `MGLTextJustify` enumeration representation of the value.
*/
@property (readonly) MGLTextJustify MGLTextJustifyValue;

/**
 Creates a new value object containing the given `MGLTextPitchAlignment` enumeration.

 @param type The value for the new object.
 @return A new value object that contains the style enumeration type.
*/
+ (instancetype)valueWithMGLTextPitchAlignment:(MGLTextPitchAlignment)textPitchAlignment;

/**
 The `MGLTextPitchAlignment` enumeration representation of the value.
*/
@property (readonly) MGLTextPitchAlignment MGLTextPitchAlignmentValue;

/**
 Creates a new value object containing the given `MGLTextRotationAlignment` enumeration.

 @param type The value for the new object.
 @return A new value object that contains the style enumeration type.
*/
+ (instancetype)valueWithMGLTextRotationAlignment:(MGLTextRotationAlignment)textRotationAlignment;

/**
 The `MGLTextRotationAlignment` enumeration representation of the value.
*/
@property (readonly) MGLTextRotationAlignment MGLTextRotationAlignmentValue;

/**
 Creates a new value object containing the given `MGLTextTransform` enumeration.

 @param type The value for the new object.
 @return A new value object that contains the style enumeration type.
*/
+ (instancetype)valueWithMGLTextTransform:(MGLTextTransform)textTransform;

/**
 The `MGLTextTransform` enumeration representation of the value.
*/
@property (readonly) MGLTextTransform MGLTextTransformValue;

/**
 Creates a new value object containing the given `MGLFillTranslateAnchor` structure.

 @param type The value for the new object.
 @return A new value object that contains the style enumeration type.
*/
+ (instancetype)valueWithMGLFillTranslateAnchor:(MGLFillTranslateAnchor)fillTranslateAnchor;

/**
 The `MGLFillTranslateAnchor` enumeration representation of the value.
*/
@property (readonly) MGLFillTranslateAnchor MGLFillTranslateAnchorValue;

/**
 Creates a new value object containing the given `MGLLineTranslateAnchor` structure.

 @param type The value for the new object.
 @return A new value object that contains the style enumeration type.
*/
+ (instancetype)valueWithMGLLineTranslateAnchor:(MGLLineTranslateAnchor)lineTranslateAnchor;

/**
 The `MGLLineTranslateAnchor` enumeration representation of the value.
*/
@property (readonly) MGLLineTranslateAnchor MGLLineTranslateAnchorValue;

/**
 Creates a new value object containing the given `MGLIconTranslateAnchor` structure.

 @param type The value for the new object.
 @return A new value object that contains the style enumeration type.
*/
+ (instancetype)valueWithMGLIconTranslateAnchor:(MGLIconTranslateAnchor)iconTranslateAnchor;

/**
 The `MGLIconTranslateAnchor` enumeration representation of the value.
*/
@property (readonly) MGLIconTranslateAnchor MGLIconTranslateAnchorValue;

/**
 Creates a new value object containing the given `MGLTextTranslateAnchor` structure.

 @param type The value for the new object.
 @return A new value object that contains the style enumeration type.
*/
+ (instancetype)valueWithMGLTextTranslateAnchor:(MGLTextTranslateAnchor)textTranslateAnchor;

/**
 The `MGLTextTranslateAnchor` enumeration representation of the value.
*/
@property (readonly) MGLTextTranslateAnchor MGLTextTranslateAnchorValue;

/**
 Creates a new value object containing the given `MGLCirclePitchScale` structure.

 @param type The value for the new object.
 @return A new value object that contains the style enumeration type.
*/
+ (instancetype)valueWithMGLCirclePitchScale:(MGLCirclePitchScale)circlePitchScale;

/**
 The `MGLCirclePitchScale` enumeration representation of the value.
*/
@property (readonly) MGLCirclePitchScale MGLCirclePitchScaleValue;

/**
 Creates a new value object containing the given `MGLCircleTranslateAnchor` structure.

 @param type The value for the new object.
 @return A new value object that contains the style enumeration type.
*/
+ (instancetype)valueWithMGLCircleTranslateAnchor:(MGLCircleTranslateAnchor)circleTranslateAnchor;

/**
 The `MGLCircleTranslateAnchor` enumeration representation of the value.
*/
@property (readonly) MGLCircleTranslateAnchor MGLCircleTranslateAnchorValue;

@end

NS_ASSUME_NONNULL_END
