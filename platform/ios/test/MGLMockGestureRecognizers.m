
#import "MGLMockGestureRecognizers.h"
#import "objc/runtime.h"

@implementation UIPinchGestureRecognizerMock
@synthesize velocity;
@synthesize state;
- (CGPoint)locationInView:(nullable UIView *)view { return self.locationInViewOverride; }
@end

@implementation UIRotationGestureRecognizerMock
- (CGPoint)locationInView:(nullable UIView*)view { return view.center; }
@synthesize state;
@end

@implementation UITapGestureRecognizerMock

+ (void)load {
    method_exchangeImplementations(class_getInstanceMethod(self, @selector(state)),
                                   class_getInstanceMethod(self, @selector(mockState)));
}

- (UIGestureRecognizerState)mockState {
    return UIGestureRecognizerStateRecognized;
}

- (UIView *)view {
    return self.mockTappedView;
}

- (CGPoint)locationInView:(UIView *)view {
    return self.mockTappedPoint;
}

@end

@implementation UILongPressGestureRecognizerMock
@synthesize state;

- (CGPoint)locationInView:(UIView *)view {
    return self.mockTappedPoint;
}
@end

@implementation UIPanGestureRecognizerMock
@synthesize state;
@synthesize numberOfTouches;

- (CGPoint)locationOfTouch:(NSUInteger)touchIndex inView:(UIView *)view {
    if (touchIndex) {
        return self.secondFingerPoint;
    }
    return self.firstFingerPoint;
}
@end
