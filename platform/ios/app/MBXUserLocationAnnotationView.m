#import "MBXUserLocationAnnotationView.h"

const CGFloat MBXUserLocationDotSize = 10;

@implementation MBXUserLocationAnnotationView

- (instancetype)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self == nil) return nil;
    self.backgroundColor = [UIColor clearColor];
    return self;
}

- (void)update
{
    [self updateFrameWithSize:self.intrinsicContentSize];
    [self setNeedsDisplay];
}


- (CGSize)intrinsicContentSize
{
    CGSize carSize = CGSizeMake(30, 60);
    return (self.mapView.userTrackingMode == MGLUserTrackingModeFollowWithCourse) ? carSize : [self dotSize];
}

- (CGSize)dotSize
{
    CGFloat minDotSize = 30;
    CGFloat dotSize = MAX(minDotSize, self.accuracyInPoints);
    return CGSizeMake(dotSize, dotSize);
}

- (void)updateFrameWithSize:(CGSize)size
{
    if (CGSizeEqualToSize(self.frame.size, size)) return;

    // Update frame size, keeping the existing center point.
    CGRect newFrame = self.frame;
    CGPoint oldCenter = self.center;
    newFrame.size = size;
    self.frame = newFrame;
    self.center = oldCenter;
}

- (CGFloat)accuracyInPoints
{
    CGFloat metersPerPoint = [self.mapView metersPerPointAtLatitude:self.userLocation.location.coordinate.latitude];
    return self.userLocation.location.horizontalAccuracy / metersPerPoint;
}

- (void)drawRect:(CGRect)rect
{
    (self.mapView.userTrackingMode == MGLUserTrackingModeFollowWithCourse) ? [self drawCar] : [self drawDot];
}

- (void)drawDot
{
    // Accuracy
    CGFloat accuracy = self.accuracyInPoints;

    CGFloat center = self.bounds.size.width / 2.0 - accuracy / 2.0;
    UIBezierPath *accuracyPath = [UIBezierPath bezierPathWithOvalInRect:CGRectMake(center, center, accuracy, accuracy)];
    UIColor *accuracyColor = [UIColor colorWithRed:1 green:0 blue:0 alpha:.4];
    [accuracyColor setFill];
    [accuracyPath fill];

    // Dot
    center = self.bounds.size.width / 2.0 - MBXUserLocationDotSize / 2.0;
    UIBezierPath *ovalPath = [UIBezierPath bezierPathWithOvalInRect: CGRectMake(center, center, MBXUserLocationDotSize, MBXUserLocationDotSize)];
    [UIColor.greenColor setFill];
    [ovalPath fill];

    [UIColor.blackColor setStroke];
    ovalPath.lineWidth = 1;
    [ovalPath stroke];

    // Accuracy text
    UIFont *font = [UIFont systemFontOfSize:11];
    [[NSString stringWithFormat:@"%.0f", accuracy]
     drawAtPoint:CGPointZero withAttributes:@{NSFontAttributeName: font,
                                              NSBackgroundColorAttributeName: [UIColor colorWithWhite:0 alpha:.5],
                                              NSForegroundColorAttributeName: [UIColor whiteColor]}];
}

- (void)drawCar
{
    UIColor* fillColor = [UIColor colorWithRed: 0 green: 0 blue: 0 alpha: 1];
    UIColor* strokeColor = [UIColor colorWithRed: 0.592 green: 0.592 blue: 0.592 alpha: 1];
    UIColor* fillColor2 = [UIColor colorWithRed: 1 green: 1 blue: 1 alpha: 1];

    UIBezierPath* bezier2Path = [UIBezierPath bezierPath];
    [bezier2Path moveToPoint: CGPointMake(30, 7.86)];
    [bezier2Path addLineToPoint: CGPointMake(30, 52.66)];
    [bezier2Path addCurveToPoint: CGPointMake(0, 52.66) controlPoint1: CGPointMake(30, 62.05) controlPoint2: CGPointMake(0, 62.84)];
    [bezier2Path addCurveToPoint: CGPointMake(0, 7.86) controlPoint1: CGPointMake(0, 42.48) controlPoint2: CGPointMake(0, 17.89)];
    [bezier2Path addCurveToPoint: CGPointMake(30, 7.86) controlPoint1: CGPointMake(-0, -2.17) controlPoint2: CGPointMake(30, -3.05)];
    [bezier2Path closePath];
    bezier2Path.usesEvenOddFillRule = YES;

    [fillColor setFill];
    [bezier2Path fill];

    UIBezierPath* bezier3Path = [UIBezierPath bezierPath];
    [bezier3Path moveToPoint: CGPointMake(30, 7.86)];
    [bezier3Path addLineToPoint: CGPointMake(30, 52.66)];
    [bezier3Path addCurveToPoint: CGPointMake(0, 52.66) controlPoint1: CGPointMake(30, 62.05) controlPoint2: CGPointMake(0, 62.84)];
    [bezier3Path addCurveToPoint: CGPointMake(0, 7.86) controlPoint1: CGPointMake(0, 42.48) controlPoint2: CGPointMake(0, 17.89)];
    [bezier3Path addCurveToPoint: CGPointMake(30, 7.86) controlPoint1: CGPointMake(0, -2.17) controlPoint2: CGPointMake(30, -3.05)];
    [bezier3Path closePath];
    [strokeColor setStroke];
    bezier3Path.lineWidth = 1;
    [bezier3Path stroke];

    UIBezierPath* bezier4Path = [UIBezierPath bezierPath];
    [bezier4Path moveToPoint: CGPointMake(15.56, 4.26)];
    [bezier4Path addCurveToPoint: CGPointMake(26, 6) controlPoint1: CGPointMake(21, 4.26) controlPoint2: CGPointMake(26, 6)];
    [bezier4Path addCurveToPoint: CGPointMake(23, 21) controlPoint1: CGPointMake(26, 6) controlPoint2: CGPointMake(29, 17)];
    [bezier4Path addCurveToPoint: CGPointMake(16, 21) controlPoint1: CGPointMake(20.03, 22.98) controlPoint2: CGPointMake(16, 21)];
    [bezier4Path addCurveToPoint: CGPointMake(7, 21) controlPoint1: CGPointMake(16, 21) controlPoint2: CGPointMake(9.02, 23.53)];
    [bezier4Path addCurveToPoint: CGPointMake(4, 6) controlPoint1: CGPointMake(3, 16) controlPoint2: CGPointMake(4, 6)];
    [bezier4Path addCurveToPoint: CGPointMake(15.56, 4.26) controlPoint1: CGPointMake(4, 6) controlPoint2: CGPointMake(10.12, 4.26)];
    [bezier4Path closePath];
    bezier4Path.usesEvenOddFillRule = YES;

    [fillColor2 setFill];
    [bezier4Path fill];

    UIBezierPath* rectanglePath = [UIBezierPath bezierPath];
    [rectanglePath moveToPoint: CGPointMake(25, 46)];
    [rectanglePath addCurveToPoint: CGPointMake(21, 55) controlPoint1: CGPointMake(31, 46) controlPoint2: CGPointMake(28.5, 55)];
    [rectanglePath addCurveToPoint: CGPointMake(9, 55) controlPoint1: CGPointMake(13.5, 55) controlPoint2: CGPointMake(14, 55)];
    [rectanglePath addCurveToPoint: CGPointMake(5, 46) controlPoint1: CGPointMake(4, 55) controlPoint2: CGPointMake(0, 46)];
    [rectanglePath addCurveToPoint: CGPointMake(25, 46) controlPoint1: CGPointMake(10, 46) controlPoint2: CGPointMake(19, 46)];
    [rectanglePath closePath];
    [UIColor.whiteColor setFill];
    [rectanglePath fill];

    UIBezierPath* bezierPath = [UIBezierPath bezierPath];
    [UIColor.whiteColor setFill];
    [bezierPath fill];

    UIBezierPath* rectangle2Path = [UIBezierPath bezierPath];
    [rectangle2Path moveToPoint: CGPointMake(2, 35)];
    [rectangle2Path addCurveToPoint: CGPointMake(4.36, 35) controlPoint1: CGPointMake(2, 39) controlPoint2: CGPointMake(4.36, 35)];
    [rectangle2Path addCurveToPoint: CGPointMake(4.36, 22) controlPoint1: CGPointMake(4.36, 35) controlPoint2: CGPointMake(5.55, 26)];
    [rectangle2Path addCurveToPoint: CGPointMake(2, 22) controlPoint1: CGPointMake(3.18, 18) controlPoint2: CGPointMake(2, 22)];
    [rectangle2Path addCurveToPoint: CGPointMake(2, 35) controlPoint1: CGPointMake(2, 22) controlPoint2: CGPointMake(2, 31)];
    [rectangle2Path closePath];
    [UIColor.whiteColor setFill];
    [rectangle2Path fill];

    UIBezierPath* rectangle3Path = [UIBezierPath bezierPath];
    [rectangle3Path moveToPoint: CGPointMake(28, 35)];
    [rectangle3Path addCurveToPoint: CGPointMake(25.64, 35) controlPoint1: CGPointMake(28, 39) controlPoint2: CGPointMake(25.64, 35)];
    [rectangle3Path addCurveToPoint: CGPointMake(25.64, 22) controlPoint1: CGPointMake(25.64, 35) controlPoint2: CGPointMake(24.45, 26)];
    [rectangle3Path addCurveToPoint: CGPointMake(28, 22) controlPoint1: CGPointMake(26.82, 18) controlPoint2: CGPointMake(28, 22)];
    [rectangle3Path addCurveToPoint: CGPointMake(28, 35) controlPoint1: CGPointMake(28, 22) controlPoint2: CGPointMake(28, 31)];
    [rectangle3Path closePath];
    [UIColor.whiteColor setFill];
    [rectangle3Path fill];
}

@end
