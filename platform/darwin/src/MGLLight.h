// This file is generated.
// Edit platform/darwin/scripts/generate-style-code.js, then run `make darwin-style-code`.

#import <CoreLocation/CoreLocation.h>

#import "MGLFoundation.h"
#import "MGLStyleValue.h"

NS_ASSUME_NONNULL_BEGIN

/**
 Whether extruded geometries are lit relative to the map or viewport.
 */
typedef NS_ENUM(NSUInteger, MGLLightAnchor) {
    /**
     The position of the light source is aligned to the rotation of the map.
     */
    MGLLightAnchorMap,
    /**
     The position of the light source is aligned to the rotation of the
     viewport.
     */
    MGLLightAnchorViewport,
};

/**
 A structure containing information about the position of the light source
 relative to lit geometries.
 */
typedef struct MGLSphericalPosition {
    /** Distance from the center of the base of an object to its light. */
    CGFloat radial;
    /** Position of the light relative to 0° (0° when `MGLLight.anchor` is set to viewport corresponds
     to the top of the viewport, or 0° when `MGLLight.anchor` is set to map corresponds to due north,
     and degrees proceed clockwise). */
    CLLocationDirection azimuthal;
    /** Indicates the height of the light (from 0°, directly above, to 180°, directly below). */
    CLLocationDirection polar;
} MGLSphericalPosition;

/**
 Creates a new `MGLSphericalPosition` from the given radial, azimuthal, polar.
 
 @param radial The radial coordinate.
 @param azimuthal The azimuthal angle.
 @param polar The polar angle.
 
 @return Returns a `MGLSphericalPosition` struct containing the position attributes.
 */
NS_INLINE MGLSphericalPosition MGLSphericalPositionMake(CGFloat radial, CLLocationDirection azimuthal, CLLocationDirection polar) {
    MGLSphericalPosition position;
    position.radial = radial;
    position.azimuthal = azimuthal;
    position.polar = polar;
    
    return position;
}

/**
 An `MGLLight` object represents the light source for extruded geometries in `MGLStyle`.
 */
MGL_EXPORT
@interface MGLLight : NSObject

/**
 Whether extruded geometries are lit relative to the map or viewport.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSValue` object containing `MGLLightAnchorViewport`.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of
 `MGLInterpolationModeInterval`

 This property corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-js/style-spec/#light-anchor"><code>anchor</code></a>
 light property in the Mapbox Style Specification.
 */
@property (nonatomic) MGLStyleValue<NSValue *> *anchor;

/**
 Position of the `MGLLight` source relative to lit (extruded) geometries, in a
 `MGLSphericalPosition` struct [radial coordinate, azimuthal angle, polar angle]
 where radial indicates the distance from the center of the base of an object to
 its light, azimuthal indicates the position of the light relative to 0° (0°
 when `MGLLight.anchor` is set to `MGLLightAnchorViewport` corresponds to the
 top of the viewport, or 0° when `MGLLight.anchor` is set to `MGLLightAnchorMap`
 corresponds to due north, and degrees proceed clockwise), and polar indicates
 the height of the light (from 0°, directly above, to 180°, directly below).
 
 The default value of this property is an `MGLStyleValue` object containing an
 `MGLSphericalPosition` struct set to 1.15 radial, 210 azimuthal and 30 polar.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`

 This property corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-js/style-spec/#light-position"><code>position</code></a>
 light property in the Mapbox Style Specification.
 */
@property (nonatomic) MGLStyleValue<NSValue *> *position;

/**
 The transition affecting any changes to this layer’s `position` property.

 This property corresponds to the `position-transition` property in the style JSON file format.
*/
@property (nonatomic) MGLTransition positionTransition;

#if TARGET_OS_IPHONE
/**
 Color tint for lighting extruded geometries.
 
 The default value of this property is an `MGLStyleValue` object containing
 `UIColor.whiteColor`.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`

 This property corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-js/style-spec/#light-color"><code>color</code></a>
 light property in the Mapbox Style Specification.
 */
@property (nonatomic) MGLStyleValue<UIColor *> *color;
#else
/**
 Color tint for lighting extruded geometries.
 
 The default value of this property is an `MGLStyleValue` object containing
 `NSColor.whiteColor`.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`

 This property corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-js/style-spec/#light-color"><code>color</code></a>
 light property in the Mapbox Style Specification.
 */
@property (nonatomic) MGLStyleValue<NSColor *> *color;
#endif

/**
 The transition affecting any changes to this layer’s `color` property.

 This property corresponds to the `color-transition` property in the style JSON file format.
*/
@property (nonatomic) MGLTransition colorTransition;

/**
 Intensity of lighting (on a scale from 0 to 1). Higher numbers will present as
 more extreme contrast.
 
 The default value of this property is an `MGLStyleValue` object containing an
 `NSNumber` object containing the float `0.5`.
 
 You can set this property to an instance of:
 
 * `MGLConstantStyleValue`
 * `MGLCameraStyleFunction` with an interpolation mode of:
   * `MGLInterpolationModeExponential`
   * `MGLInterpolationModeInterval`

 This property corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-js/style-spec/#light-intensity"><code>intensity</code></a>
 light property in the Mapbox Style Specification.
 */
@property (nonatomic) MGLStyleValue<NSNumber *> *intensity;

/**
 The transition affecting any changes to this layer’s `intensity` property.

 This property corresponds to the `intensity-transition` property in the style JSON file format.
*/
@property (nonatomic) MGLTransition intensityTransition;


@end

NS_ASSUME_NONNULL_END
