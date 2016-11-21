#import "SMCalloutView.h"
#import "MGLCalloutView.h"
#import "MGLTypes.h"

/**
 A concrete implementation of `MGLCalloutView` based on
 <a href="https://github.com/nfarina/calloutview">SMCalloutView</a>. This
 callout view displays the represented annotation’s title, subtitle, and
 accessory views in a compact, two-line layout.
 */
MGL_EXTERN
@interface MGLCompactCalloutView : SMCalloutView <MGLCalloutView>

+ (instancetype)platformCalloutView;

@end
