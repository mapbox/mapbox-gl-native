#import "MGLGeometry.h"
#import "MGLMapCamera.h"

#import <UIKit/UIKit.h>
#import <CoreLocation/CoreLocation.h>

#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

@class MGLAnnotationImage;
@class MGLUserLocation;
@class MGLPolyline;
@class MGLPolygon;
@class MGLShape;

@protocol MGLMapViewDelegate;
@protocol MGLAnnotation;
@protocol MGLOverlay;
@protocol MGLCalloutView;

/** The vertical alignment of an annotation within a map view. */
typedef NS_ENUM(NSUInteger, MGLAnnotationVerticalAlignment) {
    /** Aligns the annotation vertically in the center of the map view. */
    MGLAnnotationVerticalAlignmentCenter = 0,
    /** Aligns the annotation vertically at the top of the map view. */
    MGLAnnotationVerticalAlignmentTop,
    /** Aligns the annotation vertically at the bottom of the map view. */
    MGLAnnotationVerticalAlignmentBottom,
};

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

/**
 An interactive, customizable map view with an interface similar to the one
 provided by Apple's MapKit.
 
 Using `MGLMapView`, you can embed the map inside a view, allow users to
 manipulate it with standard gestures, animate the map between different
 viewpoints, and present information in the form of annotations and overlays.
 
 The map view loads scalable vector tiles that conform to the
 [Mapbox Vector Tile Specification](https://github.com/mapbox/vector-tile-spec).
 It styles them with a style that conforms to the
 [Mapbox GL style specification](https://www.mapbox.com/mapbox-gl-style-spec/).
 Such styles can be designed in [Mapbox Studio](https://www.mapbox.com/studio/)
 and hosted on mapbox.com.
 
 A collection of Mapbox-hosted styles is available through the `MGLStyle`
 class. These basic styles use
 [Mapbox Streets](https://www.mapbox.com/developers/vector-tiles/mapbox-streets)
 or [Mapbox Satellite](https://www.mapbox.com/satellite/) data sources, but
 you can specify a custom style that makes use of your own data.
 
 Mapbox-hosted vector tiles and styles require an API access token, which you
 can obtain from the
 [Mapbox account page](https://www.mapbox.com/studio/account/tokens/). Access
 tokens associate requests to Mapbox's vector tile and style APIs with your
 Mapbox account. They also deter other developers from using your styles
 without your permission.
 
 @note You are responsible for getting permission to use the map data and for
 ensuring that your use adheres to the relevant terms of use.
 */
IB_DESIGNABLE
@interface MGLMapView : UIView

#pragma mark Creating Instances

/**
 Initializes and returns a newly allocated map view with the specified frame
 and the default style.
 
 @param frame The frame for the view, measured in points.
 @return An initialized map view.
 */
- (instancetype)initWithFrame:(CGRect)frame;

/**
 Initializes and returns a newly allocated map view with the specified frame
 and style URL.
 
 @param frame The frame for the view, measured in points.
 @param styleURL URL of the map style to display. The URL may be a full HTTP
    or HTTPS URL, a Mapbox URL indicating the style's map ID
    (`mapbox://styles/{user}/{style}`), or a path to a local file relative
    to the application's resource path. Specify `nil` for the default style.
 @return An initialized map view.
 */
- (instancetype)initWithFrame:(CGRect)frame styleURL:(nullable NSURL *)styleURL;

#pragma mark Accessing the Delegate

/**
 The receiver's delegate.
 
 A map view sends messages to its delegate to notify it of changes to its
 contents or the viewpoint. The delegate also provides information about
 annotations displayed on the map, such as the styles to apply to individual
 annotations.
 */
@property(nonatomic, weak, nullable) IBOutlet id<MGLMapViewDelegate> delegate;

#pragma mark Configuring the Map's Appearance

/**
 URLs of the styles bundled with the library.
 
 @deprecated Call the relevant class method of `MGLStyle` for the URL of a
    particular default style.
 */
@property (nonatomic, readonly) NS_ARRAY_OF(NSURL *) *bundledStyleURLs __attribute__((deprecated("Call the relevant class method of MGLStyle for the URL of a particular default style.")));

/**
 URL of the style currently displayed in the receiver.
 
 The URL may be a full HTTP or HTTPS URL, a Mapbox URL indicating the style's
 map ID (`mapbox://styles/{user}/{style}`), or a path to a local file
 relative to the application's resource path.
 
 If you set this property to `nil`, the receiver will use the default style
 and this property will automatically be set to that style's URL.
 */
@property (nonatomic, null_resettable) NSURL *styleURL;

/**
 A control indicating the map's direction and allowing the user to manipulate
 the direction, positioned in the upper-right corner.
 */
@property (nonatomic, readonly) UIImageView *compassView;

/**
 The Mapbox logo, positioned in the lower-left corner.
 
 @note The Mapbox terms of service, which governs the use of Mapbox-hosted
    vector tiles and styles,
    [requires](https://www.mapbox.com/help/mapbox-logo/) most Mapbox
    customers to display the Mapbox logo. If this applies to you, do not
    hide this view or change its contents.
 */
@property (nonatomic, readonly) UIImageView *logoView;

/** 
 A view showing legally required copyright notices and telemetry settings,
 positioned at the bottom-right of the map view.
 
 @note The Mapbox terms of service, which governs the use of Mapbox-hosted
    vector tiles and styles,
    [requires](https://www.mapbox.com/help/attribution/) these copyright
    notices to accompany any map that features Mapbox-designed styles,
    OpenStreetMap data, or other Mapbox data such as satellite or terrain
    data. If that applies to this map view, do not hide this view or remove
    any notices from it.

 @note You are additionally
    [required](https://www.mapbox.com/help/metrics-opt-out-for-users/) to
    provide users with the option to disable anonymous usage and location
    sharing (telemetry). If this view is hidden, you must implement this
    setting elsewhere in your app or via `Settings.bundle`. See our
    [website](https://www.mapbox.com/ios-sdk/#telemetry_opt_out) for
    implementation help.
 */
@property (nonatomic, readonly) UIButton *attributionButton;

/** 
 Currently active style classes, represented as an array of string identifiers.
 */
@property (nonatomic) NS_ARRAY_OF(NSString *) *styleClasses;

/**
 Returns a Boolean value indicating whether the style class with the given
 identifier is currently active.
 
 @param styleClass The style class to query for.
 @return Whether the style class is currently active.
 */
- (BOOL)hasStyleClass:(NSString *)styleClass;

/**
 Activates the style class with the given identifier.
 
 @param styleClass The style class to activate.
 */
- (void)addStyleClass:(NSString *)styleClass;

/**
 Deactivates the style class with the given identifier.
 
 @param styleClass The style class to deactivate.
 */
- (void)removeStyleClass:(NSString *)styleClass;

#pragma mark Displaying the User's Location

/**
 A Boolean value indicating whether the map may display the user location.
 
 Setting this property to `YES` causes the map view to use the Core Location
 framework to find the current location. As long as this property is `YES`, the
 map view continues to track the user's location and update it periodically.
 
 This property does not indicate whether the user's position is actually visible
 on the map, only whether the map view is allowed to display it. To determine
 whether the user's position is visible, use the `userLocationVisible` property.
 The default value of this property is `NO`.
 
 On iOS 8 and above, your app must specify a value for
 `NSLocationWhenInUseUsageDescription` or `NSLocationAlwaysUsageDescription` in
 its `Info.plist` to satisfy the requirements of the underlying Core Location
 framework when enabling this property.
 */
@property (nonatomic, assign) BOOL showsUserLocation;

/** 
 A Boolean value indicating whether the device's current location is visible in
 the map view.
 
 Use `showsUserLocation` to control the visibility of the on-screen user
 location annotation.
 */
@property (nonatomic, assign, readonly, getter=isUserLocationVisible) BOOL userLocationVisible;

/**
 Returns the annotation object indicating the user's current location.
 */
@property (nonatomic, readonly, nullable) MGLUserLocation *userLocation;

/** 
 The mode used to track the user location. The default value is
 `MGLUserTrackingModeNone`.
 
 Changing the value of this property updates the map view with an animated
 transition. If you don’t want to animate the change, use the
 `-setUserTrackingMode:animated:` method instead.
 */
@property (nonatomic, assign) MGLUserTrackingMode userTrackingMode;

/**
 Sets the mode used to track the user location, with an optional transition.
 
 @param mode The mode used to track the user location.
 @param animated If `YES`, there is an animated transition from the current
    viewport to a viewport that results from the change to `mode`. If `NO`, the
    map view instantaneously changes to the new viewport. This parameter only
    affects the initial transition; subsequent changes to the user location or
    heading are always animated.
 */
- (void)setUserTrackingMode:(MGLUserTrackingMode)mode animated:(BOOL)animated;

/**
 The vertical alignment of the user location annotation within the receiver. The
 default value is `MGLAnnotationVerticalAlignmentCenter`.
 
 Changing the value of this property updates the map view with an animated
 transition. If you don’t want to animate the change, use the
 `-setUserLocationVerticalAlignment:animated:` method instead.
 */
@property (nonatomic, assign) MGLAnnotationVerticalAlignment userLocationVerticalAlignment;

/**
 Sets the vertical alignment of the user location annotation within the
 receiver, with an optional transition.
 
 @param alignment The vertical alignment of the user location annotation.
 @param animated If `YES`, the user location annotation animates to its new
    position within the map view. If `NO`, the user location annotation
    instantaneously moves to its new position.
 */
- (void)setUserLocationVerticalAlignment:(MGLAnnotationVerticalAlignment)alignment animated:(BOOL)animated;

/**
 Whether the map view should display a heading calibration alert when necessary.
 The default value is `YES`.
 */
@property (nonatomic, assign) BOOL displayHeadingCalibration;

/**
 The geographic coordinate that is the subject of observation as the user
 location is being tracked.
 
 By default, this property is set to an invalid coordinate, indicating that
 there is no target. In course tracking mode, the target forms one of two foci
 in the viewport, the other being the user location annotation. Typically, this
 property is set to a destination or waypoint in a real-time navigation scene.
 As the user annotation moves toward the target, the map automatically zooms in
 to fit both foci optimally within the viewport.
 
 This property has no effect if the `userTrackingMode` property is set to a
 value other than `MGLUserTrackingModeFollowWithCourse`.
 
 Changing the value of this property updates the map view with an animated
 transition. If you don’t want to animate the change, use the
 `-setTargetCoordinate:animated:` method instead.
 */
@property (nonatomic, assign) CLLocationCoordinate2D targetCoordinate;

/**
 Sets the geographic coordinate that is the subject of observation as the user
 location is being tracked, with an optional transition animation.
 
 By default, the target coordinate is set to an invalid coordinate, indicating
 that there is no target. In course tracking mode, the target forms one of two
 foci in the viewport, the other being the user location annotation. Typically,
 the target is set to a destination or waypoint in a real-time navigation scene.
 As the user annotation moves toward the target, the map automatically zooms in
 to fit both foci optimally within the viewport.
 
 This method has no effect if the `userTrackingMode` property is set to a value
 other than `MGLUserTrackingModeFollowWithCourse`.
 
 @param targetCoordinate The target coordinate to fit within the viewport.
 @param animated If `YES`, the map animates to fit the target within the map
    view. If `NO`, the map fits the target instantaneously.
 */
- (void)setTargetCoordinate:(CLLocationCoordinate2D)targetCoordinate animated:(BOOL)animated;

#pragma mark Configuring How the User Interacts with the Map

/**
 A Boolean value that determines whether the user may zoom the map in and
 out, changing the zoom level.
 
 When this property is set to `YES`, the default, the user may zoom the map
 in and out by pinching two fingers or by double tapping, holding, and moving
 the finger up and down.
 
 This property controls only user interactions with the map. If you set the
 value of this property to `NO`, you may still change the map zoom
 programmatically.
 */
@property(nonatomic, getter=isZoomEnabled) BOOL zoomEnabled;

/**
 A Boolean value that determines whether the user may scroll around the map,
 changing the center coordinate.
 
 When this property is set to `YES`, the default, the user may scroll the map
 by dragging or swiping with one finger.
 
 This property controls only user interactions with the map. If you set the
 value of this property to `NO`, you may still change the map location
 programmatically.
 */
@property(nonatomic, getter=isScrollEnabled) BOOL scrollEnabled;

/**
 A Boolean value that determines whether the user may rotate the map,
 changing the direction.
 
 When this property is set to `YES`, the default, the user may rotate the map
 by moving two fingers in a circular motion.
 
 This property controls only user interactions with the map. If you set the
 value of this property to `NO`, you may still rotate the map
 programmatically.
 */
@property(nonatomic, getter=isRotateEnabled) BOOL rotateEnabled;

/**
 A Boolean value that determines whether the user may change the pitch (tilt) of
 the map.
 
 When this property is set to `YES`, the default, the user may tilt the map by
 vertically dragging two fingers.
 
 This property controls only user interactions with the map. If you set the
 value of this property to `NO`, you may still change the pitch of the map
 programmatically.
 
 The default value of this property is `YES`.
 */
@property(nonatomic, getter=isPitchEnabled) BOOL pitchEnabled;

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
 Changes the center coordinate and zoom level of the map and optionally animates
 the change.
 
 @param centerCoordinate The new center coordinate for the map.
 @param zoomLevel The new zoom level for the map.
 @param animated Specify `YES` if you want the map view to animate scrolling and
    zooming to the new location or `NO` if you want the map to display the new
    location immediately.
 */
- (void)setCenterCoordinate:(CLLocationCoordinate2D)centerCoordinate zoomLevel:(double)zoomLevel animated:(BOOL)animated;

/**
 Changes the center coordinate, zoom level, and direction of the map and
 optionally animates the change.
 
 @param centerCoordinate The new center coordinate for the map.
 @param zoomLevel The new zoom level for the map.
 @param direction The new direction for the map, measured in degrees relative to
    true north.
 @param animated Specify `YES` if you want the map view to animate scrolling,
    zooming, and rotating to the new location or `NO` if you want the map to
    display the new location immediately.
 */
- (void)setCenterCoordinate:(CLLocationCoordinate2D)centerCoordinate zoomLevel:(double)zoomLevel direction:(CLLocationDirection)direction animated:(BOOL)animated;

/**
 Changes the center coordinate, zoom level, and direction of the map, calling a
 completion handler at the end of an optional animation.
 
 @param centerCoordinate The new center coordinate for the map.
 @param zoomLevel The new zoom level for the map.
 @param direction The new direction for the map, measured in degrees relative to
    true north.
 @param animated Specify `YES` if you want the map view to animate scrolling,
    zooming, and rotating to the new location or `NO` if you want the map to
    display the new location immediately.
 @param completion The block executed after the animation finishes.
 */
- (void)setCenterCoordinate:(CLLocationCoordinate2D)centerCoordinate zoomLevel:(double)zoomLevel direction:(CLLocationDirection)direction animated:(BOOL)animated completionHandler:(nullable void (^)(void))completion;

/** The zoom level of the receiver.
 
 In addition to affecting the visual size and detail of features on the map,
 the zoom level affects the size of the vector tiles that are loaded. At zoom
 level 0, each tile covers the entire world map; at zoom level 1, it covers ¼
 of the world; at zoom level 2, <sup>1</sup>⁄<sub>16</sub> of the world, and
 so on.
 
 Changing the value of this property updates the map view immediately. If you
 want to animate the change, use the `-setZoomLevel:animated:` method instead.
 */
@property (nonatomic) double zoomLevel;

/**
 Changes the zoom level of the map and optionally animates the change.
 
 Changing the zoom level scales the map without changing the current center
 coordinate.
 
 @param zoomLevel The new zoom level for the map.
 @param animated Specify `YES` if you want the map view to animate the change
    to the new zoom level or `NO` if you want the map to display the new
    zoom level immediately.
 */
- (void)setZoomLevel:(double)zoomLevel animated:(BOOL)animated;

/**
 * The minimum zoom level at which the map can be shown.
 *
 * Depending on the map view’s aspect ratio, the map view may be prevented
 * from reaching the minimum zoom level, in order to keep the map from
 * repeating within the current viewport.
 *
 * If the value of this property is greater than that of the
 * maximumZoomLevel property, the behavior is undefined.
 *
 * The default minimumZoomLevel is 0.
 */
@property (nonatomic) double minimumZoomLevel;

/**
 * The maximum zoom level the map can be shown at.
 *
 * If the value of this property is smaller than that of the
 * minimumZoomLevel property, the behavior is undefined.
 *
 * The default maximumZoomLevel is 20.
 */
@property (nonatomic) double maximumZoomLevel;

/**
 The heading of the map, measured in degrees clockwise from true north.
 
 The value `0` means that the top edge of the map view corresponds to true
 north. The value `90` means the top of the map is pointing due east. The
 value `180` means the top of the map points due south, and so on.
 
 Changing the value of this property updates the map view immediately. If you
 want to animate the change, use the `-setDirection:animated:` method instead.
 */
@property (nonatomic) CLLocationDirection direction;

/**
 Changes the heading of the map and optionally animates the change.
 
 @param direction The heading of the map, measured in degrees clockwise from
    true north.
 @param animated Specify `YES` if you want the map view to animate the change
    to the new heading or `NO` if you want the map to display the new
    heading immediately.
 
 Changing the heading rotates the map without changing the current center
 coordinate or zoom level.
 */
- (void)setDirection:(CLLocationDirection)direction animated:(BOOL)animated;

/**
 Resets the map rotation to a northern heading — a `direction` of `0` degrees.
 */
- (IBAction)resetNorth;

/**
 The coordinate bounds visible in the receiver's viewport.
 
 Changing the value of this property updates the receiver immediately. If you
 want to animate the change, call `-setVisibleCoordinateBounds:animated:`
 instead.
 */
@property (nonatomic) MGLCoordinateBounds visibleCoordinateBounds;

/**
 Changes the receiver’s viewport to fit the given coordinate bounds,
 optionally animating the change.
 
 @param bounds The bounds that the viewport will show in its entirety.
 @param animated Specify `YES` to animate the change by smoothly scrolling
    and zooming or `NO` to immediately display the given bounds.
 */
- (void)setVisibleCoordinateBounds:(MGLCoordinateBounds)bounds animated:(BOOL)animated;

/**
 Changes the receiver's viewport to fit the given coordinate bounds and
 optionally some additional padding on each side.
 
 @param bounds The bounds that the viewport will show in its entirety.
 @param insets The minimum padding (in screen points) that will be visible
    around the given coordinate bounds.
 @param animated Specify `YES` to animate the change by smoothly scrolling and
    zooming or `NO` to immediately display the given bounds.
 */
- (void)setVisibleCoordinateBounds:(MGLCoordinateBounds)bounds edgePadding:(UIEdgeInsets)insets animated:(BOOL)animated;

/**
 Changes the receiver's viewport to fit all of the given coordinates and
 optionally some additional padding on each side.
 
 @param coordinates The coordinates that the viewport will show.
 @param count The number of coordinates. This number must not be greater than
    the number of elements in `coordinates`.
 @param insets The minimum padding (in screen points) that will be visible
    around the given coordinate bounds.
 @param animated Specify `YES` to animate the change by smoothly scrolling and
    zooming or `NO` to immediately display the given bounds.
 */
- (void)setVisibleCoordinates:(CLLocationCoordinate2D *)coordinates count:(NSUInteger)count edgePadding:(UIEdgeInsets)insets animated:(BOOL)animated;

/**
 Changes the receiver's viewport to fit all of the given coordinates and
 optionally some additional padding on each side.
 
 @param coordinates The coordinates that the viewport will show.
 @param count The number of coordinates. This number must not be greater than
    the number of elements in `coordinates`.
 @param insets The minimum padding (in screen points) that will be visible
    around the given coordinate bounds.
 @param direction The direction to rotate the map to, measured in degrees
    relative to true north.
 @param duration The duration to animate the change in seconds.
 @param function The timing function to animate the change.
 @param completion The block executed after the animation finishes.
 */
- (void)setVisibleCoordinates:(CLLocationCoordinate2D *)coordinates count:(NSUInteger)count edgePadding:(UIEdgeInsets)insets direction:(CLLocationDirection)direction duration:(NSTimeInterval)duration animationTimingFunction:(nullable CAMediaTimingFunction *)function completionHandler:(nullable void (^)(void))completion;

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

 Calling this method updates the value in the visibleCoordinateBounds property
 and potentially other properties to reflect the new map region.

 @param annotations The annotations that you want to be visible in the map.
 @param insets The minimum padding (in screen points) around the edges of the
    map view to keep clear of annotations.
 @param animated `YES` if you want the map region change to be animated, or `NO`
    if you want the map to display the new region immediately without animations.
 */
- (void)showAnnotations:(NS_ARRAY_OF(id <MGLAnnotation>) *)annotations edgePadding:(UIEdgeInsets)insets animated:(BOOL)animated;

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
 */
- (void)setCamera:(MGLMapCamera *)camera withDuration:(NSTimeInterval)duration animationTimingFunction:(nullable CAMediaTimingFunction *)function;

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
 The distance from the edges of the map view’s frame to the edges of the map
 view’s logical viewport.
 
 When the value of this property is equal to `UIEdgeInsetsZero`, viewport
 properties such as `centerCoordinate` assume a viewport that matches the map
 view’s frame. Otherwise, those properties are inset, excluding part of the
 frame from the viewport. For instance, if the only the top edge is inset, the
 map center is effectively shifted downward.
 
 When the map view’s superview is an instance of `UIViewController` whose
 `automaticallyAdjustsScrollViewInsets` property is `YES`, the value of this
 property may be overridden at any time.
 
 Changing the value of this property updates the map view immediately. If you
 want to animate the change, use the `-setContentInset:animated:` method
 instead.
 */
@property (nonatomic, assign) UIEdgeInsets contentInset;

/**
 Sets the distance from the edges of the map view’s frame to the edges of the
 map view’s logical viewport with an optional transition animation.
 
 When the value of this property is equal to `UIEdgeInsetsZero`, viewport
 properties such as `centerCoordinate` assume a viewport that matches the map
 view’s frame. Otherwise, those properties are inset, excluding part of the
 frame from the viewport. For instance, if the only the top edge is inset, the
 map center is effectively shifted downward.
 
 When the map view’s superview is an instance of `UIViewController` whose
 `automaticallyAdjustsScrollViewInsets` property is `YES`, the value of this
 property may be overridden at any time.
 
 @param contentInset The new values to inset the content by.
 @param animated Specify `YES` if you want the map view to animate the change to
    the content inset or `NO` if you want the map to inset the content
    immediately.
 */
- (void)setContentInset:(UIEdgeInsets)contentInset animated:(BOOL)animated;

#pragma mark Converting Geographic Coordinates

/**
 Converts a point in the given view's coordinate system to a geographic
 coordinate.
 
 @param point The point to convert.
 @param view The view in whose coordinate system the point is expressed.
 @return The geographic coordinate at the given point.
 */
- (CLLocationCoordinate2D)convertPoint:(CGPoint)point toCoordinateFromView:(nullable UIView *)view;

/**
 Converts a geographic coordinate to a point in the given view's coordinate
 system.
 
 @param coordinate The geographic coordinate to convert.
 @param view The view in whose coordinate system the returned point should be
    expressed. If this parameter is `nil`, the returned point is expressed
    in the window's coordinate system. If `view` is not `nil`, it must
    belong to the same window as the map view.
 @return The point (in the appropriate view or window coordinate system)
    corresponding to the given geographic coordinate.
 */
- (CGPoint)convertCoordinate:(CLLocationCoordinate2D)coordinate toPointToView:(nullable UIView *)view;

/**
 Converts a rectangle in the given view’s coordinate system to a geographic
 bounding box.
 
 @param rect The rectangle to convert.
 @param view The view in whose coordinate system the rectangle is expressed.
 @return The geographic bounding box coextensive with the given rectangle.
 */
- (MGLCoordinateBounds)convertRect:(CGRect)rect toCoordinateBoundsFromView:(nullable UIView *)view;

/**
 Converts a geographic bounding box to a rectangle in the given view’s
 coordinate system.
 
 @param bounds The geographic bounding box to convert.
 @param view The view in whose coordinate system the returned rectangle should
    be expressed. If this parameter is `nil`, the returned rectangle is
    expressed in the window’s coordinate system. If `view` is not `nil`, it must
    belong to the same window as the map view.
 */
- (CGRect)convertCoordinateBounds:(MGLCoordinateBounds)bounds toRectToView:(nullable UIView *)view;

/**
 Returns the distance spanned by one point in the map view's coordinate system
 at the given latitude and current zoom level.
 
 The distance between points decreases as the latitude approaches the poles.
 This relationship parallels the relationship between longitudinal coordinates
 at different latitudes.
 
 @param latitude The latitude of the geographic coordinate represented by the
    point.
 @return The distance in meters spanned by a single point.
 */
- (CLLocationDistance)metersPerPointAtLatitude:(CLLocationDegrees)latitude;

- (CLLocationDistance)metersPerPixelAtLatitude:(CLLocationDegrees)latitude __attribute__((deprecated("Use -metersPerPointAtLatitude:.")));

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
 
 @param annotation The annotation object to add to the receiver. This object
    must conform to the `MGLAnnotation` protocol. The map view retains the
    annotation object. */
- (void)addAnnotation:(id <MGLAnnotation>)annotation;

/**
 Adds an array of annotations to the map view.
 
 @param annotations An array of annotation objects. Each object in the array
    must conform to the `MGLAnnotation` protocol. The map view retains each
    individual annotation object.
 */
- (void)addAnnotations:(NS_ARRAY_OF(id <MGLAnnotation>) *)annotations;

/**
 Removes an annotation from the map view, deselecting it if it is selected.
 
 Removing an annotation object dissociates it from the map view entirely,
 preventing it from being displayed on the map. Thus you would typically call
 this method only when you want to hide or delete a given annotation.
 
 @param annotation The annotation object to remove. This object must conform
    to the `MGLAnnotation` protocol
 */
- (void)removeAnnotation:(id <MGLAnnotation>)annotation;

/**
 Removes an array of annotations from the map view, deselecting any selected
 annotations in the array.
 
 Removing annotation objects dissociates them from the map view entirely,
 preventing them from being displayed on the map. Thus you would typically
 call this method only when you want to hide or delete the given annotations.
 
 @param annotations The array of annotation objects to remove. Objects in the
    array must conform to the `MGLAnnotation` protocol.
 */
- (void)removeAnnotations:(NS_ARRAY_OF(id <MGLAnnotation>) *)annotations;

/**
 Returns a reusable annotation image object associated with its identifier.
 
 For performance reasons, you should generally reuse `MGLAnnotationImage`
 objects for identical-looking annotations in your map views. Dequeueing
 saves time and memory during performance-critical operations such as
 scrolling.
 
 @param identifier A string identifying the annotation image to be reused.
    This string is the same one you specify when initially returning the
    annotation image object using the `-mapView:imageForAnnotation:` method.
 @return An annotation image object with the given identifier, or `nil` if no
    such object exists in the reuse queue.
 */
- (nullable MGLAnnotationImage *)dequeueReusableAnnotationImageWithIdentifier:(NSString *)identifier;

#pragma mark Managing Annotation Selections

/**
 The currently selected annotations.
 
 Assigning a new array to this property selects only the first annotation in
 the array.
 */
@property (nonatomic, copy) NS_ARRAY_OF(id <MGLAnnotation>) *selectedAnnotations;

/**
 Selects an annotation and displays a callout view for it.
 
 If the given annotation is not visible within the current viewport, this
 method has no effect.
 
 @param annotation The annotation object to select.
 @param animated If `YES`, the callout view is animated into position.
 */
- (void)selectAnnotation:(id <MGLAnnotation>)annotation animated:(BOOL)animated;

/**
 Deselects an annotation and hides its callout view.
 
 @param annotation The annotation object to deselect.
 @param animated If `YES`, the callout view is animated offscreen.
 */
- (void)deselectAnnotation:(nullable id <MGLAnnotation>)annotation animated:(BOOL)animated;

#pragma mark Overlaying the Map

/**
 Adds a single overlay object to the map.
 
 To remove an overlay from a map, use the `-removeOverlay:` method.
 
 @param overlay The overlay object to add. This object must conform to the
    `MGLOverlay` protocol. */
- (void)addOverlay:(id <MGLOverlay>)overlay;

/**
 Adds an array of overlay objects to the map.
 
 To remove multiple overlays from a map, use the `-removeOverlays:` method.
 
 @param overlays An array of objects, each of which must conform to the
    `MGLOverlay` protocol.
 */
- (void)addOverlays:(NS_ARRAY_OF(id <MGLOverlay>) *)overlays;

/**
 Removes a single overlay object from the map.
 
 If the specified overlay is not currently associated with the map view, this
 method does nothing.
 
 @param overlay The overlay object to remove.
 */
- (void)removeOverlay:(id <MGLOverlay>)overlay;

/**
 Removes one or more overlay objects from the map.
 
 If a given overlay object is not associated with the map view, it is ignored.
 
 @param overlays An array of objects, each of which conforms to the `MGLOverlay`
    protocol.
 */
- (void)removeOverlays:(NS_ARRAY_OF(id <MGLOverlay>) *)overlays;

#pragma mark Debugging the Map

/**
 The options that determine which debugging aids are shown on the map.
 
 These options are all disabled by default and should remain disabled in
 released software for performance and aesthetic reasons.
 */
@property (nonatomic) MGLMapDebugMaskOptions debugMask;

@property (nonatomic, getter=isDebugActive) BOOL debugActive __attribute__((deprecated("Use -debugMask and -setDebugMask:.")));

- (void)toggleDebug __attribute__((deprecated("Use -setDebugMask:.")));

/**
    Empties the in-memory tile cache.
 */
- (void)emptyMemoryCache;

/**
    Resets the map to the minimum zoom level, a center coordinate of (0, 0), and
    a northern heading.
 */
- (void)resetPosition;

@end

#pragma mark - MGLMapViewDelegate

/** The MGLMapViewDelegate protocol defines a set of optional methods that you can use to receive map-related update messages. Because many map operations require the `MGLMapView` class to load data asynchronously, the map view calls these methods to notify your application when specific operations complete. The map view also uses these methods to request annotation marker symbology and to manage interactions with those markers. */
@protocol MGLMapViewDelegate <NSObject>

@optional

#pragma mark Responding to Map Position Changes

/**
 Tells the delegate that the region displayed by the map view is about to change.
 
 This method is called whenever the currently displayed map region will start changing.
 
 @param mapView The map view whose visible region will change.
 @param animated Whether the change will cause an animated effect on the map.
 */
- (void)mapView:(MGLMapView *)mapView regionWillChangeAnimated:(BOOL)animated;

/**
 Tells the delegate that the region displayed by the map view is changing.
 
 This method is called whenever the currently displayed map region changes. During movement, this method may be called many times to report updates to the map position. Therefore, your implementation of this method should be as lightweight as possible to avoid affecting performance.
 
 @param mapView The map view whose visible region is changing.
 */
- (void)mapViewRegionIsChanging:(MGLMapView *)mapView;

/**
 Tells the delegate that the region displayed by the map view just changed.
 
 This method is called whenever the currently displayed map region has finished changing.
 
 @param mapView The map view whose visible region changed.
 @param animated Whether the change caused an animated effect on the map.
 */
- (void)mapView:(MGLMapView *)mapView regionDidChangeAnimated:(BOOL)animated;

#pragma mark Loading the Map

/**
 Tells the delegate that the map view will begin to load.
 
 This method is called whenever the map view starts loading, including when a new style has been set and the map must reload.
 
 @param mapView The map view that is starting to load.
 */
- (void)mapViewWillStartLoadingMap:(MGLMapView *)mapView;

/**
 Tells the delegate that the map view has finished loading.
 
 This method is called whenever the map view finishes loading, either after the initial load or after a style change has forced a reload.
 
 @param mapView The map view that has finished loading.
 */
- (void)mapViewDidFinishLoadingMap:(MGLMapView *)mapView;

// TODO
- (void)mapViewDidFailLoadingMap:(MGLMapView *)mapView withError:(NSError *)error;

// TODO
- (void)mapViewWillStartRenderingMap:(MGLMapView *)mapView;

// TODO
- (void)mapViewDidFinishRenderingMap:(MGLMapView *)mapView fullyRendered:(BOOL)fullyRendered;

// TODO
- (void)mapViewWillStartRenderingFrame:(MGLMapView *)mapView;

// TODO
- (void)mapViewDidFinishRenderingFrame:(MGLMapView *)mapView fullyRendered:(BOOL)fullyRendered;

#pragma mark Tracking User Location

/**
 Tells the delegate that the map view will begin tracking the user's location.
 
 This method is called when the value of the `showsUserLocation` property changes to `YES`.
 
 @param mapView The map view that is tracking the user's location.
 */
- (void)mapViewWillStartLocatingUser:(MGLMapView *)mapView;

/**
 Tells the delegate that the map view has stopped tracking the user's location.
 
 This method is called when the value of the `showsUserLocation` property changes to `NO`.
 
 @param mapView The map view that is tracking the user's location.
 */
- (void)mapViewDidStopLocatingUser:(MGLMapView *)mapView;

/**
 Tells the delegate that the location of the user was updated.
 
 While the `showsUserLocation` property is set to `YES`, this method is called whenever a new location update is received by the map view. This method is also called if the map view's user tracking mode is set to `MGLUserTrackingModeFollowWithHeading` and the heading changes, or if it is set to `MGLUserTrackingModeFollowWithCourse` and the course changes.
 
 This method is not called if the application is currently running in the background. If you want to receive location updates while running in the background, you must use the Core Location framework.
 
 @param mapView The map view that is tracking the user's location.
 @param userLocation The location object representing the user's latest location. This property may be `nil`.
 */
- (void)mapView:(MGLMapView *)mapView didUpdateUserLocation:(nullable MGLUserLocation *)userLocation;

/**
 Tells the delegate that an attempt to locate the user's position failed.
 
 @param mapView The map view that is tracking the user's location.
 @param error An error object containing the reason why location tracking failed.
 */
- (void)mapView:(MGLMapView *)mapView didFailToLocateUserWithError:(NSError *)error;

/**
 Tells the delegate that the map view's user tracking mode has changed.
 
 This method is called after the map view asynchronously changes to reflect the new user tracking mode, for example by beginning to zoom or rotate.
 
 @param mapView The map view that changed its tracking mode.
 @param mode The new tracking mode.
 @param animated Whether the change caused an animated effect on the map.
 */
- (void)mapView:(MGLMapView *)mapView didChangeUserTrackingMode:(MGLUserTrackingMode)mode animated:(BOOL)animated;

#pragma mark Managing the Display of Annotations

/**
 Returns an image object to use for the marker for the specified point annotation object.
 
 @param mapView The map view that requested the annotation image.
 @param annotation The object representing the annotation that is about to be displayed.
 @return The image object to display for the specified annotation or `nil` if you want to display the default marker image.
 */
- (nullable MGLAnnotationImage *)mapView:(MGLMapView *)mapView imageForAnnotation:(id <MGLAnnotation>)annotation;

/**
 Returns the alpha value to use when rendering a shape annotation. Defaults to `1.0`.
 
 @param mapView The map view rendering the shape annotation.
 @param annotation The annotation being rendered.
 @return An alpha value between `0` and `1.0`.
 */
- (CGFloat)mapView:(MGLMapView *)mapView alphaForShapeAnnotation:(MGLShape *)annotation;

/**
 Returns the stroke color to use when rendering a shape annotation. Defaults to black.
 
 @param mapView The map view rendering the shape annotation.
 @param annotation The annotation being rendered.
 @return A color to use for the shape outline.
 */
- (UIColor *)mapView:(MGLMapView *)mapView strokeColorForShapeAnnotation:(MGLShape *)annotation;

/**
 Returns the fill color to use when rendering a polygon annotation. Defaults to blue.
 
 @param mapView The map view rendering the polygon annotation.
 @param annotation The annotation being rendered.
 @return A color to use for the polygon interior.
 */
- (UIColor *)mapView:(MGLMapView *)mapView fillColorForPolygonAnnotation:(MGLPolygon *)annotation;

/**
 Returns the line width to use when rendering a polyline annotation. Defaults to `3.0`.
 
 @param mapView The map view rendering the polygon annotation.
 @param annotation The annotation being rendered.
 @return A line width for the polyline.
 */
- (CGFloat)mapView:(MGLMapView *)mapView lineWidthForPolylineAnnotation:(MGLPolyline *)annotation;

/**
 Returns a Boolean value indicating whether the annotation is able to display extra information in a callout bubble.
 
 If the value returned is `YES`, a standard callout bubble is shown when the user taps a selected annotation. The callout uses the title and subtitle text from the associated annotation object. If there is no title text, though, the annotation will not show a callout. The callout also displays any custom callout views returned by the delegate for the left and right callout accessory views.
 
 If the value returned is `NO`, the value of the title and subtitle strings are ignored.
 
 @param mapView The map view that requested the annotation callout ability.
 @param annotation The object representing the annotation.
 @return A Boolean indicating whether the annotation should show a callout.
 */
- (BOOL)mapView:(MGLMapView *)mapView annotationCanShowCallout:(id <MGLAnnotation>)annotation;

/**
 Returns a callout view to display for the specified annotation.
 
 If this method is present in the delegate, it must return a new instance of a view dedicated to display the callout bubble. It will be configured by the map view. If this method is not present, or if it returns `nil`, a standard, two-line, bubble-like callout view is displayed by default.
 
 @param mapView The map view that requested the callout view.
 @param annotation The object representing the annotation.
 @return A view conforming to the `MGLCalloutView` protocol, or `nil` to use the default callout view.
 */
- (nullable UIView <MGLCalloutView> *)mapView:(MGLMapView *)mapView calloutViewForAnnotation:(id <MGLAnnotation>)annotation;

/**
 Returns the view to display on the left side of the standard callout bubble.
 
 The default value is treated as if `nil`. The left callout view is typically used to display information about the annotation or to link to custom information provided by your application.
 
 If the view you specify is also a descendant of the `UIControl` class, you can use the map view's delegate to receive notifications when your control is tapped. If it does not descend from `UIControl`, your view is responsible for handling any touch events within its bounds.
 
 @param mapView The map view presenting the annotation callout.
 @param annotation The object representing the annotation with the callout.
 @return The accessory view to display.
 */
- (nullable UIView *)mapView:(MGLMapView *)mapView leftCalloutAccessoryViewForAnnotation:(id <MGLAnnotation>)annotation;

/**
 Returns the view to display on the right side of the standard callout bubble.
 
 The default value is treated is if `nil`. The right callout view is typically used to link to more detailed information about the annotation. A common view to specify for this property is `UIButton` object whose type is set to `UIButtonTypeDetailDisclosure`.
 
 If the view you specify is also a descendant of the `UIControl` class, you can use the map view's delegate to receive notifications when your control is tapped. If it does not descend from `UIControl`, your view is responsible for handling any touch events within its bounds.
 
 @param mapView The map view presenting the annotation callout.
 @param annotation The object representing the annotation with the callout.
 @return The accessory view to display.
 */
- (nullable UIView *)mapView:(MGLMapView *)mapView rightCalloutAccessoryViewForAnnotation:(id <MGLAnnotation>)annotation;

#pragma mark Managing Annotations

/**
 Tells the delegate that the user tapped one of the annotation's accessory buttons.
 
 Accessory views contain custom content and are positioned on either side of the annotation title text. If a view you specify is a descendant of the `UIControl` class, the map view calls this method as a convenience whenever the user taps your view. You can use this method to respond to taps and perform any actions associated with that control. For example, if your control displayed additional information about the annotation, you could use this method to present a modal panel with that information.
 
 If your custom accessory views are not descendants of the `UIControl` class, the map view does not call this method.
 
 @param mapView The map view containing the specified annotation.
 @param annotation The annotation whose button was tapped.
 @param control The control that was tapped.
 */
- (void)mapView:(MGLMapView *)mapView annotation:(id <MGLAnnotation>)annotation calloutAccessoryControlTapped:(UIControl *)control;

/**
 Tells the delegate that the user tapped on an annotation's callout view.
 
 @param mapView The map view containing the specified annotation.
 @param annotation The annotation whose callout was tapped.
 */
- (void)mapView:(MGLMapView *)mapView tapOnCalloutForAnnotation:(id <MGLAnnotation>)annotation;

#pragma mark Selecting Annotations

/**
 Tells the delegate that one of its annotations was selected.
 
 You can use this method to track changes in the selection state of annotations.
 
 @param mapView The map view containing the annotation.
 @param annotation The annotation that was selected.
 */
- (void)mapView:(MGLMapView *)mapView didSelectAnnotation:(id <MGLAnnotation>)annotation;

/**
 Tells the delegate that one of its annotations was deselected.
 
 You can use this method to track changes in the selection state of annotations.
 
 @param mapView The map view containing the annotation.
 @param annotation The annotation that was deselected.
 */
- (void)mapView:(MGLMapView *)mapView didDeselectAnnotation:(id <MGLAnnotation>)annotation;

@end

NS_ASSUME_NONNULL_END
