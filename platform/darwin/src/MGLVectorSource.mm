#import "MGLVectorSource_Private.h"

#import "MGLFeature_Private.h"
#import "MGLSource_Private.h"
#import "MGLTileSource_Private.h"
#import "MGLStyle_Private.h"
#import "MGLMapView_Private.h"
#import "NSPredicate+MGLAdditions.h"
#import "NSURL+MGLAdditions.h"

#include <mbgl/map/map.hpp>
#include <mbgl/style/sources/vector_source.hpp>
#include <mbgl/renderer/renderer.hpp>

@interface MGLVectorSource ()

@property (nonatomic, readonly) mbgl::style::VectorSource *rawSource;

@end

@implementation MGLVectorSource

- (instancetype)initWithIdentifier:(NSString *)identifier configurationURL:(NSURL *)configurationURL {
    auto source = std::make_unique<mbgl::style::VectorSource>(identifier.UTF8String,
                                                              configurationURL.mgl_URLByStandardizingScheme.absoluteString.UTF8String);
    return self = [super initWithPendingSource:std::move(source)];
}

- (instancetype)initWithIdentifier:(NSString *)identifier tileURLTemplates:(NS_ARRAY_OF(NSString *) *)tileURLTemplates options:(nullable NS_DICTIONARY_OF(MGLTileSourceOption, id) *)options {
    mbgl::Tileset tileSet = MGLTileSetFromTileURLTemplates(tileURLTemplates, options);
    auto source = std::make_unique<mbgl::style::VectorSource>(identifier.UTF8String, tileSet);
    return self = [super initWithPendingSource:std::move(source)];
}

- (mbgl::style::VectorSource *)rawSource {
    return (mbgl::style::VectorSource *)super.rawSource;
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
    
    std::vector<mbgl::Feature> features;
    if (self.style) {
        features = self.style.mapView.renderer->querySourceFeatures(self.rawSource->getID(), { optionalSourceLayerIDs, optionalFilter });
    }
    return MGLFeaturesFromMBGLFeatures(features);
}

@end
