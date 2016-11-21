// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleValue.h"
#import "MGLVectorStyleLayer.h"

NS_ASSUME_NONNULL_BEGIN

/**
 Controls the scaling behavior of the circle when the map is pitched.
 
 Values of this type are used in the `circlePitchScale` property of `MGLCircleStyleLayer`.
 */
typedef NS_ENUM(NSUInteger, MGLCirclePitchScale) {
    /**
     Circles are scaled according to their apparent distance to the camera.
     */
    MGLCirclePitchScaleMap,
    /**
     Circles are not scaled.
     */
    MGLCirclePitchScaleViewport,
};

/**
 Controls the translation reference point.
 
 Values of this type are used in the `circleTranslateAnchor` property of `MGLCircleStyleLayer`.
 */
typedef NS_ENUM(NSUInteger, MGLCircleTranslateAnchor) {
    /**
     The circle is translated relative to the map.
     */
    MGLCircleTranslateAnchorMap,
    /**
     The circle is translated relative to the viewport.
     */
    MGLCircleTranslateAnchorViewport,
};

/**
 A circle layer which allows customization of styling properties at runtime. You may 
 instantiate a new circle layer to add to a map style or you may query an 
 `MGLMapView` for its `style` and obtain existing layers using the 
 `-[MGLStyle layerWithIdentifier:]` method. 
 */
@interface MGLCircleStyleLayer : MGLVectorStyleLayer

#pragma mark - Accessing the Paint Attributes

/**
 Amount to blur the circle. 1 blurs the circle such that only the centerpoint is full opacity.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSNumber` object containing the float `0`. Set this property to `nil` to reset it to the default value.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *circleBlur;

#if TARGET_OS_IPHONE
/**
 The fill color of the circle.
 
 The default value of this property is an `MGLStyleValue` object containing `UIColor.blackColor`. Set this property to `nil` to reset it to the default value.
 */
@property (nonatomic, null_resettable) MGLStyleValue<MGLColor *> *circleColor;
#else
/**
 The fill color of the circle.
 
 The default value of this property is an `MGLStyleValue` object containing `NSColor.blackColor`. Set this property to `nil` to reset it to the default value.
 */
@property (nonatomic, null_resettable) MGLStyleValue<MGLColor *> *circleColor;
#endif

/**
 The opacity at which the circle will be drawn.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSNumber` object containing the float `1`. Set this property to `nil` to reset it to the default value.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *circleOpacity;

/**
 Controls the scaling behavior of the circle when the map is pitched.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSValue` object containing `MGLCirclePitchScaleMap`. Set this property to `nil` to reset it to the default value.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *circlePitchScale;

/**
 Circle radius.

 This property is measured in points.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSNumber` object containing the float `5`. Set this property to `nil` to reset it to the default value.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *circleRadius;

/**
 The geometry's offset.

 This property is measured in points.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSValue` object containing a `CGVector` struct set to 0 points from the left and 0 points from the top. Set this property to `nil` to reset it to the default value.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *circleTranslate;

/**
 Controls the translation reference point.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSValue` object containing `MGLCircleTranslateAnchorMap`. Set this property to `nil` to reset it to the default value.

 This property is only applied to the style if `circleTranslate` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *circleTranslateAnchor;

@end

NS_ASSUME_NONNULL_END
