#import <UIKit/UIKit.h>
#import <CoreLocation/CoreLocation.h>

#import "MGLUserLocation.h"

@class MGLMapView;

/** The MGLUserLocationAnnotationView class defines a specific type of annotation that identifies the user’s current location. You do not create instances of this class directly. Instead, you retrieve an existing MGLUserLocationAnnotationView object from the userLocation property of the map view displayed in your application. */
@interface MGLUserLocationAnnotationView : UIImageView

@property (nonatomic, weak) MGLMapView *mapView;
@property (nonatomic) MGLUserLocation *annotation;

/** @name Determining the User’s Position */

/** A Boolean value indicating whether the user’s location is currently being updated. (read-only) */
@property (nonatomic, readonly, getter=isUpdating) BOOL updating;

@property (nonatomic, readonly) CALayer *haloLayer;

- (instancetype)initInMapView:(MGLMapView *)mapView;
- (void)setupLayers;

@end
