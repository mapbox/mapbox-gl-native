#import <UIKit/UIKit.h>

#import "MGLTypes.h"

@class MGLAnnotationView;

NS_ASSUME_NONNULL_BEGIN

MGL_EXTERN
@interface MGLAnnotationContainerView : UIView

+ (instancetype)annotationContainerViewWithAnnotationContainerView:(MGLAnnotationContainerView *)annotationContainerView;

- (void)addSubviews:(NS_ARRAY_OF(MGLAnnotationView *) *)subviews;

@end

NS_ASSUME_NONNULL_END
