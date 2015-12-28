#import <Foundation/Foundation.h>

#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

@protocol MGLCalloutViewDelegate;

@protocol MGLCalloutViewProtocol <NSObject>

@property (nonatomic, copy) NSString *title, *subtitle;
@property (nonatomic, strong) UIView *leftAccessoryView, *rightAccessoryView;
@property (nonatomic, weak) id<MGLCalloutViewDelegate> delegate;

// Presents a callout view by adding it to "inView" and pointing at the given rect of inView's bounds.
// Constrains the callout to the bounds of the given view.
- (void)presentCalloutFromRect:(CGRect)rect inView:(UIView *)view constrainedToView:(UIView *)constrainedView animated:(BOOL)animated;

- (void)dismissCalloutAnimated:(BOOL)animated;

@end


@protocol MGLCalloutViewDelegate <NSObject>

@optional
// Controls whether the callout "highlights" when pressed. default YES. You must also respond to `-calloutViewClicked` below.
- (BOOL)calloutViewShouldHighlight:(UIView<MGLCalloutViewProtocol> *)calloutView;

// Called when the callout view is clicked.
- (void)calloutViewClicked:(UIView<MGLCalloutViewProtocol> *)calloutView;

@end

NS_ASSUME_NONNULL_END