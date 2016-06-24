#import <UIKit/UIKit.h>

#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

@protocol MGLAnnotation;

typedef NS_ENUM(NSUInteger, MGLAnnotationViewDragState) {
    MGLAnnotationViewDragStateNone = 0,     // View is sitting on the map.
    MGLAnnotationViewDragStateStarting,     // View is beginning to drag.
    MGLAnnotationViewDragStateDragging,     // View is being dragged.
    MGLAnnotationViewDragStateCanceling,    // View dragging was cancelled and will be returned to its starting positon.
    MGLAnnotationViewDragStateEnding        // View was dragged.
};

/** The MGLAnnotationView class is responsible for representing point-based annotation markers as a view. Annotation views represent an annotation object, which is an object that corresponds to the MGLAnnotation protocol. When an annotation’s coordinate point is visible on the map view, the map view delegate is asked to provide a corresponding annotation view. If an annotation view is created with a reuse identifier, the map view may recycle the view when it goes offscreen. */
@interface MGLAnnotationView : UIView

/**
 Initializes and returns a new annotation view object.
 
 @param reuseIdentifier The string that identifies that this annotation view is reusable.
 @return The initialized annotation view object.
 */
- (instancetype)initWithReuseIdentifier:(nullable NSString *)reuseIdentifier;

/**
 This property will be set to the associated annotation when the view is visible.
 
 When the view is queued and waiting to be reused, the value will be set to nil.
 */
@property (nonatomic, readonly, nullable) id <MGLAnnotation> annotation;

/**
 The string that identifies that this annotation view is reusable. (read-only)
 
 You specify the reuse identifier when you create the view. You use the identifier later to retrieve an annotation view that was
 created previously but which is currently unused because its annotation is not on screen.
 
 If you define distinctly different types of annotations (with distinctly different annotation views to go with them), you can
 differentiate between the annotation types by specifying different reuse identifiers for each one.
 */
@property (nonatomic, readonly, nullable) NSString *reuseIdentifier;

/**
 Annotation view is centered at the coordinate point of the associated annotation.
 
 By changing this property you can reposition the view as needed. The offset is measured in points.
 Positive offset moves the annotation view towards the bottom right, while negative offset moves it towards the top left.
 */
@property (nonatomic) CGVector centerOffset;


/**
 Setting this property to YES will force the annotation view to tilt according to the associated map view.
 */
@property (nonatomic, assign, getter=isFlat) BOOL flat;

/**
 Defaults to NO and becomes YES when the view is tapped on.
 
 Selecting another view will first deselect the currently selected view.
 This property should not be changed directly.
 */
@property (nonatomic, assign, getter=isSelected) BOOL selected;

/**
 Subclasses may override this method in order to customize appearance.
 This method should not be called directly.
 */
- (void)setSelected:(BOOL)selected animated:(BOOL)animated;

/*
 This property defaults to YES. Setting it to NO will cause the annotation view to ignore all touch events.
 Subclasses may use this property to customize the appearance.
 */
@property (nonatomic, assign, getter=isEnabled) BOOL enabled;

/**
 Setting this property to YES will make the view draggable. Long-press followed by a pan gesture will start to move the
 view around the map. `-mapView:didDragAnnotationView:toCoordinate:` will be called when a view is dropped.
 */
@property (nonatomic, assign, getter=isDraggable) BOOL draggable;

/**
 All states are handled automatically when `draggable` is set to YES.
 Custom animations can be achieved by overriding setDragState:animated:
 */
@property (nonatomic, readonly) MGLAnnotationViewDragState dragState;

/**
 Called when the `dragState` changes.
 
 Implementer may override this method in order to customize animations in subclasses.
 */
- (void)setDragState:(MGLAnnotationViewDragState)dragState animated:(BOOL)animated NS_REQUIRES_SUPER;

/**
 Setting this property to YES will cause the annotation view to shrink as it approaches the horizon and grow as it moves away from the
 horizon when the associated map view is tilted. Conversely, setting this property to NO will ensure that the annotation view maintains
 a constant size even when the map view is tilted. To maintain consistency with annotation representations that are not backed by an
 MGLAnnotationView object, the default value of this property is YES.
 */
@property (nonatomic, assign, getter=isScaledWithViewingDistance) BOOL scalesWithViewingDistance;

/**
 Called when the view is removed from the reuse queue.

 The default implementation of this method does nothing. You can override it in your custom annotation views and use it to put the view 
 in a known state before it is returned to your map view delegate.
 */
- (void)prepareForReuse;

@end

NS_ASSUME_NONNULL_END
