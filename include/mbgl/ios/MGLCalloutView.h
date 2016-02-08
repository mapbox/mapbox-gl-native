#import <Foundation/Foundation.h>

#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

@protocol MGLCalloutViewDelegate;
@protocol MGLAnnotation;

/**
 A protocol for a `UIView` subclass that displays information about a selected annotation near that annotation.
 */
@protocol MGLCalloutView <NSObject>

/**
 An object conforming to the `MGLAnnotation` protocol whose details this callout view displays.
 */
@property (nonatomic, strong) id <MGLAnnotation> representedObject;

/**
 A view that the user may tap to perform an action. This view is conventionally positioned on the left side of the callout view.
 */
@property (nonatomic, strong) UIView *leftAccessoryView;

/**
 A view that the user may tap to perform an action. This view is conventionally positioned on the right side of the callout view.
 */
@property (nonatomic, strong) UIView *rightAccessoryView;

/**
 An object conforming to the `MGLCalloutViewDelegate` method that receives messages related to the callout view’s interactive subviews.
 */
@property (nonatomic, weak) id<MGLCalloutViewDelegate> delegate;

/**
 Presents a callout view by adding it to `inView` and pointing at the given rect of `inView`’s bounds. Constrains the callout to the bounds of the given view.
 */
- (void)presentCalloutFromRect:(CGRect)rect inView:(UIView *)view constrainedToView:(UIView *)constrainedView animated:(BOOL)animated;

/**
 Dismisses the callout view.
 */
- (void)dismissCalloutAnimated:(BOOL)animated;

@end

/**
 The MGLCalloutViewDelegate protocol defines a set of optional methods that you can use to receive messages from an object that conforms to the MGLCalloutView protocol. The callout view uses these methods to inform the delegate that the user has interacted with the the callout view.
 */
@protocol MGLCalloutViewDelegate <NSObject>

@optional
/**
 Returns a Boolean value indicating whether the entire callout view “highlights” when tapped. The default value is `YES`, which means the callout view highlights when tapped.
 
 The return value of this method is ignored unless the delegate also responds to the `-calloutViewTapped` method.
 */
- (BOOL)calloutViewShouldHighlight:(UIView<MGLCalloutView> *)calloutView;

/**
 Tells the delegate that the callout view has been tapped.
 */
- (void)calloutViewTapped:(UIView<MGLCalloutView> *)calloutView;

@end

NS_ASSUME_NONNULL_END