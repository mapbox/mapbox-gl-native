#import <UIKit/UIKit.h>

#import "MGLFoundation.h"

NS_ASSUME_NONNULL_BEGIN

@protocol MGLAnnotation;

/** These constants indicate the current drag state of an annotation view. */
typedef NS_ENUM(NSUInteger, MGLAnnotationViewDragState) {
    /**
     The view is not involved in a drag operation.
     */
    MGLAnnotationViewDragStateNone = 0,
    /**
     An action occurred that indicated the view should begin dragging.

     The map view automatically moves draggable annotation views to this state
     in response to the dragging the view after pressing and holding on it.
     */
    MGLAnnotationViewDragStateStarting,
    /**
     The view is in the midst of a drag operation and is actively tracking the
     user’s gesture.
     */
    MGLAnnotationViewDragStateDragging,
    /**
     An action occurred that indicated the view should cancel the drag
     operation.
     */
    MGLAnnotationViewDragStateCanceling,
    /**
     An action occurred that indicated the view was dropped by the user.

     The map view automatically moves annotation views to this state in response
     to the user lifting their finger at the end of a drag gesture.
     */
    MGLAnnotationViewDragStateEnding,
};

/**
 The `MGLAnnotationView` class is responsible for marking a point annotation
 with a view. Annotation views represent an annotation object, which is an
 object that corresponds to the `MGLAnnotation` protocol. When an annotation’s
 geographic coordinate is visible in the map view, the map view asks its
 delegate to a corresponding annotation view. If an annotation view is created
 with a reuse identifier, the map view may recycle the view when it goes
 offscreen.

 Annotation views are compatible with UIKit, Core Animation, and other Cocoa
 Touch frameworks. On the other hand, if you do not need animation or
 interactivity such as dragging, you can use an `MGLAnnotationImage` instead to
 conserve memory and optimize drawing performance.
 */
MGL_EXPORT
@interface MGLAnnotationView : UIView <NSSecureCoding>

#pragma mark Initializing and Preparing the View

/**
 Initializes and returns a new annotation view object.

 The reuse identifier provides a way for you to improve performance by recycling
 annotation views as they enter and leave the map’s viewport. As an annotation
 leaves the viewport, the map view moves its associated view to a reuse queue.
 When a new annotation becomes visible, you can request a view for that
 annotation by passing the appropriate reuse identifier string to the
 `-[MGLMapView dequeueReusableAnnotationViewWithIdentifier:]` method.

 @param reuseIdentifier A unique string identifier for this view that allows you
    to reuse this view with multiple similar annotations. You can set this
    parameter to `nil` if you don’t intend to reuse the view, but it is a good
    idea in general to specify a reuse identifier to avoid creating redundant
    views.
 @return The initialized annotation view object.
 */
- (instancetype)initWithReuseIdentifier:(nullable NSString *)reuseIdentifier;

/**
 Initializes and returns a new annotation view object.
 
 Providing an annotation allows you to explicitly associate the annotation instance
 with the new view and, in custom subclasses of `MGLAnnotationView`, customize the view
 based on properties of the annotation instance in an overridden initializer. However,
 annotation views that are reused will not necessarily be associated with the
 same annotation they were initialized with. Also, annotation views that are in
 the reuse queue will have a nil value for the annotation property. Passing an annotation
 instance to the view is optional and the map view will automatically associate annotations
 with views when views are provided to the map via the `-[MGLMapViewDelegate mapView:viewForAnnotation:]`
 method.
 
 The reuse identifier provides a way for you to improve performance by recycling
 annotation views as they enter and leave the map’s viewport. As an annotation
 leaves the viewport, the map view moves its associated view to a reuse queue.
 When a new annotation becomes visible, you can request a view for that
 annotation by passing the appropriate reuse identifier string to the
 `-[MGLMapView dequeueReusableAnnotationViewWithIdentifier:]` method.
 
 @param annotation The annotation object to associate with the new view.
 @param reuseIdentifier A unique string identifier for this view that allows you
 to reuse this view with multiple similar annotations. You can set this
 parameter to `nil` if you don’t intend to reuse the view, but it is a good
 idea in general to specify a reuse identifier to avoid creating redundant
 views.
 @return The initialized annotation view object.
 */
- (instancetype)initWithAnnotation:(nullable id<MGLAnnotation>)annotation reuseIdentifier:(nullable NSString *)reuseIdentifier;

/**
 Called when the view is removed from the reuse queue.

 The default implementation of this method does nothing. You can override it in
 your custom annotation view implementation to put the view in a known state
 before it is returned to your map view delegate.
 */
- (void)prepareForReuse;

/**
 The annotation object currently associated with the view.

 You should not change the value of this property directly. This property
 contains a non-`nil` value while the annotation view is visible on the map. If
 the view is queued, waiting to be reused, the value is `nil`.
 */
@property (nonatomic, nullable) id <MGLAnnotation> annotation;

/**
 The string that identifies that this annotation view is reusable.

 You specify the reuse identifier when you create the view. You use the
 identifier later to retrieve an annotation view that was created previously but
 which is currently unused because its annotation is not on-screen.

 If you define distinctly different types of annotations (with distinctly
 different annotation views to go with them), you can differentiate between the
 annotation types by specifying different reuse identifiers for each one.
 */
@property (nonatomic, readonly, nullable) NSString *reuseIdentifier;

#pragma mark Configuring the Appearance

/**
 The offset, measured in points, at which to place the center of the view.

 By default, the center point of an annotation view is placed at the geographic
 coordinate point of the associated annotation. If you do not want the view to
 be centered, you can use this property to reposition the view. The offset’s
 `dx` and `dy` values are measured in points. Positive offset values move the
 annotation view down and to the right, while negative values move it up and to
 the left.

 Set the offset if the annotation view’s visual center point is somewhere other
 than the logical center of the view. For example, the view may contain an image
 that depicts a downward-pointing pushpin or thumbtack, with the tip positioned
 at the center-bottom of the view. In that case, you would set the offset’s `dx`
 to zero and its `dy` to half the height of the view.
 */
@property (nonatomic) CGVector centerOffset;

/**
 A Boolean value that determines whether the annotation view grows and shrinks
 as the distance between the viewpoint and the annotation view changes on a
 tilted map.

 When the value of this property is `YES` and the map is tilted, the annotation
 view appears smaller if it is towards the top of the view (closer to the
 horizon) and larger if it is towards the bottom of the view (closer to the
 viewpoint). This is also the behavior of `MGLAnnotationImage` objects. When the
 value of this property is `NO` or the map’s pitch is zero, the annotation view
 remains the same size regardless of its position on-screen.

 The default value of this property is `NO`. Keep this property set to `NO` if
 the view’s legibility is important.

 @note Scaling many on-screen annotation views can contribute to poor map
    performance. Consider keeping this property disabled if your use case
    involves hundreds or thousands of annotation views.
 */
@property (nonatomic, assign) BOOL scalesWithViewingDistance;

/**
 A Boolean value that determines whether the annotation view rotates together
 with the map.

 When the value of this property is `YES` and the map is rotated, the annotation
 view rotates. This is also the behavior of `MGLAnnotationImage` objects. When the
 value of this property is `NO` the annotation has its rotation angle fixed.

 The default value of this property is `NO`. Set this property to `YES` if the
 view’s rotation is important.
 */
@property (nonatomic, assign) BOOL rotatesToMatchCamera;

#pragma mark Managing the Selection State

/**
 A Boolean value indicating whether the annotation view is currently selected.

 You should not set the value of this property directly. If the property is set
 to `YES`, the annotation view is displaying a callout.

 By default, this property is set to `NO` and becomes `YES` when the user taps
 the view. Selecting another annotation, whether it is associated with an
 `MGLAnnotationView` or `MGLAnnotationImage` object, deselects any currently
 selected view.

 Setting this property changes the view’s appearance to reflect the new value
 immediately. If you want the change to be animated, use the
 `-setSelected:animated:` method instead.
 */
@property (nonatomic, assign, getter=isSelected) BOOL selected;

/**
 Sets the selection state of the annotation view with an optional animation.

 You should not call this method directly. A map view calls this method in
 response to user interactions with the annotation. Subclasses may override this
 method in order to customize the appearance of the view depending on its
 selection state.

 @param selected `YES` if the view should display itself as selected; `NO`
    if it should display itself as unselected.
 @param animated `YES` if the change in selection state is animated; `NO` if the
    change is immediate.
 */
- (void)setSelected:(BOOL)selected animated:(BOOL)animated;

/*
 A Boolean value indicating whether the annotation is enabled.

 The default value of this property is `YES`. If the value of this property is
 `NO`, the annotation view ignores touch events and cannot be selected.
 Subclasses may also customize the appearance of the view depending on its
 enabled state.
 */
@property (nonatomic, assign, getter=isEnabled) BOOL enabled;

#pragma mark Supporting Drag Operations

/**
 A Boolean value indicating whether the annotation view is draggable.

 If this property is set to `YES`, the user can drag the annotation after
 pressing and holding the view, and the associated annotation object must also
 implement the `-setCoordinate:` method. The default value of this property is
 `NO`.

 Setting this property to `YES` lets the map view know that the annotation is
 always draggable. In other words, you cannot conditionalize drag operations by
 attempting to stop an operation that has already been initiated; doing so can
 lead to undefined behavior. Once begun, the drag operation should always
 continue to completion.
 */
@property (nonatomic, assign, getter=isDraggable) BOOL draggable;

/**
 The current drag state of the annotation view.

 All states are handled automatically when the `draggable` property is set to
 `YES`. To perform a custom animation in response to a change to this property,
 override the `-setDragState:animated:` method.
 */
@property (nonatomic, readonly) MGLAnnotationViewDragState dragState;

/**
 Sets the current drag state for the annotation view.

 You can override this method to animate a custom annotation view as the user
 drags it. As the system detects user actions that would indicate a drag, it
 calls this method to update the drag state.
 */
- (void)setDragState:(MGLAnnotationViewDragState)dragState animated:(BOOL)animated NS_REQUIRES_SUPER;

@end

NS_ASSUME_NONNULL_END
