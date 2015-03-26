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
@interface MGLMapView : UIView

#pragma mark - Initializing a Map View

/** @name Initializing a Map View */

/** Initialize a map view with a given frame, style, and access token.
*   @param frame The frame with which to initialize the map view.
*   @param accessToken A Mapbox API access token.
*   @param styleJSON The map stylesheet as JSON text.
*   @return An initialized map view, or `nil` if the map view was unable to be initialized. */
- (instancetype)initWithFrame:(CGRect)frame accessToken:(NSString *)accessToken styleJSON:(NSString *)styleJSON;

/** Initialize a map view with a given frame, bundled style name, and access token.
*   @param frame The frame with which to initialize the map view.
*   @param accessToken A Mapbox API access token.
*   @param styleName The map style name to use.
*   @return An initialized map view, or `nil` if the map view was unable to be initialized. */
- (instancetype)initWithFrame:(CGRect)frame accessToken:(NSString *)accessToken bundledStyleNamed:(NSString *)styleName;

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

/** A Boolean value indicating whether the map may display the user location.
 
    This property does not indicate whether the user’s position is actually visible on the map, only whether the map view is allowed to display it. To determine whether the user’s position is visible, use the userLocationVisible property. The default value of this property is `NO`.

    Setting this property to `YES` causes the map view to use the Core Location framework to find the current location. As long as this property is `YES`, the map view continues to track the user’s location and update it periodically.

    On iOS 8 and above, your app must specify a value for `NSLocationWhenInUseUsageDescription` in its `Info.plist` to satisfy the requirements of the underlying Core Location framework when enabling this property.
 */
@property (nonatomic, assign) BOOL showsUserLocation;

/// Returns a Boolean value indicating whether the user currently sees the user location annotation.
@property (nonatomic, assign, readonly, getter=isUserLocationVisible) BOOL userLocationVisible;

/// Returns the annotation object indicating the user’s current location.
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

/** Resets the map to the minimum zoom level, a center coordinate of (0, 0), and a northern heading. */
- (void)resetPosition;

@end

#pragma mark - MGLMapViewDelegate

/** The MGLMapViewDelegate protocol defines a set of optional methods that you can use to receive map-related update messages. Because many map operations require the MGLMapView class to load data asynchronously, the map view calls these methods to notify your application when specific operations complete. The map view also uses these methods to request annotation marker symbology and to manage interactions with those markers. */
@protocol MGLMapViewDelegate <NSObject>

@optional

#pragma mark - Managing the Display of Annotations

/** @name Managing the Display of Annotations */

/** Returns the style's symbol name to use for the marker for the specified point annotation object.
*   @param mapView The map view that requested the annotation symbol name.
*   @param annotation The object representing the annotation that is about to be displayed. 
*   @return The marker symbol to display for the specified annotation or `nil` if you want to display the default symbol. */
- (NSString *)mapView:(MGLMapView *)mapView symbolNameForAnnotation:(id <MGLAnnotation>)annotation;

/** Returns a Boolean value indicating whether the annotation is able to display extra information in a callout bubble.
*
*   If the value returned is `YES`, a standard callout bubble is shown when the user taps a selected annotation. The callout uses the title and subtitle text from the associated annotation object. If there is no title text, though, the annotation will not show a callout. The callout also displays any custom callout views returned by the delegate for the left and right callout accessory views.
*
*   If the value returned is `NO`, the value of the title and subtitle strings are ignored.
*
*   @param mapView The map view that requested the annotation callout ability.
*   @param annotation The object representing the annotation.
*   @return A Boolean indicating whether the annotation should show a callout. */
- (BOOL)mapView:(MGLMapView *)mapView annotationCanShowCallout:(id <MGLAnnotation>)annotation;

/** Return the view to display on the left side of the standard callout bubble.
*
*   The default value is treated as if `nil`. The left callout view is typically used to display information about the annotation or to link to custom information provided by your application.
*
*   If the view you specify is also a descendant of the `UIControl` class, you can use the map view’s delegate to receive notifications when your control is tapped. If it does not descend from `UIControl`, your view is responsible for handling any touch events within its bounds.
*
*   @param mapView The map view presenting the annotation callout.
*   @param annotation The object representing the annotation with the callout.
*   @return The accessory view to display. */
- (UIView *)mapView:(MGLMapView *)mapView leftCalloutAccessoryViewForAnnotation:(id <MGLAnnotation>)annotation;

/** Return the view to display on the right side of the standard callout bubble.
*
*   The default value is treated is if `nil`. The right callout view is typically used to link to more detailed information about the annotation. A common view to specify for this property is `UIButton` object whose type is set to `UIButtonTypeDetailDisclosure`.
*
*   If the view you specify is also a descendant of the `UIControl` class, you can use the map view’s delegate to receive notifications when your control is tapped. If it does not descend from `UIControl`, your view is responsible for handling any touch events within its bounds.
*
*   @param mapView The map view presenting the annotation callout.
*   @param annotation The object representing the annotation with the callout.
*   @return The accessory view to display. */
- (UIView *)mapView:(MGLMapView *)mapView rightCalloutAccessoryViewForAnnotation:(id <MGLAnnotation>)annotation;

#pragma mark - Responding to Map Position Changes

// Responding to Map Position Changes

// TODO
- (void)mapView:(MGLMapView *)mapView regionWillChangeAnimated:(BOOL)animated;

// TODO
- (void)mapViewRegionIsChanging:(MGLMapView *)mapView;

// TODO
- (void)mapView:(MGLMapView *)mapView regionDidChangeAnimated:(BOOL)animated;

#pragma mark - Loading the Map Data

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

#pragma mark - Tracking the User Location

/// Tells the delegate that the map view will begin tracking the user’s location.
- (void)mapViewWillStartLocatingUser:(MGLMapView *)mapView;

/// Tells the delegate that the map view has stopped tracking the user’s location.
- (void)mapViewDidStopLocatingUser:(MGLMapView *)mapView;

/// Tells the delegate that the map view has updated the user’s location to the given location.
- (void)mapView:(MGLMapView *)mapView didUpdateUserLocation:(MGLUserLocation *)userLocation;

/// Tells the delegate that the map view has failed to locate the user.
- (void)mapView:(MGLMapView *)mapView didFailToLocateUserWithError:(NSError *)error;

/**
    Tells the delegate that the map view’s user tracking mode has changed.
 
    This method is called after the map view asynchronously changes to reflect the new user tracking mode, for example by beginning to zoom or rotate.
 */
- (void)mapView:(MGLMapView *)mapView didChangeUserTrackingMode:(MGLUserTrackingMode)mode animated:(BOOL)animated;

#pragma mark - Managing Annotations

/** @name Managing Annotations */

/* Tells the delegate that the user tapped one of the annotation's accessory buttons.
*
*  Accessory views contain custom content and are positioned on either side of the annotation title text. If a view you specify is a descendant of the `UIControl` class, the map view calls this method as a convenience whenever the user taps your view. You can use this method to respond to taps and perform any actions associated with that control. For example, if your control displayed additional information about the annotation, you could use this method to present a modal panel with that information.
*
*  If your custom accessory views are not descendants of the `UIControl` class, the map view does not call this method.
*
*  @param mapView The map view containing the specified annotation.
*  @param annotation The annotation whose button was tapped.
*  @param control The control that was tapped. */
- (void)mapView:(MGLMapView *)mapView annotation:(id <MGLAnnotation>)annotation calloutAccessoryControlTapped:(UIControl *)control;

#pragma mark - Selecting Annotations

/** @name Selecting Annotations */

/* Tells the delegate that one of its annotations was selected.
*
*  You can use this method to track changes in the selection state of annotations.
*
*  @param mapView The map view containing the annotation.
*  @param annotation The annotation that was selected. */
- (void)mapView:(MGLMapView *)mapView didSelectAnnotation:(id <MGLAnnotation>)annotation;

/* Tells the delegate that one of its annotations was deselected.
*
*  You can use this method to track changes in the selection state of annotations.
*
*  @param mapView The map view containing the annotation.
*  @param annotation The annotation that was deselected. */
- (void)mapView:(MGLMapView *)mapView didDeselectAnnotation:(id <MGLAnnotation>)annotation;

@end
