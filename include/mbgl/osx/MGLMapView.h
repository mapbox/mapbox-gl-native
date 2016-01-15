#import <Cocoa/Cocoa.h>
#import <CoreLocation/CoreLocation.h>

#import "MGLGeometry.h"

NS_ASSUME_NONNULL_BEGIN

/** Options for enabling debugging features in an MGLMapView instance. */
typedef NS_OPTIONS(NSUInteger, MGLMapDebugMaskOptions) {
    /** Edges of tile boundaries are shown as thick, red lines to help diagnose
        tile clipping issues. */
    MGLMapDebugTileBoundariesMask = 1 << 1,
    
    /** Each tile shows its tile coordinate (x/y/z) in the upper-left corner. */
    MGLMapDebugTileInfoMask = 1 << 2,
    
    /** Each tile shows a timestamp indicating when it was loaded. */
    MGLMapDebugTimestampsMask = 1 << 3,
    
    /** Edges of glyphs and symbols are shown as faint, green lines to help
        diagnose collision and label placement issues. */
    MGLMapDebugCollisionBoxesMask = 1 << 4,
};

@class MGLAnnotationImage;
@class MGLMapCamera;

@protocol MGLAnnotation;
@protocol MGLMapViewDelegate;
@protocol MGLOverlay;

/** An interactive, customizable map view with an interface similar to the one
    provided by Apple’s MapKit.
    
    Using MGLMapView, you can embed the map inside a view, allow users to
    manipulate it with standard gestures, animate the map between different
    viewpoints, and present information in the form of annotations and overlays.
    
    The map view loads scalable vector tiles that conform to the
    [Mapbox Vector Tile Specification](https://github.com/mapbox/vector-tile-spec).
    It styles them with a style that conforms to the
    [Mapbox GL style specification](https://www.mapbox.com/mapbox-gl-style-spec/).
    Such styles can be designed in [Mapbox Studio](https://www.mapbox.com/studio/)
    and hosted on mapbox.com.
    
    A collection of Mapbox-hosted styles is available through the MGLStyle
    class. These basic styles use
    [Mapbox Streets](https://www.mapbox.com/developers/vector-tiles/mapbox-streets)
    or [Mapbox Satellite](https://www.mapbox.com/satellite/) data sources, but
    you can specify a custom style that makes use of your own data.
    
    Mapbox-hosted vector tiles and styles require an API access token, which you
    can obtain from the
    [Mapbox account page](https://www.mapbox.com/studio/account/tokens/). Access
    tokens associate requests to Mapbox’s vector tile and style APIs with your
    Mapbox account. They also deter other developers from using your styles
    without your permission.
    
    @note You are responsible for getting permission to use the map data and for
        ensuring that your use adheres to the relevant terms of use. */
IB_DESIGNABLE
@interface MGLMapView : NSView

#pragma mark Creating instances
/** @name Creating Instances */

/** Initializes and returns a newly allocated map view with the specified frame
    and the default style.
    
    @param frame The frame for the view, measured in points.
    @return An initialized map view. */
- (instancetype)initWithFrame:(NSRect)frame;

/** Initializes and returns a newly allocated map view with the specified frame
    and style URL.
    
    @param frame The frame for the view, measured in points.
    @param styleURL URL of the map style to display. The URL may be a full HTTP
        or HTTPS URL, a Mapbox URL indicating the style’s map ID
        (`mapbox://styles/<user>/<style>`), or a path to a local file relative
        to the application’s resource path. Specify `nil` for the default style.
    @return An initialized map view. */
- (instancetype)initWithFrame:(NSRect)frame styleURL:(nullable NSURL *)styleURL;

#pragma mark Accessing the delegate
/** @name Accessing the Delegate */

/** The receiver’s delegate.
    
    A map view sends messages to its delegate to notify it of changes to its
    contents or the viewpoint. The delegate also provides information about
    annotations displayed on the map, such as the styles to apply to individual
    annotations. */
@property (nonatomic, weak, nullable) IBOutlet id <MGLMapViewDelegate> delegate;

#pragma mark Configuring the map’s appearance
/** @name Configuring the Map’s Appearance */


/** URL of the style currently displayed in the receiver.
    
    The URL may be a full HTTP or HTTPS URL, a Mapbox URL indicating the style’s
    map ID (`mapbox://styles/<user>/<style>`), or a path to a local file
    relative to the application’s resource path.
    
    If you set this property to `nil`, the receiver will use the default style
    and this property will automatically be set to that style’s URL. */
@property (nonatomic, null_resettable) NSURL *styleURL;

/** Reloads the style.
 
    You do not normally need to call this method. The map view automatically
    responds to changes in network connectivity by reloading the style. You may
    need to call this method if you change the access token after a style has
    loaded but before loading a style associated with a different Mapbox
    account. */
- (IBAction)reloadStyle:(id)sender;

/** A control for zooming in and out, positioned in the lower-right corner. */
@property (nonatomic, readonly) NSSegmentedControl *zoomControls;

/** A control indicating the map’s direction and allowing the user to manipulate
    the direction, positioned above the zoom controls in the lower-right corner.
 */
@property (nonatomic, readonly) NSSlider *compass;

/** The Mapbox logo, positioned in the lower-left corner.
    
    @note The Mapbox terms of service, which governs the use of Mapbox-hosted
        vector tiles and styles,
        [requires](https://www.mapbox.com/help/mapbox-logo/) most Mapbox
        customers to display the Mapbox logo. If this applies to you, do not
        hide this view or change its contents. */
@property (nonatomic, readonly) NSImageView *logoView;

/** A view showing legally required copyright notices, positioned along the
    bottom of the map view, to the left of the Mapbox logo.
    
    @note The Mapbox terms of service, which governs the use of Mapbox-hosted
        vector tiles and styles,
        [requires](https://www.mapbox.com/help/attribution/) these copyright
        notices to accompany any map that features Mapbox-designed styles,
        OpenStreetMap data, or other Mapbox data such as satellite or terrain
        data. If that applies to this map view, do not hide this view or remove
        any notices from it. */
@property (nonatomic, readonly) NSView *attributionView;

#pragma mark Manipulating the viewpoint
/** @name Manipulating the Viewpoint */

/** The geographic coordinate at the center of the map view.
    
    Changing the value of this property centers the map on the new coordinate
    without changing the current zoom level.
    
    Changing the value of this property updates the map view immediately. If you
    want to animate the change, use the -setCenterCoordinate:animated: method
    instead. */
@property (nonatomic) CLLocationCoordinate2D centerCoordinate;

/** Changes the center coordinate of the map and optionally animates the change.
    
    Changing the center coordinate centers the map on the new coordinate without
    changing the current zoom level.
    
    @param coordinate The new center coordinate for the map.
    @param animated Specify `YES` if you want the map view to scroll to the new
        location or `NO` if you want the map to display the new location
        immediately. */
- (void)setCenterCoordinate:(CLLocationCoordinate2D)coordinate animated:(BOOL)animated;

/** The zoom level of the receiver.
    
    In addition to affecting the visual size and detail of features on the map,
    the zoom level affects the size of the vector tiles that are loaded. At zoom
    level 0, each tile covers the entire world map; at zoom level 1, it covers ¼
    of the world; at zoom level 2, <sup>1</sup>⁄<sub>16</sub> of the world, and
    so on.
    
    Changing the value of this property updates the map view immediately. If you
    want to animate the change, use the -setZoomLevel:animated: method instead.
 */
@property (nonatomic) double zoomLevel;

/** The minimum zoom level that can be displayed by the receiver using the
    current style. */
@property (nonatomic, readonly) double maximumZoomLevel;

/** The maximum zoom level that can be displayed by the receiver using the
    current style. */
@property (nonatomic, readonly) double minimumZoomLevel;

/** Changes the zoom level of the map and optionally animates the change.
    
    Changing the zoom level scales the map without changing the current center
    coordinate.
    
    @param zoomLevel The new zoom level for the map.
    @param animated Specify `YES` if you want the map view to animate the change
        to the new zoom level or `NO` if you want the map to display the new
        zoom level immediately. */
- (void)setZoomLevel:(double)zoomLevel animated:(BOOL)animated;

/** The heading of the map, measured in degrees clockwise from true north.
    
    The value `0` means that the top edge of the map view corresponds to true
    north. The value `90` means the top of the map is pointing due east. The
    value `180` means the top of the map points due south, and so on.
    
    Changing the value of this property updates the map view immediately. If you
    want to animate the change, use the -setDirection:animated: method instead.
 */
@property (nonatomic) CLLocationDirection direction;

/** Changes the heading of the map and optionally animates the change.
    
    @param direction The heading of the map, measured in degrees clockwise from
        true north.
    @param animated Specify `YES` if you want the map view to animate the change
        to the new heading or `NO` if you want the map to display the new
        heading immediately.
    
    Changing the heading rotates the map without changing the current center
    coordinate or zoom level. */
- (void)setDirection:(CLLocationDirection)direction animated:(BOOL)animated;

/** A camera representing the current viewpoint of the map. */
@property (nonatomic, copy) MGLMapCamera *camera;

/** Moves the viewpoint to a different location with respect to the map with an
    optional transition animation.
    
    @param camera The new viewpoint.
    @param animated Specify `YES` if you want the map view to animate the change
        to the new viewpoint or `NO` if you want the map to display the new
        viewpoint immediately. */
- (void)setCamera:(MGLMapCamera *)camera animated:(BOOL)animated;

/** Moves the viewpoint to a different location with respect to the map with an
    optional transition duration and timing function.
    
    @param camera The new viewpoint.
    @param duration The amount of time, measured in seconds, that the transition
        animation should take. Specify `0` to jump to the new viewpoint
        instantaneously.
    @param function A timing function used for the animation. Set this parameter
        to `nil` for a transition that matches most system animations. If the
        duration is `0`, this parameter is ignored.
    @param completion The block to execute after the animation finishes. */
- (void)setCamera:(MGLMapCamera *)camera withDuration:(NSTimeInterval)duration animationTimingFunction:(nullable CAMediaTimingFunction *)function completionHandler:(nullable void (^)(void))completion;

/** Moves the viewpoint to a different location using a transition animation
    that evokes powered flight and a default duration based on the length of the
    flight path.
    
    The transition animation seamlessly incorporates zooming and panning to help
    the user find his or her bearings even after traversing a great distance.
    
    @param camera The new viewpoint.
    @param completion The block to execute after the animation finishes. */
- (void)flyToCamera:(MGLMapCamera *)camera completionHandler:(nullable void (^)(void))completion;

/** Moves the viewpoint to a different location using a transition animation
    that evokes powered flight and an optional transition duration.
    
    The transition animation seamlessly incorporates zooming and panning to help
    the user find his or her bearings even after traversing a great distance.
    
    @param camera The new viewpoint.
    @param duration The amount of time, measured in seconds, that the transition
        animation should take. Specify `0` to jump to the new viewpoint
        instantaneously. Specify a negative value to use the default duration,
        which is based on the length of the flight path.
    @param completion The block to execute after the animation finishes. */
- (void)flyToCamera:(MGLMapCamera *)camera withDuration:(NSTimeInterval)duration completionHandler:(nullable void (^)(void))completion;

/** Moves the viewpoint to a different location using a transition animation
    that evokes powered flight and an optional transition duration and peak
    altitude.
    
    The transition animation seamlessly incorporates zooming and panning to help
    the user find his or her bearings even after traversing a great distance.
    
    @param camera The new viewpoint.
    @param duration The amount of time, measured in seconds, that the transition
        animation should take. Specify `0` to jump to the new viewpoint
        instantaneously. Specify a negative value to use the default duration,
        which is based on the length of the flight path.
    @param peakAltitude The altitude, measured in meters, at the midpoint of the
        animation. The value of this parameter is ignored if it is negative or
        if the animation transition resulting from a similar call to
        `-setCamera:animated:` would have a midpoint at a higher altitude.
    @param completion The block to execute after the animation finishes. */
- (void)flyToCamera:(MGLMapCamera *)camera withDuration:(NSTimeInterval)duration peakAltitude:(CLLocationDistance)peakAltitude completionHandler:(nullable void (^)(void))completion;

/** The geographic coordinate bounds visible in the receiver’s viewport.
    
    Changing the value of this property updates the receiver immediately. If you
    want to animate the change, use the -setVisibleCoordinateBounds:animated:
    method instead. */
@property (nonatomic) MGLCoordinateBounds visibleCoordinateBounds;

/** Changes the receiver’s viewport to fit the given coordinate bounds,
    optionally animating the change.
    
    @param bounds The bounds that the viewport will show in its entirety.
    @param animated Specify `YES` to animate the change by smoothly scrolling
        and zooming or `NO` to immediately display the given bounds. */
- (void)setVisibleCoordinateBounds:(MGLCoordinateBounds)bounds animated:(BOOL)animated;

#pragma mark Configuring gesture recognition
/** @name Configuring How the User Interacts with the Map */

/** A Boolean value that determines whether the user may zoom the map in and
    out, changing the zoom level.
    
    When this property is set to `YES`, the default, the user may zoom the map
    in and out by pinching two fingers, by using a scroll wheel on a
    traditional mouse, or by dragging the mouse cursor up and down while holding
    down the Shift key. When the receiver has focus, the user may also zoom by
    pressing the up and down arrow keys while holding down the Option key.
    
    This property controls only user interactions with the map. If you set the
    value of this property to `NO`, you may still change the map zoom
    programmatically. */
@property (nonatomic, getter=isZoomEnabled) BOOL zoomEnabled;

/** A Boolean value that determines whether the user may scroll around the map,
    changing the center coordinate.
    
    When this property is set to `YES`, the default, the user may scroll the map
    by swiping with two fingers or dragging the mouse cursor. When the receiver
    has focus, the user may also scroll around the map by pressing the arrow
    keys.
    
    This property controls only user interactions with the map. If you set the
    value of this property to `NO`, you may still change the map location
    programmatically. */
@property (nonatomic, getter=isScrollEnabled) BOOL scrollEnabled;

/** A Boolean value that determines whether the user may rotate the map,
    changing the direction.
    
    When this property is set to `YES`, the default, the user may rotate the map
    by moving two fingers in a circular motion or by dragging the mouse cursor
    left and right while holding down the Option key. When the receiver has
    focus, the user may also zoom by pressing the left and right arrow keys
    while holding down the Option key.
    
    This property controls only user interactions with the map. If you set the
    value of this property to `NO`, you may still rotate the map
    programmatically. */
@property (nonatomic, getter=isRotateEnabled) BOOL rotateEnabled;

/** A Boolean value that determines whether the user may tilt of the map,
    changing the pitch.
    
    When this property is set to `YES`, the default, the user may rotate the map
    by dragging the mouse cursor up and down while holding down the Option key.
    
    This property controls only user interactions with the map. If you set the
    value of this property to `NO`, you may still change the pitch of the map
    programmatically. */
@property (nonatomic, getter=isPitchEnabled) BOOL pitchEnabled;

#pragma mark Annotating the map
/** @name Annotating the Map */

/** The complete list of annotations associated with the receiver. (read-only)
    
    The objects in this array must adopt the MGLAnnotation protocol. If no
    annotations are associated with the map view, the value of this property is
    `nil`. */
@property (nonatomic, readonly, nullable) NS_ARRAY_OF(id <MGLAnnotation>) *annotations;

/** Adds an annotation to the map view.
    
    @param annotation The annotation object to add to the receiver. This object
        must conform to the MGLAnnotation protocol. The map view retains the
        annotation object. */
- (void)addAnnotation:(id <MGLAnnotation>)annotation;

/** Adds an array of annotations to the map view.
    
    @param annotations An array of annotation objects. Each object in the array
        must conform to the MGLAnnotation protocol. The map view retains each
        individual annotation object. */
- (void)addAnnotations:(NS_ARRAY_OF(id <MGLAnnotation>) *)annotations;

/** Removes an annotation from the map view, deselecting it if it is selected.
    
    Removing an annotation object dissociates it from the map view entirely,
    preventing it from being displayed on the map. Thus you would typically call
    this method only when you want to hide or delete a given annotation.
    
    @param annotation The annotation object to remove. This object must conform
        to the MGLAnnotation protocol. */
- (void)removeAnnotation:(id <MGLAnnotation>)annotation;

/** Removes an array of annotations from the map view, deselecting any selected
    annotations in the array.
    
    Removing annotation objects dissociates them from the map view entirely,
    preventing them from being displayed on the map. Thus you would typically
    call this method only when you want to hide or delete the given annotations.
    
    @param annotations The array of annotation objects to remove. Objects in the
        array must conform to the MGLAnnotation protocol. */
- (void)removeAnnotations:(NS_ARRAY_OF(id <MGLAnnotation>) *)annotations;

/** Returns a reusable annotation image object associated with its identifier.
    
    For performance reasons, you should generally reuse MGLAnnotationImage
    objects for identical-looking annotations in your map views. Dequeueing
    saves time and memory during performance-critical operations such as
    scrolling.
    
    @param identifier A string identifying the annotation image to be reused.
        This string is the same one you specify when initially returning the
        annotation image object using the -mapView:imageForAnnotation: method.
    @return An annotation image object with the given identifier, or `nil` if no
        such object exists in the reuse queue. */
- (nullable MGLAnnotationImage *)dequeueReusableAnnotationImageWithIdentifier:(NSString *)identifier;

#pragma mark Managing annotation selections
/** @name Managing Annotation Selections */

/** The currently selected annotations.
    
    Assigning a new array to this property selects only the first annotation in
    the array. */
@property (nonatomic, copy) NS_ARRAY_OF(id <MGLAnnotation>) *selectedAnnotations;

/** Selects an annotation and displays a callout popover for it.
    
    If the given annotation is not visible within the current viewport, this
    method has no effect.
    
    @param annotation The annotation object to select. */
- (void)selectAnnotation:(id <MGLAnnotation>)annotation;

/** Deselects an annotation and hides its callout popover.
    
    @param annotation The annotation object to deselect. */
- (void)deselectAnnotation:(nullable id <MGLAnnotation>)annotation;

/** A common view controller for managing a callout popover’s content view.
    
    Like any instance of NSPopover, an annotation callout manages its contents
    with a view controller. The annotation object is the view controller’s
    represented object. This means that you can bind controls in the view
    controller’s content view to KVO-compliant properties of the annotation
    object, such as -title and -subtitle.
    
    This property defines a common view controller that is used for every
    annotation’s callout view. If you set this property to `nil`, a default
    view controller will be used that manages a simple title label and subtitle
    label. If you need distinct view controllers for different annotations, the
    map view’s delegate should implement
    -mapView:calloutViewControllerForAnnotation: instead. */
@property (nonatomic, strong, null_resettable) IBOutlet NSViewController *calloutViewController;

#pragma mark Finding annotations
/** @name Finding Annotations */

/** Returns a point annotation located at the given point.
    
    @param point A point in the view’s coordinate system.
    @return A point annotation whose annotation image coincides with the point.
        If multiple point annotations coincide with the point, the return value
        is the annotation that would be selected if the user clicks at this
        point.
 */
- (id <MGLAnnotation>)annotationAtPoint:(NSPoint)point;

#pragma mark Overlaying the map
/** @name Overlaying the Map */

/** Adds a single overlay to the map.
    
    To remove an overlay from a map, use the -removeOverlay: method.
    
    @param overlay The overlay object to add. This object must conform to the
        MGLOverlay protocol. */
- (void)addOverlay:(id <MGLOverlay>)overlay;

/** Adds an array of overlays to the map.
    
    To remove multiple overlays from a map, use the -removeOverlays: method.
    
    @param overlays An array of objects, each of which must conform to the
        MGLOverlay protocol. */
- (void)addOverlays:(NS_ARRAY_OF(id <MGLOverlay>) *)overlays;

/** Removes a single overlay from the map.
    
    If the specified overlay is not currently associated with the map view, this
    method does nothing.
    
    @param overlay The overlay object to remove. */
- (void)removeOverlay:(id <MGLOverlay>)overlay;

/** Removes an array of overlays from the map.
    
    If a given overlay object is not associated with the map view, it is
    ignored.
    
    @param overlays An array of objects, each of which conforms to the
        MGLOverlay protocol. */
- (void)removeOverlays:(NS_ARRAY_OF(id <MGLOverlay>) *)overlays;

#pragma mark Converting geographic coordinates
/** @name Converting Geographic Coordinates */

/** Converts a geographic coordinate to a point in the given view’s coordinate
    system.
    
    @param coordinate The geographic coordinate to convert.
    @param view The view in whose coordinate system the returned point should be
        expressed. If this parameter is `nil`, the returned point is expressed
        in the window’s coordinate system. If `view` is not `nil`, it must
        belong to the same window as the map view.
    @return The point (in the appropriate view or window coordinate system)
        corresponding to the given geographic coordinate. */
- (NSPoint)convertCoordinate:(CLLocationCoordinate2D)coordinate toPointToView:(nullable NSView *)view;

/** Converts a point in the given view’s coordinate system to a geographic
    coordinate.
    
    @param point The point to convert.
    @param view The view in whose coordinate system the point is expressed.
    @return The geographic coordinate at the given point. */
- (CLLocationCoordinate2D)convertPoint:(NSPoint)point toCoordinateFromView:(nullable NSView *)view;

/** Converts a geographic bounding box to a rectangle in the given view’s
    coordinate system.
    
    @param bounds The geographic bounding box to convert.
    @param view The view in whose coordinate system the returned rectangle
        should be expressed. If this parameter is `nil`, the returned rectangle
        is expressed in the window’s coordinate system. If `view` is not `nil`,
        it must belong to the same window as the map view. */
- (NSRect)convertCoordinateBounds:(MGLCoordinateBounds)bounds toRectToView:(nullable NSView *)view;

/** Converts a rectangle in the given view’s coordinate system to a geographic
    bounding box.
    
    @param rect The rectangle to convert.
    @param view The view in whose coordinate system the rectangle is expressed.
    @return The geographic bounding box coextensive with the given rectangle. */
- (MGLCoordinateBounds)convertRect:(NSRect)rect toCoordinateBoundsFromView:(nullable NSView *)view;

/** Returns the distance spanned by one point in the map view’s coordinate
    system at the given latitude and current zoom level.
    
    The distance between points decreases as the latitude approaches the poles.
    This relationship parallels the relationship between longitudinal
    coordinates at different latitudes.
    
    @param latitude The latitude of the geographic coordinate represented by the
        point.
    @return The distance in meters spanned by a single point. */
- (CLLocationDistance)metersPerPointAtLatitude:(CLLocationDegrees)latitude;

#pragma mark Debugging the map
/** @name Debugging the Map */

/** The options that determine which debugging aids are shown on the map.
    
    These options are all disabled by default and should remain disabled in
    released software for performance and aesthetic reasons. */
@property (nonatomic) MGLMapDebugMaskOptions debugMask;

@end

NS_ASSUME_NONNULL_END
