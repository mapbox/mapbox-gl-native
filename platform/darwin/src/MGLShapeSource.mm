#import "MGLFoundation_Private.h"
#import "MGLShapeSource_Private.h"

#import "MGLLoggingConfiguration_Private.h"
#import "MGLStyle_Private.h"
#import "MGLMapView_Private.h"
#import "MGLSource_Private.h"
#import "MGLFeature_Private.h"
#import "MGLShape_Private.h"
#import "MGLCluster.h"

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
    MGLAssertStyleSourceIsValid();
    auto url = self.rawSource->getURL();
    return url ? [NSURL URLWithString:@(url->c_str())] : nil;
}

- (void)setURL:(NSURL *)url {
    MGLAssertStyleSourceIsValid();
    if (url) {
        self.rawSource->setURL(url.mgl_URLByStandardizingScheme.absoluteString.UTF8String);
        _shape = nil;
    } else {
        self.shape = nil;
    }
}

- (void)setShape:(MGLShape *)shape {
    MGLAssertStyleSourceIsValid();
    self.rawSource->setGeoJSON({ shape.geoJSONObject });
    _shape = shape;
}

- (NSString *)description {
    return [NSString stringWithFormat:@"<%@: %p; identifier = %@; URL = %@; shape = %@>",
            NSStringFromClass([self class]), (void *)self, self.identifier, self.URL, self.shape];
}

- (NSArray<id <MGLFeature>> *)featuresMatchingPredicate:(nullable NSPredicate *)predicate {
    MGLAssertStyleSourceIsValid();
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

#pragma mark - MGLCluster management

- (mbgl::optional<mbgl::FeatureExtensionValue>)featureExtensionValueOfCluster:(MGLShape<MGLCluster> *)cluster extension:(std::string)extension options:(const std::map<std::string, mbgl::Value>)options {
    MGLAssertStyleSourceIsValid();
    mbgl::optional<mbgl::FeatureExtensionValue> extensionValue;
    
    // Check parameters
    if (!self.rawSource || !self.mapView || !cluster) {
        return extensionValue;
    }

    auto geoJSON = [cluster geoJSONObject];
    
    if (!geoJSON.is<mbgl::Feature>()) {
        MGLAssert(0, @"cluster geoJSON object is not a feature.");
        return extensionValue;
    }
    
    auto clusterFeature = geoJSON.get<mbgl::Feature>();
    
    extensionValue = self.mapView.renderer->queryFeatureExtensions(self.rawSource->getID(),
                                                                   clusterFeature,
                                                                   "supercluster",
                                                                   extension,
                                                                   options);
    return extensionValue;
}

- (NSArray<id <MGLFeature>> *)leavesOfCluster:(MGLPointFeatureCluster *)cluster offset:(NSUInteger)offset limit:(NSUInteger)limit {
    const std::map<std::string, mbgl::Value> options = {
        { "limit", static_cast<uint64_t>(limit) },
        { "offset", static_cast<uint64_t>(offset) }
    };

    auto featureExtension = [self featureExtensionValueOfCluster:cluster extension:"leaves" options:options];

    if (!featureExtension) {
        return @[];
    }
    
    if (!featureExtension->is<mbgl::FeatureCollection>()) {
        return @[];
    }
    
    std::vector<mbgl::Feature> leaves = featureExtension->get<mbgl::FeatureCollection>();
    return MGLFeaturesFromMBGLFeatures(leaves);
}

- (NSArray<id <MGLFeature>> *)childrenOfCluster:(MGLPointFeatureCluster *)cluster {
    auto featureExtension = [self featureExtensionValueOfCluster:cluster extension:"children" options:{}];
    
    if (!featureExtension) {
        return @[];
    }
    
    if (!featureExtension->is<mbgl::FeatureCollection>()) {
        return @[];
    }
    
    std::vector<mbgl::Feature> leaves = featureExtension->get<mbgl::FeatureCollection>();
    return MGLFeaturesFromMBGLFeatures(leaves);
}

- (double)zoomLevelForExpandingCluster:(MGLPointFeatureCluster *)cluster {
    auto featureExtension = [self featureExtensionValueOfCluster:cluster extension:"expansion-zoom" options:{}];

    if (!featureExtension) {
        return -1.0;
    }
    
    if (!featureExtension->is<mbgl::Value>()) {
        return -1.0;
    }
    
    auto value = featureExtension->get<mbgl::Value>();
    if (value.is<uint64_t>()) {
        auto zoom = value.get<uint64_t>();
        return static_cast<double>(zoom);
    }
    
    return -1.0;
}

- (void)debugRecursiveLogForFeature:(id <MGLFeature>)feature indent:(NSUInteger)indent {
    NSString *description = feature.description;
    
    // Want our recursive log on a single line
    NSString *log = [description stringByReplacingOccurrencesOfString:@"\\s+"
                                                           withString:@" "
                                                              options:NSRegularExpressionSearch
                                                                range:NSMakeRange(0, description.length)];
    
    printf("%*s%s\n", (int)indent, "", log.UTF8String);
    
    MGLPointFeatureCluster *cluster = MGL_OBJC_DYNAMIC_CAST(feature, MGLPointFeatureCluster);
    
    if (cluster) {
        for (id <MGLFeature> child in [self childrenOfCluster:cluster]) {
            [self debugRecursiveLogForFeature:child indent:indent + 4];
        }
    }
}

@end
