#import <Foundation/Foundation.h>

#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

/**
 A version number identifying the latest released version of the suite of default styles provided by Mapbox. This version number may be passed into one of the “StyleURLWithVersion” class methods of MGLStyle.
 
 The value of this constant is current as of the date on which this SDK was published. Consult the <a href="https://www.mapbox.com/api-documentation/#styles">Mapbox Styles API documentation</a> for the most up-to-date style versioning information.
 
 @warning The value of this constant may change in a future release of the SDK. If you use any feature that depends on a specific implementation detail in a default style, you may use the current value of this constant or the underlying style URL, but do not use the constant itself. Such implementation details may change significantly from version to version.
 */
static const NSInteger MGLStyleCurrentVersion = 9;

/**
 A collection of convenience methods for creating style URLs of default styles provided by Mapbox.
 */
@interface MGLStyle : NSObject

/**
 Returns the URL to the current version of the Streets style.
 
 Mapbox Streets is a complete base map that balances nature, commerce, and infrastructure.
 */
+ (NSURL *)streetsStyleURL __attribute__((deprecated("Use -streetsStyleURLWithVersion:.")));

/**
 Returns the URL to the given version of the Streets style.
 
 Mapbox Streets is a complete base map that balances nature, commerce, and infrastructure.
 
 @param version The style’s latest released version. The current version is given by `MGLStyleCurrentVersion`.
 */
+ (NSURL *)streetsStyleURLWithVersion:(NSInteger)version;

/**
 Returns the URL to the current version of the Emerald style.
 
 Mapbox Emerald is a versatile style with emphasis on road networks and public transportation.
 */
+ (NSURL *)emeraldStyleURL __attribute__((deprecated("Use <mapbox://styles/mapbox/emerald-v8>.")));

/**
 Returns the URL to the given version of the Outdoors style.
 
 Mapbox Outdoors is a rugged style that emphasizes physical terrain and outdoor activities.
 
 @param version The style’s latest released version. The current version is given by `MGLStyleCurrentVersion`.
 */
+ (NSURL *)outdoorsStyleURLWithVersion:(NSInteger)version;

/**
 Returns the URL to the current version of the Light style.
 
 Mapbox Light is a subtle, light-colored backdrop for data visualizations.
 */
+ (NSURL *)lightStyleURL __attribute__((deprecated("Use -lightStyleURLWithVersion:.")));

/**
 Returns the URL to the given version of the Light style.
 
 Mapbox Light is a subtle, light-colored backdrop for data visualizations.
 
 @param version The style’s latest released version. The current version is given by `MGLStyleCurrentVersion`.
 */
+ (NSURL *)lightStyleURLWithVersion:(NSInteger)version;

/**
 Returns the URL to the current version of the Dark style.
 
 Mapbox Dark is a subtle, dark-colored backdrop for data visualizations.
 */
+ (NSURL *)darkStyleURL __attribute__((deprecated("Use -darkStyleURLWithVersion:.")));

/**
 Returns the URL to the given version of the Dark style.
 
 Mapbox Dark is a subtle, dark-colored backdrop for data visualizations.
 
 @param version The style’s latest released version. The current version is given by `MGLStyleCurrentVersion`.
 */
+ (NSURL *)darkStyleURLWithVersion:(NSInteger)version;

/**
 Returns the URL to the current version of the Satellite style.
 
 Mapbox Satellite is a beautiful global satellite and aerial imagery layer.
 */
+ (NSURL *)satelliteStyleURL __attribute__((deprecated("Use -satelliteStyleURLWithVersion:.")));

/**
 Returns the URL to the given version of the Satellite style.
 
 Mapbox Satellite is a beautiful global satellite and aerial imagery layer.
 
 @param version The style’s latest released version. The current version is given by `MGLStyleCurrentVersion`.
 */
+ (NSURL *)satelliteStyleURLWithVersion:(NSInteger)version;

/**
 Returns the URL to the current version of the Hybrid style.
 
 Mapbox Hybrid combines the global satellite and aerial imagery of Mapbox Satellite with unobtrusive labels.
 */
+ (NSURL *)hybridStyleURL __attribute__((deprecated("Use -hybridStyleURLWithVersion:.")));

/**
 Returns the URL to the given version of the Hybrid style.
 
 Mapbox Hybrid combines the global satellite and aerial imagery of Mapbox Satellite with unobtrusive labels.
 
 @param version The style’s latest released version. The current version is given by `MGLStyleCurrentVersion`.
 */
+ (NSURL *)hybridStyleURLWithVersion:(NSInteger)version;

- (instancetype)init NS_UNAVAILABLE;

@end

NS_ASSUME_NONNULL_END
