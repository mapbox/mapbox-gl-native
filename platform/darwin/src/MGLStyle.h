#import <Foundation/Foundation.h>

#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

/**
 A version number identifying the default version of the suite of default styles
 provided by Mapbox. This version number may be passed into one of the
 “StyleURLWithVersion” class methods of MGLStyle.
 
 The value of this constant generally corresponds to the latest released version
 as of the date on which this SDK was published. You can use this constant to
 ascertain the style used by `MGLMapView` and `MGLTilePyramidOfflineRegion` when
 no style URL is specified. Consult the
 <a href="https://www.mapbox.com/api-documentation/#styles">Mapbox Styles API documentation</a>
 for the most up-to-date style versioning information.
 
 @warning The value of this constant may change in a future release of the SDK.
    If you use any feature that depends on a specific aspect of a default style
    – for instance, the minimum zoom level that includes roads – you may use the
    current value of this constant or the underlying style URL, but do not use
    the constant itself. Such details may change significantly from version to
    version.
 */
static const NSInteger MGLStyleDefaultVersion = 9;

/**
 A collection of convenience methods for creating style URLs of default styles
 provided by Mapbox.
 <a href="https://www.mapbox.com/maps/">Learn more about Mapbox default styles</a>.
 */
@interface MGLStyle : NSObject

/**
 Returns the URL to version 8 of the
 <a href="https://www.mapbox.com/maps/streets/">Mapbox Streets</a> style.
 
 Streets is a general-purpose style with detailed road and transit networks.
 
 `MGLMapView` and `MGLTilePyramidOfflineRegion` use Mapbox Streets when no style
 is specified explicitly.
 */
+ (NSURL *)streetsStyleURL __attribute__((deprecated("Use -streetsStyleURLWithVersion:.")));

/**
 Returns the URL to the given version of the
 <a href="https://www.mapbox.com/maps/streets/">Mapbox Streets</a> style.
 
 Streets is a general-purpose style with detailed road and transit networks.
 
 `MGLMapView` and `MGLTilePyramidOfflineRegion` use Mapbox Streets when no style
 is specified explicitly.
 
 @param version The style’s latest released version. As of publication, the
    current version is `9`.
 */
+ (NSURL *)streetsStyleURLWithVersion:(NSInteger)version;

/**
 Returns the URL to version 8 of the
 <a href="https://www.mapbox.com/blog/emerald-gl/">Mapbox Emerald</a> style.
 
 Emerald is a tactile style with subtle textures and dramatic hillshading.
 */
+ (NSURL *)emeraldStyleURL __attribute__((deprecated("Create an NSURL object with the string “mapbox://styles/mapbox/emerald-v8”.")));

/**
 Returns the URL to the given version of the
 <a href="https://www.mapbox.com/maps/outdoors/">Mapbox Outdoors</a> style.
 
 Outdoors is a general-purpose style tailored to outdoor activities.
 
 @param version The style’s latest released version. As of publication, the
    current version is `9`.
 */
+ (NSURL *)outdoorsStyleURLWithVersion:(NSInteger)version;

/**
 Returns the URL to version 8 of the
 <a href="https://www.mapbox.com/maps/light-dark/">Mapbox Light</a> style.
 
 Light is a subtle, light-colored backdrop for data visualizations.
 */
+ (NSURL *)lightStyleURL __attribute__((deprecated("Use -lightStyleURLWithVersion:.")));

/**
 Returns the URL to the given version of the
 <a href="https://www.mapbox.com/maps/light-dark/">Mapbox Light</a> style.
 
 Light is a subtle, light-colored backdrop for data visualizations.
 
 @param version The style’s latest released version. As of publication, the
    current version is `9`.
 */
+ (NSURL *)lightStyleURLWithVersion:(NSInteger)version;

/**
 Returns the URL to version 8 of the
 <a href="https://www.mapbox.com/maps/light-dark/">Mapbox Dark</a> style.
 
 Dark is a subtle, dark-colored backdrop for data visualizations.
 */
+ (NSURL *)darkStyleURL __attribute__((deprecated("Use -darkStyleURLWithVersion:.")));

/**
 Returns the URL to the given version of the
 <a href="https://www.mapbox.com/maps/light-dark/">Mapbox Dark</a> style.
 
 Dark is a subtle, dark-colored backdrop for data visualizations.
 
 @param version The style’s latest released version. As of publication, the
    current version is `9`.
 */
+ (NSURL *)darkStyleURLWithVersion:(NSInteger)version;

/**
 Returns the URL to version 8 of the
 <a href="https://www.mapbox.com/maps/satellite/">Mapbox Satellite</a> style.
 
 Satellite is high-resolution satellite and aerial imagery.
 */
+ (NSURL *)satelliteStyleURL __attribute__((deprecated("Use -satelliteStyleURLWithVersion:.")));

/**
 Returns the URL to the given version of the
 <a href="https://www.mapbox.com/maps/satellite/">Mapbox Satellite</a> style.
 
 Satellite is high-resolution satellite and aerial imagery.
 
 @param version The style’s latest released version. As of publication, the
    current version is `9`.
 */
+ (NSURL *)satelliteStyleURLWithVersion:(NSInteger)version;

/**
 Returns the URL to version 8 of the
 <a href="https://www.mapbox.com/maps/satellite/">Mapbox Satellite Streets</a>
 style.
 
 Satellite Streets combines the high-resolution satellite and aerial imagery of
 Mapbox Satellite with unobtrusive labels and translucent roads from Mapbox
 Streets.
 */
+ (NSURL *)hybridStyleURL __attribute__((deprecated("Use -satelliteStreetsStyleURLWithVersion:.")));

/**
 Returns the URL to the given version of the
 <a href="https://www.mapbox.com/maps/satellite/">Mapbox Satellite Streets</a>
 style.
 
 Satellite Streets combines the high-resolution satellite and aerial imagery of
 Mapbox Satellite with unobtrusive labels and translucent roads from Mapbox
 Streets.
 
 @param version The style’s latest released version. As of publication, the
    current version is `9`.
 */
+ (NSURL *)satelliteStreetsStyleURLWithVersion:(NSInteger)version;

- (instancetype)init NS_UNAVAILABLE;

@end

NS_ASSUME_NONNULL_END
