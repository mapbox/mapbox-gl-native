#import <UIKit/UIKit.h>
#import <CoreLocation/CoreLocation.h>
#import <Mapbox/Mapbox.h>

#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

@class MGLMapView;
@class MGLUserLocation;

/** View representing an `MGLUserLocation` on screen. */
@interface MGLUserLocationAnnotationView : MGLAnnotationView

/**
 Returns the associated map view.
 
 The value of this property is nil during initialization.
 */
@property (nonatomic, readonly, weak, nullable) MGLMapView *mapView;
 
/**
 Returns the annotation object indicating the user's current location.
 
 The value of this property is nil during initialization and while user tracking is inactive.
 */
@property (nonatomic, readonly, weak, nullable) MGLUserLocation *userLocation;

@end

NS_ASSUME_NONNULL_END
