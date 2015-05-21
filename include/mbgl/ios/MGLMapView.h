#import "MGLTypes.h"

#import <UIKit/UIKit.h>
#import <CoreLocation/CoreLocation.h>

@class MGLUserLocation;

@protocol MGLMapViewDelegate;
@protocol MGLAnnotation;

/** An MGLMapView object provides an embeddable map interface, similar to the one provided by Apple's MapKit. You use this class to display map information and to manipulate the map contents from your application. You can center the map on a given coordinate, specify the size of the area you want to display, and style the features of the map to fit your application's use case.
*
*   Use of MGLMapView requires a Mapbox API access token. Obtain an access token on the [Mapbox account page](https://www.mapbox.com/account/apps/). If you instantiate an MGLMapView from Interface Builder, rendering of the map won't begin until the accessToken property has been set.
*
*   @warning Please note that you are responsible for getting permission to use the map data, and for ensuring your use adheres to the relevant terms of use. */
IB_DESIGNABLE
@interface MGLMapView : UIView

#pragma mark - Initializing a Map View

/** @name Initializing a Map View */

/** Initializes and returns a newly allocated map view with the specified frame and the default style.
*   @param frame The frame for the view, measured in points.
*   @return An initialized map view or `nil` if the map view couldn’t be created. */
- (instancetype)initWithFrame:(CGRect)frame;
- (instancetype)initWithFrame:(CGRect)frame accessToken:(NSString *)accessToken __attribute__((unavailable("Use -initWithFrame:. Set MGLMapboxAccessToken in the Info.plist or call +[MGLAccountManager setAccessToken:].")));

/** Initializes and returns a newly allocated map view with the specified frame and style URL.
*   @param frame The frame for the view, measured in points.
*   @param styleURL The map style URL to use. Can be either an HTTP/HTTPS URL or a Mapbox map ID style URL (`mapbox://<user.style>`).
*   @return An initialized map view or `nil` if the map view couldn’t be created. */
- (instancetype)initWithFrame:(CGRect)frame styleURL:(NSURL *)styleURL;
- (instancetype)initWithFrame:(CGRect)frame accessToken:(NSString *)accessToken styleURL:(NSURL *)styleURL __attribute__((unavailable("Use -initWithFrame:styleURL:. Set MGLMapboxAccessToken in the Info.plist or call +[MGLAccountManager setAccessToken:].")));

#pragma mark - Authorizing Access

/** @name Authorizing Access */

/** Mapbox API access token for the map view. */
@property (nonatomic) NSString *accessToken __attribute__((unavailable("Use +[MGLAccountManager accessToken] and +[MGLAccountManager setAccessToken:].")));

#pragma mark - Managing Constraints

/** @name Managing Constraints */

#pragma mark - Accessing Map Properties

/** @name Accessing Map Properties */

/** A Boolean value that determines whether the user may zoom the map.
*
*   This property controls only user interactions with the map. If you set the value of this property to `NO`, you may still change the map zoom programmatically.
*
*   The default value of this property is `YES`. */
@property(nonatomic, getter=isZoomEnabled) BOOL zoomEnabled;

/** A Boolean value that determines whether the user may scroll around the map.
*
*   This property controls only user interactions with the map. If you set the value of this property to `NO`, you may still change the map location programmatically.
*
*   The default value of this property is `YES`. */
@property(nonatomic, getter=isScrollEnabled) BOOL scrollEnabled;

/** A Boolean value that determines whether the user may rotate the map.
*
*   This property controls only user interactions with the map. If you set the value of this property to `NO`, you may still rotate the map programmatically.
*
*   The default value of this property is `YES`. */
@property(nonatomic, getter=isRotateEnabled) BOOL rotateEnabled;

#pragma mark - Accessing the Delegate

/** @name Accessing the Delegate */

// TODO
@property(nonatomic, weak) IBOutlet id<MGLMapViewDelegate> delegate;

#pragma mark - Manipulating the Visible Portion of the Map

/** @name Manipulating the Visible Portion of the Map */

/** The map coordinate at the center of the map view.
*
*   Changing the value in this property centers the map on the new coordinate without changing the current zoom level. 
*
*   Changing the value of this property updates the map view immediately. If you want to animate the change, use the setCenterCoordinate:animated: method instead. */
@property (nonatomic) CLLocationCoordinate2D centerCoordinate;

/** Changes the center coordinate of the map and optionally animates the change.
*   @param coordinate The new center coordinate for the map.
*   @param animated Specify `YES` if you want the map view to scroll to the new location or `NO` if you want the map to display the new location immediately.
*
*   Changing the center coordinate centers the map on the new coordinate without changing the current zoom level. */
- (void)setCenterCoordinate:(CLLocationCoordinate2D)coordinate animated:(BOOL)animated;

/** The zoom level of the map view.
*
*   Changing the value in this property zooms the map in or out without changing the center coordinate. At zoom level 0, tiles cover the entire world map; at zoom level 1, tiles cover 1/4 of the world; at zoom level 2, tiles cover 1/16 of the world, and so on.
*
*   Changing the value of this property updates the map view immediately. If you want to animate the change, use the setZoomLevel:animated: method instead. */
@property (nonatomic) double zoomLevel;

/** Changes the zoom level of the map and optionally animates the change.
*   @param zoomLevel The new zoom level for the map.
*   @param animated Specify `YES` if you want the map view to animate the change to the new zoom level or `NO` if you want the map to display the new zoom level immediately.
*
*   Changing the zoom level scales the map without changing the current center coordinate. At zoom level 0, tiles cover the entire world map; at zoom level 1, tiles cover 1/4 of the world; at zoom level 2, tiles cover 1/16 of the world, and so on. */
- (void)setZoomLevel:(double)zoomLevel animated:(BOOL)animated;

/** Changes the center coordinate and zoom level of the and optionally animates the change. 
*   @param centerCoordinate The new center coordinate for the map.
*   @param zoomLevel The new zoom level for the map.
*   @param animated Specify `YES` if you want the map view to animate scrolling and zooming to the new location or `NO` if you want the map to display the new location immediately. */
- (void)setCenterCoordinate:(CLLocationCoordinate2D)centerCoordinate zoomLevel:(double)zoomLevel animated:(BOOL)animated;

/** The heading of the map (measured in degrees) relative to true north. 
*
*   The value `0` means that the top edge of the map view corresponds to true north. The value `90` means the top of the map is pointing due east. The value `180` means the top of the map points due south, and so on. */
@property (nonatomic) CLLocationDirection direction;

/** Changes the heading of the map and optionally animates the changes.
*   @param direction The heading of the map (measured in degrees) relative to true north.
*   @param animated Specify `YES` if you want the map view to animate the change to the new heading or `NO` if you want the map to display the new heading immediately. 
*
*   Changing the heading rotates the map without changing the current center coordinate or zoom level. */
- (void)setDirection:(CLLocationDirection)direction animated:(BOOL)animated;

/** Resets the map rotation to a northern heading. */
- (IBAction)resetNorth;

#pragma mark - Converting Map Coordinates

/** @name Converting Map Coordinates */

/** Converts a point in the specified view’s coordinate system to a map coordinate.
*   @param point The point you want to convert.
*   @param view The view that serves as the reference coordinate system for the `point` parameter.
*   @return The map coordinate at the specified point. */
- (CLLocationCoordinate2D)convertPoint:(CGPoint)point toCoordinateFromView:(UIView *)view;

/** Converts a map coordinate to a point in the specified view.
*   @param coordinate The map coordinate for which you want to find the corresponding point.
*   @param view The view in whose coordinate system you want to locate the specified map coordinate. If this parameter is `nil`, the returned point is specified in the window’s coordinate system. If `view` is not `nil`, it must belong to the same window as the map view.
*   @return The point (in the appropriate view or window coordinate system) corresponding to the specified latitude and longitude value. */
- (CGPoint)convertCoordinate:(CLLocationCoordinate2D)coordinate toPointToView:(UIView *)view;

/** Returns the distance spanned by one pixel at the specified latitude and current zoom level.
*
*   The distance between pixels decreases as the latitude approaches the poles. This relationship parallels the relationship between longitudinal coordinates at different latitudes.
*
*   @param latitude The latitude for which to return the value.
*   @return The distance (in meters) spanned by a single pixel. */
- (CLLocationDistance)metersPerPixelAtLatitude:(CLLocationDegrees)latitude;

#pragma mark - Styling the Map

/** @name Styling the Map */

/** Mapbox ID of the style currently displayed in the receiver, or `nil` if the style does not have an ID.
*
*   The style may lack an ID if it is located at an HTTP, HTTPS, or local file URL. Use `styleURL` to get the URL in these cases.
*
*   To display the default style, set this property to `nil`. */
@property (nonatomic) NSString *styleID;
@property (nonatomic) NSString *mapID __attribute__((unavailable("Use styleID.")));

/** Returns the URLs to the styles bundled with the library. */
- (NSArray *)bundledStyleURLs;

/** URL of the style currently displayed in the receiver.
*
*   The URL may be a full HTTP or HTTPS URL or a Mapbox URL indicating the style’s map ID (`mapbox://<user.style>`).
*
*   To display the default style, set this property to `nil`. */
@property (nonatomic) NSURL *styleURL;

/** Currently active style classes, represented as an array of string identifiers. */
@property (nonatomic) NSArray *styleClasses;

/** Returns a Boolean value indicating whether the style class with the given identifier is currently active.
    @param styleClass The style class to query for.
    @return Whether the style class is currently active. */
- (BOOL)hasStyleClass:(NSString *)styleClass;

/** Activates the style class with the given identifier.
    @param styleClass The style class to activate. */
- (void)addStyleClass:(NSString *)styleClass;

/** Deactivates the style class with the given identifier. 
    @param styleClass The style class to deactivate. */
- (void)removeStyleClass:(NSString *)styleClass;

#pragma mark - Annotating the Map

/** @name Annotating the Map */

/** The complete list of annotations associated with the receiver. (read-only)
*
*   The objects in this array must adopt the MGLAnnotation protocol. If no annotations are associated with the map view, the value of this property is `nil`. */
@property (nonatomic, readonly) NSArray *annotations;

/** Adds the specified annotation to the map view.
*   @param annotation The annotation object to add to the receiver. This object must conform to the MGLAnnotation protocol. The map view retains the specified object. */
- (void)addAnnotation:(id <MGLAnnotation>)annotation;

/** Adds an array of annotation objects to the map view.
*   @param annotations An array of annotation objects. Each object in the array must conform to the MGLAnnotation protocol. The map view retains the individual annotation objects. */
- (void)addAnnotations:(NSArray *)annotations;

/** Removes the specified annotation object from the map view.
*
*   Removing an annotation object disassociates it from the map view entirely, preventing it from being displayed on the map. Thus, you would typically call this method only when you want to hide or delete a given annotation.
*
*   @param annotation The annotation object to remove. This object must conform to the MGLAnnotation protocol. */
- (void)removeAnnotation:(id <MGLAnnotation>)annotation;

/** Removes an array of annotation objects from the map view.
*
*   Removing annotation objects disassociates them from the map view entirely, preventing them from being displayed on the map. Thus, you would typically call this method only when you want to hide or delete the specified annotations.
*
*   @param annotations The array of annotations to remove. Objects in the array must conform to the MGLAnnotation protocol. */
- (void)removeAnnotations:(NSArray *)annotations;

/** The annotations that are currently selected.
*
*   Assigning a new array to this property selects only the first annotation in the array. */
@property (nonatomic, copy) NSArray *selectedAnnotations;

/** Selects the specified annotation and displays a callout view for it.
*
*   If the specified annotation is not onscreen, this method has no effect.
*
*   @param annotation The annotation object to select.
*   @param animated If `YES`, the callout view is animated into position. */
- (void)selectAnnotation:(id <MGLAnnotation>)annotation animated:(BOOL)animated;

/** Deselects the specified annotation and hides its callout view.
*   @param annotation The annotation object to deselect.
*   @param animated If `YES`, the callout view is animated offscreen. */
- (void)deselectAnnotation:(id <MGLAnnotation>)annotation animated:(BOOL)animated;

#pragma mark - Displaying the User's Location

/** @name Displaying the User's Location */

/** A Boolean value indicating whether the map may display the user location.
*
*   This property does not indicate whether the user’s position is actually visible on the map, only whether the map view is allowed to display it. To determine whether the user’s position is visible, use the userLocationVisible property. The default value of this property is `NO`.
*
*   Setting this property to `YES` causes the map view to use the Core Location framework to find the current location. As long as this property is `YES`, the map view continues to track the user’s location and update it periodically.
*
*   On iOS 8 and above, your app must specify a value for `NSLocationWhenInUseUsageDescription` or `NSLocationAlwaysUsageDescription` in its `Info.plist` to satisfy the requirements of the underlying Core Location framework when enabling this property. */
@property (nonatomic, assign) BOOL showsUserLocation;

/** A Boolean value indicating whether the device’s current location is visible in the map view. (read-only) */
@property (nonatomic, assign, readonly, getter=isUserLocationVisible) BOOL userLocationVisible;

/** Returns the annotation object indicating the user’s current location. */
@property (nonatomic, readonly) MGLUserLocation *userLocation;

/** The mode used to track the user location. */
@property (nonatomic, assign) MGLUserTrackingMode userTrackingMode;

/** Whether the map view should display a heading calibration alert when necessary. The default value is `YES`. */
@property (nonatomic, assign) BOOL displayHeadingCalibration;

#pragma mark - Debugging

/** @name Debugging */

/** A Boolean value that determines whether map debugging information is shown. 
*
*   The default value of this property is `NO`. */
@property (nonatomic, getter=isDebugActive) BOOL debugActive;

/** Toggle the current value of debugActive. */
- (void)toggleDebug;

/** Empties the in-memory tile cache. */
- (void)emptyMemoryCache;

/** Resets the map to the minimum zoom level, a center coordinate of (0, 0), and a northern heading. */
- (void)resetPosition;

@end
