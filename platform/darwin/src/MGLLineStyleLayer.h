// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleValue.h"
#import "MGLVectorStyleLayer.h"

NS_ASSUME_NONNULL_BEGIN

/**
 The display of line endings.
 
 Values of this type are used in the `lineCap` property of `MGLLineStyleLayer`.
 */
typedef NS_ENUM(NSUInteger, MGLLineCap) {
    /**
     A cap with a squared-off end which is drawn to the exact endpoint of the line.
     */
    MGLLineCapButt,
    /**
     A cap with a rounded end which is drawn beyond the endpoint of the line at a radius of one-half of the line's width and centered on the endpoint of the line.
     */
    MGLLineCapRound,
    /**
     A cap with a squared-off end which is drawn beyond the endpoint of the line at a distance of one-half of the line's width.
     */
    MGLLineCapSquare,
};

/**
 The display of lines when joining.
 
 Values of this type are used in the `lineJoin` property of `MGLLineStyleLayer`.
 */
typedef NS_ENUM(NSUInteger, MGLLineJoin) {
    /**
     A join with a squared-off end which is drawn beyond the endpoint of the line at a distance of one-half of the line's width.
     */
    MGLLineJoinBevel,
    /**
     A join with a rounded end which is drawn beyond the endpoint of the line at a radius of one-half of the line's width and centered on the endpoint of the line.
     */
    MGLLineJoinRound,
    /**
     A join with a sharp, angled corner which is drawn with the outer sides beyond the endpoint of the path until they meet.
     */
    MGLLineJoinMiter,
};

/**
 Controls the translation reference point.
 
 Values of this type are used in the `lineTranslateAnchor` property of `MGLLineStyleLayer`.
 */
typedef NS_ENUM(NSUInteger, MGLLineTranslateAnchor) {
    /**
     The line is translated relative to the map.
     */
    MGLLineTranslateAnchorMap,
    /**
     The line is translated relative to the viewport.
     */
    MGLLineTranslateAnchorViewport,
};

/**
 A line layer which allows customization of styling properties at runtime. You may 
 instantiate a new line layer to add to a map style or you may query an 
 `MGLMapView` for its `style` and obtain existing layers using the 
 `-[MGLStyle layerWithIdentifier:]` method. 
 */
@interface MGLLineStyleLayer : MGLVectorStyleLayer

#pragma mark - Accessing the Layout Attributes

/**
 The display of line endings.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSValue` object containing `MGLLineCapButt`. Set this property to `nil` to reset it to the default value.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *lineCap;

/**
 The display of lines when joining.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSValue` object containing `MGLLineJoinMiter`. Set this property to `nil` to reset it to the default value.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *lineJoin;

/**
 Used to automatically convert miter joins to bevel joins for sharp angles.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSNumber` object containing the float `2`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `lineJoin` is set to an `MGLStyleValue` object containing an `NSValue` object containing `MGLLineJoinMiter`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *lineMiterLimit;

/**
 Used to automatically convert round joins to miter joins for shallow angles.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSNumber` object containing the float `1.05`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `lineJoin` is set to an `MGLStyleValue` object containing an `NSValue` object containing `MGLLineJoinRound`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *lineRoundLimit;

#pragma mark - Accessing the Paint Attributes

/**
 Blur applied to the line, in points.

 This property is measured in points.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSNumber` object containing the float `0`. Set this property to `nil` to reset it to the default value.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *lineBlur;

#if TARGET_OS_IPHONE
/**
 The color with which the line will be drawn.
 
 The default value of this property is an `MGLStyleValue` object containing `UIColor.blackColor`. Set this property to `nil` to reset it to the default value.

 This property is only applied to the style if `linePattern` is set to `nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) MGLStyleValue<MGLColor *> *lineColor;
#else
/**
 The color with which the line will be drawn.
 
 The default value of this property is an `MGLStyleValue` object containing `NSColor.blackColor`. Set this property to `nil` to reset it to the default value.

 This property is only applied to the style if `linePattern` is set to `nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) MGLStyleValue<MGLColor *> *lineColor;
#endif

/**
 Specifies the lengths of the alternating dashes and gaps that form the dash pattern. The lengths are later scaled by the line width. To convert a dash length to points, multiply the length by the current line width.

 This property is measured in line widths.

 This property is only applied to the style if `linePattern` is set to `nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSArray<NSNumber *> *> *lineDasharray;

/**
 Draws a line casing outside of a line's actual path. Value indicates the width of the inner gap.

 This property is measured in points.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSNumber` object containing the float `0`. Set this property to `nil` to reset it to the default value.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *lineGapWidth;

/**
 The line's offset. For linear features, a positive value offsets the line to the right, relative to the direction of the line, and a negative value to the left. For polygon features, a positive value results in an inset, and a negative value results in an outset.

 This property is measured in points.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSNumber` object containing the float `0`. Set this property to `nil` to reset it to the default value.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *lineOffset;

/**
 The opacity at which the line will be drawn.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSNumber` object containing the float `1`. Set this property to `nil` to reset it to the default value.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *lineOpacity;

/**
 Name of image in style images to use for drawing image lines. For seamless patterns, image width must be a factor of two (2, 4, 8, ..., 512).
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSString *> *linePattern;

/**
 The geometry's offset.

 This property is measured in points.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSValue` object containing a `CGVector` struct set to 0 points from the left and 0 points from the top. Set this property to `nil` to reset it to the default value.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *lineTranslate;

/**
 Controls the translation reference point.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSValue` object containing `MGLLineTranslateAnchorMap`. Set this property to `nil` to reset it to the default value.

 This property is only applied to the style if `lineTranslate` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSValue *> *lineTranslateAnchor;

/**
 Stroke thickness.

 This property is measured in points.
 
 The default value of this property is an `MGLStyleValue` object containing an `NSNumber` object containing the float `1`. Set this property to `nil` to reset it to the default value.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *lineWidth;

@end

NS_ASSUME_NONNULL_END
