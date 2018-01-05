#import "MGLGeometry.h"
#import "MGLMapCamera.h"

#import <UIKit/UIKit.h>
#import <CoreLocation/CoreLocation.h>

#import "MGLFoundation.h"
#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

@class MGLAnnotationView;
@class MGLAnnotationImage;
@class MGLUserLocation;
@class MGLPolyline;
@class MGLPolygon;
@class MGLShape;
@class MGLStyle;

@protocol MGLMapViewDelegate;
@protocol MGLAnnotation;
@protocol MGLOverlay;
@protocol MGLCalloutView;
@protocol MGLFeature;

/** The default deceleration rate for a map view. */
extern MGL_EXPORT const CGFloat MGLMapViewDecelerationRateNormal;

/** A fast deceleration rate for a map view. */
extern MGL_EXPORT const CGFloat MGLMapViewDecelerationRateFast;

/** Disables deceleration in a map view. */
extern MGL_EXPORT const CGFloat MGLMapViewDecelerationRateImmediate;

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

/**
 The mode used to track the user location on the map. Used with
 `MGLMapView.userTrackingMode`.
 */
typedef NS_ENUM(NSUInteger, MGLUserTrackingMode) {
    /** The map does not follow the user location. */
    MGLUserTrackingModeNone              = 0,
    /** The map follows the user location. This tracking mode falls back
        to `MGLUserTrackingModeNone` if the user pans the map view. */
    MGLUserTrackingModeFollow,
    /**
        The map follows the user location and rotates when the heading changes.
        The default user location annotation displays a fan-shaped indicator with
        the current heading. The heading indicator represents the direction the
        device is facing, which is sized according to the reported accuracy.
     
        This tracking mode is disabled if the user pans the map view, but
        remains enabled if the user zooms in. If the user rotates the map
        view, this tracking mode will fall back to `MGLUserTrackingModeFollow`.
     */
    MGLUserTrackingModeFollowWithHeading,
    /**
        The map follows the user location and rotates when the course changes.
        Course represents the direction in which the device is traveling.
        The default user location annotation shows a puck-shaped indicator
        that rotates as the course changes.
     
        This tracking mode is disabled if the user pans the map view, but
        remains enabled if the user zooms in. If the user rotates the map view,
        this tracking mode will fall back to `MGLUserTrackingModeFollow`.
     */
    MGLUserTrackingModeFollowWithCourse,
};

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

 A collection of Mapbox-hosted styles is available through the `MGLStyle`
 class. These basic styles use
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
 gesture takes precedence. For example, you can create your own
 `UITapGestureRecognizer` that will be invoked only if the default `MGLMapView`
 tap gesture fails:

 ```swift
 let mapTapGestureRecognizer = UITapGestureRecognizer(target: self, action: #selector(myCustomFunction))
 for recognizer in mapView.gestureRecognizers! where recognizer is UITapGestureRecognizer {
     mapTapGestureRecognizer.require(toFail: recognizer)
 }
 mapView.addGestureRecognizer(mapTapGestureRecognizer)
 ```

 @note You are responsible for getting permission to use the map data and for
 ensuring that your use adheres to the relevant terms of use.

 */
MGL_EXPORT IB_DESIGNABLE
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
    or HTTPS URL, a Mapbox URL indicating the style’s map ID
    (`mapbox://styles/{user}/{style}`), or a path to a local file relative
    to the application’s resource path. Specify `nil` for the default style.
 @return An initialized map view.
 */
- (instancetype)initWithFrame:(CGRect)frame styleURL:(nullable NSURL *)styleURL;

#pragma mark Accessing the Delegate

/**
 The receiver’s delegate.

 A map view sends messages to its delegate to notify it of changes to its
 contents or the viewpoint. The delegate also provides information about
 annotations displayed on the map, such as the styles to apply to individual
 annotations.
 */
@property(nonatomic, weak, nullable) IBOutlet id<MGLMapViewDelegate> delegate;

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
    manipulate a style’s sources and layers must first set the style URL to an
    explicitly versioned style using a convenience method like
    `+[MGLStyle outdoorsStyleURLWithVersion:]`, `MGLMapView`’s “Style URL”
    inspectable in Interface Builder, or a manually constructed `NSURL`.
 */
@property (nonatomic, readonly, nullable) MGLStyle *style;

@property (nonatomic, readonly) NS_ARRAY_OF(NSURL *) *bundledStyleURLs __attribute__((unavailable("Call the relevant class method of MGLStyle for the URL of a particular default style.")));

/**
 URL of the style currently displayed in the receiver.

 The URL may be a full HTTP or HTTPS URL, a Mapbox URL indicating the style’s
 map ID (`mapbox://styles/{user}/{style}`), or a path to a local file
 relative to the application’s resource path.

 If you set this property to `nil`, the receiver will use the default style
 and this property will automatically be set to that style’s URL.

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

 This method does not bust the cache. Even if the style has recently changed on
 the server, calling this method does not necessarily ensure that the map view
 reflects those changes.
 */
- (IBAction)reloadStyle:(id)sender;

/**
 A control indicating the scale of the map. The scale bar is positioned in the
 upper-left corner. The scale bar is hidden by default.
 */
@property (nonatomic, readonly) UIView *scaleBar;

/**
 A control indicating the map’s direction and allowing the user to manipulate
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

 If you choose to reimplement this view, assign the `-showAttribution:` method
 as the action for your view to present the default notices and settings.

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
 Show the attribution and telemetry action sheet.

 This action is performed when the user taps on the attribution button provided
 by default via the `attributionButton` property. If you implement a custom
 attribution button, you should add this action to the button.
 */
- (IBAction)showAttribution:(id)sender;

@property (nonatomic) NS_ARRAY_OF(NSString *) *styleClasses __attribute__((unavailable("This property is non-functional.")));

- (BOOL)hasStyleClass:(NSString *)styleClass __attribute__((unavailable("This method is non-functional.")));

- (void)addStyleClass:(NSString *)styleClass __attribute__((unavailable("This method is non-functional.")));

- (void)removeStyleClass:(NSString *)styleClass __attribute__((unavailable("This method is non-functional.")));

#pragma mark Displaying the User’s Location

/**
 A Boolean value indicating whether the map may display the user location.

 Setting this property to `YES` causes the map view to use the Core Location
 framework to find the current location. As long as this property is `YES`, the
 map view continues to track the user’s location and update it periodically.

 This property does not indicate whether the user’s position is actually visible
 on the map, only whether the map view is allowed to display it. To determine
 whether the user’s position is visible, use the `userLocationVisible` property.
 The default value of this property is `NO`.

 Your app must specify a value for `NSLocationWhenInUseUsageDescription` or
 `NSLocationAlwaysUsageDescription` in its `Info.plist` to satisfy the
 requirements of the underlying Core Location framework when enabling this
 property.
 */
@property (nonatomic, assign) BOOL showsUserLocation;

/**
 A Boolean value indicating whether the device’s current location is visible in
 the map view.

 Use `showsUserLocation` to control the visibility of the on-screen user
 location annotation.
 */
@property (nonatomic, assign, readonly, getter=isUserLocationVisible) BOOL userLocationVisible;

/**
 Returns the annotation object indicating the user’s current location.
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
 A Boolean value indicating whether the user location annotation may display a
 permanent heading indicator.

 Setting this property to `YES` causes the default user location annotation to
 appear and always show an arrow-shaped heading indicator, if heading is
 available. This property does not rotate the map; for that, see
 `MGLUserTrackingModeFollowWithHeading`.

 This property has no effect when `userTrackingMode` is
 `MGLUserTrackingModeFollowWithHeading` or
 `MGLUserTrackingModeFollowWithCourse`.

 The default value of this property is `NO`.
 */
@property (nonatomic, assign) BOOL showsUserHeadingIndicator;

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
 A Boolean value that determines whether the user will receive haptic feedback
 for certain interactions with the map.

 When this property is set to `YES`, the default, a `UIImpactFeedbackStyleLight`
 haptic feedback event be played when the user rotates the map to due north
 (0°).

 This feature requires a device that supports haptic feedback, running iOS 10 or
 newer.
 */
@property(nonatomic, getter=isHapticFeedbackEnabled) BOOL hapticFeedbackEnabled;

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
 * The default maximumZoomLevel is 22. The upper bound for this property
 * is 25.5.
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
 Resets the map to the current style’s default viewport.

 If the style doesn’t specify a default viewport, the map resets to a minimum
 zoom level, a center coordinate of (0, 0), and a northern heading.
 */
- (IBAction)resetPosition;

/**
 The coordinate bounds visible in the receiver’s viewport.

 Changing the value of this property updates the receiver immediately. If you
 want to animate the change, call `-setVisibleCoordinateBounds:animated:`
 instead.
 
 If a longitude is less than −180 degrees or greater than 180 degrees, the
 visible bounds straddles the antimeridian or international date line. For
 example, if both Tokyo and San Francisco are visible, the visible bounds might
 extend from (35.68476, −220.24257) to (37.78428, −122.41310).
 */
@property (nonatomic) MGLCoordinateBounds visibleCoordinateBounds;

/**
 Changes the receiver’s viewport to fit the given coordinate bounds,
 optionally animating the change.
 
 To bring both sides of the antimeridian or international date line into view,
 specify some longitudes less than −180 degrees or greater than 180 degrees. For
 example, to show both Tokyo and San Francisco simultaneously, you could set the
 visible bounds to extend from (35.68476, −220.24257) to (37.78428, −122.41310).

 @param bounds The bounds that the viewport will show in its entirety.
 @param animated Specify `YES` to animate the change by smoothly scrolling
    and zooming or `NO` to immediately display the given bounds.
 */
- (void)setVisibleCoordinateBounds:(MGLCoordinateBounds)bounds animated:(BOOL)animated;

/**
 Changes the receiver’s viewport to fit the given coordinate bounds and
 optionally some additional padding on each side.
 
 To bring both sides of the antimeridian or international date line into view,
 specify some longitudes less than −180 degrees or greater than 180 degrees. For
 example, to show both Tokyo and San Francisco simultaneously, you could set the
 visible bounds to extend from (35.68476, −220.24257) to (37.78428, −122.41310).

 @param bounds The bounds that the viewport will show in its entirety.
 @param insets The minimum padding (in screen points) that will be visible
    around the given coordinate bounds.
 @param animated Specify `YES` to animate the change by smoothly scrolling and
    zooming or `NO` to immediately display the given bounds.
 */
- (void)setVisibleCoordinateBounds:(MGLCoordinateBounds)bounds edgePadding:(UIEdgeInsets)insets animated:(BOOL)animated;

/**
 Changes the receiver’s viewport to fit all of the given coordinates and
 optionally some additional padding on each side.
 
 To bring both sides of the antimeridian or international date line into view,
 specify some longitudes less than −180 degrees or greater than 180 degrees. For
 example, to show both Tokyo and San Francisco simultaneously, you could set the
 visible coordinates to (35.68476, −220.24257) and (37.78428, −122.41310).

 @param coordinates The coordinates that the viewport will show.
 @param count The number of coordinates. This number must not be greater than
    the number of elements in `coordinates`.
 @param insets The minimum padding (in screen points) that will be visible
    around the given coordinate bounds.
 @param animated Specify `YES` to animate the change by smoothly scrolling and
    zooming or `NO` to immediately display the given bounds.
 */
- (void)setVisibleCoordinates:(const CLLocationCoordinate2D *)coordinates count:(NSUInteger)count edgePadding:(UIEdgeInsets)insets animated:(BOOL)animated;

/**
 Changes the receiver’s viewport to fit all of the given coordinates and
 optionally some additional padding on each side.
 
 To bring both sides of the antimeridian or international date line into view,
 specify some longitudes less than −180 degrees or greater than 180 degrees. For
 example, to show both Tokyo and San Francisco simultaneously, you could set the
 visible coordinates to (35.68476, −220.24257) and (37.78428, −122.41310).

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
- (void)setVisibleCoordinates:(const CLLocationCoordinate2D *)coordinates count:(NSUInteger)count edgePadding:(UIEdgeInsets)insets direction:(CLLocationDirection)direction duration:(NSTimeInterval)duration animationTimingFunction:(nullable CAMediaTimingFunction *)function completionHandler:(nullable void (^)(void))completion;

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
- (void)setCamera:(MGLMapCamera *)camera withDuration:(NSTimeInterval)duration animationTimingFunction:(nullable CAMediaTimingFunction *)function edgePadding:(UIEdgeInsets)edgePadding completionHandler:(nullable void (^)(void))completion;

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
- (MGLMapCamera *)cameraThatFitsShape:(MGLShape *)shape direction:(CLLocationDirection)direction edgePadding:(UIEdgeInsets)insets;

/**
 Returns the point in this view’s coordinate system on which to "anchor" in
 response to a user-initiated gesture.

 For example, a pinch-to-zoom gesture would anchor the map at the midpoint of
 the pinch.

 If the `userTrackingMode` property is not `MGLUserTrackingModeNone`, the
 user annotation is used as the anchor point.

 Subclasses may override this method to provide specialized behavior - for
 example, anchoring on the map’s center point to provide a "locked" zooming
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
 Converts a point in the given view’s coordinate system to a geographic
 coordinate.

 @param point The point to convert.
 @param view The view in whose coordinate system the point is expressed.
 @return The geographic coordinate at the given point.
 */
- (CLLocationCoordinate2D)convertPoint:(CGPoint)point toCoordinateFromView:(nullable UIView *)view;

/**
 Converts a geographic coordinate to a point in the given view’s coordinate
 system.

 @param coordinate The geographic coordinate to convert.
 @param view The view in whose coordinate system the returned point should be
    expressed. If this parameter is `nil`, the returned point is expressed
    in the window’s coordinate system. If `view` is not `nil`, it must
    belong to the same window as the map view.
 @return The point (in the appropriate view or window coordinate system)
    corresponding to the given geographic coordinate.
 */
- (CGPoint)convertCoordinate:(CLLocationCoordinate2D)coordinate toPointToView:(nullable UIView *)view;

/**
 Converts a rectangle in the given view’s coordinate system to a geographic
 bounding box.
 
 If a longitude is less than −180 degrees or greater than 180 degrees, the
 bounding box straddles the antimeridian or international date line.

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

 @note `MGLMultiPolyline`, `MGLMultiPolygon`, `MGLShapeCollection`, and
    `MGLPointCollection` objects cannot be added to the map view at this time.
    Any multipoint, multipolyline, multipolygon, shape or point collection
    object that is specified is silently ignored.

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

/**
 The complete list of annotations associated with the receiver that are
 currently visible.

 The objects in this array must adopt the `MGLAnnotation` protocol. If no
 annotations are associated with the map view or if no annotations associated
 with the map view are currently visible, the value of this property is `nil`.
 */
@property (nonatomic, readonly, nullable) NS_ARRAY_OF(id <MGLAnnotation>) *visibleAnnotations;

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
 The complete list of overlays associated with the receiver. (read-only)

 The objects in this array must adopt the `MGLOverlay` protocol. If no
 overlays are associated with the map view, the value of this property is
 empty array.
 */
@property (nonatomic, readonly, nonnull) NS_ARRAY_OF(id <MGLOverlay>) *overlays;

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
 
 This method returns all the intersecting features from the specified layers. To
 filter the returned features, use the
 `-visibleFeaturesAtPoint:inStyleLayersWithIdentifiers:predicate:` method. For
 more information about searching for map features, see that method’s
 documentation.
 
 @param point A point expressed in the map view’s coordinate system.
 @param styleLayerIdentifiers A set of strings that correspond to the names 
    of layers defined in the current style. Only the features contained in 
    these layers are included in the returned array.
 @return An array of objects conforming to the `MGLFeature` protocol that 
    represent features in the sources used by the current style.
 */
- (NS_ARRAY_OF(id <MGLFeature>) *)visibleFeaturesAtPoint:(CGPoint)point inStyleLayersWithIdentifiers:(nullable NS_SET_OF(NSString *) *)styleLayerIdentifiers NS_SWIFT_NAME(visibleFeatures(at:styleLayerIdentifiers:));

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

 Only visible features are returned. To obtain features regardless of
 visibility, use the
 `-[MGLVectorSource featuresInSourceLayersWithIdentifiers:predicate:]` and
 `-[MGLShapeSource featuresMatchingPredicate:]` methods on the relevant sources.

 The returned features may also include features corresponding to annotations.
 These features are not object-equal to the `MGLAnnotation` objects that were
 originally added to the map. To query the map for annotations, use
 `visibleAnnotations` or `-[MGLMapView visibleAnnotationsInRect:]`.

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
- (NS_ARRAY_OF(id <MGLFeature>) *)visibleFeaturesAtPoint:(CGPoint)point inStyleLayersWithIdentifiers:(nullable NS_SET_OF(NSString *) *)styleLayerIdentifiers predicate:(nullable NSPredicate *)predicate NS_SWIFT_NAME(visibleFeatures(at:styleLayerIdentifiers:predicate:));

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
- (NS_ARRAY_OF(id <MGLFeature>) *)visibleFeaturesInRect:(CGRect)rect inStyleLayersWithIdentifiers:(nullable NS_SET_OF(NSString *) *)styleLayerIdentifiers NS_SWIFT_NAME(visibleFeatures(in:styleLayerIdentifiers:));

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

 Only visible features are returned. To obtain features regardless of
 visibility, use the
 `-[MGLVectorSource featuresInSourceLayersWithIdentifiers:predicate:]` and
 `-[MGLShapeSource featuresMatchingPredicate:]` methods on the relevant sources.

 @note Layer identifiers are not guaranteed to exist across styles or different
 versions of the same style. Applications that use this API must first set the
 style URL to an explicitly versioned style using a convenience method like
 `+[MGLStyle outdoorsStyleURLWithVersion:]`, `MGLMapView`’s “Style URL”
 inspectable in Interface Builder, or a manually constructed `NSURL`. This
 approach also avoids layer identifer name changes that will occur in the default
 style’s layers over time.
 
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
- (NS_ARRAY_OF(id <MGLFeature>) *)visibleFeaturesInRect:(CGRect)rect inStyleLayersWithIdentifiers:(nullable NS_SET_OF(NSString *) *)styleLayerIdentifiers predicate:(nullable NSPredicate *)predicate NS_SWIFT_NAME(visibleFeatures(in:styleLayerIdentifiers:predicate:));

#pragma mark Debugging the Map

/**
 The options that determine which debugging aids are shown on the map.

 These options are all disabled by default and should remain disabled in
 released software for performance and aesthetic reasons.
 */
@property (nonatomic) MGLMapDebugMaskOptions debugMask;

@property (nonatomic, getter=isDebugActive) BOOL debugActive __attribute__((unavailable("Use -debugMask and -setDebugMask:.")));

- (void)toggleDebug __attribute__((unavailable("Use -setDebugMask:.")));

- (void)emptyMemoryCache __attribute__((unavailable));

@end

NS_ASSUME_NONNULL_END
