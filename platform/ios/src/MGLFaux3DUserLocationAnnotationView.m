#import "MGLFaux3DUserLocationAnnotationView.h"

#import "MGLMapView.h"
#import "MGLUserLocation.h"

const CGFloat MGLUserLocationAnnotationDotSize = 22.0;
const CGFloat MGLUserLocationAnnotationHaloSize = 115.0;

const CGFloat MGLUserLocationAnnotationPuckSize = 45.0;
const CGFloat MGLUserLocationAnnotationArrowSize = MGLUserLocationAnnotationPuckSize * 0.6;

#pragma mark -

@implementation MGLFaux3DUserLocationAnnotationView
{
    BOOL _puckModeActivated;

    CALayer *_puckDot;
    CAShapeLayer *_puckArrow;

    CALayer *_headingIndicatorLayer;
    CAShapeLayer *_headingIndicatorMaskLayer;
    CALayer *_accuracyRingLayer;
    CALayer *_dotBorderLayer;
    CALayer *_dotLayer;
    CALayer *_haloLayer;

    double _oldHeadingAccuracy;
    CLLocationAccuracy _oldHorizontalAccuracy;
    double _oldZoom;
    double _oldPitch;
}

- (CALayer *)hitTestLayer
{
    // Only the main dot should be interactive (i.e., exclude the accuracy ring and halo).
    return _dotBorderLayer ?: _puckDot;
}

- (void)update
{
    if (CGSizeEqualToSize(self.frame.size, CGSizeZero))
    {
        CGFloat frameSize = (self.mapView.userTrackingMode == MGLUserTrackingModeFollowWithCourse) ? MGLUserLocationAnnotationPuckSize : MGLUserLocationAnnotationDotSize;
        [self updateFrameWithSize:frameSize];
    }

    if (CLLocationCoordinate2DIsValid(self.userLocation.coordinate))
    {
        (self.mapView.userTrackingMode == MGLUserTrackingModeFollowWithCourse) ? [self drawPuck] : [self drawDot];
        [self updatePitch];
    }

    _haloLayer.hidden = ! CLLocationCoordinate2DIsValid(self.mapView.userLocation.coordinate) || self.mapView.userLocation.location.horizontalAccuracy > 10;
}

- (void)setTintColor:(UIColor *)tintColor
{
    if (_puckModeActivated)
    {
        _puckArrow.fillColor = [tintColor CGColor];
    }
    else
    {
        if (_accuracyRingLayer)
        {
            _accuracyRingLayer.backgroundColor = [tintColor CGColor];
        }

        _haloLayer.backgroundColor = [tintColor CGColor];
        _dotLayer.backgroundColor = [tintColor CGColor];

        _headingIndicatorLayer.contents = (__bridge id)[[self headingIndicatorTintedGradientImage] CGImage];
    }
}

- (void)updatePitch
{
    if (self.mapView.camera.pitch != _oldPitch)
    {
        // disable implicit animation
        [CATransaction begin];
        [CATransaction setValue:(id)kCFBooleanTrue forKey:kCATransactionDisableActions];

        CATransform3D t = CATransform3DRotate(CATransform3DIdentity, MGLRadiansFromDegrees(self.mapView.camera.pitch), 1.0, 0, 0);
        self.layer.sublayerTransform = t;

        [self updateFaux3DEffect];

        [CATransaction commit];

        _oldPitch = self.mapView.camera.pitch;
    }
}

- (void)updateFaux3DEffect
{
    CGFloat pitch = MGLRadiansFromDegrees(self.mapView.camera.pitch);

    if (_puckDot)
    {
        _puckDot.shadowOffset = CGSizeMake(0, fmaxf(pitch * 10.f, 1.f));
        _puckDot.shadowRadius = fmaxf(pitch * 5.f, 0.75f);
    }

    if (_dotBorderLayer)
    {
        _dotBorderLayer.shadowOffset = CGSizeMake(0.f, pitch * 10.f);
        _dotBorderLayer.shadowRadius = fmaxf(pitch * 5.f, 3.f);
    }

    if (_dotLayer)
    {
        _dotLayer.zPosition = pitch * 2.f;
    }
}

- (void)updateFrameWithSize:(CGFloat)size
{
    CGSize newSize = CGSizeMake(size, size);
    if (CGSizeEqualToSize(self.frame.size, newSize))
    {
        return;
    }

    // Update frame size, keeping the existing center point.
    CGPoint oldCenter = self.center;
    CGRect newFrame = self.frame;
    newFrame.size = newSize;
    [self setFrame:newFrame];
    [self setCenter:oldCenter];
}

- (void)drawPuck
{
    if ( ! _puckModeActivated)
    {
        self.layer.sublayers = nil;

        _headingIndicatorLayer = nil;
        _headingIndicatorMaskLayer = nil;
        _accuracyRingLayer = nil;
        _haloLayer = nil;
        _dotBorderLayer = nil;
        _dotLayer = nil;

        [self updateFrameWithSize:MGLUserLocationAnnotationPuckSize];
    }

    // background dot (white with black shadow)
    //
    if ( ! _puckDot)
    {
        _puckDot = [self circleLayerWithSize:MGLUserLocationAnnotationPuckSize];
        _puckDot.backgroundColor = [[UIColor whiteColor] CGColor];
        _puckDot.shadowColor = [[UIColor blackColor] CGColor];
        _puckDot.shadowOpacity = 0.25;
        _puckDot.shadowPath = [[UIBezierPath bezierPathWithOvalInRect:_puckDot.bounds] CGPath];

        if (self.mapView.camera.pitch)
        {
            [self updateFaux3DEffect];
        }
        else
        {
            _puckDot.shadowOffset = CGSizeMake(0, 1);
            _puckDot.shadowRadius = 0.75;
        }

        [self.layer addSublayer:_puckDot];
    }

    // arrow
    //
    if ( ! _puckArrow)
    {
        _puckArrow = [CAShapeLayer layer];
        _puckArrow.path = [[self puckArrow] CGPath];
        _puckArrow.fillColor = [self.mapView.tintColor CGColor];
        _puckArrow.bounds = CGRectMake(0, 0, MGLUserLocationAnnotationArrowSize, MGLUserLocationAnnotationArrowSize);
        _puckArrow.position = CGPointMake(super.bounds.size.width / 2.0, super.bounds.size.height / 2.0);
        _puckArrow.shouldRasterize = YES;
        _puckArrow.rasterizationScale = [UIScreen mainScreen].scale;
        _puckArrow.drawsAsynchronously = YES;

        [self.layer addSublayer:_puckArrow];
    }
    if (self.userLocation.location.course >= 0)
    {
        _puckArrow.affineTransform = CGAffineTransformRotate(CGAffineTransformIdentity, -MGLRadiansFromDegrees(self.mapView.direction - self.userLocation.location.course));
    }

    if ( ! _puckModeActivated)
    {
        _puckModeActivated = YES;

        [self updateFaux3DEffect];
    }
}

- (UIBezierPath *)puckArrow
{
    CGFloat max = MGLUserLocationAnnotationArrowSize;

    UIBezierPath *bezierPath = UIBezierPath.bezierPath;
    [bezierPath moveToPoint:    CGPointMake(max * 0.5, 0)];
    [bezierPath addLineToPoint: CGPointMake(max * 0.1, max)];
    [bezierPath addLineToPoint: CGPointMake(max * 0.5, max * 0.65)];
    [bezierPath addLineToPoint: CGPointMake(max * 0.9, max)];
    [bezierPath addLineToPoint: CGPointMake(max * 0.5, 0)];
    [bezierPath closePath];

    return bezierPath;
}

- (void)drawDot
{
    if (_puckModeActivated)
    {
        self.layer.sublayers = nil;

        _puckDot = nil;
        _puckArrow = nil;

        [self updateFrameWithSize:MGLUserLocationAnnotationDotSize];
    }

    BOOL showHeadingIndicator = self.mapView.userTrackingMode == MGLUserTrackingModeFollowWithHeading;

    // update heading indicator
    //
    if (showHeadingIndicator)
    {
        _headingIndicatorLayer.hidden = NO;

        // heading indicator (tinted, semi-circle)
        //
        if ( ! _headingIndicatorLayer && self.userLocation.heading.headingAccuracy)
        {
            CGFloat headingIndicatorSize = MGLUserLocationAnnotationHaloSize;

            _headingIndicatorLayer = [CALayer layer];
            _headingIndicatorLayer.bounds = CGRectMake(0, 0, headingIndicatorSize, headingIndicatorSize);
            _headingIndicatorLayer.position = CGPointMake(super.bounds.size.width / 2.0, super.bounds.size.height / 2.0);
            _headingIndicatorLayer.contents = (__bridge id)[[self headingIndicatorTintedGradientImage] CGImage];
            _headingIndicatorLayer.contentsGravity = kCAGravityBottom;
            _headingIndicatorLayer.contentsScale = [UIScreen mainScreen].scale;
            _headingIndicatorLayer.opacity = 0.4;
            _headingIndicatorLayer.shouldRasterize = YES;
            _headingIndicatorLayer.rasterizationScale = [UIScreen mainScreen].scale;
            _headingIndicatorLayer.drawsAsynchronously = YES;

            [self.layer insertSublayer:_headingIndicatorLayer below:_dotBorderLayer];
        }

        // heading indicator accuracy mask (fan-shaped)
        //
        if ( ! _headingIndicatorMaskLayer && self.userLocation.heading.headingAccuracy)
        {
            _headingIndicatorMaskLayer = [CAShapeLayer layer];
            _headingIndicatorMaskLayer.frame = _headingIndicatorLayer.bounds;
            _headingIndicatorMaskLayer.path = [[self headingIndicatorClippingMask] CGPath];

            // apply the mask to the halo-radius-sized gradient layer
            _headingIndicatorLayer.mask = _headingIndicatorMaskLayer;

            _oldHeadingAccuracy = self.userLocation.heading.headingAccuracy;

        }
        else if (_oldHeadingAccuracy != self.userLocation.heading.headingAccuracy)
        {
            // recalculate the clipping mask based on updated accuracy
            _headingIndicatorMaskLayer.path = [[self headingIndicatorClippingMask] CGPath];

            _oldHeadingAccuracy = self.userLocation.heading.headingAccuracy;
        }

        if (self.userLocation.heading.trueHeading >= 0)
        {
            _headingIndicatorLayer.affineTransform = CGAffineTransformRotate(CGAffineTransformIdentity, -MGLRadiansFromDegrees(self.mapView.direction - self.userLocation.heading.trueHeading));
        }
    }
    else
    {
        [_headingIndicatorLayer removeFromSuperlayer];
        [_headingIndicatorMaskLayer removeFromSuperlayer];
        _headingIndicatorLayer = nil;
        _headingIndicatorMaskLayer = nil;
    }


    // update accuracy ring (if zoom or horizontal accuracy have changed)
    //
    if (_accuracyRingLayer && (_oldZoom != self.mapView.zoomLevel || _oldHorizontalAccuracy != self.userLocation.location.horizontalAccuracy))
    {
        CGFloat accuracyRingSize = [self calculateAccuracyRingSize];

        // only show the accuracy ring if it won't be obscured by the location dot
        if (accuracyRingSize > MGLUserLocationAnnotationDotSize + 15)
        {
            _accuracyRingLayer.hidden = NO;

            // disable implicit animation of the accuracy ring, unless triggered by a change in accuracy
            id shouldDisableActions = (_oldHorizontalAccuracy == self.userLocation.location.horizontalAccuracy) ? (id)kCFBooleanTrue : (id)kCFBooleanFalse;

            [CATransaction begin];
            [CATransaction setValue:shouldDisableActions forKey:kCATransactionDisableActions];

            _accuracyRingLayer.bounds = CGRectMake(0, 0, accuracyRingSize, accuracyRingSize);
            _accuracyRingLayer.cornerRadius = accuracyRingSize / 2;

            // match the halo to the accuracy ring
            _haloLayer.bounds = _accuracyRingLayer.bounds;
            _haloLayer.cornerRadius = _accuracyRingLayer.cornerRadius;
            _haloLayer.shouldRasterize = NO;

            [CATransaction commit];
        }
        else
        {
            _accuracyRingLayer.hidden = YES;

            _haloLayer.bounds = CGRectMake(0, 0, MGLUserLocationAnnotationHaloSize, MGLUserLocationAnnotationHaloSize);
            _haloLayer.cornerRadius = MGLUserLocationAnnotationHaloSize / 2.0;
            _haloLayer.shouldRasterize = YES;
            _haloLayer.rasterizationScale = [UIScreen mainScreen].scale;
        }

        // store accuracy and zoom so we're not redrawing unchanged location updates
        _oldHorizontalAccuracy = self.userLocation.location.horizontalAccuracy;
        _oldZoom = self.mapView.zoomLevel;
    }

    // accuracy ring (circular, tinted, mostly-transparent)
    //
    if ( ! _accuracyRingLayer && self.userLocation.location.horizontalAccuracy)
    {
        CGFloat accuracyRingSize = [self calculateAccuracyRingSize];
        _accuracyRingLayer = [self circleLayerWithSize:accuracyRingSize];
        _accuracyRingLayer.backgroundColor = [self.mapView.tintColor CGColor];
        _accuracyRingLayer.opacity = 0.1;
        _accuracyRingLayer.shouldRasterize = NO;
        _accuracyRingLayer.allowsGroupOpacity = NO;

        [self.layer addSublayer:_accuracyRingLayer];
    }

    // expanding sonar-like pulse (circular, tinted, fades out)
    //
    if ( ! _haloLayer)
    {
        _haloLayer = [self circleLayerWithSize:MGLUserLocationAnnotationHaloSize];
        _haloLayer.backgroundColor = [self.mapView.tintColor CGColor];
        _haloLayer.allowsGroupOpacity = NO;
        _haloLayer.zPosition = -0.1f;

        // set defaults for the animations
        CAAnimationGroup *animationGroup = [self loopingAnimationGroupWithDuration:3.0];

        // scale out radially with initial acceleration
        CAKeyframeAnimation *boundsAnimation = [CAKeyframeAnimation animationWithKeyPath:@"transform.scale.xy"];
        boundsAnimation.values = @[@0, @0.35, @1];
        boundsAnimation.keyTimes = @[@0, @0.2, @1];

        // go transparent as scaled out, start semi-opaque
        CAKeyframeAnimation *opacityAnimation = [CAKeyframeAnimation animationWithKeyPath:@"opacity"];
        opacityAnimation.values = @[@0.4, @0.4, @0];
        opacityAnimation.keyTimes = @[@0, @0.2, @1];

        animationGroup.animations = @[boundsAnimation, opacityAnimation];

        [_haloLayer addAnimation:animationGroup forKey:@"animateTransformAndOpacity"];

        [self.layer addSublayer:_haloLayer];
    }

    // background dot (white with black shadow)
    //
    if ( ! _dotBorderLayer)
    {
        _dotBorderLayer = [self circleLayerWithSize:MGLUserLocationAnnotationDotSize];
        _dotBorderLayer.backgroundColor = [[UIColor whiteColor] CGColor];
        _dotBorderLayer.shadowColor = [[UIColor blackColor] CGColor];
        _dotBorderLayer.shadowOpacity = 0.25;
        _dotBorderLayer.shadowPath = [[UIBezierPath bezierPathWithOvalInRect:_dotBorderLayer.bounds] CGPath];

        if (self.mapView.camera.pitch)
        {
            [self updateFaux3DEffect];
        }
        else
        {
            _dotBorderLayer.shadowOffset = CGSizeMake(0, 0);
            _dotBorderLayer.shadowRadius = 3;
        }

        [self.layer addSublayer:_dotBorderLayer];
    }

    // inner dot (pulsing, tinted)
    //
    if ( ! _dotLayer)
    {
        _dotLayer = [self circleLayerWithSize:MGLUserLocationAnnotationDotSize * 0.75];
        _dotLayer.backgroundColor = [self.mapView.tintColor CGColor];

        // set defaults for the animations
        CAAnimationGroup *animationGroup = [self loopingAnimationGroupWithDuration:1.5];
        animationGroup.autoreverses = YES;
        animationGroup.fillMode = kCAFillModeBoth;

        // scale the dot up and down
        CABasicAnimation *pulseAnimation = [CABasicAnimation animationWithKeyPath:@"transform.scale.xy"];
        pulseAnimation.fromValue = @0.8;
        pulseAnimation.toValue = @1;

        // fade opacity in and out, subtly
        CABasicAnimation *opacityAnimation = [CABasicAnimation animationWithKeyPath:@"opacity"];
        opacityAnimation.fromValue = @0.8;
        opacityAnimation.toValue = @1;

        animationGroup.animations = @[pulseAnimation, opacityAnimation];

        [_dotLayer addAnimation:animationGroup forKey:@"animateTransformAndOpacity"];

        [self.layer addSublayer:_dotLayer];
    }

    if (_puckModeActivated)
    {
        _puckModeActivated = NO;

        [self updateFaux3DEffect];
    }
}

- (CALayer *)circleLayerWithSize:(CGFloat)layerSize
{
    CALayer *circleLayer = [CALayer layer];
    circleLayer.bounds = CGRectMake(0, 0, layerSize, layerSize);
    circleLayer.position = CGPointMake(super.bounds.size.width / 2.0, super.bounds.size.height / 2.0);
    circleLayer.cornerRadius = layerSize / 2.0;
    circleLayer.shouldRasterize = YES;
    circleLayer.rasterizationScale = [UIScreen mainScreen].scale;
    circleLayer.drawsAsynchronously = YES;

    return circleLayer;
}

- (CAAnimationGroup *)loopingAnimationGroupWithDuration:(CGFloat)animationDuration
{
    CAAnimationGroup *animationGroup = [CAAnimationGroup animation];
    animationGroup.duration = animationDuration;
    animationGroup.repeatCount = INFINITY;
    animationGroup.removedOnCompletion = NO;
    animationGroup.timingFunction = [CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionDefault];

    return animationGroup;
}

- (CGFloat)calculateAccuracyRingSize
{
    // diameter in screen points
    return self.userLocation.location.horizontalAccuracy / [self.mapView metersPerPointAtLatitude:self.userLocation.coordinate.latitude] * 2.0;
}

- (UIImage *)headingIndicatorTintedGradientImage
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
                                (__bridge CFArrayRef)@[
                                    (id)[self.mapView.tintColor CGColor],
                                    (id)[[self.mapView.tintColor colorWithAlphaComponent:0] CGColor]],
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

    return image;
}

- (UIBezierPath *)headingIndicatorClippingMask
{
    CGFloat accuracy = self.userLocation.heading.headingAccuracy;

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

    return ovalPath;
}

@end
