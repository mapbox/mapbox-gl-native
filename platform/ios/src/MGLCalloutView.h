#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@protocol MGLCalloutViewDelegate;
@protocol MGLAnnotation;

/**
 A protocol for a `UIView` subclass that displays information about a selected
 annotation near that annotation.
 */
@protocol MGLCalloutView <NSObject>

/**
 An object conforming to the `MGLAnnotation` protocol whose details this callout
 view displays.
 */
@property (nonatomic, strong) id <MGLAnnotation> representedObject;

/**
 A view that the user may tap to perform an action. This view is conventionally
 positioned on the left side of the callout view.
 */
@property (nonatomic, strong) UIView *leftAccessoryView;

/**
 A view that the user may tap to perform an action. This view is conventionally
 positioned on the right side of the callout view.
 */
@property (nonatomic, strong) UIView *rightAccessoryView;

/**
 An object conforming to the `MGLCalloutViewDelegate` method that receives
 messages related to the callout view’s interactive subviews.
 */
@property (nonatomic, weak) id<MGLCalloutViewDelegate> delegate;

/**
 Presents a callout view by adding it to `view` and pointing at the given rect
 of `view`’s bounds. Constrains the callout to the bounds of the given view.
 */
- (void)presentCalloutFromRect:(CGRect)rect inView:(UIView *)view constrainedToView:(UIView *)constrainedView animated:(BOOL)animated;

/**
 Dismisses the callout view.
 */
- (void)dismissCalloutAnimated:(BOOL)animated;

@optional

/**
 A Boolean value indicating whether the callout view should be anchored to
 the corresponding annotation. You can adjust the callout view’s precise location by
 overriding -[UIView setCenter:]. The callout view will not be anchored to the
 annotation if this optional property is unimplemented.
 */
@property (nonatomic, readonly, assign, getter=isAnchoredToAnnotation) BOOL anchoredToAnnotation;

/**
 A Boolean value indicating whether the callout view should be dismissed automatically
 when the map view’s viewport changes. Note that a single tap on the map view
 still dismisses the callout view regardless of the value of this property.
 The callout view will be dismissed if this optional property is unimplemented.
 */
@property (nonatomic, readonly, assign) BOOL dismissesAutomatically;

@end

/**
 The `MGLCalloutViewDelegate` protocol defines a set of optional methods that
 you can use to receive messages from an object that conforms to the
 `MGLCalloutView` protocol. The callout view uses these methods to inform the
 delegate that the user has interacted with the the callout view.
 */
@protocol MGLCalloutViewDelegate <NSObject>

@optional
/**
 Returns a Boolean value indicating whether the entire callout view “highlights”
 when tapped. The default value is `YES`, which means the callout view
 highlights when tapped.

 The return value of this method is ignored unless the delegate also responds to
 the `-calloutViewTapped` method.
 */
- (BOOL)calloutViewShouldHighlight:(UIView<MGLCalloutView> *)calloutView;

/**
 Tells the delegate that the callout view has been tapped.
 */
- (void)calloutViewTapped:(UIView<MGLCalloutView> *)calloutView;

/**
 Called before the callout view appears on screen, or before the appearance
 animation will start.
 */
- (void)calloutViewWillAppear:(UIView<MGLCalloutView> *)calloutView;

/**
 Called after the callout view appears on screen, or after the appearance
 animation is complete.
 */
- (void)calloutViewDidAppear:(UIView<MGLCalloutView> *)calloutView;

@end

NS_ASSUME_NONNULL_END
