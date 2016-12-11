#import "MGLTileSource_Private.h"

#import "MGLAttributionInfo.h"

#include <mbgl/util/tileset.hpp>

const MGLTileSourceOption MGLTileSourceOptionMinimumZoomLevel = @"MGLTileSourceOptionMinimumZoomLevel";
const MGLTileSourceOption MGLTileSourceOptionMaximumZoomLevel = @"MGLTileSourceOptionMaximumZoomLevel";
const MGLTileSourceOption MGLTileSourceOptionAttribution = @"MGLTileSourceOptionAttribution";
const MGLTileSourceOption MGLTileSourceOptionTileCoordinateSystem = @"MGLTileSourceOptionTileCoordinateSystem";

@implementation MGLTileSource

- (instancetype)initWithIdentifier:(NSString *)identifier configurationURL:(NSURL *)configurationURL {
    return [super initWithIdentifier:identifier];
}

- (instancetype)initWithIdentifier:(NSString *)identifier tileURLTemplates:(NS_ARRAY_OF(NSString *) *)tileURLTemplates options:(NS_DICTIONARY_OF(MGLTileSourceOption, id) *)options {
    return [super initWithIdentifier:identifier];
}

- (nullable NS_ARRAY_OF(MGLAttributionInfo *) *)attributionInfosWithFontSize:(CGFloat)fontSize linkColor:(nullable MGLColor *)linkColor {
    return [MGLAttributionInfo attributionInfosFromHTMLString:self.attribution
                                                     fontSize:fontSize
                                                    linkColor:linkColor];
}

- (NSString *)attribution {
    [NSException raise:@"MGLAbstractClassException"
                format:@"MGLTileSource is an abstract class"];
    return nil;
}

@end

mbgl::Tileset MGLTileSetFromTileURLTemplates(NS_ARRAY_OF(NSString *) *tileURLTemplates, NS_DICTIONARY_OF(MGLTileSourceOption, id) * _Nullable options) {
    mbgl::Tileset tileSet;
    
    for (NSString *tileURLTemplate in tileURLTemplates) {
        tileSet.tiles.push_back(tileURLTemplate.UTF8String);
    }
    
    // set the minimum / maximum zoom range to the values specified by this class if they
    // were set. otherwise, use the core objects default values
    if (NSNumber *minimumZoomLevel = options[MGLTileSourceOptionMinimumZoomLevel]) {
        if (![minimumZoomLevel isKindOfClass:[NSNumber class]]) {
            [NSException raise:NSInvalidArgumentException
                        format:@"MGLTileSourceOptionMinimumZoomLevel must be set to an NSNumber."];
        }
        tileSet.zoomRange.min = minimumZoomLevel.integerValue;
    }
    if (NSNumber *maximumZoomLevel = options[MGLTileSourceOptionMaximumZoomLevel]) {
        if (![maximumZoomLevel isKindOfClass:[NSNumber class]]) {
            [NSException raise:NSInvalidArgumentException
                        format:@"MGLTileSourceOptionMinimumZoomLevel must be set to an NSNumber."];
        }
        tileSet.zoomRange.max = maximumZoomLevel.integerValue;
    }
    if (tileSet.zoomRange.min > tileSet.zoomRange.max) {
        [NSException raise:NSInvalidArgumentException
                    format:@"MGLTileSourceOptionMinimumZoomLevel must be less than MGLTileSourceOptionMaximumZoomLevel."];
    }
    
    if (NSString *attribution = options[MGLTileSourceOptionAttribution]) {
        if (![attribution isKindOfClass:[NSString class]]) {
            [NSException raise:NSInvalidArgumentException
                        format:@"MGLTileSourceOptionAttribution must be set to a string."];
        }
        tileSet.attribution = attribution.UTF8String;
    }
    
    if (NSNumber *tileCoordinateSystemNumber = options[MGLTileSourceOptionTileCoordinateSystem]) {
        if (![tileCoordinateSystemNumber isKindOfClass:[NSValue class]]) {
            [NSException raise:NSInvalidArgumentException
                        format:@"MGLTileSourceOptionTileCoordinateSystem must be set to an NSValue or NSNumber."];
        }
        MGLTileCoordinateSystem tileCoordinateSystem;
        [tileCoordinateSystemNumber getValue:&tileCoordinateSystem];
        switch (tileCoordinateSystem) {
            case MGLTileCoordinateSystemXYZ:
                tileSet.scheme = mbgl::Tileset::Scheme::XYZ;
                break;
            case MGLTileCoordinateSystemTMS:
                tileSet.scheme = mbgl::Tileset::Scheme::TMS;
                break;
        }
    }
    
    return tileSet;
}
