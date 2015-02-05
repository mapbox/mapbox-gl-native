#import <UIKit/UIKit.h>
#import <CoreLocation/CoreLocation.h>

@protocol MGLMapViewDelegate;

/** An MGLMapView object provides an embeddable map interface, similar to the one provided by Apple's MapKit. You use this class to display map information and to manipulate the map contents from your application. You can center the map on a given coordinate, specify the size of the area you want to display, and style the features of the map to fit your application's use case.
*
*   Use of MGLMapView requires a Mapbox API access token. Obtain an access token on the [Mapbox account page](https://www.mapbox.com/account/apps/). If you instantiate an MGLMapView from Interface Builder, rendering of the map won't begin until the accessToken property has been set.
*
*   @warning Please note that you are responsible for getting permission to use the map data, and for ensuring your use adheres to the relevant terms of use. */
@interface MGLMapView : UIView

#pragma mark - Initializing a Map View

/** @name Initializing a Map View */

/** Initialize a map view with a given frame, style, and access token.
*   @param frame The frame with which to initialize the map view.
*   @param styleJSON The map stylesheet as JSON text.
*   @param accessToken A Mapbox API access token.
*   @return An initialized map view, or `nil` if the map view was unable to be initialized. */
- (instancetype)initWithFrame:(CGRect)frame styleJSON:(NSString *)styleJSON accessToken:(NSString *)accessToken;

/** Initialize a map view with a given frame, the default style, and an access token.
*   @param frame The frame with which to initialize the map view.
*   @param accessToken A Mapbox API access token.
*   @return An initialized map view, or `nil` if the map view was unable to be initialized. */
- (instancetype)initWithFrame:(CGRect)frame accessToken:(NSString *)accessToken;

- (instancetype)initWithFrame:(CGRect)frame __attribute__((unavailable("Instantiating an MGLMapView requires setting a style and/or an access token.")));

#pragma mark - Authorizing Access

/** @name Authorizing Access */

/** Sets a Mapbox API access token for the map view. 
*   @param accessToken A Mapbox API token. */
- (void)setAccessToken:(NSString *)accessToken;

#pragma mark - Managing Constraints

/** @name Managing Constraints */

/** A view controller whose top and bottom layout guides to use for proper setup of constraints in the map view internals.
*
*   Certain components of the map view, such as the heading compass and the data attribution button, need to be aware of the view controller layout in order to avoid positioning content under a top navigation bar or a bottom toolbar. */
@property (nonatomic, weak) UIViewController *viewControllerForLayoutGuides;

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
@property(nonatomic, weak) id<MGLMapViewDelegate> delegate;

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
- (void)resetNorth;

#pragma mark - Styling the Map

/** @name Styling the Map */

/** Sets the map style.
*   @param styleJSON The map stylesheet as JSON text. */
- (void)setStyleJSON:(NSString *)styleJSON;

/** Returns the raw JSON style as a native dictionary object. */
- (NSDictionary *)getRawStyle;

/** Sets the raw JSON style as a native dictionary object with a transition animation.
*   @param style The style JSON as a dictionary object. */
- (void)setRawStyle:(NSDictionary *)style;

/** Returns the names of the styles bundled with the library. */
- (NSArray *)bundledStyleNames;

/** Sets the map style to a named, bundled style.
*   @param styleName The map style name to use. */
- (void)useBundledStyleNamed:(NSString *)styleName;

#pragma mark - Debugging

/** @name Debugging */

/** A Boolean value that determines whether map debugging information is shown. 
*
*   The default value of this property is `NO`. */
@property (nonatomic, getter=isDebugActive) BOOL debugActive;

/** Toggle the current value of debugActive. */
- (void)toggleDebug;

/** Resets the map to the minimum zoom level, a center coordinate of (0, 0), and a northern heading. */
- (void)resetPosition;

@end

// TODO
@protocol MGLMapViewDelegate <NSObject>

@optional

// Responding to Map Position Changes

// TODO
- (void)mapView:(MGLMapView *)mapView regionWillChangeAnimated:(BOOL)animated;

// TODO
- (void)mapView:(MGLMapView *)mapView regionDidChangeAnimated:(BOOL)animated;

// Loading the Map Data

// TODO
- (void)mapViewWillStartLoadingMap:(MGLMapView *)mapView;

// TODO
- (void)mapViewDidFinishLoadingMap:(MGLMapView *)mapView;

// TODO
- (void)mapViewDidFailLoadingMap:(MGLMapView *)mapView withError:(NSError *)error;

// TODO
- (void)mapViewWillStartRenderingMap:(MGLMapView *)mapView;

// TODO
- (void)mapViewDidFinishRenderingMap:(MGLMapView *)mapView fullyRendered:(BOOL)fullyRendered;

@end
