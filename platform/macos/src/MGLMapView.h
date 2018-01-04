#import <Cocoa/Cocoa.h>
#import <CoreLocation/CoreLocation.h>

#import "MGLFoundation.h"
#import "MGLTypes.h"
#import "MGLGeometry.h"

NS_ASSUME_NONNULL_BEGIN

@class MGLAnnotationImage;
@class MGLMapCamera;
@class MGLStyle;
@class MGLShape;

@protocol MGLAnnotation;
@protocol MGLMapViewDelegate;
@protocol MGLOverlay;
@protocol MGLFeature;

/**
 An interactive, customizable map view with an interface similar to the one
 provided by Apple’s MapKit.

 Using `MGLMapView`, you can embed the map inside a view, allow users to
 manipulate it with standard gestures, animate the map between different
 viewpoints, and present information in the form of annotations and overlays.

 The map view loads scalable vector tiles that conform to the
 <a href="https://github.com/mapbox/vector-tile-spec">Mapbox Vector Tile Specification</a>.
 It styles them with a style that conforms to the
 <a href="https://www.mapbox.com/mapbox-gl-style-spec/">Mapbox Style Specification</a>.
 Such styles can be designed in
 <a href="https://www.mapbox.com/studio/">Mapbox Studio</a> and hosted on
 mapbox.com.

 A collection of Mapbox-hosted styles is available through the `MGLStyle` class.
 These basic styles use
 <a href="https://www.mapbox.com/developers/vector-tiles/mapbox-streets">Mapbox Streets</a>
 or <a href="https://www.mapbox.com/satellite/">Mapbox Satellite</a> data
 sources, but you can specify a custom style that makes use of your own data.

 Mapbox-hosted vector tiles and styles require an API access token, which you
 can obtain from the
 <a href="https://www.mapbox.com/studio/account/tokens/">Mapbox account page</a>.
 Access tokens associate requests to Mapbox’s vector tile and style APIs with
 your Mapbox account. They also deter other developers from using your styles
 without your permission.

 Adding your own gesture recognizer to `MGLMapView` will block the corresponding
 gesture recognizer built into `MGLMapView`. To avoid conflicts, define which
 gesture recognizer takes precedence. For example, you can subclass
 `NSClickGestureRecognizer` and override `-[NSGestureRecognizer shouldRequireFailureOfGestureRecognizer:]`,
 so that your subclass will be invoked only if the default `MGLMapView` click
 gesture recognizer fails:

 ```swift
 class MapClickGestureRecognizer: NSClickGestureRecognizer {
     override func shouldRequireFailure(of otherGestureRecognizer: NSGestureRecognizer) -> Bool {
         return otherGestureRecognizer is NSClickGestureRecognizer
     }
 }
 ```

 @note You are responsible for getting permission to use the map data and for
    ensuring that your use adheres to the relevant terms of use.
 */
MGL_EXPORT IB_DESIGNABLE
@interface MGLMapView : NSView

#pragma mark Creating Instances

/**
 Initializes and returns a newly allocated map view with the specified frame and
 the default style.

 @param frame The frame for the view, measured in points.
 @return An initialized map view.
 */
- (instancetype)initWithFrame:(NSRect)frame;

/**
 Initializes and returns a newly allocated map view with the specified frame and
 style URL.

 @param frame The frame for the view, measured in points.
 @param styleURL URL of the map style to display. The URL may be a full HTTP or
    HTTPS URL, a Mapbox URL indicating the style’s map ID
    (`mapbox://styles/<user>/<style>`), or a path to a local file relative to
    the application’s resource path. Specify `nil` for the default style.
 @return An initialized map view.
 */
- (instancetype)initWithFrame:(NSRect)frame styleURL:(nullable NSURL *)styleURL;

#pragma mark Accessing the Delegate

/**
 The receiver’s delegate.

 A map view sends messages to its delegate to notify it of changes to its
 contents or the viewpoint. The delegate also provides information about
 annotations displayed on the map, such as the styles to apply to individual
 annotations.
 */
@property (nonatomic, weak, nullable) IBOutlet id <MGLMapViewDelegate> delegate;

#pragma mark Configuring the Map’s Appearance

/**
 The style currently displayed in the receiver.

 Unlike the `styleURL` property, this property is set to an object that allows
 you to manipulate every aspect of the style locally.

 If the style is loading, this property is set to `nil` until the style finishes
 loading. If the style has failed to load, this property is set to `nil`.
 Because the style loads asynchronously, you should manipulate it in the
 `-[MGLMapViewDelegate mapView:didFinishLoadingStyle:]` or
 `-[MGLMapViewDelegate mapViewDidFinishLoadingMap:]` method. It is not possible
 to manipulate the style before it has finished loading.

 @note The default styles provided by Mapbox contain sources and layers with
    identifiers that will change over time. Applications that use APIs that
    manipulate a style's sources and layers must first set the style URL to an
    explicitly versioned style using a convenience method like
    `+[MGLStyle outdoorsStyleURLWithVersion:]`, `MGLMapView`’s “Style URL”
    inspectable in Interface Builder, or a manually constructed `NSURL`.
 */
@property (nonatomic, readonly, nullable) MGLStyle *style;

/**
 URL of the style currently displayed in the receiver.

 The URL may be a full HTTP or HTTPS URL, a Mapbox URL indicating the style’s
 map ID (`mapbox://styles/<user>/<style>`), or a path to a local file relative
 to the application’s resource path.

 If you set this property to `nil`, the receiver will use the default style and
 this property will automatically be set to that style’s URL.

 If you want to modify the current style without replacing it outright, or if
 you want to introspect individual style attributes, use the `style` property.
 */
@property (nonatomic, null_resettable) NSURL *styleURL;

/**
 Reloads the style.

 You do not normally need to call this method. The map view automatically
 responds to changes in network connectivity by reloading the style. You may
 need to call this method if you change the access token after a style has
 loaded but before loading a style associated with a different Mapbox account.
 */
- (IBAction)reloadStyle:(id)sender;

/**
 A control for zooming in and out, positioned in the lower-right corner.
 */
@property (nonatomic, readonly) NSSegmentedControl *zoomControls;

/**
 A control indicating the map’s direction and allowing the user to manipulate
 the direction, positioned above the zoom controls in the lower-right corner.
 */
@property (nonatomic, readonly) NSSlider *compass;

/**
 The Mapbox logo, positioned in the lower-left corner.

 @note The Mapbox terms of service, which governs the use of Mapbox-hosted
    vector tiles and styles,
    <a href="https://www.mapbox.com/help/mapbox-logo/">requires</a> most Mapbox
    customers to display the Mapbox logo. If this applies to you, do not hide
    this view or change its contents.
 */
@property (nonatomic, readonly) NSImageView *logoView;

/**
 A view showing legally required copyright notices, positioned along the bottom
 of the map view, to the left of the Mapbox logo.

 @note The Mapbox terms of service, which governs the use of Mapbox-hosted
    vector tiles and styles,
    <a href="https://www.mapbox.com/help/attribution/">requires</a> these
    copyright notices to accompany any map that features Mapbox-designed styles,
    OpenStreetMap data, or other Mapbox data such as satellite or terrain data.
    If that applies to this map view, do not hide this view or remove any
    notices from it.
 */
@property (nonatomic, readonly) NSView *attributionView;

#pragma mark Manipulating the Viewpoint

/**
 The geographic coordinate at the center of the map view.

 Changing the value of this property centers the map on the new coordinate
 without changing the current zoom level.

 Changing the value of this property updates the map view immediately. If you
 want to animate the change, use the `-setCenterCoordinate:animated:` method
 instead.
 */
@property (nonatomic) CLLocationCoordinate2D centerCoordinate;

/**
 Changes the center coordinate of the map and optionally animates the change.

 Changing the center coordinate centers the map on the new coordinate without
 changing the current zoom level.

 @param coordinate The new center coordinate for the map.
 @param animated Specify `YES` if you want the map view to scroll to the new
    location or `NO` if you want the map to display the new location
    immediately.
 */
- (void)setCenterCoordinate:(CLLocationCoordinate2D)coordinate animated:(BOOL)animated;

/**
 The zoom level of the receiver.

 In addition to affecting the visual size and detail of features on the map, the
 zoom level affects the size of the vector tiles that are loaded. At zoom level
 0, each tile covers the entire world map; at zoom level 1, it covers ¼ of the
 world; at zoom level 2, <sup>1</sup>⁄<sub>16</sub> of the world, and so on.

 Changing the value of this property updates the map view immediately. If you
 want to animate the change, use the `-setZoomLevel:animated:` method instead.
 */
@property (nonatomic) double zoomLevel;

/**
 The minimum zoom level at which the map can be shown.

 Depending on the map view’s aspect ratio, the map view may be prevented from
 reaching the minimum zoom level, in order to keep the map from repeating within
 the current viewport.

 If the value of this property is greater than that of the `maximumZoomLevel`
 property, the behavior is undefined.

 The default value of this property is 0.
 */
@property (nonatomic) IBInspectable double minimumZoomLevel;

/**
 The maximum zoom level the map can be shown at.

 If the value of this property is smaller than that of the `minimumZoomLevel`
 property, the behavior is undefined.

 The default value of this property is 22. The upper bound for this property
 is 25.5.
 */
@property (nonatomic) IBInspectable double maximumZoomLevel;

/**
 Changes the zoom level of the map and optionally animates the change.

 Changing the zoom level scales the map without changing the current center
 coordinate.

 @param zoomLevel The new zoom level for the map.
 @param animated Specify `YES` if you want the map view to animate the change
    to the new zoom level or `NO` if you want the map to display the new zoom
    level immediately.
 */
- (void)setZoomLevel:(double)zoomLevel animated:(BOOL)animated;

/**
 The heading of the map, measured in degrees clockwise from true north.

 The value `0` means that the top edge of the map view corresponds to true
 north. The value `90` means the top of the map is pointing due east. The value
 `180` means the top of the map points due south, and so on.

 Changing the value of this property updates the map view immediately. If you
 want to animate the change, use the `-setDirection:animated:` method instead.
 */
@property (nonatomic) CLLocationDirection direction;

/**
 Changes the heading of the map and optionally animates the change.

 Changing the heading rotates the map without changing the current center
 coordinate or zoom level.

 @param direction The heading of the map, measured in degrees clockwise from
    true north.
 @param animated Specify `YES` if you want the map view to animate the change
    to the new heading or `NO` if you want the map to display the new heading
    immediately.
 */
- (void)setDirection:(CLLocationDirection)direction animated:(BOOL)animated;

/**
 A camera representing the current viewpoint of the map.
 */
@property (nonatomic, copy) MGLMapCamera *camera;

/**
 Moves the viewpoint to a different location with respect to the map with an
 optional transition animation.

 @param camera The new viewpoint.
 @param animated Specify `YES` if you want the map view to animate the change to
    the new viewpoint or `NO` if you want the map to display the new viewpoint
    immediately.
 */
- (void)setCamera:(MGLMapCamera *)camera animated:(BOOL)animated;

/**
 Moves the viewpoint to a different location with respect to the map with an
 optional transition duration and timing function.

 @param camera The new viewpoint.
 @param duration The amount of time, measured in seconds, that the transition
    animation should take. Specify `0` to jump to the new viewpoint
    instantaneously.
 @param function A timing function used for the animation. Set this parameter to
    `nil` for a transition that matches most system animations. If the duration
    is `0`, this parameter is ignored.
 @param completion The block to execute after the animation finishes.
 */
- (void)setCamera:(MGLMapCamera *)camera withDuration:(NSTimeInterval)duration animationTimingFunction:(nullable CAMediaTimingFunction *)function completionHandler:(nullable void (^)(void))completion;

 /**
 Moves the viewpoint to a different location with respect to the map with an
 optional transition duration and timing function.
 
 @param camera The new viewpoint.
 @param duration The amount of time, measured in seconds, that the transition
 animation should take. Specify `0` to jump to the new viewpoint
 instantaneously.
 @param function A timing function used for the animation. Set this parameter to
 `nil` for a transition that matches most system animations. If the duration
 is `0`, this parameter is ignored.
 @param edgePadding The minimum padding (in screen points) that would be visible
 around the returned camera object if it were set as the receiver’s camera.
  @param completion The block to execute after the animation finishes.
 */
- (void)setCamera:(MGLMapCamera *)camera withDuration:(NSTimeInterval)duration animationTimingFunction:(nullable CAMediaTimingFunction *)function edgePadding:(NSEdgeInsets)edgePadding completionHandler:(nullable void (^)(void))completion;


/**
 Moves the viewpoint to a different location using a transition animation that
 evokes powered flight and a default duration based on the length of the flight
 path.

 The transition animation seamlessly incorporates zooming and panning to help
 the user find his or her bearings even after traversing a great distance.

 @param camera The new viewpoint.
 @param completion The block to execute after the animation finishes.
 */
- (void)flyToCamera:(MGLMapCamera *)camera completionHandler:(nullable void (^)(void))completion;

/**
 Moves the viewpoint to a different location using a transition animation that
 evokes powered flight and an optional transition duration.

 The transition animation seamlessly incorporates zooming and panning to help
 the user find his or her bearings even after traversing a great distance.

 @param camera The new viewpoint.
 @param duration The amount of time, measured in seconds, that the transition
    animation should take. Specify `0` to jump to the new viewpoint
    instantaneously. Specify a negative value to use the default duration, which
    is based on the length of the flight path.
 @param completion The block to execute after the animation finishes.
 */
- (void)flyToCamera:(MGLMapCamera *)camera withDuration:(NSTimeInterval)duration completionHandler:(nullable void (^)(void))completion;

/**
 Moves the viewpoint to a different location using a transition animation that
 evokes powered flight and an optional transition duration and peak altitude.

 The transition animation seamlessly incorporates zooming and panning to help
 the user find his or her bearings even after traversing a great distance.

 @param camera The new viewpoint.
 @param duration The amount of time, measured in seconds, that the transition
    animation should take. Specify `0` to jump to the new viewpoint
    instantaneously. Specify a negative value to use the default duration, which
    is based on the length of the flight path.
 @param peakAltitude The altitude, measured in meters, at the midpoint of the
    animation. The value of this parameter is ignored if it is negative or if
    the animation transition resulting from a similar call to
    `-setCamera:animated:` would have a midpoint at a higher altitude.
 @param completion The block to execute after the animation finishes.
 */
- (void)flyToCamera:(MGLMapCamera *)camera withDuration:(NSTimeInterval)duration peakAltitude:(CLLocationDistance)peakAltitude completionHandler:(nullable void (^)(void))completion;

/**
 The geographic coordinate bounds visible in the receiver’s viewport.

 Changing the value of this property updates the receiver immediately. If you
 want to animate the change, use the `-setVisibleCoordinateBounds:animated:`
 method instead.
 
 If a longitude is less than −180 degrees or greater than 180 degrees, the visible
 bounds straddles the antimeridian or international date line.
 
 For example, a visible bounds that stretches from Tokyo to San Francisco would have
 coordinates of (35.68476, -220.24257) and (37.78428, -122.41310).
 */
@property (nonatomic) MGLCoordinateBounds visibleCoordinateBounds;

/**
 Changes the receiver’s viewport to fit the given coordinate bounds, optionally
 animating the change.
 
 To make the visible bounds go across the antimeridian or international date line,
 specify some longitudes less than −180 degrees or greater than 180 degrees.
 
 For example, a visible bounds that stretches from Tokyo to San Francisco would have
 coordinates of (35.68476, -220.24257) and (37.78428, -122.41310).

 @param bounds The bounds that the viewport will show in its entirety.
 @param animated Specify `YES` to animate the change by smoothly scrolling and
    zooming or `NO` to immediately display the given bounds.
 */
- (void)setVisibleCoordinateBounds:(MGLCoordinateBounds)bounds animated:(BOOL)animated;

/**
 Changes the receiver’s viewport to fit the given coordinate bounds and
 optionally some additional padding on each side.

 @param bounds The bounds that the viewport will show in its entirety.
 @param insets The minimum padding (in screen points) that will be visible
    around the given coordinate bounds.
 @param animated Specify `YES` to animate the change by smoothly scrolling and
    zooming or `NO` to immediately display the given bounds.
 */
- (void)setVisibleCoordinateBounds:(MGLCoordinateBounds)bounds edgePadding:(NSEdgeInsets)insets animated:(BOOL)animated;

/**
 Sets the visible region so that the map displays the specified annotations.

 Calling this method updates the value in the `visibleCoordinateBounds` property
 and potentially other properties to reflect the new map region. A small amount
 of padding is reserved around the edges of the map view. To specify a different
 amount of padding, use the `-showAnnotations:edgePadding:animated:` method.

 @param annotations The annotations that you want to be visible in the map.
 @param animated `YES` if you want the map region change to be animated, or `NO`
 if you want the map to display the new region immediately without animations.
 */
- (void)showAnnotations:(NS_ARRAY_OF(id <MGLAnnotation>) *)annotations animated:(BOOL)animated;

/**
 Sets the visible region so that the map displays the specified annotations with
 the specified amount of padding on each side.

 Calling this method updates the value in the `visibleCoordinateBounds` property
 and potentially other properties to reflect the new map region.

 @param annotations The annotations that you want to be visible in the map.
 @param insets The minimum padding (in screen points) around the edges of the
 map view to keep clear of annotations.
 @param animated `YES` if you want the map region change to be animated, or `NO`
 if you want the map to display the new region immediately without animations.
 */
- (void)showAnnotations:(NS_ARRAY_OF(id <MGLAnnotation>) *)annotations edgePadding:(NSEdgeInsets)insets animated:(BOOL)animated;

/**
 Returns the camera that best fits the given coordinate bounds.

 @param bounds The coordinate bounds to fit to the receiver’s viewport.
 @return A camera object centered on the same location as the coordinate bounds
    with zoom level as high (close to the ground) as possible while still
    including the entire coordinate bounds. The camera object uses the current
    direction and pitch.
 */
- (MGLMapCamera *)cameraThatFitsCoordinateBounds:(MGLCoordinateBounds)bounds;

/**
 Returns the camera that best fits the given coordinate bounds, optionally with
 some additional padding on each side.

 @param bounds The coordinate bounds to fit to the receiver’s viewport.
 @param insets The minimum padding (in screen points) that would be visible
    around the returned camera object if it were set as the receiver’s camera.
 @return A camera object centered on the same location as the coordinate bounds
    with zoom level as high (close to the ground) as possible while still
    including the entire coordinate bounds. The camera object uses the current
    direction and pitch.
 */
- (MGLMapCamera *)cameraThatFitsCoordinateBounds:(MGLCoordinateBounds)bounds edgePadding:(NSEdgeInsets)insets;

/**
 Returns the camera that best fits the given shape, with the specified direction,
 optionally with some additional padding on each side.

 @param shape The shape to fit to the receiver’s viewport.
 @param direction The direction of the viewport, measured in degrees clockwise from true north.
 @param insets The minimum padding (in screen points) that would be visible
    around the returned camera object if it were set as the receiver’s camera.
 @return A camera object centered on the shape's center with zoom level as high 
    (close to the ground) as possible while still including the entire shape. The
    camera object uses the current pitch.
 */
- (MGLMapCamera *)cameraThatFitsShape:(MGLShape *)shape direction:(CLLocationDirection)direction edgePadding:(NSEdgeInsets)insets;

/**
 A Boolean value indicating whether the receiver automatically adjusts its
 content insets.

 When the value of this property is `YES`, the map view automatically updates
 its `contentInsets` property to account for any overlapping title bar or
 toolbar. To overlap with the title bar or toolbar, the containing window’s
 style mask must have `NSFullSizeContentViewWindowMask` set, and the title bar
 must not be transparent.

 The default value of this property is `YES`.
 */
@property (nonatomic, assign) BOOL automaticallyAdjustsContentInsets;

/**
 The distance from the edges of the map view’s frame to the edges of the map
 view’s logical viewport.

 When the value of this property is equal to `NSEdgeInsetsZero`, viewport
 properties such as `centerCoordinate` assume a viewport that matches the map
 view’s frame. Otherwise, those properties are inset, excluding part of the
 frame from the viewport. For instance, if the only the top edge is inset, the
 map center is effectively shifted downward.

 When the value of the `automaticallyAdjustsContentInsets` property is `YES`,
 the value of this property may be overridden at any time.

 Changing the value of this property updates the map view immediately. If you
 want to animate the change, use the `-setContentInsets:animated:` method
 instead.
 */
@property (nonatomic, assign) NSEdgeInsets contentInsets;

/**
 Sets the distance from the edges of the map view’s frame to the edges of the
 map view’s logical viewport, with an optional transition animation.

 When the value of this property is equal to `NSEdgeInsetsZero`, viewport
 properties such as `centerCoordinate` assume a viewport that matches the map
 view’s frame. Otherwise, those properties are inset, excluding part of the
 frame from the viewport. For instance, if the only the top edge is inset, the
 map center is effectively shifted downward.

 When the value of the `automaticallyAdjustsContentInsets` property is `YES`,
 the value of this property may be overridden at any time.

 @param contentInsets The new values to inset the content by.
 @param animated Specify `YES` if you want the map view to animate the change to
    the content insets or `NO` if you want the map to inset the content
    immediately.
 */
- (void)setContentInsets:(NSEdgeInsets)contentInsets animated:(BOOL)animated;

#pragma mark Configuring How the User Interacts with the Map

/**
 A Boolean value that determines whether the user may zoom the map in and out,
 changing the zoom level.

 When this property is set to `YES`, the default, the user may zoom the map in
 and out by pinching two fingers, by using a scroll wheel on a traditional
 mouse, or by dragging the mouse cursor up and down while holding down the Shift
 key. When the receiver has focus, the user may also zoom by pressing the up and
 down arrow keys while holding down the Option key.

 This property controls only user interactions with the map. If you set the
 value of this property to `NO`, you may still change the map zoom
 programmatically.
 */
@property (nonatomic, getter=isZoomEnabled) BOOL zoomEnabled;

/**
 A Boolean value that determines whether the user may scroll around the map,
 changing the center coordinate.

 When this property is set to `YES`, the default, the user may scroll the map by
 swiping with two fingers or dragging the mouse cursor. When the receiver has
 focus, the user may also scroll around the map by pressing the arrow keys.

 This property controls only user interactions with the map. If you set the
 value of this property to `NO`, you may still change the map location
 programmatically.
 */
@property (nonatomic, getter=isScrollEnabled) BOOL scrollEnabled;

/**
 A Boolean value that determines whether the user may rotate the map, changing
 the direction.

 When this property is set to `YES`, the default, the user may rotate the map by
 moving two fingers in a circular motion or by dragging the mouse cursor left
 and right while holding down the Option key. When the receiver has focus, the
 user may also zoom by pressing the left and right arrow keys while holding down
 the Option key.

 This property controls only user interactions with the map. If you set the
 value of this property to `NO`, you may still rotate the map programmatically.
 */
@property (nonatomic, getter=isRotateEnabled) BOOL rotateEnabled;

/**
 A Boolean value that determines whether the user may tilt of the map, changing
 the pitch.

 When this property is set to `YES`, the default, the user may rotate the map by
 dragging the mouse cursor up and down while holding down the Option key.

 This property controls only user interactions with the map. If you set the
 value of this property to `NO`, you may still change the pitch of the map
 programmatically.
 */
@property (nonatomic, getter=isPitchEnabled) BOOL pitchEnabled;

#pragma mark Annotating the Map

/**
 The complete list of annotations associated with the receiver. (read-only)

 The objects in this array must adopt the `MGLAnnotation` protocol. If no
 annotations are associated with the map view, the value of this property is
 `nil`.
 */
@property (nonatomic, readonly, nullable) NS_ARRAY_OF(id <MGLAnnotation>) *annotations;

/**
 Adds an annotation to the map view.

 @note `MGLMultiPolyline`, `MGLMultiPolygon`, and `MGLShapeCollection` objects
    cannot be added to the map view at this time. Nor can `MGLMultiPoint`
    objects that are not instances of `MGLPolyline` or `MGLPolygon`. Any
    multipoint, multipolyline, multipolygon, or shape collection object that is
    specified is silently ignored.

 @param annotation The annotation object to add to the receiver. This object
    must conform to the `MGLAnnotation` protocol. The map view retains the
    annotation object.
 */
- (void)addAnnotation:(id <MGLAnnotation>)annotation;

/**
 Adds an array of annotations to the map view.

 @note `MGLMultiPolyline`, `MGLMultiPolygon`, and `MGLShapeCollection` objects
    cannot be added to the map view at this time. Nor can `MGLMultiPoint`
    objects that are not instances of `MGLPolyline` or `MGLPolygon`. Any
    multipoint, multipolyline, multipolygon, or shape collection objects that
    are specified are silently ignored.

 @param annotations An array of annotation objects. Each object in the array
    must conform to the `MGLAnnotation` protocol. The map view retains each
    individual annotation object.
 */
- (void)addAnnotations:(NS_ARRAY_OF(id <MGLAnnotation>) *)annotations;

/**
 The complete list of annotations associated with the receiver that are
 currently visible.

 The objects in this array must adopt the `MGLAnnotation` protocol. If no
 annotations are associated with the map view or if no annotations associated
 with the map view are currently visible, the value of this property is `nil`.
 */
@property (nonatomic, readonly, nullable) NS_ARRAY_OF(id <MGLAnnotation>) *visibleAnnotations;

/**
 Removes an annotation from the map view, deselecting it if it is selected.

 Removing an annotation object dissociates it from the map view entirely,
 preventing it from being displayed on the map. Thus you would typically call
 this method only when you want to hide or delete a given annotation.

 @param annotation The annotation object to remove. This object must conform to
    the `MGLAnnotation` protocol.
 */
- (void)removeAnnotation:(id <MGLAnnotation>)annotation;

/**
 Removes an array of annotations from the map view, deselecting any selected
 annotations in the array.

 Removing annotation objects dissociates them from the map view entirely,
 preventing them from being displayed on the map. Thus you would typically call
 this method only when you want to hide or delete the given annotations.

 @param annotations The array of annotation objects to remove. Objects in the
    array must conform to the `MGLAnnotation` protocol.
 */
- (void)removeAnnotations:(NS_ARRAY_OF(id <MGLAnnotation>) *)annotations;

/**
 Returns a reusable annotation image object associated with its identifier.

 For performance reasons, you should generally reuse `MGLAnnotationImage`
 objects for identical-looking annotations in your map views. Dequeueing saves
 time and memory during performance-critical operations such as scrolling.

 @param identifier A string identifying the annotation image to be reused. This
    string is the same one you specify when initially returning the annotation
    image object using the `-mapView:imageForAnnotation:` method.
 @return An annotation image object with the given identifier, or `nil` if no
    such object exists in the reuse queue.
 */
- (nullable MGLAnnotationImage *)dequeueReusableAnnotationImageWithIdentifier:(NSString *)identifier;

/**
 Returns the list of annotations associated with the receiver that intersect with
 the given rectangle.

 @param rect A rectangle expressed in the map view’s coordinate system.
 @return An array of objects that adopt the `MGLAnnotation` protocol or `nil` if
 no annotations associated with the map view are currently visible in the
 rectangle.
 */
- (nullable NS_ARRAY_OF(id <MGLAnnotation>) *)visibleAnnotationsInRect:(CGRect)rect;

#pragma mark Managing Annotation Selections

/**
 The currently selected annotations.

 Assigning a new array to this property selects only the first annotation in the
 array.
 */
@property (nonatomic, copy) NS_ARRAY_OF(id <MGLAnnotation>) *selectedAnnotations;

/**
 Selects an annotation and displays a callout popover for it.

 If the given annotation is not visible within the current viewport, this method
 has no effect.

 @param annotation The annotation object to select.
 */
- (void)selectAnnotation:(id <MGLAnnotation>)annotation;

/**
 Deselects an annotation and hides its callout popover.

 @param annotation The annotation object to deselect.
 */
- (void)deselectAnnotation:(nullable id <MGLAnnotation>)annotation;

/**
 A common view controller for managing a callout popover’s content view.

 Like any instance of `NSPopover`, an annotation callout manages its contents
 with a view controller. The annotation object is the view controller’s
 represented object. This means that you can bind controls in the view
 controller’s content view to KVO-compliant properties of the annotation object,
 such as `title` and `subtitle`.

 This property defines a common view controller that is used for every
 annotation’s callout view. If you set this property to `nil`, a default view
 controller will be used that manages a simple title label and subtitle label.
 If you need distinct view controllers for different annotations, the map view’s
 delegate should implement `-mapView:calloutViewControllerForAnnotation:`
 instead.
 */
@property (nonatomic, strong, null_resettable) IBOutlet NSViewController *calloutViewController;

#pragma mark Finding Annotations

/**
 Returns a point annotation located at the given point.

 @param point A point in the view’s coordinate system.
 @return A point annotation whose annotation image coincides with the point. If
    multiple point annotations coincide with the point, the return value is the
    annotation that would be selected if the user clicks at this point.
 */
- (id <MGLAnnotation>)annotationAtPoint:(NSPoint)point;

#pragma mark Overlaying the Map

/**
 The complete list of overlays associated with the receiver. (read-only)

 The objects in this array must adopt the `MGLOverlay` protocol. If no
 overlays are associated with the map view, the value of this property is
 empty array.
 */
@property (nonatomic, readonly, nonnull) NS_ARRAY_OF(id <MGLOverlay>) *overlays;

/**
 Adds a single overlay to the map.

 To remove an overlay from a map, use the `-removeOverlay:` method.

 @param overlay The overlay object to add. This object must conform to the
     `MGLOverlay` protocol.
 */
- (void)addOverlay:(id <MGLOverlay>)overlay;

/**
 Adds an array of overlays to the map.

 To remove multiple overlays from a map, use the `-removeOverlays:` method.

 @param overlays An array of objects, each of which must conform to the
     `MGLOverlay` protocol.
 */
- (void)addOverlays:(NS_ARRAY_OF(id <MGLOverlay>) *)overlays;

/**
 Removes a single overlay from the map.

 If the specified overlay is not currently associated with the map view, this
 method does nothing.

 @param overlay The overlay object to remove.
 */
- (void)removeOverlay:(id <MGLOverlay>)overlay;

/**
 Removes an array of overlays from the map.

 If a given overlay object is not associated with the map view, it is ignored.

 @param overlays An array of objects, each of which conforms to the `MGLOverlay`
     protocol.
 */
- (void)removeOverlays:(NS_ARRAY_OF(id <MGLOverlay>) *)overlays;

#pragma mark Accessing the Underlying Map Data

/**
 Returns an array of rendered map features that intersect with a given point.

 This method may return features from any of the map’s style layers. To restrict
 the search to a particular layer or layers, use the
 `-visibleFeaturesAtPoint:inStyleLayersWithIdentifiers:` method. For more
 information about searching for map features, see that method’s documentation.

 @param point A point expressed in the map view’s coordinate system.
 @return An array of objects conforming to the `MGLFeature` protocol that
    represent features in the sources used by the current style.
 */
- (NS_ARRAY_OF(id <MGLFeature>) *)visibleFeaturesAtPoint:(NSPoint)point NS_SWIFT_NAME(visibleFeatures(at:));

/**
 Returns an array of rendered map features that intersect with a given point,
 restricted to the given style layers.

 This method returns all the intersecting features from the specified layers. To
 filter the returned features, use the
 `-visibleFeaturesAtPoint:inStyleLayersWithIdentifiers:predicate:` method. For
 more information about searching for map features, see that method’s
 documentation.

 @param point A point expressed in the map view’s coordinate system.
 @param styleLayerIdentifiers A set of strings that correspond to the names of
    layers defined in the current style. Only the features contained in these
    layers are included in the returned array.
 @return An array of objects conforming to the `MGLFeature` protocol that
    represent features in the sources used by the current style.
 */
- (NS_ARRAY_OF(id <MGLFeature>) *)visibleFeaturesAtPoint:(NSPoint)point inStyleLayersWithIdentifiers:(nullable NS_SET_OF(NSString *) *)styleLayerIdentifiers NS_SWIFT_NAME(visibleFeatures(at:styleLayerIdentifiers:));

/**
 Returns an array of rendered map features that intersect with a given point,
 restricted to the given style layers and filtered by the given predicate.
 
 Each object in the returned array represents a feature rendered by the
 current style and provides access to attributes specified by the relevant map
 content sources. The returned array includes features loaded by
 `MGLShapeSource` and `MGLVectorSource` objects but does not include anything
 from `MGLRasterSource` objects, or from image, video, or canvas sources, which
 are unsupported by this SDK.

 The returned features are drawn by a style layer in the current style. For
 example, suppose the current style uses the
 <a href="https://www.mapbox.com/vector-tiles/mapbox-streets/">Mapbox Streets source</a>,
 but none of the specified style layers includes features that have the `maki`
 property set to `bus`. If you pass a point corresponding to the location of a
 bus stop into this method, the bus stop feature does not appear in the
 resulting array. On the other hand, if the style does include bus stops, an
 `MGLFeature` object representing that bus stop is returned and its
 `attributes` dictionary has the `maki` key set to `bus` (along with other
 attributes). The dictionary contains only the attributes provided by the
 tile source; it does not include computed attribute values or rules about how
 the feature is rendered by the current style.

 The returned array is sorted by z-order, starting with the topmost rendered
 feature and ending with the bottommost rendered feature. A feature that is
 rendered multiple times due to wrapping across the antimeridian at low zoom
 levels is included only once, subject to the caveat that follows.

 Features come from tiled vector data or GeoJSON data that is converted to tiles
 internally, so feature geometries are clipped at tile boundaries and features
 may appear duplicated across tiles. For example, suppose the specified point
 lies along a road that spans the screen. The resulting array includes those
 parts of the road that lie within the map tile that contain the specified
 point, even if the road extends into other tiles.

 To find out the layer names in a particular style, view the style in
 <a href="https://www.mapbox.com/studio/">Mapbox Studio</a>.
 
 Only visible features are returned. To obtain features regardless of
 visibility, use the
 `-[MGLVectorSource featuresInSourceLayersWithIdentifiers:predicate:]` and
 `-[MGLShapeSource featuresMatchingPredicate:]` methods on the relevant sources.

 @note Layer identifiers are not guaranteed to exist across styles or different
    versions of the same style. Applications that use this API must first set
    the style URL to an explicitly versioned style using a convenience method
    like `+[MGLStyle outdoorsStyleURLWithVersion:]`, `MGLMapView`’s “Style URL”
    inspectable in Interface Builder, or a manually constructed `NSURL`. This
    approach also avoids layer identifer name changes that will occur in the
    default style’s layers over time.
 
 @param point A point expressed in the map view’s coordinate system.
 @param styleLayerIdentifiers A set of strings that correspond to the names of
    layers defined in the current style. Only the features contained in these
    layers are included in the returned array.
 @param predicate A predicate to filter the returned features.
 @return An array of objects conforming to the `MGLFeature` protocol that
    represent features in the sources used by the current style.
 */
- (NS_ARRAY_OF(id <MGLFeature>) *)visibleFeaturesAtPoint:(NSPoint)point inStyleLayersWithIdentifiers:(nullable NS_SET_OF(NSString *) *)styleLayerIdentifiers predicate:(nullable NSPredicate *)predicate NS_SWIFT_NAME(visibleFeatures(at:styleLayerIdentifiers:predicate:));

/**
 Returns an array of rendered map features that intersect with the given
 rectangle.

 This method may return features from any of the map’s style layers. To restrict
 the search to a particular layer or layers, use the
 `-visibleFeaturesAtPoint:inStyleLayersWithIdentifiers:` method. For more
 information about searching for map features, see that method’s documentation.

 @param rect A rectangle expressed in the map view’s coordinate system.
 @return An array of objects conforming to the `MGLFeature` protocol that
    represent features in the sources used by the current style.
 */
- (NS_ARRAY_OF(id <MGLFeature>) *)visibleFeaturesInRect:(NSRect)rect NS_SWIFT_NAME(visibleFeatures(in:));

/**
 Returns an array of rendered map features that intersect with the given
 rectangle, restricted to the given style layers.
 
 This method returns all the intersecting features from the specified layers. To
 filter the returned features, use the
 `-visibleFeaturesAtPoint:inStyleLayersWithIdentifiers:predicate:` method. For
 more information about searching for map features, see that method’s
 documentation.

 @param rect A rectangle expressed in the map view’s coordinate system.
 @param styleLayerIdentifiers A set of strings that correspond to the names of
    layers defined in the current style. Only the features contained in these
    layers are included in the returned array.
 @return An array of objects conforming to the `MGLFeature` protocol that
    represent features in the sources used by the current style.
 */
- (NS_ARRAY_OF(id <MGLFeature>) *)visibleFeaturesInRect:(NSRect)rect inStyleLayersWithIdentifiers:(nullable NS_SET_OF(NSString *) *)styleLayerIdentifiers NS_SWIFT_NAME(visibleFeatures(at:styleLayerIdentifiers:));

/**
 Returns an array of rendered map features that intersect with the given
 rectangle, restricted to the given style layers and filtered by the given
 predicate.
 
 Each object in the returned array represents a feature rendered by the
 current style and provides access to attributes specified by the relevant map
 content sources. The returned array includes features loaded by
 `MGLShapeSource` and `MGLVectorSource` objects but does not include anything
 from `MGLRasterSource` objects, or from image, video, or canvas sources, which
 are unsupported by this SDK.

 The returned features are drawn by a style layer in the current style. For
 example, suppose the current style uses the
 <a href="https://www.mapbox.com/vector-tiles/mapbox-streets/">Mapbox Streets source</a>,
 but none of the specified style layers includes features that have the `maki`
 property set to `bus`. If you pass a rectangle containing the location of a bus
 stop into this method, the bus stop feature does not appear in the resulting
 array. On the other hand, if the style does include bus stops, an `MGLFeature`
 object representing that bus stop is returned and its `attributes` dictionary
 has the `maki` key set to `bus` (along with other attributes). The dictionary
 contains only the attributes provided by the tile source; it does not include
 computed attribute values or rules about how the feature is rendered by the
 current style.

 The returned array is sorted by z-order, starting with the topmost rendered
 feature and ending with the bottommost rendered feature. A feature that is
 rendered multiple times due to wrapping across the antimeridian at low zoom
 levels is included only once, subject to the caveat that follows.

 Features come from tiled vector data or GeoJSON data that is converted to tiles
 internally, so feature geometries are clipped at tile boundaries and features
 may appear duplicated across tiles. For example, suppose the specified
 rectangle intersects with a road that spans the screen. The resulting array
 includes those parts of the road that lie within the map tiles covering the
 specified rectangle, even if the road extends into other tiles. The portion of
 the road within each map tile is included individually.

 To find out the layer names in a particular style, view the style in
 <a href="https://www.mapbox.com/studio/">Mapbox Studio</a>.
 
 Only visible features are returned. To obtain features regardless of
 visibility, use the
 `-[MGLVectorSource featuresInSourceLayersWithIdentifiers:predicate:]` and
 `-[MGLShapeSource featuresMatchingPredicate:]` methods on the relevant sources.

 @note Layer identifiers are not guaranteed to exist across styles or different
    versions of the same style. Applications that use this API must first set
    the style URL to an explicitly versioned style using a convenience method
    like `+[MGLStyle outdoorsStyleURLWithVersion:]`, `MGLMapView`’s “Style URL”
    inspectable in Interface Builder, or a manually constructed `NSURL`. This
    approach also avoids layer identifer name changes that will occur in the
    default style’s layers over time.
 
 @param rect A rectangle expressed in the map view’s coordinate system.
 @param styleLayerIdentifiers A set of strings that correspond to the names of
    layers defined in the current style. Only the features contained in these
    layers are included in the returned array.
 @param predicate A predicate to filter the returned features.
 @return An array of objects conforming to the `MGLFeature` protocol that
    represent features in the sources used by the current style.
 */
- (NS_ARRAY_OF(id <MGLFeature>) *)visibleFeaturesInRect:(NSRect)rect inStyleLayersWithIdentifiers:(nullable NS_SET_OF(NSString *) *)styleLayerIdentifiers predicate:(nullable NSPredicate *)predicate NS_SWIFT_NAME(visibleFeatures(in:styleLayerIdentifiers:predicate:));

#pragma mark Converting Geographic Coordinates

/**
 Converts a geographic coordinate to a point in the given view’s coordinate
 system.

 @param coordinate The geographic coordinate to convert.
 @param view The view in whose coordinate system the returned point should be
    expressed. If this parameter is `nil`, the returned point is expressed in
    the window’s coordinate system. If `view` is not `nil`, it must belong to
    the same window as the map view.
 @return The point (in the appropriate view or window coordinate system)
    corresponding to the given geographic coordinate.
 */
- (NSPoint)convertCoordinate:(CLLocationCoordinate2D)coordinate toPointToView:(nullable NSView *)view;

/**
 Converts a point in the given view’s coordinate system to a geographic
 coordinate.

 @param point The point to convert.
 @param view The view in whose coordinate system the point is expressed.
 @return The geographic coordinate at the given point.
 */
- (CLLocationCoordinate2D)convertPoint:(NSPoint)point toCoordinateFromView:(nullable NSView *)view;

/**
 Converts a geographic bounding box to a rectangle in the given view’s
 coordinate system.

 @param bounds The geographic bounding box to convert.
 @param view The view in whose coordinate system the returned rectangle should
    be expressed. If this parameter is `nil`, the returned rectangle is
    expressed in the window’s coordinate system. If `view` is not `nil`, it must
    belong to the same window as the map view.
 */
- (NSRect)convertCoordinateBounds:(MGLCoordinateBounds)bounds toRectToView:(nullable NSView *)view;

/**
 Converts a rectangle in the given view’s coordinate system to a geographic
 bounding box.

 @param rect The rectangle to convert.
 @param view The view in whose coordinate system the rectangle is expressed.
 @return The geographic bounding box coextensive with the given rectangle.
 */
- (MGLCoordinateBounds)convertRect:(NSRect)rect toCoordinateBoundsFromView:(nullable NSView *)view;

/**
 Returns the distance spanned by one point in the map view’s coordinate system
 at the given latitude and current zoom level.

 The distance between points decreases as the latitude approaches the poles.
 This relationship parallels the relationship between longitudinal coordinates
 at different latitudes.

 @param latitude The latitude of the geographic coordinate represented by the
    point.
 @return The distance in meters spanned by a single point.
 */
- (CLLocationDistance)metersPerPointAtLatitude:(CLLocationDegrees)latitude;

#pragma mark Giving Feedback to Improve the Map

/**
 Opens one or more webpages in the default Web browser in which the user can
 provide feedback about the map data.

 You should add a menu item to the Help menu of your application that invokes
 this method. Title it “Improve This Map” or similar. Set its target to the
 first responder and its action to `giveFeedback:`.

 This map view searches the current style’s sources for webpages to open.
 Specifically, each source’s tile set has an `attribution` property containing
 HTML code; if an <code>&lt;a></code> tag (link) within that code has an
 <code>class</code> attribute set to <code>mapbox-improve-map</code>, its
 <code>href</code> attribute defines the URL to open. Such links are omitted
 from the attribution view.
 */
- (IBAction)giveFeedback:(id)sender;

#pragma mark Debugging the Map

/**
 The options that determine which debugging aids are shown on the map.

 These options are all disabled by default and should remain disabled in
 released software for performance and aesthetic reasons.
 */
@property (nonatomic) MGLMapDebugMaskOptions debugMask;

@end

NS_ASSUME_NONNULL_END
