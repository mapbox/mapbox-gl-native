#import "MGLPinAnnotationView.h"

@implementation MGLPinAnnotationView

- (instancetype)initWithReuseIdentifier:(nullable NSString *)reuseIdentifier {
    self = [super initWithReuseIdentifier:reuseIdentifier];
    _shadowColor = [UIColor blackColor];
    _pinColor = [UIColor blueColor];
    _strokeColor = [UIColor whiteColor];
    _innerColor = [UIColor whiteColor];

    return self;
}

- (void)drawRect:(CGRect)rect {
    
    //// General Declarations
    CGContextRef context = UIGraphicsGetCurrentContext();
    
    //// Group
    {
        //// Oval 2 Drawing
        CGContextSaveGState(context);
        CGContextSetAlpha(context, 0.1);
        
        UIBezierPath* oval2Path = [UIBezierPath bezierPathWithOvalInRect: CGRectMake(10, 40.61, 19, 8)];
        [self.shadowColor setFill];
        [oval2Path fill];
        
        CGContextRestoreGState(context);
        
        
        //// Group 2
        {
            //// Bezier 2 Drawing
            UIBezierPath* bezier2Path = [UIBezierPath bezierPath];
            [bezier2Path moveToPoint: CGPointMake(18.04, 43.43)];
            [bezier2Path addCurveToPoint: CGPointMake(20.96, 43.43) controlPoint1: CGPointMake(18.85, 44.18) controlPoint2: CGPointMake(20.16, 44.17)];
            [bezier2Path addCurveToPoint: CGPointMake(37, 19.55) controlPoint1: CGPointMake(20.96, 43.43) controlPoint2: CGPointMake(37, 29.24)];
            [bezier2Path addCurveToPoint: CGPointMake(19.5, 2) controlPoint1: CGPointMake(37, 9.86) controlPoint2: CGPointMake(29.16, 2)];
            [bezier2Path addCurveToPoint: CGPointMake(2, 19.55) controlPoint1: CGPointMake(9.84, 2) controlPoint2: CGPointMake(2, 9.86)];
            [bezier2Path addCurveToPoint: CGPointMake(18.04, 43.43) controlPoint1: CGPointMake(2, 29.24) controlPoint2: CGPointMake(18.04, 43.43)];
            [bezier2Path closePath];
            bezier2Path.usesEvenOddFillRule = YES;
            [self.pinColor setFill];
            [bezier2Path fill];
            
            
            //// Bezier 3 Drawing
            UIBezierPath* bezier3Path = [UIBezierPath bezierPath];
            [bezier3Path moveToPoint: CGPointMake(18.04, 43.43)];
            [bezier3Path addCurveToPoint: CGPointMake(20.96, 43.43) controlPoint1: CGPointMake(18.85, 44.18) controlPoint2: CGPointMake(20.16, 44.17)];
            [bezier3Path addCurveToPoint: CGPointMake(37, 19.55) controlPoint1: CGPointMake(20.96, 43.43) controlPoint2: CGPointMake(37, 29.24)];
            [bezier3Path addCurveToPoint: CGPointMake(19.5, 2) controlPoint1: CGPointMake(37, 9.86) controlPoint2: CGPointMake(29.16, 2)];
            [bezier3Path addCurveToPoint: CGPointMake(2, 19.55) controlPoint1: CGPointMake(9.84, 2) controlPoint2: CGPointMake(2, 9.86)];
            [bezier3Path addCurveToPoint: CGPointMake(18.04, 43.43) controlPoint1: CGPointMake(2, 29.24) controlPoint2: CGPointMake(18.04, 43.43)];
            [bezier3Path closePath];
            [self.strokeColor setStroke];
            bezier3Path.lineWidth = 3;
            [bezier3Path stroke];
            
            
            //// Oval Drawing
            UIBezierPath* ovalPath = [UIBezierPath bezierPathWithOvalInRect: CGRectMake(12.5, 12.16, 14, 14)];
            [self.innerColor setFill];
            [ovalPath fill];
        }
    }
    
}

@end
