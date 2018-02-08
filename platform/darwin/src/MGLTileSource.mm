#import "MGLTileSource_Private.h"

#import "MGLAttributionInfo_Private.h"
#import "MGLGeometry_Private.h"
#import "NSString+MGLAdditions.h"
#import "NSValue+MGLAdditions.h"

#if TARGET_OS_IPHONE
    #import <UIKit/UIKit.h>
#else
    #import <Cocoa/Cocoa.h>
#endif

#include <mbgl/util/tileset.hpp>

const MGLTileSourceOption MGLTileSourceOptionMinimumZoomLevel = @"MGLTileSourceOptionMinimumZoomLevel";
const MGLTileSourceOption MGLTileSourceOptionMaximumZoomLevel = @"MGLTileSourceOptionMaximumZoomLevel";
const MGLTileSourceOption MGLTileSourceOptionCoordinateBounds = @"MGLTileSourceOptionCoordinateBounds";
const MGLTileSourceOption MGLTileSourceOptionAttributionHTMLString = @"MGLTileSourceOptionAttributionHTMLString";
const MGLTileSourceOption MGLTileSourceOptionAttributionInfos = @"MGLTileSourceOptionAttributionInfos";
const MGLTileSourceOption MGLTileSourceOptionTileCoordinateSystem = @"MGLTileSourceOptionTileCoordinateSystem";

@implementation MGLTileSource

- (NSURL *)configurationURL {
    [NSException raise:@"MGLAbstractClassException"
                format:@"MGLTileSource is an abstract class"];
    return nil;
}

- (NS_ARRAY_OF(MGLAttributionInfo *) *)attributionInfos {
    return [self attributionInfosWithFontSize:0 linkColor:nil];
}

- (NS_ARRAY_OF(MGLAttributionInfo *) *)attributionInfosWithFontSize:(CGFloat)fontSize linkColor:(nullable MGLColor *)linkColor {
    return [MGLAttributionInfo attributionInfosFromHTMLString:self.attributionHTMLString
                                                     fontSize:fontSize
                                                    linkColor:linkColor];
}

- (NSString *)attributionHTMLString {
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
    
    if (NSValue *coordinateBounds = options[MGLTileSourceOptionCoordinateBounds]) {
        if (![coordinateBounds isKindOfClass:[NSValue class]]
            && strcmp(coordinateBounds.objCType, @encode(MGLCoordinateBounds)) == 0) {
            [NSException raise:NSInvalidArgumentException
                        format:@"MGLTileSourceOptionCoordinateBounds must be set to an NSValue containing an MGLCoordinateBounds."];
        }
        tileSet.bounds = MGLLatLngBoundsFromCoordinateBounds(coordinateBounds.MGLCoordinateBoundsValue);
    }

    if (NSString *attribution = options[MGLTileSourceOptionAttributionHTMLString]) {
        if (![attribution isKindOfClass:[NSString class]]) {
            [NSException raise:NSInvalidArgumentException
                        format:@"MGLTileSourceOptionAttributionHTMLString must be set to a string."];
        }
        tileSet.attribution = attribution.UTF8String;
    }

    if (NSArray *attributionInfos = options[MGLTileSourceOptionAttributionInfos]) {
        if (![attributionInfos isKindOfClass:[NSArray class]]) {
            [NSException raise:NSInvalidArgumentException
                        format:@"MGLTileSourceOptionAttributionInfos must be set to a string."];
        }

        NSAttributedString *attributedString = [MGLAttributionInfo attributedStringForAttributionInfos:attributionInfos];
#if TARGET_OS_IPHONE
        static NSString * const NSExcludedElementsDocumentAttribute = @"ExcludedElements";
#endif
        NSDictionary *documentAttributes = @{
            NSDocumentTypeDocumentAttribute: NSHTMLTextDocumentType,
            NSCharacterEncodingDocumentAttribute: @(NSUTF8StringEncoding),
            // The attribution string is meant to be a simple, inline fragment, not a full-fledged, validating document.
            NSExcludedElementsDocumentAttribute: @[@"XML", @"DOCTYPE", @"html", @"head", @"meta", @"title", @"style", @"body", @"p"],
        };
        NSData *data = [attributedString dataFromRange:attributedString.mgl_wholeRange documentAttributes:documentAttributes error:NULL];
        NSString *html = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
        tileSet.attribution = html.UTF8String;
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
