// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleValue.h"
#import "MGLForegroundStyleLayer.h"

NS_ASSUME_NONNULL_BEGIN

/**
 An `MGLRasterStyleLayer` is a style layer that renders raster tiles on the map.
 
 Use a raster style layer to configure the color parameters of raster tiles
 loaded by an `MGLRasterSource` object. For example, you could use a raster
 style layer to render <a href="https://www.mapbox.com/satellite/">Mapbox
 Satellite</a> imagery, a <a
 href="https://www.mapbox.com/help/define-tileset/#raster-tilesets">raster tile
 set</a> uploaded to Mapbox Studio, or a raster map authored in <a
 href="https://tilemill-project.github.io/tilemill/">TileMill</a>, the classic
 Mapbox Editor, or Mapbox Studio Classic.
 
 You can access an existing raster style layer using the
 `-[MGLStyle layerWithIdentifier:]` method if you know its identifier;
 otherwise, find it using the `MGLStyle.layers` property. You can also create a
 new raster style layer and add it to the style using a method such as
 `-[MGLStyle addLayer:]`.
 
 ### Example
 
 ```swift
 let layer = MGLRasterStyleLayer(identifier: "clouds", source: source)
 layer.rasterOpacity = MGLStyleValue(rawValue: 0.5)
 mapView.style?.addLayer(layer)
 ```
 */
@interface MGLRasterStyleLayer : MGLForegroundStyleLayer

#pragma mark - Accessing the Paint Attributes

/**
 Increase or reduce the brightness of the image. The value is the maximum
 brightness.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSNumber` object containing the float `1`. Set this property to `nil` to reset
 it to the default value.
 
 This attribute corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-style-spec/#paint-raster-brightness-max"><code>raster-brightness-max</code></a>
 layout property in the Mapbox Style Specification.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *maximumRasterBrightness;

@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *rasterBrightnessMax __attribute__((unavailable("Use maximumRasterBrightness instead.")));

/**
 Increase or reduce the brightness of the image. The value is the minimum
 brightness.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSNumber` object containing the float `0`. Set this property to `nil` to reset
 it to the default value.
 
 This attribute corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-style-spec/#paint-raster-brightness-min"><code>raster-brightness-min</code></a>
 layout property in the Mapbox Style Specification.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *minimumRasterBrightness;

@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *rasterBrightnessMin __attribute__((unavailable("Use minimumRasterBrightness instead.")));

/**
 Increase or reduce the contrast of the image.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSNumber` object containing the float `0`. Set this property to `nil` to reset
 it to the default value.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *rasterContrast;

/**
 Fade duration when a new tile is added.
 
 This property is measured in milliseconds.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSNumber` object containing the float `300`. Set this property to `nil` to
 reset it to the default value.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *rasterFadeDuration;

/**
 Rotates hues around the color wheel.
 
 This property is measured in degrees.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSNumber` object containing the float `0`. Set this property to `nil` to reset
 it to the default value.
 
 This attribute corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-style-spec/#paint-raster-hue-rotate"><code>raster-hue-rotate</code></a>
 layout property in the Mapbox Style Specification.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *rasterHueRotation;

@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *rasterHueRotate __attribute__((unavailable("Use rasterHueRotation instead.")));

/**
 The opacity at which the image will be drawn.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSNumber` object containing the float `1`. Set this property to `nil` to reset
 it to the default value.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *rasterOpacity;

/**
 Increase or reduce the saturation of the image.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSNumber` object containing the float `0`. Set this property to `nil` to reset
 it to the default value.
 */
@property (nonatomic, null_resettable) MGLStyleValue<NSNumber *> *rasterSaturation;

@end

NS_ASSUME_NONNULL_END
