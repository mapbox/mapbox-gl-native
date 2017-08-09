#import "MGLUserLocationHeadingBeamLayer.h"

#import "MGLFaux3DUserLocationAnnotationView.h"
#import "MGLGeometry.h"

@implementation MGLUserLocationHeadingBeamLayer
{
    CAShapeLayer *_maskLayer;
}

- (instancetype)initWithUserLocationAnnotationView:(MGLUserLocationAnnotationView *)userLocationView
{
    CGFloat size = MGLUserLocationAnnotationHaloSize;

    self = [super init];
    self.bounds = CGRectMake(0, 0, size, size);
    self.position = CGPointMake(CGRectGetMidX(userLocationView.bounds), CGRectGetMidY(userLocationView.bounds));
    self.contents = (__bridge id)[self gradientImageWithTintColor:userLocationView.tintColor.CGColor];
    self.contentsGravity = kCAGravityBottom;
    self.contentsScale = UIScreen.mainScreen.scale;
    self.opacity = 0.4;
    self.shouldRasterize = YES;
    self.rasterizationScale = UIScreen.mainScreen.scale;
    self.drawsAsynchronously = YES;

    _maskLayer = [CAShapeLayer layer];
    _maskLayer.frame = self.bounds;
    _maskLayer.path = [self clippingMaskForAccuracy:0];
    self.mask = _maskLayer;

    return self;
}

- (void)updateHeadingAccuracy:(CLLocationDirection)accuracy
{
    // recalculate the clipping mask based on updated accuracy
    _maskLayer.path = [self clippingMaskForAccuracy:accuracy];
}

- (void)updateTintColor:(CGColorRef)color
{
    // redraw the raw tinted gradient
    self.contents = (__bridge id)[self gradientImageWithTintColor:color];
}

- (CGImageRef)gradientImageWithTintColor:(CGColorRef)tintColor
{
    UIImage *image;

    CGFloat haloRadius = MGLUserLocationAnnotationHaloSize / 2.0;

    UIGraphicsBeginImageContextWithOptions(CGSizeMake(MGLUserLocationAnnotationHaloSize, haloRadius), NO, 0);

    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    CGContextRef context = UIGraphicsGetCurrentContext();

    // gradient from the tint color to no-alpha tint color
    CGFloat gradientLocations[] = {0.0, 1.0};
    CGGradientRef gradient = CGGradientCreateWithColors(
        colorSpace,
        (__bridge CFArrayRef)@[(__bridge id)tintColor,
                               (id)CFBridgingRelease(CGColorCreateCopyWithAlpha(tintColor, 0))],
        gradientLocations);

    // draw the gradient from the center point to the edge (full halo radius)
    CGPoint centerPoint = CGPointMake(haloRadius, haloRadius);
    CGContextDrawRadialGradient(context, gradient,
                                centerPoint, 0.0,
                                centerPoint, haloRadius,
                                kNilOptions);

    image = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();

    CGGradientRelease(gradient);
    CGColorSpaceRelease(colorSpace);

    return image.CGImage;
}

- (CGPathRef)clippingMaskForAccuracy:(CGFloat)accuracy
{
    // size the mask using accuracy, but keep within a good display range
    CGFloat clippingDegrees = 90 - accuracy;
    clippingDegrees = fmin(clippingDegrees, 70); // most accurate
    clippingDegrees = fmax(clippingDegrees, 10); // least accurate

    CGRect ovalRect = CGRectMake(0, 0, MGLUserLocationAnnotationHaloSize, MGLUserLocationAnnotationHaloSize);
    UIBezierPath *ovalPath = UIBezierPath.bezierPath;

    // clip the oval to ± incoming accuracy degrees (converted to radians), from the top
    [ovalPath addArcWithCenter:CGPointMake(CGRectGetMidX(ovalRect), CGRectGetMidY(ovalRect))
                        radius:CGRectGetWidth(ovalRect) / 2.0
                    startAngle:MGLRadiansFromDegrees(-180 + clippingDegrees)
                      endAngle:MGLRadiansFromDegrees(-clippingDegrees)
                     clockwise:YES];

    [ovalPath addLineToPoint:CGPointMake(CGRectGetMidX(ovalRect), CGRectGetMidY(ovalRect))];
    [ovalPath closePath];

    return ovalPath.CGPath;
}

@end
