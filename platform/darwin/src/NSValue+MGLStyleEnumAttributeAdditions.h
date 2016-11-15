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
@property (readonly) MGLLineCap lineCap;

/**
 Creates a new value object containing the given `MGLLineJoin` enumeration.

 @param type The value for the new object.
 @return A new value object that contains the style enumeration type.
*/
+ (instancetype)valueWithMGLLineJoin:(MGLLineJoin)lineJoin;

/**
 The `MGLLineJoin` enumeration representation of the value.
*/
@property (readonly) MGLLineJoin lineJoin;

/**
 Creates a new value object containing the given `MGLSymbolPlacement` enumeration.

 @param type The value for the new object.
 @return A new value object that contains the style enumeration type.
*/
+ (instancetype)valueWithMGLSymbolPlacement:(MGLSymbolPlacement)symbolPlacement;

/**
 The `MGLSymbolPlacement` enumeration representation of the value.
*/
@property (readonly) MGLSymbolPlacement symbolPlacement;

/**
 Creates a new value object containing the given `MGLIconRotationAlignment` enumeration.

 @param type The value for the new object.
 @return A new value object that contains the style enumeration type.
*/
+ (instancetype)valueWithMGLIconRotationAlignment:(MGLIconRotationAlignment)iconRotationAlignment;

/**
 The `MGLIconRotationAlignment` enumeration representation of the value.
*/
@property (readonly) MGLIconRotationAlignment iconRotationAlignment;

/**
 Creates a new value object containing the given `MGLIconTextFit` enumeration.

 @param type The value for the new object.
 @return A new value object that contains the style enumeration type.
*/
+ (instancetype)valueWithMGLIconTextFit:(MGLIconTextFit)iconTextFit;

/**
 The `MGLIconTextFit` enumeration representation of the value.
*/
@property (readonly) MGLIconTextFit iconTextFit;

/**
 Creates a new value object containing the given `MGLTextPitchAlignment` enumeration.

 @param type The value for the new object.
 @return A new value object that contains the style enumeration type.
*/
+ (instancetype)valueWithMGLTextPitchAlignment:(MGLTextPitchAlignment)textPitchAlignment;

/**
 The `MGLTextPitchAlignment` enumeration representation of the value.
*/
@property (readonly) MGLTextPitchAlignment textPitchAlignment;

/**
 Creates a new value object containing the given `MGLTextRotationAlignment` enumeration.

 @param type The value for the new object.
 @return A new value object that contains the style enumeration type.
*/
+ (instancetype)valueWithMGLTextRotationAlignment:(MGLTextRotationAlignment)textRotationAlignment;

/**
 The `MGLTextRotationAlignment` enumeration representation of the value.
*/
@property (readonly) MGLTextRotationAlignment textRotationAlignment;

/**
 Creates a new value object containing the given `MGLTextJustify` enumeration.

 @param type The value for the new object.
 @return A new value object that contains the style enumeration type.
*/
+ (instancetype)valueWithMGLTextJustify:(MGLTextJustify)textJustify;

/**
 The `MGLTextJustify` enumeration representation of the value.
*/
@property (readonly) MGLTextJustify textJustify;

/**
 Creates a new value object containing the given `MGLTextAnchor` enumeration.

 @param type The value for the new object.
 @return A new value object that contains the style enumeration type.
*/
+ (instancetype)valueWithMGLTextAnchor:(MGLTextAnchor)textAnchor;

/**
 The `MGLTextAnchor` enumeration representation of the value.
*/
@property (readonly) MGLTextAnchor textAnchor;

/**
 Creates a new value object containing the given `MGLTextTransform` enumeration.

 @param type The value for the new object.
 @return A new value object that contains the style enumeration type.
*/
+ (instancetype)valueWithMGLTextTransform:(MGLTextTransform)textTransform;

/**
 The `MGLTextTransform` enumeration representation of the value.
*/
@property (readonly) MGLTextTransform textTransform;

/**
 Creates a new value object containing the given `MGLFillTranslateAnchor` structure.

 @param type The value for the new object.
 @return A new value object that contains the style enumeration type.
*/
+ (instancetype)valueWithMGLFillTranslateAnchor:(MGLFillTranslateAnchor)fillTranslateAnchor;

/**
 The `MGLFillTranslateAnchor` enumeration representation of the value.
*/
@property (readonly) MGLFillTranslateAnchor fillTranslateAnchor;

/**
 Creates a new value object containing the given `MGLLineTranslateAnchor` structure.

 @param type The value for the new object.
 @return A new value object that contains the style enumeration type.
*/
+ (instancetype)valueWithMGLLineTranslateAnchor:(MGLLineTranslateAnchor)lineTranslateAnchor;

/**
 The `MGLLineTranslateAnchor` enumeration representation of the value.
*/
@property (readonly) MGLLineTranslateAnchor lineTranslateAnchor;

/**
 Creates a new value object containing the given `MGLIconTranslateAnchor` structure.

 @param type The value for the new object.
 @return A new value object that contains the style enumeration type.
*/
+ (instancetype)valueWithMGLIconTranslateAnchor:(MGLIconTranslateAnchor)iconTranslateAnchor;

/**
 The `MGLIconTranslateAnchor` enumeration representation of the value.
*/
@property (readonly) MGLIconTranslateAnchor iconTranslateAnchor;

/**
 Creates a new value object containing the given `MGLTextTranslateAnchor` structure.

 @param type The value for the new object.
 @return A new value object that contains the style enumeration type.
*/
+ (instancetype)valueWithMGLTextTranslateAnchor:(MGLTextTranslateAnchor)textTranslateAnchor;

/**
 The `MGLTextTranslateAnchor` enumeration representation of the value.
*/
@property (readonly) MGLTextTranslateAnchor textTranslateAnchor;

/**
 Creates a new value object containing the given `MGLCircleTranslateAnchor` structure.

 @param type The value for the new object.
 @return A new value object that contains the style enumeration type.
*/
+ (instancetype)valueWithMGLCircleTranslateAnchor:(MGLCircleTranslateAnchor)circleTranslateAnchor;

/**
 The `MGLCircleTranslateAnchor` enumeration representation of the value.
*/
@property (readonly) MGLCircleTranslateAnchor circleTranslateAnchor;

/**
 Creates a new value object containing the given `MGLCirclePitchScale` structure.

 @param type The value for the new object.
 @return A new value object that contains the style enumeration type.
*/
+ (instancetype)valueWithMGLCirclePitchScale:(MGLCirclePitchScale)circlePitchScale;

/**
 The `MGLCirclePitchScale` enumeration representation of the value.
*/
@property (readonly) MGLCirclePitchScale circlePitchScale;

@end

NS_ASSUME_NONNULL_END
