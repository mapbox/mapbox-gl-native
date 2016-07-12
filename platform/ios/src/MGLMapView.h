#import "MGLGeometry.h"
#import "MGLMapCamera.h"

#import <UIKit/UIKit.h>
#import <CoreLocation/CoreLocation.h>

#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

@class MGLAnnotationView;
@class MGLAnnotationImage;
@class MGLUserLocation;
@class MGLPolyline;
@class MGLPolygon;
@class MGLShape;

@protocol MGLMapViewDelegate;
@protocol MGLAnnotation;
@protocol MGLOverlay;
@protocol MGLCalloutView;
@protocol MGLFeature;

/** The default deceleration rate for a map view. */
extern const CGFloat MGLMapViewDecelerationRateNormal;

/** A fast deceleration rate for a map view. */
extern const CGFloat MGLMapViewDecelerationRateFast;

/** Disables decleration in a map view. */
extern const CGFloat MGLMapViewDecelerationRateImmediate;

/**
 The vertical alignment of an annotation within a map view. Used with
 `MGLMapView.userLocationVerticalAlignment`.
 */
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
    /** Each drawing operation is replaced by a translucent fill. Overlapping
        drawing operations appear more prominent to help diagnose overdrawing.
     */
    MGLMapDebugOverdrawVisualizationMask = 1 << 5,
};

/**
 An interactive, customizable map view with an interface similar to the one
 provided by Apple's MapKit.
 
 Using `MGLMapView`, you can embed the map inside a view, allow users to
 manipulate it with standard gestures, animate the map between different
 viewpoints, and present information in the form of annotations and overlays.
 
 The map view loads scalable vector tiles that conform to the
 <a href="https://github.com/mapbox/vector-tile-spec">Mapbox Vector Tile Specification</a>.
 It styles them with a style that conforms to the
 <a href="https://www.mapbox.com/mapbox-gl-style-spec/">Mapbox GL style specification</a>.
 Such styles can be designed in
 <a href="https://www.mapbox.com/studio/">Mapbox Studio</a> and hosted on
 mapbox.com.
 
 A collection of Mapbox-hosted styles is available through the `MGLStyle`
 class. These basic styles use
 <a href="https://www.mapbox.com/developers/vector-tiles/mapbox-streets">Mapbox Streets</a>
 or <a href="https://www.mapbox.com/satellite/">Mapbox Satellite</a> data
 sources, but you can specify a custom style that makes use of your own data.
 
 Mapbox-hosted vector tiles and styles require an API access token, which you
 can obtain from the
 <a href="https://www.mapbox.com/studio/account/tokens/">Mapbox account page</a>.
 Access tokens associate requests to Mapbox's vector tile and style APIs with
 your Mapbox account. They also deter other developers from using your styles
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
 Reloads the style.
 
 You do not normally need to call this method. The map view automatically
 responds to changes in network connectivity by reloading the style. You may
 need to call this method if you change the access token after a style has
 loaded but before loading a style associated with a different Mapbox account.
 
 This method does not bust the cache. Even if the style has recently changed on
 the server, calling this method does not necessarily ensure that the map view
 reflects those changes.
 */
- (IBAction)reloadStyle:(id)sender;

/**
 A control indicating the map's direction and allowing the user to manipulate
 the direction, positioned in the upper-right corner.
 */
@property (nonatomic, readonly) UIImageView *compassView;

/**
 The Mapbox logo, positioned in the lower-left corner.
 
 @note The Mapbox terms of service, which governs the use of Mapbox-hosted
    vector tiles and styles,
    <a href="https://www.mapbox.com/help/mapbox-logo/">requires</a> most Mapbox
    customers to display the Mapbox logo. If this applies to you, do not
    hide this view or change its contents.
 */
@property (nonatomic, readonly) UIImageView *logoView;

/** 
 A view showing legally required copyright notices and telemetry settings,
 positioned at the bottom-right of the map view.
 
 @note The Mapbox terms of service, which governs the use of Mapbox-hosted
    vector tiles and styles,
    <a href="https://www.mapbox.com/help/attribution/">requires</a> these
    copyright notices to accompany any map that features Mapbox-designed styles,
    OpenStreetMap data, or other Mapbox data such as satellite or terrain
    data. If that applies to this map view, do not hide this view or remove
    any notices from it.

 @note You are additionally
    <a href="https://www.mapbox.com/help/telemetry-opt-out-for-users/">required</a>
    to provide users with the option to disable anonymous usage and location
    sharing (telemetry). If this view is hidden, you must implement this
    setting elsewhere in your app or via `Settings.bundle`. See our
    <a href="https://www.mapbox.com/ios-sdk/#telemetry_opt_out">website</a> for
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

/**
 A floating-point value that determines the rate of deceleration after the user
 lifts their finger.

 Your application can use the `MGLMapViewDecelerationRateNormal` and
 `MGLMapViewDecelerationRateFast` constants as reference points for reasonable
 deceleration rates. `MGLMapViewDecelerationRateImmediate` can be used to
 disable deceleration entirely.
 */
@property(nonatomic) CGFloat decelerationRate;

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
 Returns the camera that best fits the given coordinate bounds.
 
 @param bounds The coordinate bounds to fit to the receiver’s viewport.
 @return A camera object centered on the same location as the coordinate
    bounds with zoom level as high (close to the ground) as possible while still
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
- (MGLMapCamera *)cameraThatFitsCoordinateBounds:(MGLCoordinateBounds)bounds edgePadding:(UIEdgeInsets)insets;

/**
 Returns the point in this view's coordinate system on which to "anchor" in
 response to a user-initiated gesture.
 
 For example, a pinch-to-zoom gesture would anchor the map at the midpoint of
 the pinch.
 
 If the `userTrackingMode` property is not `MGLUserTrackingModeNone`, the
 user annotation is used as the anchor point.
 
 Subclasses may override this method to provide specialized behavior - for
 example, anchoring on the map's center point to provide a "locked" zooming
 mode.
 
 @param gesture An anchorable user gesture.
 @return The point on which to anchor in response to the gesture.
 */
- (CGPoint)anchorPointForGesture:(UIGestureRecognizer *)gesture;

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
 
 @note `MGLMultiPolyline`, `MGLMultiPolygon`, and `MGLShapeCollection` objects
    cannot be added to the map view at this time. Nor can `MGLMultiPoint`
    objects that are not instances of `MGLPolyline` or `MGLPolygon`. Any
    multipoint, multipolyline, multipolygon, or shape collection object that is
    specified is silently ignored.
 
 @param annotation The annotation object to add to the receiver. This object
    must conform to the `MGLAnnotation` protocol. The map view retains the
    annotation object. */
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
 Returns an `MGLAnnotationView` if the given annotation is currently associated
 with a view, otherwise nil.
 
 @param annotation The annotation associated with the view. 
    Annotation must conform to the `MGLAnnotation` protocol.
 */
- (nullable MGLAnnotationView *)viewForAnnotation:(id <MGLAnnotation>)annotation;

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
- (nullable __kindof MGLAnnotationImage *)dequeueReusableAnnotationImageWithIdentifier:(NSString *)identifier;

/**
 Returns a reusable annotation view object associated with its identifier.
 
 For performance reasons, you should generally reuse `MGLAnnotationView`
 objects for identical-looking annotations in your map views. Dequeueing
 saves time and memory during performance-critical operations such as
 scrolling.
 
 @param identifier A string identifying the annotation view to be reused.
    This string is the same one you specify when initially returning the
    annotation view object using the `-mapView:viewForAnnotation:` method.
 @return An annotation view object with the given identifier, or `nil` if no
    such object exists in the reuse queue.
 */
- (nullable __kindof MGLAnnotationView *)dequeueReusableAnnotationViewWithIdentifier:(NSString *)identifier;

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
- (NS_ARRAY_OF(id <MGLFeature>) *)visibleFeaturesAtPoint:(CGPoint)point NS_SWIFT_NAME(visibleFeatures(at:));

/**
 Returns an array of rendered map features that intersect with a given point,
 restricted to the given style layers.
 
 Each object in the returned array represents a feature rendered by the
 current style and provides access to attributes specified by the relevant
 <a href="https://www.mapbox.com/mapbox-gl-style-spec/#sources">tile sources</a>.
 The returned array includes features specified in vector and GeoJSON tile
 sources but does not include anything from raster, image, or video sources.
 
 Only visible features are returned. For example, suppose the current style uses
 the
 <a href="https://www.mapbox.com/vector-tiles/mapbox-streets/">Mapbox Streets source</a>,
 but none of the specified style layers includes features that have the `maki`
 property set to `bus`. If you pass a point corresponding to the location of a
 bus stop into this method, the bus stop feature does not appear in the
 resulting array. On the other hand, if the style does include bus stops, an
 `MGLFeature` object representing that bus stop is returned and its
 `featureAttributes` dictionary has the `maki` key set to `bus` (along with
 other attributes). The dictionary contains only the attributes provided by the
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
 
 @param point A point expressed in the map view’s coordinate system.
 @param styleLayerIdentifiers A set of strings that correspond to the names of
    layers defined in the current style. Only the features contained in these
    layers are included in the returned array.
 @return An array of objects conforming to the `MGLFeature` protocol that
    represent features in the sources used by the current style.
 */
- (NS_ARRAY_OF(id <MGLFeature>) *)visibleFeaturesAtPoint:(CGPoint)point inStyleLayersWithIdentifiers:(nullable NS_SET_OF(NSString *) *)styleLayerIdentifiers NS_SWIFT_NAME(visibleFeatures(at:styleLayerIdentifiers:));

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
- (NS_ARRAY_OF(id <MGLFeature>) *)visibleFeaturesInRect:(CGRect)rect NS_SWIFT_NAME(visibleFeatures(in:));

/**
 Returns an array of rendered map features that intersect with the given
 rectangle, restricted to the given style layers.
 
 Each object in the returned array represents a feature rendered by the
 current style and provides access to attributes specified by the relevant
 <a href="https://www.mapbox.com/mapbox-gl-style-spec/#sources">tile sources</a>.
 The returned array includes features specified in vector and GeoJSON tile
 sources but does not include anything from raster, image, or video sources.
 
 Only visible features are returned. For example, suppose the current style uses
 the
 <a href="https://www.mapbox.com/vector-tiles/mapbox-streets/">Mapbox Streets source</a>,
 but none of the specified style layers includes features that have the `maki`
 property set to `bus`. If you pass a rectangle containing the location of a bus
 stop into this method, the bus stop feature does not appear in the resulting
 array. On the other hand, if the style does include bus stops, an `MGLFeature`
 object representing that bus stop is returned and its `featureAttributes`
 dictionary has the `maki` key set to `bus` (along with other attributes). The
 dictionary contains only the attributes provided by the tile source; it does
 not include computed attribute values or rules about how the feature is
 rendered by the current style.
 
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
 
 @param rect A rectangle expressed in the map view’s coordinate system.
 @param styleLayerIdentifiers A set of strings that correspond to the names of
    layers defined in the current style. Only the features contained in these
    layers are included in the returned array.
 @return An array of objects conforming to the `MGLFeature` protocol that
    represent features in the sources used by the current style.
 */
- (NS_ARRAY_OF(id <MGLFeature>) *)visibleFeaturesInRect:(CGRect)rect inStyleLayersWithIdentifiers:(nullable NS_SET_OF(NSString *) *)styleLayerIdentifiers NS_SWIFT_NAME(visibleFeatures(in:styleLayerIdentifiers:));

#pragma mark Debugging the Map

/**
 The options that determine which debugging aids are shown on the map.
 
 These options are all disabled by default and should remain disabled in
 released software for performance and aesthetic reasons.
 */
@property (nonatomic) MGLMapDebugMaskOptions debugMask;

@property (nonatomic, getter=isDebugActive) BOOL debugActive __attribute__((deprecated("Use -debugMask and -setDebugMask:.")));

- (void)toggleDebug __attribute__((deprecated("Use -setDebugMask:.")));

- (void)emptyMemoryCache __attribute__((deprecated));

/**
    Resets the map to the minimum zoom level, a center coordinate of (0, 0), and
    a northern heading.
 */
- (void)resetPosition;

@end

NS_ASSUME_NONNULL_END
