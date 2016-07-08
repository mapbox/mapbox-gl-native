#import "MBXAnnotationView.h"

@interface MBXAnnotationView ()
@end

@implementation MBXAnnotationView

- (void)layoutSubviews {
    [super layoutSubviews];
    
    self.layer.borderColor = [UIColor blueColor].CGColor;
    self.layer.borderWidth = 1;
    self.layer.cornerRadius = 2;
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated
{
    [super setSelected:selected animated:animated];
    
    self.layer.borderColor = selected ? [UIColor blackColor].CGColor : [UIColor whiteColor].CGColor;
    self.layer.borderWidth = selected ? 2.0 : 0;
}

- (void)setDragState:(MGLAnnotationViewDragState)dragState animated:(BOOL)animated
{
    [super setDragState:dragState animated:NO];
    
    switch (dragState) {
        case MGLAnnotationViewDragStateNone:
            break;
        case MGLAnnotationViewDragStateStarting: {
            [UIView animateWithDuration:.4 delay:0 usingSpringWithDamping:.4 initialSpringVelocity:.5 options:UIViewAnimationOptionCurveLinear animations:^{
                self.transform = CGAffineTransformScale(CGAffineTransformIdentity, 2, 2);
            } completion:nil];
            break;
        }
        case MGLAnnotationViewDragStateDragging:
            break;
        case MGLAnnotationViewDragStateCanceling:
            break;
        case MGLAnnotationViewDragStateEnding: {
            self.transform = CGAffineTransformScale(CGAffineTransformIdentity, 2, 2);
            [UIView animateWithDuration:.4 delay:0 usingSpringWithDamping:.4 initialSpringVelocity:.5 options:UIViewAnimationOptionCurveLinear animations:^{
                self.transform = CGAffineTransformScale(CGAffineTransformIdentity, 1, 1);
            } completion:nil];
            break;
        }
    }
    
}

- (nullable id<CAAction>)actionForLayer:(CALayer *)layer forKey:(NSString *)event
{
    if (([event isEqualToString:@"transform"] || [event isEqualToString:@"position"])
        && self.dragState == MGLAnnotationViewDragStateNone)
    {
        CABasicAnimation *animation = [CABasicAnimation animationWithKeyPath:event];
        animation.timingFunction = [CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionEaseInEaseOut];
        animation.speed = 0.1;
        return animation;
    }
    return [super actionForLayer:layer forKey:event];
}

@end
