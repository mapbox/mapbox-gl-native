#import "MGLUserLocationAnnotationView.h"

#import "MGLUserLocation.h"
#import "MGLUserLocation_Private.h"
#import "MGLAnnotation.h"
#import "MGLMapView.h"
#import "MGLCoordinateFormatter.h"
#import "NSBundle+MGLAdditions.h"

@implementation MGLUserLocationAnnotationView {
    MGLCoordinateFormatter *_accessibilityCoordinateFormatter;
}

- (instancetype)initWithFrame:(CGRect)frame
{
    NSAssert(NO, @"No containing map view specified. Call -initInMapView: instead.");
    return self = [self init];
}

- (instancetype)initInMapView:(MGLMapView *)mapView userLocation:(MGLUserLocation *)userLocation
{
    self = [super initWithFrame:CGRectZero];
    if (self == nil) return nil;
    _mapView = mapView;
    _userLocation = userLocation;
    self.accessibilityTraits = UIAccessibilityTraitButton | UIAccessibilityTraitAdjustable | UIAccessibilityTraitUpdatesFrequently;
    
    _accessibilityCoordinateFormatter = [[MGLCoordinateFormatter alloc] init];
    _accessibilityCoordinateFormatter.unitStyle = NSFormattingUnitStyleLong;
    return self;
}

- (instancetype)initWithCoder:(NSCoder *)decoder
{
    MGLMapView *mapView = [decoder valueForKey:@"mapView"];
    MGLUserLocation *userLocation = [decoder valueForKey:@"userLocation"];
    return [self initInMapView:mapView userLocation:userLocation];
}

- (void)didUpdateUserLocation:(MGLUserLocation *)userLocation
{
    // Left blank intentionally. Subclasses may override this in order to customize UI based new new bearing, speed etc.
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
