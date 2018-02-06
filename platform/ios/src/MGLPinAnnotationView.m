#import "MGLPinAnnotationView.h"

@implementation MGLPinAnnotationView

- (instancetype)initWithReuseIdentifier:(nullable NSString *)reuseIdentifier {
    self = [super initWithReuseIdentifier:reuseIdentifier];
    _shadowColor = [UIColor blackColor];
    _pinColor = [UIColor blueColor];
    _strokeColor = [UIColor whiteColor];
    _innerColor = [UIColor whiteColor];
    self.backgroundColor = [UIColor clearColor];

    return self;
}

- (void)drawRect:(CGRect)rect {
    [self drawMarkerWithFrame:rect innerColor:_innerColor shadowColor:_shadowColor pinColor:_pinColor strokeColor:_strokeColor];
}

- (void)setShadowColor:(UIColor *)shadowColor {
    _shadowColor = shadowColor;
    [self setNeedsDisplay];
}

- (void)setPinColor:(UIColor *)pinColor {
    _pinColor = pinColor;
    [self setNeedsDisplay];
}

- (void)setStrokeColor:(UIColor *)strokeColor {
    _strokeColor = strokeColor;
    [self setNeedsDisplay];
}

- (void)setInnerColor:(UIColor *)innerColor {
    _innerColor = innerColor;
    [self setNeedsDisplay];
}

- (void)drawMarkerWithFrame:(CGRect)frame innerColor:(UIColor*)innerColor shadowColor:(UIColor*)shadowColor pinColor:(UIColor*)pinColor strokeColor:(UIColor*)strokeColor
{
    //// General Declarations
    CGContextRef context = UIGraphicsGetCurrentContext();
    
    //// Group
    {
        //// Oval 2 Drawing
        CGContextSaveGState(context);
        CGContextSetAlpha(context, 0.1);
        
        UIBezierPath* oval2Path = [UIBezierPath bezierPathWithOvalInRect: CGRectMake(CGRectGetMinX(frame) + 10, CGRectGetMinY(frame) + 40.61, 19, 8)];
        [shadowColor setFill];
        [oval2Path fill];
        
        CGContextRestoreGState(context);
        
        
        //// Group 2
        {
            //// Bezier 2 Drawing
            UIBezierPath* bezier2Path = [UIBezierPath bezierPath];
            [bezier2Path moveToPoint: CGPointMake(CGRectGetMinX(frame) + 18.04, CGRectGetMinY(frame) + 43.43)];
            [bezier2Path addCurveToPoint: CGPointMake(CGRectGetMinX(frame) + 20.96, CGRectGetMinY(frame) + 43.43) controlPoint1: CGPointMake(CGRectGetMinX(frame) + 18.85, CGRectGetMinY(frame) + 44.18) controlPoint2: CGPointMake(CGRectGetMinX(frame) + 20.16, CGRectGetMinY(frame) + 44.17)];
            [bezier2Path addCurveToPoint: CGPointMake(CGRectGetMinX(frame) + 37, CGRectGetMinY(frame) + 19.55) controlPoint1: CGPointMake(CGRectGetMinX(frame) + 20.96, CGRectGetMinY(frame) + 43.43) controlPoint2: CGPointMake(CGRectGetMinX(frame) + 37, CGRectGetMinY(frame) + 29.24)];
            [bezier2Path addCurveToPoint: CGPointMake(CGRectGetMinX(frame) + 19.5, CGRectGetMinY(frame) + 2) controlPoint1: CGPointMake(CGRectGetMinX(frame) + 37, CGRectGetMinY(frame) + 9.86) controlPoint2: CGPointMake(CGRectGetMinX(frame) + 29.16, CGRectGetMinY(frame) + 2)];
            [bezier2Path addCurveToPoint: CGPointMake(CGRectGetMinX(frame) + 2, CGRectGetMinY(frame) + 19.55) controlPoint1: CGPointMake(CGRectGetMinX(frame) + 9.84, CGRectGetMinY(frame) + 2) controlPoint2: CGPointMake(CGRectGetMinX(frame) + 2, CGRectGetMinY(frame) + 9.86)];
            [bezier2Path addCurveToPoint: CGPointMake(CGRectGetMinX(frame) + 18.04, CGRectGetMinY(frame) + 43.43) controlPoint1: CGPointMake(CGRectGetMinX(frame) + 2, CGRectGetMinY(frame) + 29.24) controlPoint2: CGPointMake(CGRectGetMinX(frame) + 18.04, CGRectGetMinY(frame) + 43.43)];
            [bezier2Path closePath];
            bezier2Path.usesEvenOddFillRule = YES;
            [pinColor setFill];
            [bezier2Path fill];
            
            
            //// Bezier 3 Drawing
            UIBezierPath* bezier3Path = [UIBezierPath bezierPath];
            [bezier3Path moveToPoint: CGPointMake(CGRectGetMinX(frame) + 18.04, CGRectGetMinY(frame) + 43.43)];
            [bezier3Path addCurveToPoint: CGPointMake(CGRectGetMinX(frame) + 20.96, CGRectGetMinY(frame) + 43.43) controlPoint1: CGPointMake(CGRectGetMinX(frame) + 18.85, CGRectGetMinY(frame) + 44.18) controlPoint2: CGPointMake(CGRectGetMinX(frame) + 20.16, CGRectGetMinY(frame) + 44.17)];
            [bezier3Path addCurveToPoint: CGPointMake(CGRectGetMinX(frame) + 37, CGRectGetMinY(frame) + 19.55) controlPoint1: CGPointMake(CGRectGetMinX(frame) + 20.96, CGRectGetMinY(frame) + 43.43) controlPoint2: CGPointMake(CGRectGetMinX(frame) + 37, CGRectGetMinY(frame) + 29.24)];
            [bezier3Path addCurveToPoint: CGPointMake(CGRectGetMinX(frame) + 19.5, CGRectGetMinY(frame) + 2) controlPoint1: CGPointMake(CGRectGetMinX(frame) + 37, CGRectGetMinY(frame) + 9.86) controlPoint2: CGPointMake(CGRectGetMinX(frame) + 29.16, CGRectGetMinY(frame) + 2)];
            [bezier3Path addCurveToPoint: CGPointMake(CGRectGetMinX(frame) + 2, CGRectGetMinY(frame) + 19.55) controlPoint1: CGPointMake(CGRectGetMinX(frame) + 9.84, CGRectGetMinY(frame) + 2) controlPoint2: CGPointMake(CGRectGetMinX(frame) + 2, CGRectGetMinY(frame) + 9.86)];
            [bezier3Path addCurveToPoint: CGPointMake(CGRectGetMinX(frame) + 18.04, CGRectGetMinY(frame) + 43.43) controlPoint1: CGPointMake(CGRectGetMinX(frame) + 2, CGRectGetMinY(frame) + 29.24) controlPoint2: CGPointMake(CGRectGetMinX(frame) + 18.04, CGRectGetMinY(frame) + 43.43)];
            [bezier3Path closePath];
            [strokeColor setStroke];
            bezier3Path.lineWidth = 3;
            [bezier3Path stroke];
            
            
            //// Oval Drawing
            UIBezierPath* ovalPath = [UIBezierPath bezierPathWithOvalInRect: CGRectMake(CGRectGetMinX(frame) + 12.5, CGRectGetMinY(frame) + 12.16, 14, 14)];
            [innerColor setFill];
            [ovalPath fill];
        }
    }
}

@end
