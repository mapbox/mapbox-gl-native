#import <UIKit/UIKit.h>
#import <mbgl/ios/MGLCalloutViewProtocol.h>

/*
 * Basic custom callout view to demonstrate how to
 * add your own on your app. Will only show the
 * callout title for demonstration purpose.
 */

@interface MBXCustomCalloutView : UIView <MGLCalloutViewProtocol>

@property (nonatomic, copy) NSString *title, *subtitle;
@property (nonatomic, strong) UIView *leftAccessoryView, *rightAccessoryView;
@property (nonatomic, weak) id<MGLCalloutViewDelegate> delegate;

@end
