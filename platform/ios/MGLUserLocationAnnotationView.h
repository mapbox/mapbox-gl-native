#import <UIKit/UIKit.h>
#import <CoreLocation/CoreLocation.h>

@class MGLMapView;
@class MGLUserLocation;

/** The MGLUserLocationAnnotationView class defines a specific type of annotation that identifies the user’s current location. You do not create instances of this class directly. Instead, you retrieve an existing MGLUserLocationAnnotationView object from the userLocation property of the map view displayed in your application. */
@interface MGLUserLocationAnnotationView : UIView

@property (nonatomic, weak) MGLMapView *mapView;
@property (nonatomic) MGLUserLocation *annotation;
@property (nonatomic, readonly) CALayer *haloLayer;

- (instancetype)initInMapView:(MGLMapView *)mapView;
- (void)setupLayers;

@end
