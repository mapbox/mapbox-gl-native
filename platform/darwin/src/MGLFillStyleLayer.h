// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleValue.h"
#import "MGLVectorStyleLayer.h"

NS_ASSUME_NONNULL_BEGIN

/**
 Controls the translation reference point.
 
 Values of this type are used in the `fillTranslateAnchor` property of `MGLFillStyleLayer`.
 */
typedef NS_ENUM(NSUInteger, MGLFillTranslateAnchor) {
    /**
     The fill is translated relative to the map.
     */
    MGLFillTranslateAnchorMap,
    /**
     The fill is translated relative to the viewport.
     */
    MGLFillTranslateAnchorViewport,
};

/**
 A fill layer which allows customization of styling properties at runtime. You may 
 instantiate a new fill layer to add to a map style or you may query an 
 `MGLMapView` for its `style` and obtain existing layers using the 
 `-[MGLStyle layerWithIdentifier:]` method. 
 */
@interface MGLFillStyleLayer : MGLVectorStyleLayer

#pragma mark - Accessing the Paint Attributes

/**
 Whether or not the fill should be antialiased.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSNumber` object containing `YES`. Set this property to `nil` to reset it to the default value.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *fillAntialias;

#if TARGET_OS_IPHONE
/**
 The color of the filled part of this layer.
 
 The default value of this property is an `MGLStyleValue` object containing `UIColor.blackColor`. Set this property to `nil` to reset it to the default value.

 This property is only applied to the style if `fillPattern` is set to `nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) MGLStyleValue<MGLColor *> *fillColor;
#else
/**
 The color of the filled part of this layer.
 
 The default value of this property is an `MGLStyleValue` object containing `NSColor.blackColor`. Set this property to `nil` to reset it to the default value.

 This property is only applied to the style if `fillPattern` is set to `nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) MGLStyleValue<MGLColor *> *fillColor;
#endif

/**
 The opacity of the entire fill layer. In contrast to the `fillColor`, this value will also affect the 1pt stroke around the fill, if the stroke is used.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSNumber` object containing the float `1`. Set this property to `nil` to reset it to the default value.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *fillOpacity;

/**
 The outline color of the fill. Matches the value of `fillColor` if unspecified.

 This property is only applied to the style if `fillPattern` is set to `nil`, and `fillAntialias` is set to an `MGLStyleValue` object containing an `NSNumber` object containing `YES`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) MGLStyleValue<MGLColor *> *fillOutlineColor;

/**
 Name of image in sprite to use for drawing image fills. For seamless patterns, image width and height must be a factor of two (2, 4, 8, ..., 512).
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSString *> *fillPattern;

/**
 The geometry's offset.

 This property is measured in points.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSValue` object containing a `CGVector` struct set to 0 points from the left and 0 points from the top. Set this property to `nil` to reset it to the default value.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *fillTranslate;

/**
 Controls the translation reference point.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSValue` object containing `MGLFillTranslateAnchorMap`. Set this property to `nil` to reset it to the default value.

 This property is only applied to the style if `fillTranslate` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *fillTranslateAnchor;

@end

NS_ASSUME_NONNULL_END
