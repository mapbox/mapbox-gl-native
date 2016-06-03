#import "MGLAnnotationView.h"
#import "MGLAnnotationView_Private.h"
#import "MGLMapView_Internal.h"

#import "NSBundle+MGLAdditions.h"

#include <mbgl/util/constants.hpp>

@interface MGLAnnotationView ()

@property (nonatomic) id<MGLAnnotation> annotation;
@property (nonatomic, readwrite, nullable) NSString *reuseIdentifier;

@end

@implementation MGLAnnotationView

- (instancetype)initWithReuseIdentifier:(NSString *)reuseIdentifier
{
    self = [self initWithFrame:CGRectZero];
    if (self)
    {
        _reuseIdentifier = [reuseIdentifier copy];
        _scalesWithViewingDistance = YES;
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
    
    if (self.flat)
    {
        [self updatePitch:pitch];
    }
  
    if (self.scalesWithViewingDistance)
    {
        [self updateScaleForPitch:pitch];
    }
}

- (void)updatePitch:(CGFloat)pitch
{
    CATransform3D t = CATransform3DRotate(CATransform3DIdentity, MGLRadiansFromDegrees(pitch), 1.0, 0, 0);
    self.layer.transform = t;
}

- (void)updateScaleForPitch:(CGFloat)pitch
{
    CGFloat superviewHeight = CGRectGetHeight(self.superview.frame);
    if (superviewHeight > 0.0) {
        // Find the maximum amount of scale reduction to apply as the view's center moves from the top
        // of the superview to the bottom. For example, if this view's center has moved 25% of the way
        // from the top of the superview towards the bottom then the maximum scale reduction is 1 - .25
        // or 75%. The range goes from a maximum of 100% to 0% as the view moves from the top to the bottom
        // along the y axis of its superview.
        CGFloat maxScaleReduction = 1.0 - self.center.y / superviewHeight;
       
        // The pitch intensity represents how much the map view is actually pitched compared to
        // what is possible. The value will range from 0% (not pitched at all) to 100% (pitched as much
        // as the map view will allow). The map view's maximum pitch is defined in `mbgl::util::PITCH_MAX`.
        // Since it is possible for the map view to report a pitch less than 0 due to the nature of
        // how the gesture information is captured, the value is guarded with MAX.
        CGFloat pitchIntensity = MAX(pitch, 0) / MGLDegreesFromRadians(mbgl::util::PITCH_MAX);
       
        // The pitch adjusted scale is the inverse proportion of the maximum possible scale reduction
        // multiplied by the pitch intensity. For example, if the maximum scale reduction is 75% and the
        // map view is 50% pitched then the annotation view should be reduced by 37.5% (.75 * .5). The
        // reduction is then normalized for a scale of 1.0.
        CGFloat pitchAdjustedScale = 1.0 - maxScaleReduction * pitchIntensity;
        
        CATransform3D transform = self.flat ? self.layer.transform : CATransform3DIdentity;
        self.layer.transform = CATransform3DScale(transform, pitchAdjustedScale, pitchAdjustedScale, 1);
    }
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

#pragma mark UIAccessibility methods

- (BOOL)isAccessibilityElement {
    return !self.hidden;
}

- (UIAccessibilityTraits)accessibilityTraits {
    return UIAccessibilityTraitButton | UIAccessibilityTraitAdjustable;
}

- (NSString *)accessibilityLabel {
    return [self.annotation respondsToSelector:@selector(title)] ? self.annotation.title : super.accessibilityLabel;
}

- (NSString *)accessibilityValue {
    return [self.annotation respondsToSelector:@selector(subtitle)] ? self.annotation.subtitle : super.accessibilityValue;
}

- (NSString *)accessibilityHint {
    return NSLocalizedStringWithDefaultValue(@"ANNOTATION_A11Y_HINT", nil, nil, @"Shows more info", @"Accessibility hint");
}

- (CGRect)accessibilityFrame {
    CGRect accessibilityFrame = self.frame;
    CGRect minimumFrame = CGRectInset({ self.center, CGSizeZero },
                                      -MGLAnnotationAccessibilityElementMinimumSize.width / 2,
                                      -MGLAnnotationAccessibilityElementMinimumSize.height / 2);
    accessibilityFrame = CGRectUnion(accessibilityFrame, minimumFrame);
    return accessibilityFrame;
}

- (void)accessibilityIncrement {
    [self.superview accessibilityIncrement];
}

- (void)accessibilityDecrement {
    [self.superview accessibilityDecrement];
}

@end