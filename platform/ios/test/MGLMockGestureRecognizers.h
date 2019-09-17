
#import <UIKit/UIKit.h>

@interface UIPinchGestureRecognizerMock : UIPinchGestureRecognizer
@property (nonatomic, readwrite) CGFloat velocity;
@property (nonatomic) CGPoint locationInViewOverride;
@end

@interface UIRotationGestureRecognizerMock : UIRotationGestureRecognizer
@end
