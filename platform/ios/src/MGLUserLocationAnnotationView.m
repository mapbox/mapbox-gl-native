#import "MGLUserLocationAnnotationView.h"

#import "MGLUserLocation.h"
#import "MGLUserLocation_Private.h"
#import "MGLAnnotationView_Private.h"
#import "MGLAnnotation.h"
#import "MGLMapView.h"
#import "MGLCoordinateFormatter.h"
#import "NSBundle+MGLAdditions.h"

@interface MGLUserLocationAnnotationView()
@property (nonatomic, weak, nullable) MGLMapView *mapView;
@property (nonatomic, weak, nullable) MGLUserLocation *userLocation;
@property (nonatomic, weak) CALayer *hitTestLayer;
@end

@implementation MGLUserLocationAnnotationView {
    MGLCoordinateFormatter *_accessibilityCoordinateFormatter;
}

- (instancetype)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self == nil) return nil;

    self.accessibilityTraits = UIAccessibilityTraitButton | UIAccessibilityTraitAdjustable | UIAccessibilityTraitUpdatesFrequently;

    _accessibilityCoordinateFormatter = [[MGLCoordinateFormatter alloc] init];
    _accessibilityCoordinateFormatter.unitStyle = NSFormattingUnitStyleLong;

    return self;
}

- (CALayer *)hitTestLayer
{
    return self.layer.presentationLayer;
}

- (void)update
{
    // Left blank intentionally. Subclasses should usually override this in order to update the annotation’s appearance.
}

- (BOOL)isAccessibilityElement
{
    return !self.hidden;
}

- (NSString *)accessibilityLabel
{
    return self.userLocation.title;
}

- (NSString *)accessibilityValue
{
    if (self.userLocation.subtitle)
    {
        return self.userLocation.subtitle;
    }

    // Each arcminute of longitude is at most about 1 nmi, too small for low zoom levels.
    // Each arcsecond of longitude is at most about 30 m, too small for all but the very highest of zoom levels.
    double zoomLevel = self.mapView.zoomLevel;
    _accessibilityCoordinateFormatter.allowsMinutes = zoomLevel > 8;
    _accessibilityCoordinateFormatter.allowsSeconds = zoomLevel > 20;

    return [_accessibilityCoordinateFormatter stringFromCoordinate:self.mapView.centerCoordinate];
}

- (CGRect)accessibilityFrame
{
    return CGRectInset(self.frame, -15, -15);
}

- (UIBezierPath *)accessibilityPath
{
    return [UIBezierPath bezierPathWithOvalInRect:self.frame];
}

- (void)accessibilityIncrement
{
    [self.mapView accessibilityIncrement];
}

- (void)accessibilityDecrement
{
    [self.mapView accessibilityDecrement];
}

- (void)setHidden:(BOOL)hidden
{
    BOOL oldValue = super.hidden;
    [super setHidden:hidden];
    if (oldValue != hidden)
    {
        UIAccessibilityPostNotification(UIAccessibilityLayoutChangedNotification, nil);
    }
}

@end
