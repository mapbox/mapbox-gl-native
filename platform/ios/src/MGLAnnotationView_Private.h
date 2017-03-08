#import "MGLAnnotationView.h"
#import "MGLAnnotation.h"

NS_ASSUME_NONNULL_BEGIN

@class MGLMapView;

@interface MGLAnnotationView (Private)

@property (nonatomic, readwrite, nullable) NSString *reuseIdentifier;
@property (nonatomic, weak) MGLMapView *mapView;

@end

NS_ASSUME_NONNULL_END
