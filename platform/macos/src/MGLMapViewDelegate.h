#import <Foundation/Foundation.h>

#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

@class MGLMapView;
@class MGLAnnotationImage;
@class MGLPolygon;
@class MGLPolyline;
@class MGLShape;

/**
 The `MGLMapViewDelegate` protocol defines a set of optional methods that you
 can use to receive messages from an `MGLMapView` instance. Because many map
 operations require the `MGLMapView` class to load data asynchronously, the map
 view calls these methods to notify your application when specific operations
 complete. The map view also uses these methods to request information about
 annotations displayed on the map, such as the styles and interaction modes to
 apply to individual annotations.
 */
@protocol MGLMapViewDelegate <NSObject>

@optional

#pragma mark Responding to Map Viewpoint Changes

/**
 Tells the delegate that the viewpoint depicted by the map view is about to
 change.
 
 This method is called whenever the currently displayed map camera will start
 changing for any reason.
 
 @param mapView The map view whose viewpoint will change.
 @param animated Whether the change will cause an animated effect on the map.
 */
- (void)mapView:(MGLMapView *)mapView cameraWillChangeAnimated:(BOOL)animated;

/**
 Tells the delegate that the viewpoint depicted by the map view is changing.
 
 This method is called as the currently displayed map camera changes as part of
 an animation, whether due to a user gesture or due to a call to a method such
 as `-[MGLMapView setCamera:animated:]`. During the animation, this method may
 be called many times to report updates to the viewpoint. Therefore, your
 implementation of this method should be as lightweight as possible to avoid
 affecting performance.
 
 @param mapView The map view whose viewpoint is changing.
 */
- (void)mapViewCameraIsChanging:(MGLMapView *)mapView;

/**
 Tells the delegate that the viewpoint depicted by the map view has finished
 changing.
 
 This method is called whenever the currently displayed map camera has finished
 changing, after any calls to `-mapViewRegionIsChanging:` due to animation.
 
 @param mapView The map view whose viewpoint has changed.
 @param animated Whether the change caused an animated effect on the map.
 */
- (void)mapView:(MGLMapView *)mapView cameraDidChangeAnimated:(BOOL)animated;

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

- (void)mapViewWillStartRenderingMap:(MGLMapView *)mapView;
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

#pragma mark Managing the Appearance of Annotations

/**
 Returns an annotation image object to mark the given point annotation object on
 the map.
 
 @param mapView The map view that requested the annotation image.
 @param annotation The object representing the annotation that is about to be
    displayed.
 @return The image object to display for the given annotation or `nil` if you
    want to display the default marker image.
 */
- (nullable MGLAnnotationImage *)mapView:(MGLMapView *)mapView imageForAnnotation:(id <MGLAnnotation>)annotation;

/**
 Returns the alpha value to use when rendering a shape annotation.
 
 A value of 0.0 results in a completely transparent shape. A value of 1.0, the
 default, results in a completely opaque shape.
 
 @param mapView The map view rendering the shape annotation.
 @param annotation The annotation being rendered.
 @return An alpha value between 0 and 1.0.
 */
- (CGFloat)mapView:(MGLMapView *)mapView alphaForShapeAnnotation:(MGLShape *)annotation;

/**
 Returns the color to use when rendering the outline of a shape annotation.
 
 The default stroke color is the selected menu item color. If a pattern color is
 specified, the result is undefined.
 
 @param mapView The map view rendering the shape annotation.
 @param annotation The annotation being rendered.
 @return A color to use for the shape outline.
 */
- (NSColor *)mapView:(MGLMapView *)mapView strokeColorForShapeAnnotation:(MGLShape *)annotation;

/**
 Returns the color to use when rendering the fill of a polygon annotation.
 
 The default fill color is the selected menu item color. If a pattern color is
 specified, the result is undefined.
 
 @param mapView The map view rendering the polygon annotation.
 @param annotation The annotation being rendered.
 @return The polygon’s interior fill color.
 */
- (NSColor *)mapView:(MGLMapView *)mapView fillColorForPolygonAnnotation:(MGLPolygon *)annotation;

/**
 Returns the line width in points to use when rendering the outline of a
 polyline annotation.
 
 By default, the polyline is outlined with a line 3.0 points wide.
 
 @param mapView The map view rendering the polygon annotation.
 @param annotation The annotation being rendered.
 @return A line width for the polyline, measured in points.
 */
- (CGFloat)mapView:(MGLMapView *)mapView lineWidthForPolylineAnnotation:(MGLPolyline *)annotation;

#pragma mark Selecting Annotations

/**
 Tells the delegate that one of its annotations has been selected.
 
 You can use this method to track changes to the selection state of annotations.
 
 @param mapView The map view containing the annotation.
 @param annotation The annotation that was selected.
 */
- (void)mapView:(MGLMapView *)mapView didSelectAnnotation:(id <MGLAnnotation>)annotation;

/**
 Tells the delegate that one of its annotations has been deselected.
 
 You can use this method to track changes in the selection state of annotations.
 
 @param mapView The map view containing the annotation.
 @param annotation The annotation that was deselected.
 */
- (void)mapView:(MGLMapView *)mapView didDeselectAnnotation:(id <MGLAnnotation>)annotation;

#pragma mark Managing Callout Popovers

/**
 Returns a Boolean value indicating whether the annotation is able to display
 extra information in a callout popover.
 
 This method is called after an annotation is selected, before any callout is
 displayed for the annotation.
 
 If the return value is `YES`, a callout popover is shown when the user clicks
 on an annotation, selecting it. The default callout displays the annotation’s
 title and subtitle. You can customize the popover’s contents by implementing
 the `-mapView:calloutViewControllerForAnnotation:` method.
 
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
 Returns a view controller to manage the callout popover’s content view.
 
 Like any instance of `NSPopover`, an annotation callout manages its contents
 with a view controller. The annotation object is the view controller’s
 represented object. This means that you can bind controls in the view
 controller’s content view to KVO-compliant properties of the annotation object,
 such as `title` and `subtitle`.
 
 If each annotation should have an identical callout, you can set the
 `MGLMapView` instance’s `-setCalloutViewController:` method instead.
 
 @param mapView The map view that is requesting a callout view controller.
 @param annotation The object representing the annotation.
 @return A view controller for the given annotation.
 */
- (nullable NSViewController *)mapView:(MGLMapView *)mapView calloutViewControllerForAnnotation:(id <MGLAnnotation>)annotation;

@end

NS_ASSUME_NONNULL_END
