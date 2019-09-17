
#import "MGLMockGestureRecognizers.h"

@implementation UIPinchGestureRecognizerMock
@synthesize velocity;
- (CGPoint)locationInView:(nullable UIView *)view { return self.locationInViewOverride; }
@end

@implementation UIRotationGestureRecognizerMock
- (CGPoint)locationInView:(nullable UIView*)view { return view.center; }
@end
