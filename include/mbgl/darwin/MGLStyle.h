#import <Foundation/Foundation.h>

#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

/** A collection of convenience methods for creating style URLs of default styles provided by Mapbox. These instances of NSURL are cached. */
@interface MGLStyle : NSObject

/**
 Returns the Streets style URL.
 
 Mapbox Streets is a complete base map, perfect for incorporating your own data.
 */
+ (NSURL *)streetsStyleURL;

/**
 Returns the Emerald style URL.
 
 Emerald is a versatile style with emphasis on road networks and public transportation.
 */
+ (NSURL *)emeraldStyleURL;

/**
 Returns the Light style URL.
 
 Light is a subtle, light-colored backdrop for data visualizations.
 */
+ (NSURL *)lightStyleURL;

/**
 Returns the Dark style URL.
 
 Dark is a subtle, dark-colored backdrop for data visualizations.
 */
+ (NSURL *)darkStyleURL;

/**
 Returns the Satellite style URL.
 
 Mapbox Satellite is a beautiful global satellite and aerial imagery layer.
 */
+ (NSURL *)satelliteStyleURL;

/**
 Returns the Hybrid style URL.
 
 Hybrid combines the global satellite and aerial imagery of Mapbox Satellite with unobtrusive labels.
 */
+ (NSURL *)hybridStyleURL;

- (instancetype)init NS_UNAVAILABLE;

@end

NS_ASSUME_NONNULL_END
