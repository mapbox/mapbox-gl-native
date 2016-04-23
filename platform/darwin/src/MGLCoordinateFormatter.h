#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>

#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

/**
 The `MGLCoordinateFormatter` class provides properly formatted descriptions of
 geographic coordinate pairs. Use this class to create localized coordinate
 strings when displaying location information to users.
 */
@interface MGLCoordinateFormatter : NSFormatter

/**
 Determines whether the output may contain minutes of arc when nonzero.
 
 The default value of this property is `YES`, causing the receiver to include
 minutes of arc in its output. If `allowsSeconds` is `YES`, this property is
 ignored and the output always includes minutes of arc.
 */
@property (nonatomic) BOOL allowsMinutes;

/**
 Determines whether the output may contain seconds of arc when nonzero.
 
 The default value of this property is `YES`, causing the receiver to include
 seconds of arc in its output.
 */
@property (nonatomic) BOOL allowsSeconds;

/**
 The unit style used by this formatter.
 
 The default value of this property is `NSFormattingUnitStyleMedium`.
 */
@property (nonatomic) NSFormattingUnitStyle unitStyle;

/**
 Returns a coordinate string for the provided value.
 
 @param coordinate The coordinate’s value.
 @return The coordinate string appropriately formatted for the formatter’s
    locale.
 */
- (NSString *)stringFromCoordinate:(CLLocationCoordinate2D)coordinate;

/**
 This method is not supported for the `MGLCoordinateFormatter` class.
 */
- (BOOL)getObjectValue:(out id __nullable * __nullable)obj forString:(NSString *)string errorDescription:(out NSString * __nullable * __nullable)error;

@end

NS_ASSUME_NONNULL_END
