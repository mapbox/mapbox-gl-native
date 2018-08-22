#import "MGLShapeSource_Private.h"

#import "MGLStyle_Private.h"
#import "MGLMapView_Private.h"
#import "MGLSource_Private.h"
#import "MGLFeature_Private.h"
#import "MGLShape_Private.h"

#import "NSPredicate+MGLPrivateAdditions.h"
#import "NSURL+MGLAdditions.h"

#include <mbgl/map/map.hpp>
#include <mbgl/style/sources/geojson_source.hpp>
#include <mbgl/renderer/renderer.hpp>

const MGLShapeSourceOption MGLShapeSourceOptionBuffer = @"MGLShapeSourceOptionBuffer";
const MGLShapeSourceOption MGLShapeSourceOptionClusterRadius = @"MGLShapeSourceOptionClusterRadius";
const MGLShapeSourceOption MGLShapeSourceOptionClustered = @"MGLShapeSourceOptionClustered";
const MGLShapeSourceOption MGLShapeSourceOptionMaximumZoomLevel = @"MGLShapeSourceOptionMaximumZoomLevel";
const MGLShapeSourceOption MGLShapeSourceOptionMaximumZoomLevelForClustering = @"MGLShapeSourceOptionMaximumZoomLevelForClustering";
const MGLShapeSourceOption MGLShapeSourceOptionMinimumZoomLevel = @"MGLShapeSourceOptionMinimumZoomLevel";
const MGLShapeSourceOption MGLShapeSourceOptionSimplificationTolerance = @"MGLShapeSourceOptionSimplificationTolerance";
const MGLShapeSourceOption MGLShapeSourceOptionLineDistanceMetrics = @"MGLShapeSourceOptionLineDistanceMetrics";

mbgl::style::GeoJSONOptions MGLGeoJSONOptionsFromDictionary(NSDictionary<MGLShapeSourceOption, id> *options) {
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

    if (NSNumber *value = options[MGLShapeSourceOptionLineDistanceMetrics]) {
        if (![value isKindOfClass:[NSNumber class]]) {
            [NSException raise:NSInvalidArgumentException
                        format:@"MGLShapeSourceOptionLineDistanceMetrics must be an NSNumber."];
        }
        geoJSONOptions.lineMetrics = value.boolValue;
    }

    return geoJSONOptions;
}

@interface MGLShapeSource ()

@property (nonatomic, readwrite) NSDictionary *options;
@property (nonatomic, readonly) mbgl::style::GeoJSONSource *rawSource;

@end

@implementation MGLShapeSource

- (instancetype)initWithIdentifier:(NSString *)identifier URL:(NSURL *)url options:(NSDictionary<NSString *, id> *)options {
    auto geoJSONOptions = MGLGeoJSONOptionsFromDictionary(options);
    auto source = std::make_unique<mbgl::style::GeoJSONSource>(identifier.UTF8String, geoJSONOptions);
    if (self = [super initWithPendingSource:std::move(source)]) {
        self.URL = url;
    }
    return self;
}

- (instancetype)initWithIdentifier:(NSString *)identifier shape:(nullable MGLShape *)shape options:(NSDictionary<MGLShapeSourceOption, id> *)options {
    auto geoJSONOptions = MGLGeoJSONOptionsFromDictionary(options);
    auto source = std::make_unique<mbgl::style::GeoJSONSource>(identifier.UTF8String, geoJSONOptions);
    if (self = [super initWithPendingSource:std::move(source)]) {
        if ([shape isMemberOfClass:[MGLShapeCollection class]]) {
            static dispatch_once_t onceToken;
            dispatch_once(&onceToken, ^{
                NSLog(@"MGLShapeCollection initialized with MGLFeatures will not retain attributes."
                        @"Use MGLShapeCollectionFeature to retain attributes instead."
                        @"This will be logged only once.");
            });
        }
        self.shape = shape;
    }
    return self;
}

- (instancetype)initWithIdentifier:(NSString *)identifier features:(NSArray<MGLShape<MGLFeature> *> *)features options:(nullable NSDictionary<MGLShapeSourceOption, id> *)options {
    for (id <MGLFeature> feature in features) {
        if (![feature conformsToProtocol:@protocol(MGLFeature)]) {
            [NSException raise:NSInvalidArgumentException format:@"The object %@ included in the features argument does not conform to the MGLFeature protocol.", feature];
        }
    }
    MGLShapeCollectionFeature *shapeCollectionFeature = [MGLShapeCollectionFeature shapeCollectionWithShapes:features];
    return [self initWithIdentifier:identifier shape:shapeCollectionFeature options:options];
}

- (instancetype)initWithIdentifier:(NSString *)identifier shapes:(NSArray<MGLShape *> *)shapes options:(nullable NSDictionary<MGLShapeSourceOption, id> *)options {
    MGLShapeCollection *shapeCollection = [MGLShapeCollection shapeCollectionWithShapes:shapes];
    return [self initWithIdentifier:identifier shape:shapeCollection options:options];
}

- (mbgl::style::GeoJSONSource *)rawSource {
    return (mbgl::style::GeoJSONSource *)super.rawSource;
}

- (NSURL *)URL {
    auto url = self.rawSource->getURL();
    return url ? [NSURL URLWithString:@(url->c_str())] : nil;
}

- (void)setURL:(NSURL *)url {
    if (url) {
        self.rawSource->setURL(url.mgl_URLByStandardizingScheme.absoluteString.UTF8String);
        _shape = nil;
    } else {
        self.shape = nil;
    }
}

- (void)setShape:(MGLShape *)shape {
    self.rawSource->setGeoJSON({ shape.geoJSONObject });
    _shape = shape;
}

- (NSString *)description {
    return [NSString stringWithFormat:@"<%@: %p; identifier = %@; URL = %@; shape = %@>",
            NSStringFromClass([self class]), (void *)self, self.identifier, self.URL, self.shape];
}

- (NSArray<id <MGLFeature>> *)featuresMatchingPredicate:(nullable NSPredicate *)predicate {
    
    mbgl::optional<mbgl::style::Filter> optionalFilter;
    if (predicate) {
        optionalFilter = predicate.mgl_filter;
    }
    
    std::vector<mbgl::Feature> features;
    if (self.mapView) {
        features = self.mapView.renderer->querySourceFeatures(self.rawSource->getID(), { {}, optionalFilter });
    }
    return MGLFeaturesFromMBGLFeatures(features);
}

@end
