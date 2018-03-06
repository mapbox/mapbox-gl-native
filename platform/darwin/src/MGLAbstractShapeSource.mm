#import "MGLAbstractShapeSource.h"
#import "MGLAbstractShapeSource_Private.h"

const MGLShapeSourceOption MGLShapeSourceOptionBuffer = @"MGLShapeSourceOptionBuffer";
const MGLShapeSourceOption MGLShapeSourceOptionClusterRadius = @"MGLShapeSourceOptionClusterRadius";
const MGLShapeSourceOption MGLShapeSourceOptionClustered = @"MGLShapeSourceOptionClustered";
const MGLShapeSourceOption MGLShapeSourceOptionMaximumZoomLevel = @"MGLShapeSourceOptionMaximumZoomLevel";
const MGLShapeSourceOption MGLShapeSourceOptionMaximumZoomLevelForClustering = @"MGLShapeSourceOptionMaximumZoomLevelForClustering";
const MGLShapeSourceOption MGLShapeSourceOptionMinimumZoomLevel = @"MGLShapeSourceOptionMinimumZoomLevel";
const MGLShapeSourceOption MGLShapeSourceOptionSimplificationTolerance = @"MGLShapeSourceOptionSimplificationTolerance";
const MGLShapeSourceOption MGLShapeSourceOptionWrapsCoordinates = @"MGLShapeSourceOptionWrapsCoordinates";
const MGLShapeSourceOption MGLShapeSourceOptionClipsCoordinates = @"MGLShapeSourceOptionClipsCoordinates";

@interface MGLAbstractShapeSource ()

@end

@implementation MGLAbstractShapeSource

@end

mbgl::style::GeoJSONOptions MGLGeoJSONOptionsFromDictionary(NS_DICTIONARY_OF(MGLShapeSourceOption, id) *options) {
    auto geoJSONOptions = mbgl::style::GeoJSONOptions();

    if (NSNumber *value = options[MGLShapeSourceOptionMinimumZoomLevel]) {
        if (![value isKindOfClass:[NSNumber class]]) {
            [NSException raise:NSInvalidArgumentException
                        format:@"MGLShapeSourceOptionMaximumZoomLevel must be an NSNumber."];
        }
        geoJSONOptions.minzoom = value.integerValue;
    }

    if (NSNumber *value = options[MGLShapeSourceOptionMaximumZoomLevel]) {
        if (![value isKindOfClass:[NSNumber class]]) {
            [NSException raise:NSInvalidArgumentException
                        format:@"MGLShapeSourceOptionMaximumZoomLevel must be an NSNumber."];
        }
        geoJSONOptions.maxzoom = value.integerValue;
    }

    if (NSNumber *value = options[MGLShapeSourceOptionBuffer]) {
        if (![value isKindOfClass:[NSNumber class]]) {
            [NSException raise:NSInvalidArgumentException
                        format:@"MGLShapeSourceOptionBuffer must be an NSNumber."];
        }
        geoJSONOptions.buffer = value.integerValue;
    }

    if (NSNumber *value = options[MGLShapeSourceOptionSimplificationTolerance]) {
        if (![value isKindOfClass:[NSNumber class]]) {
            [NSException raise:NSInvalidArgumentException
                        format:@"MGLShapeSourceOptionSimplificationTolerance must be an NSNumber."];
        }
        geoJSONOptions.tolerance = value.doubleValue;
    }

    if (NSNumber *value = options[MGLShapeSourceOptionClusterRadius]) {
        if (![value isKindOfClass:[NSNumber class]]) {
            [NSException raise:NSInvalidArgumentException
                        format:@"MGLShapeSourceOptionClusterRadius must be an NSNumber."];
        }
        geoJSONOptions.clusterRadius = value.integerValue;
    }

    if (NSNumber *value = options[MGLShapeSourceOptionMaximumZoomLevelForClustering]) {
        if (![value isKindOfClass:[NSNumber class]]) {
            [NSException raise:NSInvalidArgumentException
                        format:@"MGLShapeSourceOptionMaximumZoomLevelForClustering must be an NSNumber."];
        }
        geoJSONOptions.clusterMaxZoom = value.integerValue;
    }

    if (NSNumber *value = options[MGLShapeSourceOptionClustered]) {
        if (![value isKindOfClass:[NSNumber class]]) {
            [NSException raise:NSInvalidArgumentException
                        format:@"MGLShapeSourceOptionClustered must be an NSNumber."];
        }
        geoJSONOptions.cluster = value.boolValue;
    }

    return geoJSONOptions;
}

mbgl::style::CustomGeometrySource::Options MBGLCustomGeometrySourceOptionsFromDictionary(NS_DICTIONARY_OF(MGLShapeSourceOption, id) *options) {
    mbgl::style::CustomGeometrySource::Options sourceOptions;

    if (NSNumber *value = options[MGLShapeSourceOptionMinimumZoomLevel]) {
        if (![value isKindOfClass:[NSNumber class]]) {
            [NSException raise:NSInvalidArgumentException
                        format:@"MGLShapeSourceOptionMaximumZoomLevelForClustering must be an NSNumber."];
        }
        sourceOptions.zoomRange.min = value.integerValue;
    }

    if (NSNumber *value = options[MGLShapeSourceOptionMaximumZoomLevel]) {
        if (![value isKindOfClass:[NSNumber class]]) {
            [NSException raise:NSInvalidArgumentException
                        format:@"MGLShapeSourceOptionMaximumZoomLevel must be an NSNumber."];
        }
        sourceOptions.zoomRange.max = value.integerValue;
    }

    if (NSNumber *value = options[MGLShapeSourceOptionBuffer]) {
        if (![value isKindOfClass:[NSNumber class]]) {
            [NSException raise:NSInvalidArgumentException
                        format:@"MGLShapeSourceOptionBuffer must be an NSNumber."];
        }
        sourceOptions.tileOptions.buffer = value.integerValue;
    }

    if (NSNumber *value = options[MGLShapeSourceOptionSimplificationTolerance]) {
        if (![value isKindOfClass:[NSNumber class]]) {
            [NSException raise:NSInvalidArgumentException
                        format:@"MGLShapeSourceOptionSimplificationTolerance must be an NSNumber."];
        }
        sourceOptions.tileOptions.tolerance = value.doubleValue;
    }

    if (NSNumber *value = options[MGLShapeSourceOptionWrapsCoordinates]) {
        if (![value isKindOfClass:[NSNumber class]]) {
            [NSException raise:NSInvalidArgumentException
                        format:@"MGLShapeSourceOptionWrapsCoordinates must be an NSNumber."];
        }
        sourceOptions.tileOptions.wrap = value.boolValue;
    }

    if (NSNumber *value = options[MGLShapeSourceOptionClipsCoordinates]) {
        if (![value isKindOfClass:[NSNumber class]]) {
            [NSException raise:NSInvalidArgumentException
                        format:@"MGLShapeSourceOptionClipsCoordinates must be an NSNumber."];
        }
        sourceOptions.tileOptions.clip = value.boolValue;
    }

    return sourceOptions;
}
