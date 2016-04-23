#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>

#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

/**
 The `MGLClockDirectionFormatter` class provides properly formatted descriptions
 of headings relative to the user, known as <i>clock positions</i>. For
 example, a value of `90` may be formatted as “3 o’clock”, depending on the
 locale.
 
 Use this class to create localized heading strings when displaying directions
 relative to the user’s current location and heading. To format a direction
 irrespective of the user’s orientation, use `MGLCompassDirectionFormatter`
 instead.
 */
@interface MGLClockDirectionFormatter : NSFormatter

/**
 The unit style used by this formatter.
 
 This property defaults to `NSFormattingUnitStyleMedium`.
 */
@property (nonatomic) NSFormattingUnitStyle unitStyle;

/**
 Returns a clock position string for the provided value.
 
 @param direction The heading, measured in degrees, where 0° means “straight
    ahead” and 90° means “directly to your right”.
 @return The clock position string appropriately formatted for the receiver’s
    locale.
 */
- (NSString *)stringFromDirection:(CLLocationDirection)direction;

/**
 This method is not supported for the `MGLDirectionFormatter` class.
 */
- (BOOL)getObjectValue:(out id __nullable * __nullable)obj forString:(NSString *)string errorDescription:(out NSString * __nullable * __nullable)error;

@end

NS_ASSUME_NONNULL_END
