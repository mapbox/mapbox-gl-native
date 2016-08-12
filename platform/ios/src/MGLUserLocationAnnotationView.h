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
 Returns the annotation object indicating the user’s current location.
 
 The value of this property is nil during initialization and while user tracking
 is inactive.
 */
@property (nonatomic, readonly, weak, nullable) MGLUserLocation *userLocation;

/**
 Returns the layer that should be used for annotation selection hit testing.
 
 The default value of this property is the presentation layer of the view’s Core
 Animation layer. When subclassing, you may override this property to specify a
 different layer to be used for hit testing. This can be useful when you wish to
 limit the interactive area of the annotation to a specific sublayer.
 */
@property (nonatomic, readonly, weak) CALayer *hitTestLayer;

/**
 Called by associated map view to update the user location annotation.

 This method is called by the associated map view when it has determined the
 user location annotation should update. This can happen in response to a change
 in the user’s location, when the user tracking mode changes, or when the
 viewport changes.
 */
- (void)update;

@end

NS_ASSUME_NONNULL_END
