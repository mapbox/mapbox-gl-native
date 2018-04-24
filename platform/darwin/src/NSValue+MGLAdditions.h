#import <Foundation/Foundation.h>

#import "MGLGeometry.h"
#import "MGLLight.h"
#import "MGLOfflinePack.h"
#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

/**
 Methods for round-tripping values for Mapbox-defined types.
 */
@interface NSValue (MGLAdditions)

#pragma mark Working with Geographic Coordinate Values

/**
 Creates a new value object containing the specified Core Location geographic
 coordinate structure.

 @param coordinate The value for the new object.
 @return A new value object that contains the geographic coordinate information.
 */
+ (instancetype)valueWithMGLCoordinate:(CLLocationCoordinate2D)coordinate;

/**
 The Core Location geographic coordinate structure representation of the value.
 */
@property (readonly) CLLocationCoordinate2D MGLCoordinateValue;

/**
 Creates a new value object containing the specified Mapbox map point structure.

 @param point The value for the new object.
 @return A new value object that contains the coordinate and zoom level information.
 */
+ (instancetype)valueWithMGLMapPoint:(MGLMapPoint)point;

/**
 The Mapbox map point structure representation of the value.
 */
@property (readonly) MGLMapPoint MGLMapPointValue;

/**
 Creates a new value object containing the specified Mapbox coordinate span
 structure.

 @param span The value for the new object.
 @return A new value object that contains the coordinate span information.
 */
+ (instancetype)valueWithMGLCoordinateSpan:(MGLCoordinateSpan)span;

/**
 The Mapbox coordinate span structure representation of the value.
 */
@property (readonly) MGLCoordinateSpan MGLCoordinateSpanValue;

/**
 Creates a new value object containing the specified Mapbox coordinate bounds
 structure.

 @param bounds The value for the new object.
 @return A new value object that contains the coordinate bounds information.
 */
+ (instancetype)valueWithMGLCoordinateBounds:(MGLCoordinateBounds)bounds;

/**
 The Mapbox coordinate bounds structure representation of the value.
 */
@property (readonly) MGLCoordinateBounds MGLCoordinateBoundsValue;

/**
 Creates a new value object containing the specified Mapbox coordinate 
 quad structure.

 @param quad The value for the new object.
 @return A new value object that contains the coordinate quad information.
 */
+ (instancetype)valueWithMGLCoordinateQuad:(MGLCoordinateQuad)quad;

/**
 The Mapbox coordinate quad structure representation of the value.
 */
- (MGLCoordinateQuad)MGLCoordinateQuadValue;

#pragma mark Working with Offline Map Values

/**
 Creates a new value object containing the given `MGLOfflinePackProgress`
 structure.

 @param progress The value for the new object.
 @return A new value object that contains the offline pack progress information.
 */
+ (NSValue *)valueWithMGLOfflinePackProgress:(MGLOfflinePackProgress)progress;

/**
 The `MGLOfflinePackProgress` structure representation of the value.
 */
@property (readonly) MGLOfflinePackProgress MGLOfflinePackProgressValue;

#pragma mark Working with Transition Values

/**
 Creates a new value object containing the given `MGLTransition`
 structure.
 
 @param transition The value for the new object.
 @return A new value object that contains the transition information.
 */
+ (NSValue *)valueWithMGLTransition:(MGLTransition)transition;

/**
 The `MGLTransition` structure representation of the value.
 */
@property (readonly) MGLTransition MGLTransitionValue;

/**
 Creates a new value object containing the given `MGLSphericalPosition`
 structure.
 
 @param lightPosition The value for the new object.
 @return A new value object that contains the light position information.
 */
+ (instancetype)valueWithMGLSphericalPosition:(MGLSphericalPosition)lightPosition;

/**
 The `MGLSphericalPosition` structure representation of the value.
 */
@property (readonly) MGLSphericalPosition MGLSphericalPositionValue;

/**
 Creates a new value object containing the given `MGLLightAnchor`
 enum.
 
 @param lightAnchor The value for the new object.
 @return A new value object that contains the light anchor information.
 */
+ (NSValue *)valueWithMGLLightAnchor:(MGLLightAnchor)lightAnchor;

/**
 The `MGLLightAnchor` enum representation of the value.
 */
@property (readonly) MGLLightAnchor MGLLightAnchorValue;

@end

NS_ASSUME_NONNULL_END
