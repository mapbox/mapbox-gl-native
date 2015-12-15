#import <Foundation/Foundation.h>

@protocol MGLCalloutViewDelegate;

@protocol MGLCalloutViewProtocol <NSObject>

@property (nonatomic, copy) NSString *title, *subtitle;
@property (nonatomic, strong) UIView *leftAccessoryView, *rightAccessoryView;
@property (nonatomic, weak) id<MGLCalloutViewDelegate> delegate;

// Presents a callout view by adding it to "inView" and pointing at the given rect of inView's bounds.
// Constrains the callout to the bounds of the given view. Optionally scrolls the given rect into view (plus margins)
// if -delegate is set and responds to -delayForRepositionWithSize.
- (void)presentCalloutFromRect:(CGRect)rect inView:(UIView *)view constrainedToView:(UIView *)constrainedView animated:(BOOL)animated;

// Same as the view-based presentation, but inserts the callout into a CALayer hierarchy instead. Be aware that you'll have to direct
// your own touches to any accessory views, since CALayer doesn't relay touch events.
- (void)presentCalloutFromRect:(CGRect)rect inLayer:(CALayer *)layer constrainedToLayer:(CALayer *)constrainedLayer animated:(BOOL)animated;

- (void)dismissCalloutAnimated:(BOOL)animated;

@end


@protocol MGLCalloutViewDelegate <NSObject>

@optional
// Controls whether the callout "highlights" when pressed. default YES. You must also respond to `-calloutViewClicked` below.
// Not honored by SMClassicCalloutView.
- (BOOL)calloutViewShouldHighlight:(UIView<MGLCalloutViewProtocol> *)calloutView;

// Called when the callout view is clicked. Not honored by SMClassicCalloutView.
- (void)calloutViewClicked:(UIView<MGLCalloutViewProtocol> *)calloutView;

@end