#import "MGLShapeSource_Private.h"
#import "MGLAbstractShapeSource_Private.h"

#import "MGLMapView_Private.h"
#import "MGLSource_Private.h"
#import "MGLFeature_Private.h"
#import "MGLShape_Private.h"

#import "NSPredicate+MGLAdditions.h"
#import "NSURL+MGLAdditions.h"

#include <mbgl/map/map.hpp>
#include <mbgl/style/sources/geojson_source.hpp>



@interface MGLShapeSource ()

- (instancetype)initWithRawSource:(mbgl::style::GeoJSONSource *)rawSource NS_DESIGNATED_INITIALIZER;

@property (nonatomic, readwrite) NSDictionary *options;
@property (nonatomic) mbgl::style::GeoJSONSource *rawSource;

@end

@implementation MGLShapeSource {
    std::unique_ptr<mbgl::style::GeoJSONSource> _pendingSource;
}

- (instancetype)initWithIdentifier:(NSString *)identifier URL:(NSURL *)url options:(NS_DICTIONARY_OF(NSString *, id) *)options {
    if (self = [super initWithIdentifier:identifier]) {
        auto geoJSONOptions = MGLGeoJSONOptionsFromDictionary(options);
        auto source = std::make_unique<mbgl::style::GeoJSONSource>(identifier.UTF8String, geoJSONOptions);

        _pendingSource = std::move(source);
        self.rawSource = _pendingSource.get();

        self.URL = url;
    }
    return self;
}

- (instancetype)initWithIdentifier:(NSString *)identifier shape:(nullable MGLShape *)shape options:(NS_DICTIONARY_OF(MGLShapeSourceOption, id) *)options {
    if (self = [super initWithIdentifier:identifier]) {
        auto geoJSONOptions = MGLGeoJSONOptionsFromDictionary(options);
        auto source = std::make_unique<mbgl::style::GeoJSONSource>(identifier.UTF8String, geoJSONOptions);

        _pendingSource = std::move(source);
        self.rawSource = _pendingSource.get();

        self.shape = shape;
    }
    return self;
}

- (instancetype)initWithIdentifier:(NSString *)identifier features:(NS_ARRAY_OF(MGLShape<MGLFeature> *) *)features options:(nullable NS_DICTIONARY_OF(MGLShapeSourceOption, id) *)options {
    for (id <MGLFeature> feature in features) {
        if (![feature conformsToProtocol:@protocol(MGLFeature)]) {
            [NSException raise:NSInvalidArgumentException format:@"The object %@ included in the features argument does not conform to the MGLFeature protocol.", feature];
        }
    }
    MGLShapeCollectionFeature *shapeCollectionFeature = [MGLShapeCollectionFeature shapeCollectionWithShapes:features];
    return [self initWithIdentifier:identifier shape:shapeCollectionFeature options:options];
}

- (instancetype)initWithIdentifier:(NSString *)identifier shapes:(NS_ARRAY_OF(MGLShape *) *)shapes options:(nullable NS_DICTIONARY_OF(MGLShapeSourceOption, id) *)options {
    MGLShapeCollection *shapeCollection = [MGLShapeCollection shapeCollectionWithShapes:shapes];
    return [self initWithIdentifier:identifier shape:shapeCollection options:options];
}

- (instancetype)initWithRawSource:(mbgl::style::GeoJSONSource *)rawSource {
    return [super initWithRawSource:rawSource];
}

- (void)addToMapView:(MGLMapView *)mapView {
    if (_pendingSource == nullptr) {
        [NSException raise:@"MGLRedundantSourceException"
                    format:@"This instance %@ was already added to %@. Adding the same source instance " \
                            "to the style more than once is invalid.", self, mapView.style];
    }

    mapView.mbglMap->addSource(std::move(_pendingSource));
}

- (void)removeFromMapView:(MGLMapView *)mapView {
    if (self.rawSource != mapView.mbglMap->getSource(self.identifier.UTF8String)) {
        return;
    }

    auto removedSource = mapView.mbglMap->removeSource(self.identifier.UTF8String);

    mbgl::style::GeoJSONSource *source = dynamic_cast<mbgl::style::GeoJSONSource *>(removedSource.get());
    if (!source) {
        return;
    }

    removedSource.release();

    _pendingSource = std::unique_ptr<mbgl::style::GeoJSONSource>(source);
    self.rawSource = _pendingSource.get();
}

- (mbgl::style::GeoJSONSource *)rawSource {
    return (mbgl::style::GeoJSONSource *)super.rawSource;
}

- (void)setRawSource:(mbgl::style::GeoJSONSource *)rawSource {
    super.rawSource = rawSource;
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

- (NS_ARRAY_OF(id <MGLFeature>) *)featuresMatchingPredicate:(nullable NSPredicate *)predicate {
    
    mbgl::optional<mbgl::style::Filter> optionalFilter;
    if (predicate) {
        optionalFilter = predicate.mgl_filter;
    }
    
    std::vector<mbgl::Feature> features = self.rawSource->querySourceFeatures({ {}, optionalFilter });
    return MGLFeaturesFromMBGLFeatures(features);
}

@end
