#import "MGLAnnotationContainerView.h"
#import "MGLAnnotationView.h"

@class MGLAnnotationView;

NS_ASSUME_NONNULL_BEGIN

@interface MGLAnnotationContainerView (Private)

@property (nonatomic) NS_MUTABLE_ARRAY_OF(MGLAnnotationView *) *annotationViews;

@end

NS_ASSUME_NONNULL_END
