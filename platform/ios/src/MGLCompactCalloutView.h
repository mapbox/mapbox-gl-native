#import "MGLCalloutView.h"

/**
 This callout view displays the represented annotation’s title, subtitle, and accessory views in a compact, two-line layout.
 */
@interface MGLCompactCalloutView : UIView <MGLCalloutView>

+ (instancetype)calloutView;

@end
