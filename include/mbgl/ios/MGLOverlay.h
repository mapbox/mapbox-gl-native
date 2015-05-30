#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>

#import "MGLAnnotation.h"
#import "MGLTypes.h"

@protocol MGLOverlay <MGLAnnotation>

@property (nonatomic, readonly) CLLocationCoordinate2D coordinate;
@property (nonatomic, readonly) MGLMapBounds overlayBounds;

- (BOOL)intersectsOverlayBounds:(MGLMapBounds)overlayBounds;

@end
