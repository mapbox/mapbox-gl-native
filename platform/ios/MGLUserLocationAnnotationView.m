#import "MGLUserLocationAnnotationView.h"

#import "MGLUserLocation.h"
#import "MGLUserLocation_Private.h"
#import "MGLAnnotation.h"
#import "MGLMapView.h"

const CGFloat MGLUserLocationAnnotationDotSize = 24.0;
const CGFloat MGLUserLocationAnnotationHaloSize = 115.0;

@interface MGLUserLocationAnnotationView ()

@property (nonatomic, readwrite) CALayer *haloLayer;

@end

#pragma mark -

@implementation MGLUserLocationAnnotationView
{
    CALayer *_accuracyRingLayer;
    CALayer *_headingIndicatorLayer;
    CALayer *_dotBorderLayer;
    CALayer *_dotLayer;
    
    double oldHeadingAccuracy;
    CLLocationAccuracy oldHorizontalAccuracy;
    double oldZoom;
}

- (instancetype)initWithFrame:(CGRect)frame
{
    NSAssert(NO, @"No containing map view specified. Call -initInMapView: instead.");
    return self = [self init];
}

- (instancetype)initInMapView:(MGLMapView *)mapView
{
    if (self = [super initWithFrame:CGRectMake(0, 0, MGLTrackingDotRingWidth, MGLTrackingDotRingWidth)])
    {
        self.annotation = [[MGLUserLocation alloc] initWithMapView:mapView];
        _mapView = mapView;
        [self setupLayers];
        self.accessibilityLabel = @"User location";
    }
    return self;
}

- (instancetype)initWithCoder:(NSCoder *)decoder
{
    MGLMapView *mapView = [decoder valueForKey:@"mapView"];
    return [self initInMapView:mapView];
}

- (void)setTintColor:(UIColor *)tintColor
{
    if (_accuracyRingLayer)
    {
        _accuracyRingLayer.backgroundColor = [tintColor CGColor];
    }
    
    _haloLayer.backgroundColor = [tintColor CGColor];
    _dotLayer.backgroundColor = [tintColor CGColor];
}

- (void)setupLayers
{
    if (CLLocationCoordinate2DIsValid(self.annotation.coordinate))
    {
        if (_headingIndicatorLayer)
        {
            // TODO: Investigate alternative ways to determine this
            _headingIndicatorLayer.hidden = (_mapView.userTrackingMode == MGLUserTrackingModeFollowWithHeading) ? NO : YES;
            
            if (oldHeadingAccuracy != self.annotation.heading.headingAccuracy)
            {
                //_headingIndicatorLayer.contents = (id)[self headingIndicatorImageForAccuracy].CGImage;
                oldHeadingAccuracy = self.annotation.heading.headingAccuracy;
            }
        }
        
        // tinted heading indicator
        //
        if ( ! _headingIndicatorLayer && self.annotation.heading)
        {
            CGFloat headingIndicatorSize = MGLUserLocationAnnotationHaloSize;
            
            _headingIndicatorLayer = [CALayer layer];
            _headingIndicatorLayer.bounds = CGRectMake(0, 0, headingIndicatorSize, headingIndicatorSize);
            _headingIndicatorLayer.position = CGPointMake(super.bounds.size.width / 2.0, super.bounds.size.height / 2.0);
            //_headingIndicatorLayer.contents = (id)[self headingIndicatorImageForAccuracy].CGImage;
            _headingIndicatorLayer.contents = (id)[self headingIndicatorImage].CGImage;
            _headingIndicatorLayer.contentsGravity = kCAGravityBottom;
            _headingIndicatorLayer.contentsScale = [UIScreen mainScreen].scale;
            _headingIndicatorLayer.opacity = 0.4;
            
            _headingIndicatorLayer.shouldRasterize = YES;
            _headingIndicatorLayer.rasterizationScale = [UIScreen mainScreen].scale;
            _headingIndicatorLayer.drawsAsynchronously = YES;
            
            [self.layer insertSublayer:_headingIndicatorLayer below:_dotBorderLayer];
        }
        
        // update accuracy ring
        //
        if (_accuracyRingLayer)
        {
            // FIX: This stops EVERYTHING... and that isn't necessarily necessary, now is it?
            if (oldZoom == self.mapView.zoomLevel && oldHorizontalAccuracy == self.annotation.location.horizontalAccuracy)
                return;
            
            CGFloat accuracyRingSize = [self calculateAccuracyRingSize];
            
            // only show the accuracy ring if it won't be obscured by the location dot
            //
            if (accuracyRingSize > MGLUserLocationAnnotationDotSize + 15)
            {
                _accuracyRingLayer.hidden = NO;
                _accuracyRingLayer.bounds = CGRectMake(0, 0, accuracyRingSize, accuracyRingSize);
                _accuracyRingLayer.cornerRadius = accuracyRingSize / 2;
                
                // match the halo to the accuracy ring
                //
                _haloLayer.bounds = _accuracyRingLayer.bounds;
                _haloLayer.cornerRadius = _accuracyRingLayer.cornerRadius;
                _haloLayer.shouldRasterize = NO;
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
            //
            oldHorizontalAccuracy = self.annotation.location.horizontalAccuracy;
            oldZoom = self.mapView.zoomLevel;
        }
        
        // tinted, mostly-transparent accuracy ring
        //
        if ( ! _accuracyRingLayer && self.annotation.location.horizontalAccuracy)
        {
            CGFloat accuracyRingSize = [self calculateAccuracyRingSize];
            _accuracyRingLayer = [self circleLayerWithSize:accuracyRingSize];
            _accuracyRingLayer.backgroundColor = [_mapView.tintColor CGColor];
            _accuracyRingLayer.opacity = 0.1;
            _accuracyRingLayer.shouldRasterize = NO;
            _accuracyRingLayer.allowsGroupOpacity = NO;
            
            [self.layer addSublayer:_accuracyRingLayer];
        }
        
        // expanding, fading, tinted outer layer
        //
        if ( ! _haloLayer)
        {
            _haloLayer = [self circleLayerWithSize:MGLUserLocationAnnotationHaloSize];
            _haloLayer.backgroundColor = [_mapView.tintColor CGColor];
            _haloLayer.allowsGroupOpacity = NO;
            
            // set defaults for the animations
            //
            CAAnimationGroup *animationGroup = [self animationGroupWithDuration:3.0];
            
            // scale out radially with initial acceleration
            //
            CAKeyframeAnimation *boundsAnimation = [CAKeyframeAnimation animationWithKeyPath:@"transform.scale.xy"];
            boundsAnimation.values = @[@0, @0.35, @1];
            boundsAnimation.keyTimes = @[@0, @0.2, @1];

            // go transparent as scaled out, start semi-opaque
            //
            CAKeyframeAnimation *opacityAnimation = [CAKeyframeAnimation animationWithKeyPath:@"opacity"];
            opacityAnimation.values = @[@0.4, @0.4, @0];
            opacityAnimation.keyTimes = @[@0, @0.2, @1];
            
            animationGroup.animations = @[boundsAnimation, opacityAnimation];
            
            [_haloLayer addAnimation:animationGroup forKey:@"animateTransformAndOpacity"];
            
            [self.layer addSublayer:_haloLayer];
        }
        
        // white dot background with shadow
        //
        if ( ! _dotBorderLayer)
        {
            _dotBorderLayer = [self circleLayerWithSize:MGLUserLocationAnnotationDotSize];
            _dotBorderLayer.backgroundColor = [[UIColor whiteColor] CGColor];
            _dotBorderLayer.shadowColor = [[UIColor blackColor] CGColor];
            _dotBorderLayer.shadowOffset = CGSizeMake(0, 0);
            _dotBorderLayer.shadowRadius = 3;
            _dotBorderLayer.shadowOpacity = 0.25;
            
            [self.layer addSublayer:_dotBorderLayer];
        }

        // pulsing, tinted inner dot sublayer
        //
        if ( ! _dotLayer)
        {
            _dotLayer = [self circleLayerWithSize:MGLUserLocationAnnotationDotSize * 0.75];
            _dotLayer.backgroundColor = [_mapView.tintColor CGColor];
            
            // set defaults for the animations
            //
            CAAnimationGroup *animationGroup = [self animationGroupWithDuration:1.5];
            animationGroup.autoreverses = YES;
            animationGroup.fillMode = kCAFillModeBoth;
            
            // scale the dot up and down
            //
            CABasicAnimation *pulseAnimation = [CABasicAnimation animationWithKeyPath:@"transform.scale.xy"];
            pulseAnimation.fromValue = @0.8;
            pulseAnimation.toValue = @1;
            
            // fade opacity in and out, subtly
            //
            CABasicAnimation *opacityAnimation = [CABasicAnimation animationWithKeyPath:@"opacity"];
            opacityAnimation.fromValue = @0.8;
            opacityAnimation.toValue = @1;
            
            animationGroup.animations = @[pulseAnimation, opacityAnimation];
            
            [_dotLayer addAnimation:animationGroup forKey:@"animateTransformAndOpacity"];
            
            [self.layer addSublayer:_dotLayer];
        }
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

- (CAAnimationGroup *)animationGroupWithDuration:(CGFloat)animationDuration
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
    CGFloat latRadians = self.annotation.coordinate.latitude * M_PI / 180.0f;
    CGFloat pixelRadius = self.annotation.location.horizontalAccuracy / cos(latRadians) / [self.mapView metersPerPixelAtLatitude:self.annotation.coordinate.latitude];
    
    return pixelRadius * 2;
}

/*- (UIImage *)headingIndicatorImageForAccuracy
{
    NSString *fileName = @"HeadingAngleMask";
    
    double accuracy = self.annotation.heading.headingAccuracy;
    
    if (accuracy <=  10)
    {
        fileName = [fileName stringByAppendingString:@"Small"];
    }
    else if (accuracy <= 20)
    {
        fileName = [fileName stringByAppendingString:@"Medium"];
    }
    else
    {
        fileName = [fileName stringByAppendingString:@"Large"];
    }
    
    NSLog(@"headingIndicatorImageForAccuracy: %f %@", accuracy, fileName);
    
    NSString *resourceBundlePath = [[NSBundle bundleForClass:[MGLMapView class]] pathForResource:@"MapboxGL" ofType:@"bundle"];
    if ( ! resourceBundlePath) resourceBundlePath = [[NSBundle mainBundle] bundlePath];

    NSString *path = [[NSBundle bundleWithPath:resourceBundlePath] pathForResource:fileName ofType:@"png" inDirectory:@""];
        
    NSAssert(path, @"%@ not found in application", fileName);
        
    return [UIImage imageWithContentsOfFile:path];
}*/

- (UIImage *)headingIndicatorImage
{
    UIImage *image;

    CGFloat halfHalo = MGLUserLocationAnnotationHaloSize / 2.0;
    
    UIGraphicsBeginImageContextWithOptions(CGSizeMake(MGLUserLocationAnnotationHaloSize, halfHalo), NO, 0);
    
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    CGContextRef context = UIGraphicsGetCurrentContext();
    
    // TODO: Hook this up to our actual accuracy
    UIBezierPath *ovalPath = [self headingIndicatorClippingMaskForAccuracy:10.0];
    
    // gradient from the tint color to no-alpha tint color
    //
    CGFloat gradientLocations[] = {0.0, 1.0};
    CGGradientRef gradient = CGGradientCreateWithColors(
      colorSpace, (__bridge CFArrayRef)@[(id)[_mapView.tintColor CGColor],
                                         (id)[[_mapView.tintColor colorWithAlphaComponent:0] CGColor]], gradientLocations);
    
    CGContextDrawRadialGradient(context, gradient,
                                CGPointMake(halfHalo, halfHalo), 0.0,
                                CGPointMake(halfHalo, halfHalo), halfHalo,
                                kCGGradientDrawsBeforeStartLocation | kCGGradientDrawsAfterEndLocation);

    // export and cleanup
    //
    image = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();

    CGGradientRelease(gradient);
    CGColorSpaceRelease(colorSpace);
    
    return image;
}

- (UIBezierPath *)headingIndicatorClippingMaskForAccuracy:(CGFloat)accuracy
{
    CGFloat clippingDegrees = 60.0 - accuracy;
    
    CGRect ovalRect = CGRectMake(0, 0, MGLUserLocationAnnotationHaloSize, MGLUserLocationAnnotationHaloSize);
    UIBezierPath *ovalPath = UIBezierPath.bezierPath;
    
    // clip the oval to ± incoming accuracy degrees, from the top
    //
    [ovalPath addArcWithCenter:CGPointMake(CGRectGetMidX(ovalRect), CGRectGetMidY(ovalRect))
                        radius:CGRectGetWidth(ovalRect) / 2.0
                    startAngle:(-180 + clippingDegrees) * M_PI / 180
                      endAngle:-clippingDegrees * M_PI / 180
                     clockwise:YES];
    [ovalPath addLineToPoint:CGPointMake(CGRectGetMidX(ovalRect), CGRectGetMidY(ovalRect))];
    [ovalPath closePath];
    
    [ovalPath addClip];
    
    return ovalPath;
}

@end
