#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>

#import "MGLFoundation.h"

NS_ASSUME_NONNULL_BEGIN

/**
 The `MGLCompassDirectionFormatter` class provides properly formatted
 descriptions of absolute headings. For example, a value of `90` may be
 formatted as “east”, depending on the locale.

 Use this class to create localized heading strings when displaying directions
 irrespective of the user’s current location. To format a direction relative to
 the user’s current location, use `MGLClockDirectionFormatter` instead.
 */
MGL_EXPORT
@interface MGLCompassDirectionFormatter : NSFormatter

/**
 The unit style used by this formatter.

 This property defaults to `NSFormattingUnitStyleMedium`.
 */
@property (nonatomic) NSFormattingUnitStyle unitStyle;

/**
 Returns a heading string for the provided value.

 @param direction The heading, measured in degrees, where 0° means “due north”
    and 90° means “due east”.
 @return The heading string appropriately formatted for the formatter’s locale.
 */
- (NSString *)stringFromDirection:(CLLocationDirection)direction;

/**
 This method is not supported for the `MGLDirectionFormatter` class.
 */
- (BOOL)getObjectValue:(out id __nullable * __nullable)obj forString:(NSString *)string errorDescription:(out NSString * __nullable * __nullable)error;

@end

NS_ASSUME_NONNULL_END
