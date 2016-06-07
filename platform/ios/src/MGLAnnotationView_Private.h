#import "MGLAnnotationView.h"
#import "MGLAnnotation.h"

NS_ASSUME_NONNULL_BEGIN

@class MGLMapView;

@interface MGLAnnotationView (Private)

@property (nonatomic) id<MGLAnnotation> annotation;
@property (nonatomic, readwrite, nullable) NSString *reuseIdentifier;
@property (nonatomic, weak) MGLMapView *mapView;

- (void)setCenter:(CGPoint)center pitch:(CGFloat)pitch;

@end

NS_ASSUME_NONNULL_END
