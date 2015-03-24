//
//  MGLUserLocationAnnotationView.m
//  mbgl
//
// Copyright (c) 2008-2013, Route-Me Contributors
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#import "MGLUserLocationAnnotationView.h"
#import "MGLUserLocation_Private.h"
#import "MGLAnnotation.h"
#import "MGLMapView.h"

const CGFloat MGLTrackingDotRingWidth = 24.0;

@interface MGLUserLocationAnnotationView ()

@property (nonatomic, strong, readwrite) CALayer *haloLayer;

@end

#pragma mark -

@implementation MGLUserLocationAnnotationView {
    CALayer *_accuracyRingLayer;
    CALayer *_dotBorderLayer;
    CALayer *_dotLayer;
    UIColor *_tintColor;
}

- (instancetype)initInMapView:(MGLMapView *)mapView {
    if (self = [super init]) {
        self.annotation = [[MGLUserLocation alloc] init];
        _mapView = mapView;
        [self setupLayers];
    }
    return self;
}

- (void)setTintColor:(UIColor *)tintColor {
    [super setTintColor:tintColor];
    
    UIImage *tintedForeground = [self dotImage];
    _dotLayer.bounds = CGRectMake(0, 0, tintedForeground.size.width, tintedForeground.size.height);
    _dotLayer.contents = (__bridge id)[tintedForeground CGImage];
}

- (void)setupLayers {
    if (CLLocationCoordinate2DIsValid(self.annotation.coordinate)) {
        if (!_accuracyRingLayer && self.annotation.location.horizontalAccuracy) {
            UIImage *accuracyRingImage = [self accuracyRingImage];
            _accuracyRingLayer = [CALayer layer];
            _haloLayer.bounds = CGRectMake(0, 0, accuracyRingImage.size.width, accuracyRingImage.size.height);
            _haloLayer.contents = (__bridge id)[accuracyRingImage CGImage];
            _haloLayer.position = CGPointMake(super.layer.bounds.size.width / 2.0, super.layer.bounds.size.height / 2.0);
            
            [self.layer addSublayer:_accuracyRingLayer];
        }
        
        if (!_haloLayer) {
            UIImage *haloImage = [self trackingDotHaloImage];
            _haloLayer = [CALayer layer];
            _haloLayer.bounds = CGRectMake(0, 0, haloImage.size.width, haloImage.size.height);
            _haloLayer.contents = (__bridge id)[haloImage CGImage];
            _haloLayer.position = CGPointMake(super.layer.bounds.size.width / 2.0, super.layer.bounds.size.height / 2.0);
            
            [CATransaction begin];
            
            [CATransaction setAnimationDuration:3.5];
            [CATransaction setAnimationTimingFunction:[CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionEaseOut]];
            
            // scale out radially
            //
            CABasicAnimation *boundsAnimation = [CABasicAnimation animationWithKeyPath:@"transform"];
            boundsAnimation.repeatCount = MAXFLOAT;
            boundsAnimation.fromValue = [NSValue valueWithCATransform3D:CATransform3DMakeScale(0.1, 0.1, 1.0)];
            boundsAnimation.toValue   = [NSValue valueWithCATransform3D:CATransform3DMakeScale(2.0, 2.0, 1.0)];
            boundsAnimation.removedOnCompletion = NO;
            
            [_haloLayer addAnimation:boundsAnimation forKey:@"animateScale"];
            
            // go transparent as scaled out
            //
            CABasicAnimation *opacityAnimation = [CABasicAnimation animationWithKeyPath:@"opacity"];
            opacityAnimation.repeatCount = MAXFLOAT;
            opacityAnimation.fromValue = [NSNumber numberWithFloat:1.0];
            opacityAnimation.toValue   = [NSNumber numberWithFloat:-1.0];
            opacityAnimation.removedOnCompletion = NO;
            
            [_haloLayer addAnimation:opacityAnimation forKey:@"animateOpacity"];
            
            [CATransaction commit];
            
            [self.layer addSublayer:_haloLayer];
        }
        
        // white dot background with shadow
        //
        if (!_dotBorderLayer) {
            CGRect rect = CGRectMake(0, 0, MGLTrackingDotRingWidth * 1.25, MGLTrackingDotRingWidth * 1.25);
            
            UIGraphicsBeginImageContextWithOptions(rect.size, NO, [[UIScreen mainScreen] scale]);
            CGContextRef context = UIGraphicsGetCurrentContext();
            
            CGContextSetShadow(context, CGSizeMake(0, 0), MGLTrackingDotRingWidth / 4.0);
            
            CGContextSetFillColorWithColor(context, [[UIColor whiteColor] CGColor]);
            CGContextFillEllipseInRect(context, CGRectMake((rect.size.width - MGLTrackingDotRingWidth) / 2.0, (rect.size.height - MGLTrackingDotRingWidth) / 2.0, MGLTrackingDotRingWidth, MGLTrackingDotRingWidth));
            
            UIImage *whiteBackground = UIGraphicsGetImageFromCurrentImageContext();
            
            UIGraphicsEndImageContext();
            
            _dotBorderLayer = [CALayer layer];
            _dotBorderLayer.bounds = CGRectMake(0, 0, whiteBackground.size.width, whiteBackground.size.height);
            _dotBorderLayer.contents = (__bridge id)[whiteBackground CGImage];
            _dotBorderLayer.position = CGPointMake(super.layer.bounds.size.width / 2.0, super.layer.bounds.size.height / 2.0);
            [self.layer addSublayer:_dotBorderLayer];
        }

        // pulsing, tinted dot sublayer
        //
        if (!_dotLayer) {
            _dotLayer = [CALayer layer];
            _dotLayer.position = CGPointMake(super.layer.bounds.size.width / 2.0, super.layer.bounds.size.height / 2.0);
            
            self.tintColor = _mapView.tintColor;
            
            CABasicAnimation *animation = [CABasicAnimation animationWithKeyPath:@"transform"];
            animation.repeatCount = MAXFLOAT;
            animation.fromValue = [NSValue valueWithCATransform3D:CATransform3DMakeScale(1.0, 1.0, 1.0)];
            animation.toValue   = [NSValue valueWithCATransform3D:CATransform3DMakeScale(0.8, 0.8, 1.0)];
            animation.removedOnCompletion = NO;
            animation.autoreverses = YES;
            animation.duration = 1.5;
            animation.beginTime = CACurrentMediaTime() + 1.0;
            animation.timingFunction = [CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionEaseIn];
            
            [_dotLayer addAnimation:animation forKey:@"animateTransform"];
            
            [self.layer addSublayer:_dotLayer];
        }
    }
}

- (UIImage *)accuracyRingImage {
    CGFloat latRadians = self.annotation.coordinate.latitude * M_PI / 180.0f;
    CGFloat pixelRadius = self.annotation.location.horizontalAccuracy / cos(latRadians) / [self.mapView metersPerPixelAtLatitude:self.annotation.coordinate.latitude];
    UIGraphicsBeginImageContextWithOptions(CGSizeMake(pixelRadius * 2, pixelRadius * 2), NO, [[UIScreen mainScreen] scale]);
    
    CGContextSetStrokeColorWithColor(UIGraphicsGetCurrentContext(), [[UIColor colorWithRed:0.378 green:0.552 blue:0.827 alpha:0.7] CGColor]);
    CGContextSetFillColorWithColor(UIGraphicsGetCurrentContext(), [[UIColor colorWithRed:0.378 green:0.552 blue:0.827 alpha:0.15] CGColor]);
    CGContextSetLineWidth(UIGraphicsGetCurrentContext(), 2.0);
    CGContextStrokeEllipseInRect(UIGraphicsGetCurrentContext(), CGRectMake(0, 0, pixelRadius * 2, pixelRadius * 2));
    
    UIImage *finalImage = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    return finalImage;
}

- (UIImage *)trackingDotHaloImage
{
    UIGraphicsBeginImageContextWithOptions(CGSizeMake(100, 100), NO, [[UIScreen mainScreen] scale]);
    CGContextSetFillColorWithColor(UIGraphicsGetCurrentContext(), [[self.tintColor colorWithAlphaComponent:0.75] CGColor]);
    CGContextFillEllipseInRect(UIGraphicsGetCurrentContext(), CGRectMake(0, 0, 100, 100));
    UIImage *finalImage = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    
    return finalImage;
}

- (UIImage *)dotImage {
    CGFloat tintedWidth = MGLTrackingDotRingWidth * 0.7;
    
    CGRect rect = CGRectMake(0, 0, tintedWidth, tintedWidth);
    
    UIGraphicsBeginImageContextWithOptions(rect.size, NO, [[UIScreen mainScreen] scale]);
    CGContextRef context = UIGraphicsGetCurrentContext();
    
    CGContextSetFillColorWithColor(context, [self.mapView.tintColor CGColor]);
    CGContextFillEllipseInRect(context, CGRectMake((rect.size.width - tintedWidth) / 2.0, (rect.size.height - tintedWidth) / 2.0, tintedWidth, tintedWidth));
    
    UIImage *tintedForeground = UIGraphicsGetImageFromCurrentImageContext();
    
    UIGraphicsEndImageContext();
    
    return tintedForeground;
}

@end
