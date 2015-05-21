#import "MGLMapView.h"

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

/** Tells the delegate that the map view will begin tracking the user’s location.
*
*   This method is called when the value of the showsUserLocation property changes to `YES`.
*
*   @param mapView The map view that is tracking the user’s location. */
- (void)mapViewWillStartLocatingUser:(MGLMapView *)mapView;

/** Tells the delegate that the map view has stopped tracking the user’s location.
*
*   This method is called when the value of the showsUserLocation property changes to `NO`.
*
*   @param mapView The map view that is tracking the user’s location. */
- (void)mapViewDidStopLocatingUser:(MGLMapView *)mapView;

/** Tells the delegate that the location of the user was updated.
*
*   While the showsUserLocation property is set to `YES`, this method is called whenever a new location update is received by the map view. This method is also called if the map view’s user tracking mode is set to MGLUserTrackingModeFollowWithHeading and the heading changes.
*
*   This method is not called if the application is currently running in the background. If you want to receive location updates while running in the background, you must use the Core Location framework.
*
*   @param mapView The map view that is tracking the user’s location.
*   @param userLocation The location object representing the user’s latest location. This property may be `nil`. */
- (void)mapView:(MGLMapView *)mapView didUpdateUserLocation:(MGLUserLocation *)userLocation;

/** Tells the delegate that an attempt to locate the user’s position failed.
*   @param mapView The map view that is tracking the user’s location.
*   @param error An error object containing the reason why location tracking failed. */
- (void)mapView:(MGLMapView *)mapView didFailToLocateUserWithError:(NSError *)error;

/** Tells the delegate that the map view’s user tracking mode has changed.
*
*   This method is called after the map view asynchronously changes to reflect the new user tracking mode, for example by beginning to zoom or rotate.
*
*   @param mapView The map view that changed its tracking mode.
*   @param mode The new tracking mode.
*   @param animated Whether the change caused an animated effect on the map. */
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
