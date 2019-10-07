#import "MGLVectorTileSource_Private.h"

#import "MGLFeature_Private.h"
#import "MGLLoggingConfiguration_Private.h"
#import "MGLSource_Private.h"
#import "MGLTileSource_Private.h"
#import "MGLStyle_Private.h"
#import "MGLMapView_Private.h"

#import "NSPredicate+MGLPrivateAdditions.h"
#import "NSURL+MGLAdditions.h"

#include <mbgl/map/map.hpp>
#include <mbgl/style/sources/vector_source.hpp>
#include <mbgl/renderer/renderer.hpp>

@interface MGLVectorTileSource ()

@property (nonatomic, readonly) mbgl::style::VectorSource *rawSource;

@end

@implementation MGLVectorTileSource

- (instancetype)initWithIdentifier:(NSString *)identifier configurationURL:(NSURL *)configurationURL {
    auto source = std::make_unique<mbgl::style::VectorSource>(identifier.UTF8String,
                                                              configurationURL.mgl_URLByStandardizingScheme.absoluteString.UTF8String);
    return self = [super initWithPendingSource:std::move(source)];
}

- (instancetype)initWithIdentifier:(NSString *)identifier tileURLTemplates:(NSArray<NSString *> *)tileURLTemplates options:(nullable NSDictionary<MGLTileSourceOption, id> *)options {
    mbgl::Tileset tileSet = MGLTileSetFromTileURLTemplates(tileURLTemplates, options);
    auto source = std::make_unique<mbgl::style::VectorSource>(identifier.UTF8String, tileSet);
    return self = [super initWithPendingSource:std::move(source)];
}

- (mbgl::style::VectorSource *)rawSource {
    return (mbgl::style::VectorSource *)super.rawSource;
}

- (NSURL *)configurationURL {
    MGLAssertStyleSourceIsValid();
    auto url = self.rawSource->getURL();
    return url ? [NSURL URLWithString:@(url->c_str())] : nil;
}

- (NSString *)attributionHTMLString {
    if (!self.rawSource) {
        MGLAssert(0, @"Source with identifier `%@` was invalidated after a style change", self.identifier);
        return nil;
    }

    auto attribution = self.rawSource->getAttribution();
    return attribution ? @(attribution->c_str()) : nil;
}

- (NSArray<id <MGLFeature>> *)featuresInSourceLayersWithIdentifiers:(NSSet<NSString *> *)sourceLayerIdentifiers predicate:(nullable NSPredicate *)predicate {
    MGLAssertStyleSourceIsValid();
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
    if (self.mapView) {
        features = self.mapView.renderer->querySourceFeatures(self.rawSource->getID(), { optionalSourceLayerIDs, optionalFilter });
    }
    return MGLFeaturesFromMBGLFeatures(features);
}

@end

@implementation MGLVectorTileSource (Private)

/**
 An array of locale codes with dedicated name fields in the Mapbox Streets
 source.
 
 https://www.mapbox.com/vector-tiles/mapbox-streets-v8/
 */
static NSArray * const MGLMapboxStreetsLanguages = @[
    @"ar", @"de", @"en", @"es", @"fr", @"ja", @"ko", @"pt", @"ru", @"zh",
    @"zh-Hans",
];

/**
 Like `MGLMapboxStreetsLanguages`, but deanglicized for use with
 `+[NSBundle preferredLocalizationsFromArray:forPreferences:]`.
 */
static NSArray * const MGLMapboxStreetsAlternativeLanguages = @[
    @"mul", @"ar", @"de", @"es", @"fr", @"ja", @"ko", @"pt", @"ru", @"zh",
    @"zh-Hans",
];

+ (NSSet<NSString *> *)mapboxStreetsLanguages {
    static dispatch_once_t onceToken;
    static NSSet<NSString *> *mapboxStreetsLanguages;
    dispatch_once(&onceToken, ^{
        mapboxStreetsLanguages = [NSSet setWithArray:MGLMapboxStreetsLanguages];
    });
    return mapboxStreetsLanguages;
}

+ (NSString *)preferredMapboxStreetsLanguage {
    return [self preferredMapboxStreetsLanguageForPreferences:[NSLocale preferredLanguages]];
}

+ (NSString *)preferredMapboxStreetsLanguageForPreferences:(NSArray<NSString *> *)preferencesArray {
    BOOL acceptsEnglish = [preferencesArray filteredArrayUsingPredicate:
                           [NSPredicate predicateWithBlock:^BOOL(NSString * _Nullable language, NSDictionary<NSString *,id> * _Nullable bindings) {
        NSString *languageCode;

        if (@available(iOS 10.0, macOS 10.12.0, *)) {
            languageCode = [NSLocale localeWithLocaleIdentifier:language].languageCode;
        }
        else {
            languageCode = [[NSLocale localeWithLocaleIdentifier:language] objectForKey:NSLocaleLanguageCode];
        }

        return [languageCode isEqualToString:@"en"];
    }]].count;
    
    NSArray<NSString *> *availableLanguages = acceptsEnglish ? MGLMapboxStreetsLanguages : MGLMapboxStreetsAlternativeLanguages;
    NSArray<NSString *> *preferredLanguages = [NSBundle preferredLocalizationsFromArray:availableLanguages
                                                                         forPreferences:preferencesArray];
    NSString *mostSpecificLanguage;
    for (NSString *language in preferredLanguages) {
        if (language.length > mostSpecificLanguage.length) {
            mostSpecificLanguage = language;
        }
    }
    return [mostSpecificLanguage isEqualToString:@"mul"] ? nil : mostSpecificLanguage;
}

- (BOOL)isMapboxStreets {
    NSURL *url = self.configurationURL;
    if (![url.scheme isEqualToString:@"mapbox"]) {
        return NO;
    }
    NSArray *identifiers = [url.host componentsSeparatedByString:@","];
    return [identifiers containsObject:@"mapbox.mapbox-streets-v8"] || [identifiers containsObject:@"mapbox.mapbox-streets-v7"];
}

@end
