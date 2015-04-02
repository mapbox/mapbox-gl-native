#import "MGLMapView.h"

@interface MGLMapView (IBAdditions)

// Core properties that can be manipulated in the Attributes inspector in
// Interface Builder. These redeclarations merely add the IBInspectable keyword.
// They appear here to ensure that they appear above the convenience properties;
// inspectables declared in MGLMapView.h are always sorted before those in
// MGLMapView+IBAdditions.h, due to ASCII sort order.

@property (nonatomic) IBInspectable NSString *accessToken;
@property (nonatomic) IBInspectable NSString *mapID;

// Some convenience properties related to the initial viewport. These properties
// are not meant to be used outside of Interface Builder. latitude and longitude
// are backed by properties of type CLLocationDegrees, but these declarations
// must use the type double because Interface Builder is unaware that
// CLLocationDegrees is a typedef for double.

/// Initial latitude at which the receiver is centered.
@property (nonatomic) IBInspectable double latitude;

/// Initial longitude at which the receiver is centered.
@property (nonatomic) IBInspectable double longitude;

/// Initial zoom level of the receiver.
@property (nonatomic) IBInspectable double zoomLevel;

@end
