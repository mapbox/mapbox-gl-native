#import <Foundation/Foundation.h>
#import <CoreGraphics/CoreGraphics.h>
#import <CoreLocation/CoreLocation.h>

#import "MGLAttributionInfo.h"

NS_ASSUME_NONNULL_BEGIN

@interface MGLAttributionInfo (Private)

/**
 Parses and returns the attribution infos contained in the given HTML source
 code string.

 @param htmlString The HTML source code to parse.
 @param fontSize The default text size in points.
 @param linkColor The default link color.
 */
+ (NS_ARRAY_OF(MGLAttributionInfo *) *)attributionInfosFromHTMLString:(nullable NSString *)htmlString fontSize:(CGFloat)fontSize linkColor:(nullable MGLColor *)linkColor;

+ (NSAttributedString *)attributedStringForAttributionInfos:(NS_ARRAY_OF(MGLAttributionInfo *) *)attributionInfos;

/**
 Returns a copy of the `URL` property modified to account for the given style
 URL, center coordinate, and zoom level.

 @param styleURL The map’s style URL.
 @param centerCoordinate The map’s center coordinate.
 @param zoomLevel The map’s zoom level. See the `MGLMapView.zoomLevel` property
    for more information.
 @param direction The heading of the map, measured in degrees clockwise from
    true north.
 @param pitch Pitch toward the horizon measured in degrees, with 0 degrees
    resulting in a two-dimensional map.
 @return A modified URL containing a fragment that points to the specified
    viewport. If the `feedbackLink` property is set to `NO`, this method returns
    `nil`.
 */
- (nullable NSURL *)feedbackURLForStyleURL:(nullable NSURL *)styleURL atCenterCoordinate:(CLLocationCoordinate2D)centerCoordinate zoomLevel:(double)zoomLevel direction:(CLLocationDirection)direction pitch:(CGFloat)pitch;

@end

@interface NSMutableArray (MGLAttributionInfoAdditions)

/**
 Adds the given attribution info object to the receiver as long as it isn’t
 redundant to any object already in the receiver. Any existing object that is
 redundant to the given object is replaced by the given object.

 @param info The info object to add to the receiver.
 */
- (void)growArrayByAddingAttributionInfo:(MGLAttributionInfo *)info;

/**
 Adds each of the given attribution info objects to the receiver as long as it
 isn’t redundant to any object already in the receiver. Any existing object that
 is redundant to the given object is replaced by the given object.

 @param infos An array of info objects to add to the receiver.
 */
- (void)growArrayByAddingAttributionInfosFromArray:(NS_ARRAY_OF(MGLAttributionInfo *) *)infos;

@end

NS_ASSUME_NONNULL_END
