#import "MGLPinAnnotationView.h"

@implementation MGLPinAnnotationView

- (instancetype)initWithReuseIdentifier:(nullable NSString *)reuseIdentifier {
    if (self = [super initWithReuseIdentifier:reuseIdentifier]) {
        _shadowColor = [UIColor blackColor];
        _pinColor = [UIColor blueColor];
        _strokeColor = [UIColor yellowColor];
        _innerColor = [UIColor whiteColor];
        self.backgroundColor = [UIColor clearColor];
    }

    return self;
}

- (void)drawRect:(CGRect)rect {
    [self drawMarkerWithInnerColor:_innerColor shadowColor:_shadowColor pinColor:_pinColor strokeColor:_strokeColor size:rect.size strokeWidth:3];
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

- (void)drawMarkerWithInnerColor: (UIColor*)innerColor shadowColor: (UIColor*)shadowColor pinColor: (UIColor*)pinColor strokeColor: (UIColor*)strokeColor size: (CGSize)size strokeWidth: (CGFloat)strokeWidth
{
    //// General Declarations
    CGContextRef context = UIGraphicsGetCurrentContext();
    
    //// Frames
    CGRect frame = CGRectMake(0, 0, size.width, size.height);
    
    //// Subframes
    CGRect pin = CGRectMake(CGRectGetMinX(frame) + 1.5, CGRectGetMinY(frame) + 1.5, floor((frame.size.width - 1.5) * 0.98667 + 0.5), floor((frame.size.height - 1.5) * 1.00216 + 1.89) - 1.39);
    
    
    //// Pin
    {
        //// Pin shadow Drawing
        CGContextSaveGState(context);
        CGContextSetAlpha(context, 0.1);
        
        UIBezierPath* pinShadowPath = [UIBezierPath bezierPathWithOvalInRect: CGRectMake(CGRectGetMinX(pin) + floor(pin.size.width * 0.24324 + 0.5), CGRectGetMinY(pin) + floor(pin.size.height * 0.83873 - 0.11) + 0.61, floor(pin.size.width * 0.75676 + 0.5) - floor(pin.size.width * 0.24324 + 0.5), floor(pin.size.height * 1.00000 - 0.11) - floor(pin.size.height * 0.83873 - 0.11))];
        [shadowColor setFill];
        [pinShadowPath fill];
        
        CGContextRestoreGState(context);
        
        
        //// Fill Drawing
        UIBezierPath* fillPath = [UIBezierPath bezierPath];
        [fillPath moveToPoint: CGPointMake(CGRectGetMinX(pin) + 0.45823 * pin.size.width, CGRectGetMinY(pin) + 0.88889 * pin.size.height)];
        [fillPath addCurveToPoint: CGPointMake(CGRectGetMinX(pin) + 0.54177 * pin.size.width, CGRectGetMinY(pin) + 0.88889 * pin.size.height) controlPoint1: CGPointMake(CGRectGetMinX(pin) + 0.48130 * pin.size.width, CGRectGetMinY(pin) + 0.90492 * pin.size.height) controlPoint2: CGPointMake(CGRectGetMinX(pin) + 0.51899 * pin.size.width, CGRectGetMinY(pin) + 0.90491 * pin.size.height)];
        [fillPath addCurveToPoint: CGPointMake(CGRectGetMinX(pin) + 1.00000 * pin.size.width, CGRectGetMinY(pin) + 0.37658 * pin.size.height) controlPoint1: CGPointMake(CGRectGetMinX(pin) + 0.54177 * pin.size.width, CGRectGetMinY(pin) + 0.88889 * pin.size.height) controlPoint2: CGPointMake(CGRectGetMinX(pin) + 1.00000 * pin.size.width, CGRectGetMinY(pin) + 0.58455 * pin.size.height)];
        [fillPath addCurveToPoint: CGPointMake(CGRectGetMinX(pin) + 0.50000 * pin.size.width, CGRectGetMinY(pin) + 0.00000 * pin.size.height) controlPoint1: CGPointMake(CGRectGetMinX(pin) + 1.00000 * pin.size.width, CGRectGetMinY(pin) + 0.16860 * pin.size.height) controlPoint2: CGPointMake(CGRectGetMinX(pin) + 0.77614 * pin.size.width, CGRectGetMinY(pin) + 0.00000 * pin.size.height)];
        [fillPath addCurveToPoint: CGPointMake(CGRectGetMinX(pin) + 0.00000 * pin.size.width, CGRectGetMinY(pin) + 0.37658 * pin.size.height) controlPoint1: CGPointMake(CGRectGetMinX(pin) + 0.22386 * pin.size.width, CGRectGetMinY(pin) + 0.00000 * pin.size.height) controlPoint2: CGPointMake(CGRectGetMinX(pin) + 0.00000 * pin.size.width, CGRectGetMinY(pin) + 0.16860 * pin.size.height)];
        [fillPath addCurveToPoint: CGPointMake(CGRectGetMinX(pin) + 0.45823 * pin.size.width, CGRectGetMinY(pin) + 0.88889 * pin.size.height) controlPoint1: CGPointMake(CGRectGetMinX(pin) + 0.00000 * pin.size.width, CGRectGetMinY(pin) + 0.58455 * pin.size.height) controlPoint2: CGPointMake(CGRectGetMinX(pin) + 0.45823 * pin.size.width, CGRectGetMinY(pin) + 0.88889 * pin.size.height)];
        [fillPath closePath];
        fillPath.usesEvenOddFillRule = YES;
        [pinColor setFill];
        [fillPath fill];
        
        
        //// Stroke Drawing
        UIBezierPath* strokePath = [UIBezierPath bezierPath];
        [strokePath moveToPoint: CGPointMake(CGRectGetMinX(pin) + 0.45823 * pin.size.width, CGRectGetMinY(pin) + 0.88889 * pin.size.height)];
        [strokePath addCurveToPoint: CGPointMake(CGRectGetMinX(pin) + 0.54177 * pin.size.width, CGRectGetMinY(pin) + 0.88889 * pin.size.height) controlPoint1: CGPointMake(CGRectGetMinX(pin) + 0.48130 * pin.size.width, CGRectGetMinY(pin) + 0.90492 * pin.size.height) controlPoint2: CGPointMake(CGRectGetMinX(pin) + 0.51899 * pin.size.width, CGRectGetMinY(pin) + 0.90491 * pin.size.height)];
        [strokePath addCurveToPoint: CGPointMake(CGRectGetMinX(pin) + 1.00000 * pin.size.width, CGRectGetMinY(pin) + 0.37658 * pin.size.height) controlPoint1: CGPointMake(CGRectGetMinX(pin) + 0.54177 * pin.size.width, CGRectGetMinY(pin) + 0.88889 * pin.size.height) controlPoint2: CGPointMake(CGRectGetMinX(pin) + 1.00000 * pin.size.width, CGRectGetMinY(pin) + 0.58455 * pin.size.height)];
        [strokePath addCurveToPoint: CGPointMake(CGRectGetMinX(pin) + 0.50000 * pin.size.width, CGRectGetMinY(pin) + 0.00000 * pin.size.height) controlPoint1: CGPointMake(CGRectGetMinX(pin) + 1.00000 * pin.size.width, CGRectGetMinY(pin) + 0.16860 * pin.size.height) controlPoint2: CGPointMake(CGRectGetMinX(pin) + 0.77614 * pin.size.width, CGRectGetMinY(pin) + 0.00000 * pin.size.height)];
        [strokePath addCurveToPoint: CGPointMake(CGRectGetMinX(pin) + 0.00000 * pin.size.width, CGRectGetMinY(pin) + 0.37658 * pin.size.height) controlPoint1: CGPointMake(CGRectGetMinX(pin) + 0.22386 * pin.size.width, CGRectGetMinY(pin) + 0.00000 * pin.size.height) controlPoint2: CGPointMake(CGRectGetMinX(pin) + 0.00000 * pin.size.width, CGRectGetMinY(pin) + 0.16860 * pin.size.height)];
        [strokePath addCurveToPoint: CGPointMake(CGRectGetMinX(pin) + 0.45823 * pin.size.width, CGRectGetMinY(pin) + 0.88889 * pin.size.height) controlPoint1: CGPointMake(CGRectGetMinX(pin) + 0.00000 * pin.size.width, CGRectGetMinY(pin) + 0.58455 * pin.size.height) controlPoint2: CGPointMake(CGRectGetMinX(pin) + 0.45823 * pin.size.width, CGRectGetMinY(pin) + 0.88889 * pin.size.height)];
        [strokePath closePath];
        [strokeColor setStroke];
        strokePath.lineWidth = strokeWidth;
        [strokePath stroke];
        
        
        //// Inner Drawing
        UIBezierPath* innerPath = [UIBezierPath bezierPathWithOvalInRect: CGRectMake(CGRectGetMinX(pin) + floor(pin.size.width * 0.29730 + 0.5), CGRectGetMinY(pin) + floor(pin.size.height * 0.22174 - 0.5) + 1, floor(pin.size.width * 0.70270 + 0.5) - floor(pin.size.width * 0.29730 + 0.5), floor(pin.size.height * 0.52412 - 0.5) - floor(pin.size.height * 0.22174 - 0.5))];
        [innerColor setFill];
        [innerPath fill];
    }
}


@end
