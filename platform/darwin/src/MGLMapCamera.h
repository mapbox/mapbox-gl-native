#import <Foundation/Foundation.h>
#import <CoreGraphics/CoreGraphics.h>
#import <CoreLocation/CoreLocation.h>

#import "MGLFoundation.h"

NS_ASSUME_NONNULL_BEGIN

/**
 An `MGLMapCamera` object represents a viewpoint from which the user observes
 some point on an `MGLMapView`.
 
 #### Related examples
 See the <a href="https://docs.mapbox.com/ios/maps/examples/camera-animation/">
 Camera animation</a> example to learn how to create a camera that rotates
 around a central point. See the <a href="https://docs.mapbox.com/ios/maps/examples/constraining-gestures/">
 Restrict map panning to an area</a> example to learn how to restrict map
 panning using `MGLMapViewDelegate`'s
`-mapView:shouldChangeFromCamera:toCamera:` method.
 */
MGL_EXPORT
@interface MGLMapCamera : NSObject <NSSecureCoding, NSCopying>

/** Coordinate at the center of the map view. */
@property (nonatomic) CLLocationCoordinate2D centerCoordinate;

/** Heading measured in degrees clockwise from true north. */
@property (nonatomic) CLLocationDirection heading;

/**
 Pitch toward the horizon measured in degrees, with 0 degrees resulting in a
 two-dimensional map.
 */
@property (nonatomic) CGFloat pitch;

/**
 The altitude (measured in meters) above the map at which the camera is
 situated.
 
 The altitude is the distance from the viewpoint to the map, perpendicular to
 the map plane. This property does not account for physical elevation.
 
 This property’s value may be less than that of the `viewingDistance` property.
 Setting this property automatically updates the `viewingDistance` property
 based on the `pitch` property’s current value.
 */
@property (nonatomic) CLLocationDistance altitude;

/**
 The straight-line distance from the viewpoint to the `centerCoordinate`.
 
 Setting this property automatically updates the `altitude` property based on
 the `pitch` property’s current value.
 */
@property (nonatomic) CLLocationDistance viewingDistance;

/** Returns a new camera with all properties set to 0. */
+ (instancetype)camera;

/**
 Returns a new camera based on information about the camera’s viewpoint
 and focus point.

 @param centerCoordinate The geographic coordinate on which the map should be
    centered.
 @param eyeCoordinate The geometric coordinate at which the camera should be
    situated.
 @param eyeAltitude The altitude (measured in meters) above the map at which the
    camera should be situated. The altitude may be less than the distance from
    the camera’s viewpoint to the camera’s focus point.
 */
+ (instancetype)cameraLookingAtCenterCoordinate:(CLLocationCoordinate2D)centerCoordinate
                              fromEyeCoordinate:(CLLocationCoordinate2D)eyeCoordinate
                                    eyeAltitude:(CLLocationDistance)eyeAltitude;

/**
 Returns a new camera with the given distance, pitch, and heading.
 
 This method interprets the distance as a straight-line distance from the
 viewpoint to the center coordinate. To specify the altitude of the viewpoint,
 use the `-cameraLookingAtCenterCoordinate:altitude:pitch:heading:` method.
 
 @param centerCoordinate The geographic coordinate on which the map should be
    centered.
 @param distance The straight-line distance from the viewpoint to the
    `centerCoordinate`.
 @param pitch The viewing angle of the camera, measured in degrees. A value of
    `0` results in a camera pointed straight down at the map. Angles greater
    than `0` result in a camera angled toward the horizon.
 @param heading The camera’s heading, measured in degrees clockwise from true
    north. A value of `0` means that the top edge of the map view corresponds to
    true north. The value `90` means the top of the map is pointing due east.
    The value `180` means the top of the map points due south, and so on.
 */
+ (instancetype)cameraLookingAtCenterCoordinate:(CLLocationCoordinate2D)centerCoordinate
                                 acrossDistance:(CLLocationDistance)distance
                                          pitch:(CGFloat)pitch
                                        heading:(CLLocationDirection)heading;

/**
 Returns a new camera with the given altitude, pitch, and heading.

 @param centerCoordinate The geographic coordinate on which the map should be
    centered.
 @param altitude The altitude (measured in meters) above the map at which the
    camera should be situated. The altitude may be less than the distance from
    the camera’s viewpoint to the camera’s focus point.
 @param pitch The viewing angle of the camera, measured in degrees. A value of
    `0` results in a camera pointed straight down at the map. Angles greater
    than `0` result in a camera angled toward the horizon.
 @param heading The camera’s heading, measured in degrees clockwise from true
    north. A value of `0` means that the top edge of the map view corresponds to
    true north. The value `90` means the top of the map is pointing due east.
    The value `180` means the top of the map points due south, and so on.
 */
+ (instancetype)cameraLookingAtCenterCoordinate:(CLLocationCoordinate2D)centerCoordinate
                                       altitude:(CLLocationDistance)altitude
                                          pitch:(CGFloat)pitch
                                        heading:(CLLocationDirection)heading;

/**
 @note This initializer incorrectly interprets the `distance` parameter. To
    specify the straight-line distance from the viewpoint to `centerCoordinate`,
    use the `-cameraLookingAtCenterCoordinate:acrossDistance:pitch:heading:`
    method. To specify the altitude of the viewpoint, use the
    `-cameraLookingAtCenterCoordinate:altitude:pitch:heading:` method, which has
    the same behavior as this initializer.
 */
+ (instancetype)cameraLookingAtCenterCoordinate:(CLLocationCoordinate2D)centerCoordinate
                                   fromDistance:(CLLocationDistance)distance
                                          pitch:(CGFloat)pitch
                                        heading:(CLLocationDirection)heading
__attribute__((deprecated("Use -cameraLookingAtCenterCoordinate:acrossDistance:pitch:heading: "
                          "or -cameraLookingAtCenterCoordinate:altitude:pitch:heading:.")));

/**
 Returns a Boolean value indicating whether the given camera is functionally
 equivalent to the receiver.
 
 Unlike `-isEqual:`, this method returns `YES` if the difference between the
 coordinates, altitudes, pitches, or headings of the two camera objects is
 negligible.
 
 @param otherCamera The camera with which to compare the receiver.
 @return A Boolean value indicating whether the two cameras are functionally
    equivalent.
 */
- (BOOL)isEqualToMapCamera:(MGLMapCamera *)otherCamera;

@end

NS_ASSUME_NONNULL_END
