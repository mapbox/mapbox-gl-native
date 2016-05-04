#import "MGLCalloutView.h"

/**
 This callout view displays the represented annotation’s title, subtitle, and accessory views in a compact, two-line layout.
 */
@interface MGLCompactCalloutView : UIView <MGLCalloutView>

+ (instancetype)calloutView;

@property (nonatomic, strong) NSString *title;
@property (nonatomic, strong) NSString *subtitle;

@property (nonatomic, strong) UIView *leftAccessoryView;
@property (nonatomic, strong) UIView *rightAccessoryView;
@property (nonatomic, strong) UIView *detailAccessoryView;

@end
