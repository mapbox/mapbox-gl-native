#import <CoreLocation/CoreLocation.h>

#import "MGLFoundation.h"
#import "MGLStyleValue.h"

NS_ASSUME_NONNULL_BEGIN


/** Options to specify extruded geometries are lit relative to the map or viewport. */
typedef NS_ENUM(NSUInteger, MGLLightAnchor) {
    /** The position of the light source is aligned to the rotation of the map. */
    MGLLightAnchorMap,
    /** The position of the light source is aligned to the rotation of the viewport. */
    MGLLightAnchorViewport
};

/**
 A structure containing information about the position of the light source
 relative to lit geometries.
 */
typedef struct MGLSphericalPosition {
    /** Distance from the center of the base of an object to its light. */
    CLLocationDistance radial;
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
NS_INLINE MGLSphericalPosition MGLSphericalPositionMake(CLLocationDistance radial, CLLocationDirection azimuthal, CLLocationDirection polar) {
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
 `anchor` Whether extruded geometries are lit relative to the map or viewport.
 
 This property corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-js/style-spec/#light-anchor"><code>anchor</code></a>
 light property in the Mapbox Style Specification.
 */
@property (nonatomic) MGLStyleValue<NSValue *> *anchor;

/**
 Values describing animated transitions to `anchor` property.
 */
@property (nonatomic) MGLTransition anchorTransition;


/**
 Position of the light source relative to lit (extruded) geometries.
 
 This property corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-js/style-spec/#light-position"><code>position</code></a>
 light property in the Mapbox Style Specification.
 */
@property (nonatomic) MGLStyleValue<NSValue *> *position;

/**
 Values describing animated transitions to `position` property.
 */
@property (nonatomic) MGLTransition positionTransiton;


#if TARGET_OS_IPHONE
/**
 Color tint for lighting extruded geometries.
 
 This property corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-js/style-spec/#light-color"><code>color</code></a>
 light property in the Mapbox Style Specification.
 */
@property (nonatomic) MGLStyleValue<UIColor *> *color;
#else

/**
 Color tint for lighting extruded geometries.
 */
@property (nonatomic) MGLStyleValue<NSColor *> *color;
#endif

/**
 Values describing animated transitions to `color` property.
 */
@property (nonatomic) MGLTransition colorTransiton;


/**
 Intensity of lighting (on a scale from 0 to 1). Higher numbers will present as more extreme contrast.
 
 This property corresponds to the <a
 href="https://www.mapbox.com/mapbox-gl-js/style-spec/#light-intensity"><code>intensity</code></a>
 light property in the Mapbox Style Specification.
 */
@property(nonatomic) MGLStyleValue<NSNumber *> *intensity;

/**
 Values describing animated transitions to `intensity` property.
 */
@property (nonatomic) MGLTransition intensityTransition;

@end

NS_ASSUME_NONNULL_END
