#import "MGLAnnotationView.h"
#import "MGLAnnotationView_Private.h"
#import "MGLMapView.h"

@interface MGLAnnotationView ()

@property (nonatomic) id<MGLAnnotation> annotation;
@property (nonatomic, readwrite, nullable) NSString *reuseIdentifier;
@end

@implementation MGLAnnotationView

- (instancetype)initWithReuseIdentifier:(NSString *)reuseIdentifier
{
    self = [super init];
    
    if (self)
    {
        _reuseIdentifier = [reuseIdentifier copy];
    }
    
    return self;
}

- (void)prepareForReuse
{
    // Intentionally left blank. The default implementation of this method does nothing.
}

- (void)setCenterOffset:(CGVector)centerOffset
{
    _centerOffset = centerOffset;
    self.center = self.center;
}

- (void)setCenter:(CGPoint)center
{
    [self setCenter:center pitch:0];
}

- (void)setCenter:(CGPoint)center pitch:(CGFloat)pitch
{
    center.x += _centerOffset.dx;
    center.y += _centerOffset.dy;
    
    [super setCenter:center];
    
    if (_flat) {
        [self updatePitch:pitch];
    }
}

- (void)updatePitch:(CGFloat)pitch
{
    CATransform3D t = CATransform3DRotate(CATransform3DIdentity, MGLRadiansFromDegrees(pitch), 1.0, 0, 0);
    self.layer.transform = t;
}

- (id<CAAction>)actionForLayer:(CALayer *)layer forKey:(NSString *)event
{
    // Allow mbgl to drive animation of this view’s bounds.
    if ([event isEqualToString:@"bounds"])
    {
        return [NSNull null];
    }
    return [super actionForLayer:layer forKey:event];
}

@end