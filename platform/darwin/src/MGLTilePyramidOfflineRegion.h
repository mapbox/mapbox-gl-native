#import <Foundation/Foundation.h>

#import "MGLOfflineRegion.h"
#import "MGLGeometry.h"

NS_ASSUME_NONNULL_BEGIN

/**
 An offline region defined by a style URL, geographic coordinate bounds, and
 range of zoom levels.
 */
@interface MGLTilePyramidOfflineRegion : NSObject <MGLOfflineRegion, NSSecureCoding, NSCopying>

/**
 URL of the style whose resources are required for offline viewing.
 
 In addition to the JSON stylesheet, different styles may require different font
 glyphs, sprite sheets, and other resources.
 
 The URL may be a full HTTP or HTTPS URL or a Mapbox URL indicating the style’s
 map ID (`mapbox://styles/{user}/{style}`).
 */
@property (nonatomic, readonly) NSURL *styleURL;

/**
 The coordinate bounds for the geographic region covered by the downloaded
 tiles.
 */
@property (nonatomic, readonly) MGLCoordinateBounds bounds;

/**
 The minimum zoom level for which to download tiles and other resources.
 
 For more information about zoom levels, `-[MGLMapView zoomLevel]`.
 */
@property (nonatomic, readonly) double minimumZoomLevel;

/**
 The maximum zoom level for which to download tiles and other resources.
 
 For more information about zoom levels, `-[MGLMapView zoomLevel]`.
 */
@property (nonatomic, readonly) double maximumZoomLevel;

- (instancetype)init NS_UNAVAILABLE;

/**
 Initializes a newly created offline region with the given style URL, geographic
 coordinate bounds, and range of zoom levels.
 
 This is the designated initializer for `MGLTilePyramidOfflineRegion`.
 
 @param styleURL URL of the map style for which to download resources. The URL
    may be a full HTTP or HTTPS URL or a Mapbox URL indicating the style’s map
    ID (`mapbox://styles/{user}/{style}`). Specify `nil` for the default style.
    Relative file URLs cannot be used as offline style URLs. To download the
    online resources required by a local style, specify a URL to an online copy
    of the style.
 @param bounds The coordinate bounds for the geographic region to be covered by
    the downloaded tiles.
 @param minimumZoomLevel The minimum zoom level to be covered by the downloaded
    tiles. This parameter should be set to at least 0 but no greater than the
    value of the `maximumZoomLevel` parameter. For each required tile source, if
    this parameter is set to a value less than the tile source’s minimum zoom
    level, the download covers zoom levels down to the tile source’s minimum
    zoom level.
 @param maximumZoomLevel The maximum zoom level to be covered by the downloaded
    tiles. This parameter should be set to at least the value of the
    `minimumZoomLevel` parameter. For each required tile source, if this
    parameter is set to a value greater than the tile source’s minimum zoom
    level, the download covers zoom levels up to the tile source’s maximum zoom
    level.
 */
- (instancetype)initWithStyleURL:(nullable NSURL *)styleURL bounds:(MGLCoordinateBounds)bounds fromZoomLevel:(double)minimumZoomLevel toZoomLevel:(double)maximumZoomLevel NS_DESIGNATED_INITIALIZER;

@end

NS_ASSUME_NONNULL_END
