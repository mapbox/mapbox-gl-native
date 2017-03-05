#import "MGLVectorSource_Private.h"

#import "MGLMapView_Private.h"
#import "MGLFeature_Private.h"
#import "MGLSource_Private.h"
#import "MGLTileSource_Private.h"
#import "NSPredicate+MGLAdditions.h"
#import "NSURL+MGLAdditions.h"

#include <mbgl/map/map.hpp>
#include <mbgl/style/sources/vector_source.hpp>

@interface MGLVectorSource ()

- (instancetype)initWithRawSource:(mbgl::style::VectorSource *)rawSource NS_DESIGNATED_INITIALIZER;

@property (nonatomic) mbgl::style::VectorSource *rawSource;

@end

@implementation MGLVectorSource {
    std::unique_ptr<mbgl::style::VectorSource> _pendingSource;
}

- (instancetype)initWithIdentifier:(NSString *)identifier configurationURL:(NSURL *)configurationURL {
    if (self = [super initWithIdentifier:identifier configurationURL:configurationURL]) {
        auto source = std::make_unique<mbgl::style::VectorSource>(identifier.UTF8String,
                                                                  configurationURL.mgl_URLByStandardizingScheme.absoluteString.UTF8String);
        _pendingSource = std::move(source);
        self.rawSource = _pendingSource.get();
    }
    return self;
}

- (instancetype)initWithIdentifier:(NSString *)identifier tileURLTemplates:(NS_ARRAY_OF(NSString *) *)tileURLTemplates options:(nullable NS_DICTIONARY_OF(MGLTileSourceOption, id) *)options {
    if (self = [super initWithIdentifier:identifier tileURLTemplates:tileURLTemplates options:options]) {
        mbgl::Tileset tileSet = MGLTileSetFromTileURLTemplates(tileURLTemplates, options);

        auto source = std::make_unique<mbgl::style::VectorSource>(identifier.UTF8String, tileSet);
        _pendingSource = std::move(source);
        self.rawSource = _pendingSource.get();
    }
    return self;
}

- (instancetype)initWithRawSource:(mbgl::style::VectorSource *)rawSource {
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

    mbgl::style::VectorSource *source = dynamic_cast<mbgl::style::VectorSource *>(removedSource.get());
    if (!source) {
        return;
    }

    removedSource.release();

    _pendingSource = std::unique_ptr<mbgl::style::VectorSource>(source);
    self.rawSource = _pendingSource.get();
}

- (mbgl::style::VectorSource *)rawSource {
    return (mbgl::style::VectorSource *)super.rawSource;
}

- (void)setRawSource:(mbgl::style::VectorSource *)rawSource {
    super.rawSource = rawSource;
}

- (NSURL *)configurationURL {
    auto url = self.rawSource->getURL();
    return url ? [NSURL URLWithString:@(url->c_str())] : nil;
}

- (NSString *)attributionHTMLString {
    auto attribution = self.rawSource->getAttribution();
    return attribution ? @(attribution->c_str()) : nil;
}

- (NS_ARRAY_OF(id <MGLFeature>) *)featuresInSourceLayersWithIdentifiers:(NS_SET_OF(NSString *) *)sourceLayerIdentifiers predicate:(nullable NSPredicate *)predicate {
    
    mbgl::optional<std::vector<std::string>> optionalSourceLayerIDs;
    if (sourceLayerIdentifiers) {
        __block std::vector<std::string> layerIDs;
        layerIDs.reserve(sourceLayerIdentifiers.count);
        [sourceLayerIdentifiers enumerateObjectsUsingBlock:^(NSString * _Nonnull identifier, BOOL * _Nonnull stop) {
            layerIDs.push_back(identifier.UTF8String);
        }];
        optionalSourceLayerIDs = layerIDs;
    }
    
    mbgl::optional<mbgl::style::Filter> optionalFilter;
    if (predicate) {
        optionalFilter = predicate.mgl_filter;
    }
    
    std::vector<mbgl::Feature> features = self.rawSource->querySourceFeatures({ optionalSourceLayerIDs, optionalFilter });
    return MGLFeaturesFromMBGLFeatures(features);
}

@end
