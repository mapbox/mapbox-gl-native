// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleAttributeValue.h"
#import "MGLBaseStyleLayer.h"

NS_ASSUME_NONNULL_BEGIN

/**
 The display of line endings.
 */
typedef NS_ENUM(NSUInteger, MGLLineStyleLayerLineCap) {
    /**
     A cap with a squared-off end which is drawn to the exact endpoint of the line.
     */
    MGLLineStyleLayerLineCapButt,
    /**
     A cap with a rounded end which is drawn beyond the endpoint of the line at a radius of one-half of the line's width and centered on the endpoint of the line.
     */
    MGLLineStyleLayerLineCapRound,
    /**
     A cap with a squared-off end which is drawn beyond the endpoint of the line at a distance of one-half of the line's width.
     */
    MGLLineStyleLayerLineCapSquare,
};

/**
 The display of lines when joining.
 */
typedef NS_ENUM(NSUInteger, MGLLineStyleLayerLineJoin) {
    /**
     A join with a squared-off end which is drawn beyond the endpoint of the line at a distance of one-half of the line's width.
     */
    MGLLineStyleLayerLineJoinBevel,
    /**
     A join with a rounded end which is drawn beyond the endpoint of the line at a radius of one-half of the line's width and centered on the endpoint of the line.
     */
    MGLLineStyleLayerLineJoinRound,
    /**
     A join with a sharp, angled corner which is drawn with the outer sides beyond the endpoint of the path until they meet.
     */
    MGLLineStyleLayerLineJoinMiter,
};

/**
 Controls the translation reference point.
 */
typedef NS_ENUM(NSUInteger, MGLLineStyleLayerLineTranslateAnchor) {
    /**
     The line is translated relative to the map.
     */
    MGLLineStyleLayerLineTranslateAnchorMap,
    /**
     The line is translated relative to the viewport.
     */
    MGLLineStyleLayerLineTranslateAnchorViewport,
};

/**
 A line layer which allows customization of styling properties at runtime. You may 
 instantiate a new line layer to add to a map style or you may query an 
 `MGLMapView` for its `style` and obtain existing layers using the 
 `-[MGLStyle layerWithIdentifier:]` method. 
 */
@interface MGLLineStyleLayer : MGLBaseStyleLayer <MGLStyleLayer>

- (instancetype)initWithLayerIdentifier:(NSString *)layerIdentifier source:(MGLSource *)source;

- (instancetype)initWithLayerIdentifier:(NSString *)layerIdentifier source:(MGLSource *)source sourceLayer:(NSString *)sourceLayer;

/**
 A predicate that corresponds to the layer's <a href='https://www.mapbox.com/mapbox-gl-style-spec/#types-filter'>filter</a>.
 
 The predicate's left expression must be a string that identifies a feature
 property, or one of the special keys.
 */
@property (nonatomic, nullable) NSPredicate *predicate;

#pragma mark - Accessing the Layout Attributes

/**
 The display of line endings.
 
 The default value of this property is an `NSValue` object containing `MGLLineStyleLayerLineCapButt`. Set this property to `nil` to reset it to the default value.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> lineCap;

/**
 The display of lines when joining.
 
 The default value of this property is an `NSValue` object containing `MGLLineStyleLayerLineJoinMiter`. Set this property to `nil` to reset it to the default value.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> lineJoin;

/**
 Used to automatically convert miter joins to bevel joins for sharp angles.
 
 The default value of this property is an `NSNumber` object containing the float `2`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `lineJoin` is set to an `NSValue` object containing `MGLLineStyleLayerLineJoinMiter`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> lineMiterLimit;

/**
 Used to automatically convert round joins to miter joins for shallow angles.
 
 The default value of this property is an `NSNumber` object containing the float `1.05`. Set this property to `nil` to reset it to the default value.
 
 This property is only applied to the style if `lineJoin` is set to an `NSValue` object containing `MGLLineStyleLayerLineJoinRound`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> lineRoundLimit;

#pragma mark - Accessing the Paint Attributes

/**
 The opacity at which the line will be drawn.
 
 The default value of this property is an `NSNumber` object containing the float `1`. Set this property to `nil` to reset it to the default value.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> lineOpacity;

#if TARGET_OS_IPHONE
/**
 The color with which the line will be drawn.
 
 The default value of this property is `UIColor.blackColor`. Set this property to `nil` to reset it to the default value.

 This property is only applied to the style if `linePattern` is set to `nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> lineColor;
#else
/**
 The color with which the line will be drawn.
 
 The default value of this property is `NSColor.blackColor`. Set this property to `nil` to reset it to the default value.

 This property is only applied to the style if `linePattern` is set to `nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> lineColor;
#endif

/**
 The geometry's offset. Values are [x, y] where negatives indicate left and up, respectively.

 This property is measured in points.
 
 The default value of this property is an `NSValue` object containing a `CGVector` struct set to 0 points from the left and 0 points from the top. Set this property to `nil` to reset it to the default value.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> lineTranslate;

/**
 Controls the translation reference point.
 
 The default value of this property is an `NSValue` object containing `MGLLineStyleLayerLineTranslateAnchorMap`. Set this property to `nil` to reset it to the default value.

 This property is only applied to the style if `lineTranslate` is non-`nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> lineTranslateAnchor;

/**
 Stroke thickness.

 This property is measured in points.
 
 The default value of this property is an `NSNumber` object containing the float `1`. Set this property to `nil` to reset it to the default value.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> lineWidth;

/**
 Draws a line casing outside of a line's actual path. Value indicates the width of the inner gap.

 This property is measured in points.
 
 The default value of this property is an `NSNumber` object containing the float `0`. Set this property to `nil` to reset it to the default value.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> lineGapWidth;

/**
 The line's offset. For linear features, a positive value offsets the line to the right, relative to the direction of the line, and a negative value to the left. For polygon features, a positive value results in an inset, and a negative value results in an outset.

 This property is measured in points.
 
 The default value of this property is an `NSNumber` object containing the float `0`. Set this property to `nil` to reset it to the default value.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> lineOffset;

/**
 Blur applied to the line, in points.

 This property is measured in points.
 
 The default value of this property is an `NSNumber` object containing the float `0`. Set this property to `nil` to reset it to the default value.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> lineBlur;

/**
 Specifies the lengths of the alternating dashes and gaps that form the dash pattern. The lengths are later scaled by the line width. To convert a dash length to points, multiply the length by the current line width.

 This property is measured in line widths.

 This property is only applied to the style if `linePattern` is set to `nil`. Otherwise, it is ignored.
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> lineDasharray;

/**
 Name of image in sprite to use for drawing image lines. For seamless patterns, image width must be a factor of two (2, 4, 8, ..., 512).
 */
@property (nonatomic, null_resettable) id <MGLStyleAttributeValue> linePattern;

@end

NS_ASSUME_NONNULL_END
