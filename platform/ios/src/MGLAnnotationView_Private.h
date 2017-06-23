#import "MGLAnnotationView.h"
#import "MGLAnnotation.h"

#import <GLKit/GLKit.h>

NS_ASSUME_NONNULL_BEGIN

@class MGLMapView;

CATransform3D MGLTransform3DFromMatrix4(GLKMatrix4 matrix);

@interface MGLAnnotationView (Private)

@property (nonatomic, readwrite, nullable) NSString *reuseIdentifier;
@property (nonatomic, weak) MGLMapView *mapView;

@end

NS_ASSUME_NONNULL_END
