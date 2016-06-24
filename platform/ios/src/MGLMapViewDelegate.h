#import <UIKit/UIKit.h>

#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

@class MGLMapView;

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
 Returns the stroke color to use when rendering a shape annotation. Defaults to the map view’s tint color.
 
 @param mapView The map view rendering the shape annotation.
 @param annotation The annotation being rendered.
 @return A color to use for the shape outline.
 */
- (UIColor *)mapView:(MGLMapView *)mapView strokeColorForShapeAnnotation:(MGLShape *)annotation;

/**
 Returns the fill color to use when rendering a polygon annotation. Defaults to the map view’s tint color.
 
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

/**
 Tells the delegate that one of its annotation views was selected.
 
 You can use this method to track changes in the selection state of annotation views.
 
 @param mapView The map view containing the annotation.
 @param annotationView The annotation view that was selected.
 */
- (void)mapView:(MGLMapView *)mapView didSelectAnnotationView:(MGLAnnotationView *)annotationView;

/**
 Tells the delegate that one of its annotation views was deselected.
 
 You can use this method to track changes in the selection state of annotation views.
 
 @param mapView The map view containing the annotation.
 @param annotationView The annotation view that was deselected.
 */
- (void)mapView:(MGLMapView *)mapView didDeselectAnnotationView:(MGLAnnotationView *)annotationView;

#pragma mark Managing annotation views

/**
 Returns a view object to use for the marker for the specified point annotation object.
 
 @param mapView The map view that requested the annotation view.
 @param annotation The object representing the annotation that is about to be displayed.
 @return The view object to display for the specified annotation or `nil` if you want to display the default marker image.
 */
- (nullable MGLAnnotationView *)mapView:(MGLMapView *)mapView viewForAnnotation:(id <MGLAnnotation>)annotation;

/**
 Tells the delegate that new annotation views have been added and positioned on the map.
 
 You can use this method to animate the adding of the annotation views.
 
 @param mapView The map view containing the annotation views.
 @param annotationViews The annotation views that was added.
 */
- (void)mapView:(MGLMapView *)mapView didAddAnnotationViews:(NS_ARRAY_OF(MGLAnnotationView *) *)annotationViews;

/**
 Tells the delegate that one if its annotation views was dragged to a new coordinate.
 
 In order to make the new location persistent, you have to update the `coordinate` property of the associated annotation.
 
 @param mapView The map view containing the annotation view.
 @param annotationView The annotation view that was dragged.
 @param coordinate The coordinate that the annotation view was dropped on.
 
 */
- (void)mapView:(MGLMapView *)mapView didDragAnnotationView:(MGLAnnotationView *)annotationView toCoordinate:(CLLocationCoordinate2D)coordinate;

@end

NS_ASSUME_NONNULL_END
