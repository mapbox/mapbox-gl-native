#import "SMCalloutView.h"
#import "MGLCalloutView.h"

/**
 A concrete implementation of `MGLCalloutView` based on [SMCalloutView](https://github.com/nfarina/calloutview). This callout view displays the represented annotation’s title, subtitle, and accessory views in a compact, two-line layout.
 */
@interface MGLCompactCalloutView : SMCalloutView <MGLCalloutView>

+ (instancetype)platformCalloutView;

@end
