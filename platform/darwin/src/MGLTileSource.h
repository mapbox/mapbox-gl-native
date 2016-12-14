#import <Foundation/Foundation.h>

#import "MGLSource.h"
#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

@class MGLAttributionInfo;

/**
 Options for `MGLTileSource` objects.
 */
typedef NSString *MGLTileSourceOption NS_STRING_ENUM;

/**
 An `NSNumber` object containing an unsigned integer that specifies the minimum
 zoom level at which to display tiles from the source.
 
 The value should be between 0 and 22, inclusive, and less than
 `MGLTileSourceOptionMaximumZoomLevel`, if specified. The default value for this
 option is 0.
 */
extern const MGLTileSourceOption MGLTileSourceOptionMinimumZoomLevel;

/**
 An `NSNumber` object containing an unsigned integer that specifies the maximum
 zoom level at which to display tiles from the source.
 
 The value should be between 0 and 22, inclusive, and less than
 `MGLTileSourceOptionMinimumZoomLevel`, if specified. The default value for this
 option is 22.
 */
extern const MGLTileSourceOption MGLTileSourceOptionMaximumZoomLevel;

#if TARGET_OS_IPHONE
/**
 An HTML string defining the buttons to be displayed in an action sheet when the
 source is part of a map view’s style and the map view’s attribution button is
 pressed.
 
 By default, no attribution statements are displayed. If the
 `MGLTileSourceOptionAttributionInfos` option is specified, this option is
 ignored.
 */
extern const MGLTileSourceOption MGLTileSourceOptionAttributionHTMLString;

/**
 An array of `MGLAttributionInfo` objects defining the buttons to be displayed
 in an action sheet when the source is part of a map view’s style and the map
 view’s attribution button is pressed.
 
 By default, no attribution statements are displayed.
 */
extern const MGLTileSourceOption MGLTileSourceOptionAttributionInfos;
#else
/**
 An HTML string defining the buttons to be displayed in the map view’s
 attribution view when the source is part of the map view’s style.
 
 By default, no attribution statements are displayed. If the
 `MGLTileSourceOptionAttributionInfos` option is specified, this option is
 ignored.
 */
extern const MGLTileSourceOption MGLTileSourceOptionAttributionHTMLString;

/**
 An array of `MGLAttributionInfo` objects defining the buttons to be displayed
 in the map view’s attribution view when the source is part of the map view’s
 style.
 
 By default, no attribution statements are displayed.
 */
extern const MGLTileSourceOption MGLTileSourceOptionAttributionInfos;
#endif

/**
 An `NSNumber` object containing an unsigned integer that specifies the tile
 coordinate system for the source’s tile URLs. The integer corresponds to one of
 the constants described in `MGLTileCoordinateSystem`.
 
 The default value for this option is `MGLTileCoordinateSystemXYZ`.
 */
extern const MGLTileSourceOption MGLTileSourceOptionTileCoordinateSystem;

/**
 Tile coordinate systems that determine how tile coordinates in tile URLs are
 interpreted.
 */
typedef NS_ENUM(NSUInteger, MGLTileCoordinateSystem) {
    /**
     The origin is at the top-left (northwest), and `y` values increase
     southwards.
     
     This tile coordinate system is used by Mapbox and OpenStreetMap tile
     servers.
     */
    MGLTileCoordinateSystemXYZ = 0,
    
    /**
     The origin is at the bottom-left (southwest), and `y` values increase
     northwards.
     
     This tile coordinate system is used by tile servers that conform to the
     <a href="http://wiki.osgeo.org/wiki/Tile_Map_Service_Specification">Tile Map Service Specification</a>.
     */
    MGLTileCoordinateSystemTMS
};

/**
 `MGLTileSource` is a map content source that supplies map tiles to be shown on
 the map. The location of and metadata about the tiles are defined either by an
 option dictionary or by an external file that conforms to the
 <a href="https://github.com/mapbox/tilejson-spec/">TileJSON specification</a>.
 A tile source is added to an `MGLStyle` object along with one or more
 `MGLRasterStyleLayer` or `MGLVectorStyleLayer` objects. Use a style layer to
 control the appearance of content supplied by the tile source.
 
 Do not create instances of this class directly, and do not create your own
 subclasses of this class. Instead, create instances of `MGLRasterSource` and
 `MGLVectorSource`.
 */
@interface MGLTileSource : MGLSource

#pragma mark Initializing a Source

- (instancetype)init __attribute__((unavailable("Use -initWithIdentifier:configurationURL: or -initWithIdentifier:tileURLTemplates:options: instead.")));
- (instancetype)initWithIdentifier:(NSString *)identifier __attribute__((unavailable("Use -initWithIdentifier:configurationURL: or -initWithIdentifier:tileURLTemplates:options: instead.")));

/**
 Returns a tile source initialized with an identifier and configuration URL.
 
 After initializing and configuring the source, add it to a map view’s style
 using the `-[MGLStyle addSource:]` method.
 
 The URL may be a full HTTP or HTTPS URL or, for tile sets hosted by Mapbox, a
 Mapbox URL indicating a map identifier (`mapbox://<mapid>`). The URL should
 point to a JSON file that conforms to the
 <a href="https://github.com/mapbox/tilejson-spec/">TileJSON specification</a>.
 
 @param identifier A string that uniquely identifies the source in the style to
    which it is added.
 @param configurationURL A URL to a TileJSON configuration file describing the
    source’s contents and other metadata.
 @return An initialized tile source.
 */
- (instancetype)initWithIdentifier:(NSString *)identifier configurationURL:(NSURL *)configurationURL;

/**
 Returns a tile source initialized an identifier, tile URL templates, and
 options.

 After initializing and configuring the source, add it to a map view’s style
 using the `-[MGLStyle addSource:]` method.
 
 @param identifier A string that uniquely identifies the source in the style to
    which it is added.
 @param tileURLTemplates An array of tile URL template strings.
 @param options A dictionary containing configuration options. See
    `MGLTileSourceOption` for available keys and values. Pass in `nil` to use
    the default values.
 @return An initialized tile source.
 */
- (instancetype)initWithIdentifier:(NSString *)identifier tileURLTemplates:(NS_ARRAY_OF(NSString *) *)tileURLTemplates options:(nullable NS_DICTIONARY_OF(MGLTileSourceOption, id) *)options;

#pragma mark Accessing Attribution Strings

/**
 An array of `MGLAttributionInfo` objects that define the attribution
 statements to be displayed when the map is shown to the user.
 
 By default, this array is empty. If the source is initialized with a
 configuration URL, this array is also empty until the configuration JSON file
 is loaded.
 */
@property (nonatomic, copy, readonly) NS_ARRAY_OF(MGLAttributionInfo *) *attributionInfos;

@end

NS_ASSUME_NONNULL_END
