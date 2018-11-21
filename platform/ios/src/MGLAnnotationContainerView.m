#import "MGLAnnotationContainerView.h"
#import "MGLAnnotationView.h"

@interface MGLAnnotationContainerView ()

@property (nonatomic) NSMutableArray<MGLAnnotationView *> *annotationViews;

@end

@implementation MGLAnnotationContainerView

- (instancetype)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self)
    {
        _annotationViews = [NSMutableArray array];
    }
    return self;
}

+ (instancetype)annotationContainerViewWithAnnotationContainerView:(nonnull MGLAnnotationContainerView *)annotationContainerView
{
    MGLAnnotationContainerView *newAnnotationContainerView = [[MGLAnnotationContainerView alloc] initWithFrame:annotationContainerView.frame];
    [newAnnotationContainerView addSubviews:annotationContainerView.subviews];
    return newAnnotationContainerView;
}

- (void)addSubviews:(NSArray<MGLAnnotationView *> *)subviews
{
    for (MGLAnnotationView *view in subviews)
    {
        [self addSubview:view];
        [self.annotationViews addObject:view];
    }
}

#pragma mark UIAccessibility methods

- (UIAccessibilityTraits)accessibilityTraits {
    return UIAccessibilityTraitAdjustable;
}

- (void)accessibilityIncrement {
    [self.superview.superview accessibilityIncrement];
}

- (void)accessibilityDecrement {
    [self.superview.superview accessibilityDecrement];
}

@end
