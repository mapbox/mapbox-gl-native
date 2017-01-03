#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>

NS_ASSUME_NONNULL_BEGIN

/**
 `MGLDistanceFormatter` implements a formatter object meant to be used for
 geographic distances. By default, the resulting string will be based on the user’s
 current locale but can be overriden in order to force a measurement system of your choice.
 */
@interface MGLDistanceFormatter : NSFormatter

/**
 Determines what system of measurement to use.
 */
typedef NS_ENUM(NSUInteger, MGLDistanceFormatterUnits) {
    // Measurement system will be based on the current locale.
    MGLDistanceFormatterUnitsDefault,
    // Use the metric system.
    MGLDistanceFormatterUnitsMetric,
    // Imperial units using feets.
    MGLDistanceFormatterUnitsImperial,
    // Imperial units using yards.
    MGLDistanceFormatterUnitsImperialWithYards
};

/**
 Determines if the localized unit string should be abbreviated or spelled out.
 */
typedef NS_ENUM(NSUInteger, MGLDistanceFormatterUnitStyle) {
    MGLDistanceFormatterUnitStyleDefault,
    // Abbreviate units (i.e. 1 km).
    MGLDistanceFormatterUnitStyleAbbreviated,
    // Full style (i.e. 2 kilometers).
    MGLDistanceFormatterUnitStyleFull
};

/**
 Determines what system of measurement to use.
 
 Based on the user’s locale by default.
 */
@property (nonatomic, assign) MGLDistanceFormatterUnits units;

/**
 Determines how to output the localized unit string.
 */
@property (nonatomic, assign) MGLDistanceFormatterUnitStyle unitStyle;

/**
 Returns a localized formatted string for the provided distance.
 
 @param distance The distance, measured in meters. Negative distance will return nil.
 @return A localized formatted distance string including units.
 */
- (NSString *)stringFromDistance:(CLLocationDistance)distance;

/**
 This method is not supported for the `MGLDistanceFormatter` class.
 */
- (BOOL)getObjectValue:(out id __nullable * __nullable)obj forString:(NSString *)string errorDescription:(out NSString * __nullable * __nullable)error;

@end

NS_ASSUME_NONNULL_END
