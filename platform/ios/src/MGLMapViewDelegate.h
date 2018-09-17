#import <UIKit/UIKit.h>

#import "MGLTypes.h"
#import "MGLCameraChangeReason.h"

NS_ASSUME_NONNULL_BEGIN

@class MGLMapView;

/**
 The `MGLMapViewDelegate` protocol defines a set of optional methods that you
 can use to receive map-related update messages. Because many map operations
 require the `MGLMapView` class to load data asynchronously, the map view calls
 these methods to notify your application when specific operations complete. The
 map view also uses these methods to request information about annotations
 displayed on the map, such as the styles and interaction modes to apply to
 individual annotations.
 */
@protocol MGLMapViewDelegate <NSObject>

@optional

#pragma mark Responding to Map Position Changes

/**
 Asks the delegate whether the map view should be allowed to change from the
 existing camera to the new camera in response to a user gesture.

 This method is called as soon as the user gesture is recognized. It is not
 called in response to a programmatic camera change, such as by setting the
 `centerCoordinate` property or calling `-flyToCamera:completionHandler:`.

 This method is called many times during gesturing, so you should avoid performing
 complex or performance-intensive tasks in your implementation.

 @param mapView The map view that the user is manipulating.
 @param oldCamera The camera representing the viewpoint at the moment the
 gesture is recognized. If this method returns `NO`, the map view’s camera
 continues to be this camera.
 @param newCamera The expected camera after the gesture completes. If this
 method returns `YES`, this camera becomes the map view’s camera.
 @return A Boolean value indicating whether the map view should stay at
 `oldCamera` or change to `newCamera`.
 */
- (BOOL)mapView:(MGLMapView *)mapView shouldChangeFromCamera:(MGLMapCamera *)oldCamera toCamera:(MGLMapCamera *)newCamera;

/**
 :nodoc:
 Asks the delegate whether the map view should be allowed to change from the
 existing camera to the new camera in response to a user gesture.

 This method is called as soon as the user gesture is recognized. It is not
 called in response to a programmatic camera change, such as by setting the
 `centerCoordinate` property or calling `-flyToCamera:completionHandler:`.

 This method is called many times during gesturing, so you should avoid performing
 complex or performance-intensive tasks in your implementation.

 @param mapView The map view that the user is manipulating.
 @param oldCamera The camera representing the viewpoint at the moment the
 gesture is recognized. If this method returns `NO`, the map view’s camera
 continues to be this camera.
 @param newCamera The expected camera after the gesture completes. If this
 method returns `YES`, this camera becomes the map view’s camera.
 @param reason The reason for the camera change.
 @return A Boolean value indicating whether the map view should stay at
 `oldCamera` or change to `newCamera`.

 @note If this method is implemented `-mapView:shouldChangeFromCamera:toCamera:` will not be called.
 */
- (BOOL)mapView:(MGLMapView *)mapView shouldChangeFromCamera:(MGLMapCamera *)oldCamera toCamera:(MGLMapCamera *)newCamera reason:(MGLCameraChangeReason)reason;

/**
 Tells the delegate that the viewpoint depicted by the map view is about to change.
 
 This method is called whenever the currently displayed map camera will start
 changing for any reason.

 @param mapView The map view whose viewpoint will change.
 @param animated Whether the change will cause an animated effect on the map.
 */
- (void)mapView:(MGLMapView *)mapView regionWillChangeAnimated:(BOOL)animated;

/**
 :nodoc:
 Tells the delegate that the viewpoint depicted by the map view is about to change.

 This method is called whenever the currently displayed map camera will start
 changing for any reason.

 @param mapView The map view whose viewpoint will change.
 @param animated Whether the change will cause an animated effect on the map.
 @param reason The reason for the camera change.

 @note If this method is implemented `-mapView:regionWillChangeAnimated:` will not be called.
 */
- (void)mapView:(MGLMapView *)mapView regionWillChangeWithReason:(MGLCameraChangeReason)reason animated:(BOOL)animated;

/**
 Tells the delegate that the viewpoint depicted by the map view is changing.

 This method is called as the currently displayed map camera changes as part of
 an animation, whether due to a user gesture or due to a call to a method such
 as `-[MGLMapView setCamera:animated:]`. This method can be called before
 `-mapViewDidFinishLoadingMap:` is called.
 
 During the animation, this method may be called many times to report updates to 
 the viewpoint. Therefore, your implementation of this method should be as lightweight 
 as possible to avoid affecting performance.

 @param mapView The map view whose viewpoint is changing.
 */
- (void)mapViewRegionIsChanging:(MGLMapView *)mapView;

/**
 :nodoc:
 Tells the delegate that the viewpoint depicted by the map view is changing.

 This method is called as the currently displayed map camera changes as part of
 an animation, whether due to a user gesture or due to a call to a method such
 as `-[MGLMapView setCamera:animated:]`. This method can be called before
 `-mapViewDidFinishLoadingMap:` is called.

 During the animation, this method may be called many times to report updates to
 the viewpoint. Therefore, your implementation of this method should be as lightweight
 as possible to avoid affecting performance.

 @param mapView The map view whose viewpoint is changing.
 @param reason The reason for the camera change.

 @note If this method is implemented `-mapViewRegionIsChanging:` will not be called.
 */
- (void)mapView:(MGLMapView *)mapView regionIsChangingWithReason:(MGLCameraChangeReason)reason;

/**
 Tells the delegate that the viewpoint depicted by the map view has finished
 changing.

 This method is called whenever the currently displayed map camera has finished
 changing, after any calls to `-mapViewRegionIsChanging:` due to animation. Therefore, 
 this method can be called before `-mapViewDidFinishLoadingMap:` is called.

 @param mapView The map view whose viewpoint has changed.
 @param animated Whether the change caused an animated effect on the map.
 */
- (void)mapView:(MGLMapView *)mapView regionDidChangeAnimated:(BOOL)animated;

/**
 :nodoc:
 Tells the delegate that the viewpoint depicted by the map view has finished
 changing.

 This method is called whenever the currently displayed map camera has finished
 changing, after any calls to `-mapViewRegionIsChanging:` due to animation. Therefore,
 this method can be called before `-mapViewDidFinishLoadingMap:` is called.

 @param mapView The map view whose viewpoint has changed.
 @param animated Whether the change caused an animated effect on the map.
 @param reason The reason for the camera change.

 @note If this method is implemented `-mapView:regionDidChangeAnimated:` will not be called.
 */
- (void)mapView:(MGLMapView *)mapView regionDidChangeWithReason:(MGLCameraChangeReason)reason animated:(BOOL)animated;

#pragma mark Loading the Map

/**
 Tells the delegate that the map view will begin to load.

 This method is called whenever the map view starts loading, including when a
 new style has been set and the map must reload.

 @param mapView The map view that is starting to load.
 */
- (void)mapViewWillStartLoadingMap:(MGLMapView *)mapView;

/**
 Tells the delegate that the map view has finished loading.

 This method is called whenever the map view finishes loading, either after the
 initial load or after a style change has forced a reload.

 @param mapView The map view that has finished loading.
 */
- (void)mapViewDidFinishLoadingMap:(MGLMapView *)mapView;

/**
 Tells the delegate that the map view was unable to load data needed for
 displaying the map.

 This method may be called for a variety of reasons, including a network
 connection failure or a failure to fetch the style from the server. You can use
 the given error message to notify the user that map data is unavailable.

 @param mapView The map view that is unable to load the data.
 @param error The reason the data could not be loaded.
 */
- (void)mapViewDidFailLoadingMap:(MGLMapView *)mapView withError:(NSError *)error;

// TODO
- (void)mapViewWillStartRenderingMap:(MGLMapView *)mapView;

// TODO
- (void)mapViewDidFinishRenderingMap:(MGLMapView *)mapView fullyRendered:(BOOL)fullyRendered;

/**
 Tells the delegate that the map view is about to redraw.

 This method is called any time the map view needs to redraw due to a change in
 the viewpoint or style property transition. This method may be called very
 frequently, even moreso than `-mapViewRegionIsChanging:`. Therefore, your
 implementation of this method should be as lightweight as possible to avoid
 affecting performance.

 @param mapView The map view that is about to redraw.
 */
- (void)mapViewWillStartRenderingFrame:(MGLMapView *)mapView;

/**
 Tells the delegate that the map view has just redrawn.

 This method is called any time the map view needs to redraw due to a change in
 the viewpoint or style property transition. This method may be called very
 frequently, even moreso than `-mapViewRegionIsChanging:`. Therefore, your
 implementation of this method should be as lightweight as possible to avoid
 affecting performance.

 @param mapView The map view that has just redrawn.
 */
- (void)mapViewDidFinishRenderingFrame:(MGLMapView *)mapView fullyRendered:(BOOL)fullyRendered;

/**
 Tells the delegate that the map has just finished loading a style.

 This method is called during the initialization of the map view and after any
 subsequent loading of a new style. This method is called between the
 `-mapViewWillStartRenderingMap:` and `-mapViewDidFinishRenderingMap:` delegate
 methods. Changes to sources or layers of the current style do not cause this
 method to be called.

 This method is the earliest opportunity to modify the layout or appearance of
 the current style before the map view is displayed to the user.

 @param mapView The map view that has just loaded a style.
 @param style The style that was loaded.
 */
- (void)mapView:(MGLMapView *)mapView didFinishLoadingStyle:(MGLStyle *)style;

#pragma mark Tracking User Location

/**
 Tells the delegate that the map view will begin tracking the user’s location.

 This method is called when the value of the `showsUserLocation` property
 changes to `YES`.

 @param mapView The map view that is tracking the user’s location.
 */
- (void)mapViewWillStartLocatingUser:(MGLMapView *)mapView;

/**
 Tells the delegate that the map view has stopped tracking the user’s location.

 This method is called when the value of the `showsUserLocation` property
 changes to `NO`.

 @param mapView The map view that is tracking the user’s location.
 */
- (void)mapViewDidStopLocatingUser:(MGLMapView *)mapView;

/**
 Tells the delegate that the location of the user was updated.

 While the `showsUserLocation` property is set to `YES`, this method is called
 whenever a new location update is received by the map view. This method is also
 called if the map view’s user tracking mode is set to
 `MGLUserTrackingModeFollowWithHeading` and the heading changes, or if it is set
 to `MGLUserTrackingModeFollowWithCourse` and the course changes.

 This method is not called if the application is currently running in the
 background. If you want to receive location updates while running in the
 background, you must use the Core Location framework.

 @param mapView The map view that is tracking the user’s location.
 @param userLocation The location object representing the user’s latest
    location. This property may be `nil`.
 */
- (void)mapView:(MGLMapView *)mapView didUpdateUserLocation:(nullable MGLUserLocation *)userLocation;

/**
 Tells the delegate that an attempt to locate the user’s position failed.

 @param mapView The map view that is tracking the user’s location.
 @param error An error object containing the reason why location tracking
    failed.
 */
- (void)mapView:(MGLMapView *)mapView didFailToLocateUserWithError:(NSError *)error;

/**
 Tells the delegate that the map view’s user tracking mode has changed.

 This method is called after the map view asynchronously changes to reflect the
 new user tracking mode, for example by beginning to zoom or rotate.

 @param mapView The map view that changed its tracking mode.
 @param mode The new tracking mode.
 @param animated Whether the change caused an animated effect on the map.
 */
- (void)mapView:(MGLMapView *)mapView didChangeUserTrackingMode:(MGLUserTrackingMode)mode animated:(BOOL)animated;

/**
 Returns a screen coordinate at which to position the user location annotation.
 This coordinate is relative to the map view’s origin after applying the map view’s
 content insets.
 
 When unimplemented, the user location annotation is aligned within the center of
 the map view with respect to the content insets.
 
 This method will override any values set by `MGLMapView.userLocationVerticalAlignment`
 or `-[MGLMapView setUserLocationVerticalAlignment:]`.
 
 @param mapView The map view that is tracking the user's location.
 */
- (CGPoint)mapViewUserLocationAnchorPoint:(MGLMapView *)mapView;

#pragma mark Managing the Appearance of Annotations

/**
 Returns an annotation image object to mark the given point annotation object on
 the map.

 Implement this method to mark a point annotation with a static image. If you
 want to mark a particular point annotation with an annotation view instead,
 omit this method or have it return `nil` for that annotation, then implement
 `-mapView:viewForAnnotation:`.

 Static annotation images use less memory and draw more quickly than annotation
 views. On the other hand, annotation views are compatible with UIKit, Core
 Animation, and other Cocoa Touch frameworks.

 @param mapView The map view that requested the annotation image.
 @param annotation The object representing the annotation that is about to be
    displayed.
 @return The annotation image object to display for the given annotation or
    `nil` if you want to display the default marker image or an annotation view.
 */
- (nullable MGLAnnotationImage *)mapView:(MGLMapView *)mapView imageForAnnotation:(id <MGLAnnotation>)annotation;

/**
 Returns the alpha value to use when rendering a shape annotation.

 A value of `0.0` results in a completely transparent shape. A value of `1.0`,
 the default, results in a completely opaque shape.

 This method sets the opacity of an entire shape, inclusive of its stroke and
 fill. To independently set the values for stroke or fill, specify an alpha
 component in the color returned by `-mapView:strokeColorForShapeAnnotation:` or
 `-mapView:fillColorForPolygonAnnotation:`.

 @param mapView The map view rendering the shape annotation.
 @param annotation The annotation being rendered.
 @return An alpha value between `0` and `1.0`.
 */
- (CGFloat)mapView:(MGLMapView *)mapView alphaForShapeAnnotation:(MGLShape *)annotation;

/**
 Returns the color to use when rendering the outline of a shape annotation.

 The default stroke color is the map view’s tint color. If a pattern color is
 specified, the result is undefined.

 Opacity may be set by specifying an alpha component. The default alpha value is
 `1.0` and results in a completely opaque stroke.

 @param mapView The map view rendering the shape annotation.
 @param annotation The annotation being rendered.
 @return A color to use for the shape outline.
 */
- (UIColor *)mapView:(MGLMapView *)mapView strokeColorForShapeAnnotation:(MGLShape *)annotation;

/**
 Returns the color to use when rendering the fill of a polygon annotation.

 The default fill color is the map view’s tint color. If a pattern color is
 specified, the result is undefined.

 Opacity may be set by specifying an alpha component. The default alpha value is
 `1.0` and results in a completely opaque shape.

 @param mapView The map view rendering the polygon annotation.
 @param annotation The annotation being rendered.
 @return The polygon’s interior fill color.
 */
- (UIColor *)mapView:(MGLMapView *)mapView fillColorForPolygonAnnotation:(MGLPolygon *)annotation;

/**
 Returns the line width in points to use when rendering the outline of a
 polyline annotation.

 By default, the polyline is outlined with a line `3.0` points wide.

 @param mapView The map view rendering the polygon annotation.
 @param annotation The annotation being rendered.
 @return A line width for the polyline, measured in points.
 */
- (CGFloat)mapView:(MGLMapView *)mapView lineWidthForPolylineAnnotation:(MGLPolyline *)annotation;

#pragma mark Managing Annotation Views

/**
 Returns a view object to mark the given point annotation object on the map.

 Implement this method to mark a point annotation with a view object. If you
 want to mark a particular point annotation with a static image instead, omit
 this method or have it return `nil` for that annotation, then implement
 `-mapView:imageForAnnotation:` instead.

 Annotation views are compatible with UIKit, Core Animation, and other Cocoa
 Touch frameworks. On the other hand, static annotation images use less memory
 and draw more quickly than annotation views.

 The user location annotation view can also be customized via this method. When
 `annotation` is an instance of `MGLUserLocation` (or equal to the map view’s
 `userLocation` property), return an instance of `MGLUserLocationAnnotationView`
 (or a subclass thereof).

 @param mapView The map view that requested the annotation view.
 @param annotation The object representing the annotation that is about to be
    displayed.
 @return The view object to display for the given annotation or `nil` if you
    want to display an annotation image instead.
 */
- (nullable MGLAnnotationView *)mapView:(MGLMapView *)mapView viewForAnnotation:(id <MGLAnnotation>)annotation;

/**
 Tells the delegate that one or more annotation views have been added and
 positioned on the map.

 This method is called just after the views are added to the map. You can
 implement this method to animate the addition of the annotation views.

 @param mapView The map view to which the annotation views were added.
 @param annotationViews An array of `MGLAnnotationView` objects representing the
    views that were added.
 */
- (void)mapView:(MGLMapView *)mapView didAddAnnotationViews:(NSArray<MGLAnnotationView *> *)annotationViews;

#pragma mark Selecting Annotations

/**
 Returns a Boolean value indicating whether the shape annotation can be selected.
 
 If the return value is `YES`, the user can select the annotation by tapping
 on it. If the delegate does not implement this method, the default value is `YES`.
 
 @param mapView The map view that has selected the annotation.
 @param annotation The object representing the shape annotation.
 @return A Boolean value indicating whether the annotation can be selected.
 */
- (BOOL)mapView:(MGLMapView *)mapView shapeAnnotationIsEnabled:(MGLShape *)annotation;

/**
 Tells the delegate that one of its annotations was selected.

 You can use this method to track changes in the selection state of annotations.

 If the annotation is associated with an annotation view, you can also implement
 `-mapView:didSelectAnnotationView:`, which is called immediately after this
 method is called.

 @param mapView The map view containing the annotation.
 @param annotation The annotation that was selected.
 */
- (void)mapView:(MGLMapView *)mapView didSelectAnnotation:(id <MGLAnnotation>)annotation;

/**
 Tells the delegate that one of its annotations was deselected.

 You can use this method to track changes in the selection state of annotations.

 If the annotation is associated with an annotation view, you can also implement
 `-mapView:didDeselectAnnotationView:`, which is called immediately after this
 method is called.

 @param mapView The map view containing the annotation.
 @param annotation The annotation that was deselected.
 */
- (void)mapView:(MGLMapView *)mapView didDeselectAnnotation:(id <MGLAnnotation>)annotation;

/**
 Tells the delegate that one of its annotation views was selected.

 You can use this method to track changes in the selection state of annotation
 views.

 This method is only called for annotation views. To track changes in the
 selection state of all annotations, including those associated with static
 annotation images, implement `-mapView:didSelectAnnotation:`, which is called
 immediately before this method is called.

 @param mapView The map view containing the annotation.
 @param annotationView The annotation view that was selected.
 */
- (void)mapView:(MGLMapView *)mapView didSelectAnnotationView:(MGLAnnotationView *)annotationView;

/**
 Tells the delegate that one of its annotation views was deselected.

 You can use this method to track changes in the selection state of annotation
 views.

 This method is only called for annotation views. To track changes in the
 selection state of all annotations, including those associated with static
 annotation images, implement `-mapView:didDeselectAnnotation:`, which is called
 immediately before this method is called.

 @param mapView The map view containing the annotation.
 @param annotationView The annotation view that was deselected.
 */
- (void)mapView:(MGLMapView *)mapView didDeselectAnnotationView:(MGLAnnotationView *)annotationView;

#pragma mark Managing Callout Views

/**
 Returns a Boolean value indicating whether the annotation is able to display
 extra information in a callout bubble.

 This method is called after an annotation is selected, before any callout is
 displayed for the annotation.

 If the return value is `YES`, a callout view is shown when the user taps on an
 annotation, selecting it. The default callout displays the annotation’s title
 and subtitle. You can add accessory views to either end of the callout by
 implementing the `-mapView:leftCalloutAccessoryViewForAnnotation:` and
 `-mapView:rightCalloutAccessoryViewForAnnotation:` methods. You can further
 customize the callout’s contents by implementing the
 `-mapView:calloutViewForAnnotation:` method.

 If the return value is `NO`, or if this method is absent from the delegate, or
 if the annotation lacks a title, the annotation will not show a callout even
 when selected.

 @param mapView The map view that has selected the annotation.
 @param annotation The object representing the annotation.
 @return A Boolean value indicating whether the annotation should show a
    callout.
 */
- (BOOL)mapView:(MGLMapView *)mapView annotationCanShowCallout:(id <MGLAnnotation>)annotation;

/**
 Returns a callout view to display for the given annotation.

 If this method is present in the delegate, it must return a new instance of a
 view dedicated to display the callout. The returned view will be configured by
 the map view.

 If this method is absent from the delegate, or if it returns `nil`, a standard,
 two-line, bubble-like callout view is displayed by default.

 @param mapView The map view that requested the callout view.
 @param annotation The object representing the annotation.
 @return A view conforming to the `MGLCalloutView` protocol, or `nil` to use the
    default callout view.
 */
- (nullable id <MGLCalloutView>)mapView:(MGLMapView *)mapView calloutViewForAnnotation:(id <MGLAnnotation>)annotation;

/**
 Returns the view to display on the left side of the standard callout bubble.

 The left callout view is typically used to convey information about the
 annotation or to link to custom information provided by your application.

 If the view you specify is a descendant of the `UIControl` class, you can use
 the map view’s delegate to receive notifications when your control is tapped,
 by implementing the `-mapView:annotation:calloutAccessoryControlTapped:`
 method. If the view you specify does not descend from `UIControl`, your view is
 responsible for handling any touch events within its bounds.

 If this method is absent from the delegate, or if it returns `nil`, the
 standard callout view has no accessory view on its left side. The return value
 of this method is ignored if `-mapView:calloutViewForAnnotation:` is present in
 the delegate.

 To display a view on the callout’s right side, implement the
 `-mapView:rightCalloutAccessoryViewForAnnotation:` method.

 @param mapView The map view presenting the annotation callout.
 @param annotation The object representing the annotation with the callout.
 @return The accessory view to display.
 */
- (nullable UIView *)mapView:(MGLMapView *)mapView leftCalloutAccessoryViewForAnnotation:(id <MGLAnnotation>)annotation;

/**
 Returns the view to display on the right side of the standard callout bubble.

 The right callout view is typically used to convey information about the
 annotation or to link to custom information provided by your application.

 If the view you specify is a descendant of the `UIControl` class, you can use
 the map view’s delegate to receive notifications when your control is tapped,
 by implementing the `-mapView:annotation:calloutAccessoryControlTapped:`
 method. If the view you specify does not descend from `UIControl`, your view is
 responsible for handling any touch events within its bounds.

 If this method is absent from the delegate, or if it returns `nil`, the
 standard callout view has no accessory view on its right side. The return value
 of this method is ignored if `-mapView:calloutViewForAnnotation:` is present in
 the delegate.

 To display a view on the callout’s left side, implement the
 `-mapView:leftCalloutAccessoryViewForAnnotation:` method.

 @param mapView The map view presenting the annotation callout.
 @param annotation The object representing the annotation with the callout.
 @return The accessory view to display.
 */
- (nullable UIView *)mapView:(MGLMapView *)mapView rightCalloutAccessoryViewForAnnotation:(id <MGLAnnotation>)annotation;

/**
 Tells the delegate that the user tapped one of the accessory controls in the
 annotation’s callout view.

 In a standard callout view, accessory views contain custom content and are
 positioned on either side of the annotation title text. If an accessory view
 you specify is a descendant of the `UIControl` class, the map view calls this
 method as a convenience whenever the user taps your view. You can use this
 method to respond to taps and perform any actions associated with that control.
 For example, if your control displays additional information about the
 annotation, you could use this method to present a modal panel with that
 information.

 If your custom accessory views are not descendants of the `UIControl` class,
 the map view does not call this method. If the annotation has a custom callout
 view via the `-mapView:calloutViewForAnnotation:` method, you can specify the
 custom accessory views using the `MGLCalloutView` protocol’s
 `leftAccessoryView` and `rightAccessoryView` properties.

 @param mapView The map view containing the specified annotation.
 @param annotation The annotation whose accessory view was tapped.
 @param control The control that was tapped.
 */
- (void)mapView:(MGLMapView *)mapView annotation:(id <MGLAnnotation>)annotation calloutAccessoryControlTapped:(UIControl *)control;

/**
 Tells the delegate that the user tapped on an annotation’s callout view.

 This method is called when the user taps on the body of the callout view, as
 opposed to the callout’s left or right accessory view. If the annotation has a
 custom callout view via the `-mapView:calloutViewForAnnotation:` method, this
 method is only called whenever the callout view calls its delegate’s
 `-[MGLCalloutViewDelegate calloutViewTapped:]` method.

 If this method is present on the delegate, the standard callout view’s body
 momentarily highlights when the user taps it, whether or not this method does
 anything in response to the tap.

 @param mapView The map view containing the specified annotation.
 @param annotation The annotation whose callout was tapped.
 */
- (void)mapView:(MGLMapView *)mapView tapOnCalloutForAnnotation:(id <MGLAnnotation>)annotation;

@end

NS_ASSUME_NONNULL_END
