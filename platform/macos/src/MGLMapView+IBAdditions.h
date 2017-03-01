#import <Foundation/Foundation.h>

#import "MGLMapView.h"

NS_ASSUME_NONNULL_BEGIN

@interface MGLMapView (IBAdditions)

#if TARGET_INTERFACE_BUILDER

// Core properties that can be manipulated in the Attributes inspector in
// Interface Builder. These redeclarations merely add the IBInspectable keyword.
// They appear here to ensure that they appear above the convenience properties;
// inspectables declared in MGLMapView.h are always sorted before those in
// MGLMapView+IBAdditions.h, due to ASCII sort order.

// We want this property to look like a URL bar in the Attributes inspector, but
// just calling it styleURL would violate Cocoa naming conventions and conflict
// with the existing NSURL property. Fortunately, IB strips out the two
// underscores for display.

/** URL of the style currently displayed in the receiver.

    The URL may be a full HTTP or HTTPS URL, a Mapbox URL indicating the style’s
    map ID (`mapbox://styles/<user>/<style>`), or a path to a local file
    relative to the application’s resource path. Leave this field blank for the
    default style. */
@property (nonatomic, nullable) IBInspectable NSString *styleURL__;

// Convenience properties related to the initial viewport. These properties
// are not meant to be used outside of Interface Builder. latitude and longitude
// are backed by properties of type CLLocationDegrees, but these declarations
// must use the type double because Interface Builder is unaware that
// CLLocationDegrees is a typedef for double.

/** The initial center latitude. */
@property (nonatomic) IBInspectable double latitude;

/** The initial center longitude. */
@property (nonatomic) IBInspectable double longitude;

@property (nonatomic) IBInspectable double zoomLevel;

// Renamed properties. Interface Builder derives the display name of each
// inspectable from the runtime name, but runtime names don’t always make sense
// in UI.

/** A Boolean value that determines whether the user may zoom the map, changing
    its zoom level. */
@property (nonatomic) IBInspectable BOOL allowsZooming;

/** A Boolean value that determines whether the user may scroll around the map,
    changing its center coordinate. */
@property (nonatomic) IBInspectable BOOL allowsScrolling;

/** A Boolean value that determines whether the user may rotate the map,
    changing its direction. */
@property (nonatomic) IBInspectable BOOL allowsRotating;

/** A Boolean value that determines whether the user may tilt the map, changing
    its pitch. */
@property (nonatomic) IBInspectable BOOL allowsTilting;

#endif

@end

NS_ASSUME_NONNULL_END
