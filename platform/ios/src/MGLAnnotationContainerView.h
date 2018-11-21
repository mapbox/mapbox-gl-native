#import <UIKit/UIKit.h>

#import "MGLTypes.h"

@class MGLAnnotationView;

NS_ASSUME_NONNULL_BEGIN

@interface MGLAnnotationContainerView : UIView

+ (instancetype)annotationContainerViewWithAnnotationContainerView:(MGLAnnotationContainerView *)annotationContainerView;

- (void)addSubviews:(NSArray<MGLAnnotationView *> *)subviews;

@end

NS_ASSUME_NONNULL_END
