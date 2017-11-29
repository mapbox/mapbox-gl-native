#import <Foundation/Foundation.h>

@class MGLMapView;

NS_ASSUME_NONNULL_BEGIN

@interface MGLMapView (IBAdditions)

// Core properties that can be manipulated in the Attributes inspector in
// Interface Builder. These redeclarations merely add the IBInspectable keyword.
// They appear here to ensure that they appear above the convenience properties;
// inspectables declared in MGLMapView.h are always sorted before those in
// MGLMapView+IBAdditions.h, due to ASCII sort order.

#if TARGET_INTERFACE_BUILDER

// HACK: We want this property to look like a URL bar in the Attributes
// inspector, but just calling it styleURL would violate Cocoa naming
// conventions and conflict with the existing NSURL property. Fortunately, IB
// strips out the two underscores for display.
@property (nonatomic, nullable) IBInspectable NSString *styleURL__;

#endif // TARGET_INTERFACE_BUILDER

// Convenience properties related to the initial viewport. These properties
// are not meant to be used outside of Interface Builder. latitude and longitude
// are backed by properties of type CLLocationDegrees, but these declarations
// must use the type double because Interface Builder is unaware that
// CLLocationDegrees is a typedef for double.

@property (nonatomic) IBInspectable double latitude;
@property (nonatomic) IBInspectable double longitude;
@property (nonatomic) IBInspectable double zoomLevel;
@property (nonatomic) IBInspectable double minimumZoomLevel;
@property (nonatomic) IBInspectable double maximumZoomLevel;

// Renamed properties. Interface Builder derives the display name of each
// inspectable from the runtime name, but runtime names don’t always make sense
// in UI.

@property (nonatomic) IBInspectable BOOL allowsZooming;
@property (nonatomic) IBInspectable BOOL allowsScrolling;
@property (nonatomic) IBInspectable BOOL allowsRotating;
@property (nonatomic) IBInspectable BOOL allowsTilting;
@property (nonatomic) IBInspectable BOOL showsUserLocation;
@property (nonatomic) IBInspectable BOOL showsHeading;

@end

NS_ASSUME_NONNULL_END
