#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>

#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

/**
 The reference point for an `MGLDirectionFormatter`.
 */
typedef NS_ENUM(NSUInteger, MGLDirectionFormatterOrigin) {
    /**
     Directions are assumed to be relative to true north and are given as
     compass directions, such as “south” for a value of `90`.
     */
    MGLDirectionFormatterOriginNorth = 0,
    /**
     Directions are assumed to be relative to the direction in which the user is
     facing and are given as “clock directions”, such as “6 o’clock” for a value
     of `90`.
     */
    MGLDirectionFormatterOriginStraightAhead,
};

/**
 The `MGLDirectionFormatter` class provides properly formatted descriptions of
 absolute or relative headings. Use this class to create localized heading
 strings when displaying directional information to users.
 */
@interface MGLDirectionFormatter : NSFormatter

/**
 The receiver’s reference point. The receiver’s input is assumed to be relative
 to this reference point, and its output is given in the conventional form for
 directions with this reference point.
 
 This class does not convert between different reference points. To convert an
 `CLLocationDirection` with respect to true north into a `CLLocationDirection`
 with respect to the direction in which the user is currently facing, use Core
 Location to determine the user’s current heading.
 
 The default value of this property is `MGLDirectionFormatterOriginNorth`, which
 means a value of `0` is formatted as “north” in the receiver’s locale.
 */
@property (nonatomic) MGLDirectionFormatterOrigin origin;

/**
 The unit style used by this formatter.
 
 This property defaults to `NSFormattingUnitStyleMedium`.
 */
@property (nonatomic) NSFormattingUnitStyle unitStyle;

/**
 The locale of the receiver.
 
 The locale determines the output language as well as the numeral system used
 when the `relativeToUser` property is set to `YES`.
 */
@property (copy) NSLocale *locale;

/**
 Returns a heading string for the provided value.
 
 @param direction The heading, measured in degrees.
 @return The heading string appropriately formatted for the formatter’s locale.
 */
- (NSString *)stringFromDirection:(CLLocationDirection)direction;

/**
 This method is not supported for the `MGLDirectionFormatter` class.
 */
- (BOOL)getObjectValue:(out id __nullable * __nullable)obj forString:(NSString *)string errorDescription:(out NSString * __nullable * __nullable)error;

@end

NS_ASSUME_NONNULL_END
