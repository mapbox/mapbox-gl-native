#import <UIKit/UIKit.h>

#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

/** The MGLAnnotationView class is responsible for representing point-based annotation markers as a view. Annotation views represent an annotation object, which is an object that corresponds to the MGLAnnotation protocol. When an annotation’s coordinate point is visible on the map view, the map view delegate is asked to provide a corresponding annotation view. If an annotation view is created with a reuse identifier, the map view may recycle the view when it goes offscreen. */
@interface MGLAnnotationView : UIView

/**
 Initializes and returns a new annotation view object.
 
 @param reuseIdentifier The string that identifies that this annotation view is reusable.
 @return The initialized annotation view object or `nil` if there was a problem initializing the object.
 */
- (instancetype)initWithReuseIdentifier:(NSString *)reuseIdentifier;

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
 Called when the view is removed from the reuse queue.

 The default implementation of this method does nothing. You can override it in your custom annotation views and use it to put the view 
 in a known state before it is returned to your map view delegate.
 */
- (void)prepareForReuse;

@end

NS_ASSUME_NONNULL_END
