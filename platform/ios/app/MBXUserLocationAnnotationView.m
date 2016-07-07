#import "MBXUserLocationAnnotationView.h"

const CGFloat MBXUserLocationAnnotationSize = 15.0;

@interface MBXUserLocationAnnotationView()
@property (nonatomic) CAShapeLayer *shapeLayer;
@property (nonatomic) CALayer *dotLayer;
@end

@implementation MBXUserLocationAnnotationView

- (void)didUpdateUserLocation:(MGLUserLocation *)userLocation
{
    self.backgroundColor = [UIColor colorWithRed:0 green:1 blue:0 alpha:.8];
    [self updateShape];
}

- (CGSize)intrinsicContentSize
{
    return CGSizeMake(MBXUserLocationAnnotationSize, MBXUserLocationAnnotationSize);
}

- (CAShapeLayer *)shapeLayer
{
    if (!_shapeLayer) {
        _shapeLayer = [CAShapeLayer layer];
        _shapeLayer.contentsScale = [UIScreen mainScreen].scale;
        _shapeLayer.shouldRasterize = YES;
        _shapeLayer.rasterizationScale = [UIScreen mainScreen].scale;
        _shapeLayer.drawsAsynchronously = YES;
        _shapeLayer.frame = self.bounds;
        [self.layer addSublayer:_shapeLayer];
    }
    return _shapeLayer;
}

- (UIBezierPath *)headingIndicatorPath
{
    UIBezierPath *path = [UIBezierPath bezierPath];
    [path addArcWithCenter:CGPointMake(MBXUserLocationAnnotationSize/2.0, 0)
                    radius:MBXUserLocationAnnotationSize/2.0
                startAngle:-180 * M_PI / 180
                  endAngle:M_PI / 180
                 clockwise:NO];
    return path;
}

- (void)updateShape
{
    self.shapeLayer.frame = self.bounds;
    self.shapeLayer.path = [self headingIndicatorPath].CGPath;
}

@end
