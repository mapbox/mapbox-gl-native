#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

@protocol MGLCalloutViewDelegate;

/**
 A protocol for a `UIView` subclass that displays information about a selected
 annotation near that annotation.
 */
@protocol MGLCalloutView <NSObject>

/**
 Presents a callout view by adding it to `inView` and pointing at the given rect of `inView`’s bounds. Constrains the callout to the bounds of the given view.
 */
- (void)presentCalloutFromRect:(CGRect)rect inView:(UIView *)view constrainedToView:(UIView *)constrainedView animated:(BOOL)animated;

/**
 Dismisses the callout view.
 */
- (void)dismissCalloutAnimated:(BOOL)animated;

@optional

@property (nonatomic, strong) NSString *title;
@property (nonatomic, strong) NSString *subtitle;

/**
 A view that the user may tap to perform an action. This view is conventionally positioned on the left side of the callout view.
 */
@property (nonatomic, strong) UIView *leftAccessoryView;

/**
 A view that the user may tap to perform an action. This view is conventionally positioned on the right side of the callout view.
 */
@property (nonatomic, strong) UIView *rightAccessoryView;

@property (nonatomic, strong) UIView *detailAccessoryView;

@property (nonatomic, weak) id<MGLCalloutViewDelegate> delegate;

@end

NS_ASSUME_NONNULL_END
