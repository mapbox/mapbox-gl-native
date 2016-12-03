#import "MGLGeoJSONSource_Private.h"

#import "MGLMapView_Private.h"
#import "MGLSource_Private.h"
#import "MGLFeature_Private.h"

#import "NSURL+MGLAdditions.h"

#include <mbgl/style/sources/geojson_source.hpp>

const MGLGeoJSONSourceOption MGLGeoJSONSourceOptionClustered = @"MGLGeoJSONSourceOptionClustered";
const MGLGeoJSONSourceOption MGLGeoJSONSourceOptionClusterRadius = @"MGLGeoJSONSourceOptionClusterRadius";
const MGLGeoJSONSourceOption MGLGeoJSONSourceOptionMaximumZoomLevelForClustering = @"MGLGeoJSONSourceOptionMaximumZoomLevelForClustering";
const MGLGeoJSONSourceOption MGLGeoJSONSourceOptionMaximumZoomLevel = @"MGLGeoJSONSourceOptionMaximumZoomLevel";
const MGLGeoJSONSourceOption MGLGeoJSONSourceOptionBuffer = @"MGLGeoJSONSourceOptionBuffer";
const MGLGeoJSONSourceOption MGLGeoJSONSourceOptionSimplificationTolerance = @"MGLGeoJSONSourceOptionSimplificationTolerance";

@interface MGLGeoJSONSource ()

@property (nonatomic, readwrite) NSDictionary *options;
@property (nonatomic) mbgl::style::GeoJSONSource *rawSource;

@end

@implementation MGLGeoJSONSource
{
    std::unique_ptr<mbgl::style::GeoJSONSource> _pendingSource;
}

- (instancetype)initWithIdentifier:(NSString *)identifier geoJSONData:(NSData *)data options:(NS_DICTIONARY_OF(NSString *, id) *)options
{
    if (self = [super initWithIdentifier:identifier])
    {
        _geoJSONData = data;
        _options = options;
        [self commonInit];
    }
    return self;
}

- (instancetype)initWithIdentifier:(NSString *)identifier URL:(NSURL *)url options:(NS_DICTIONARY_OF(NSString *, id) *)options
{
    if (self = [super initWithIdentifier:identifier])
    {
        _URL = url;
        _options = options;
        [self commonInit];
    }
    return self;
}

- (instancetype)initWithIdentifier:(NSString *)identifier features:(NSArray<id<MGLFeature>> *)features options:(NS_DICTIONARY_OF(NSString *,id) *)options {
    if (self = [super initWithIdentifier:identifier]) {
        _features = features;
        _options = options;
        [self commonInit];
    }
    
    return self;
}

- (void)addToMapView:(MGLMapView *)mapView
{
    if (_pendingSource == nullptr) {
        [NSException raise:@"MGLRedundantSourceException"
                    format:@"This instance %@ was already added to %@. Adding the same source instance " \
                            "to the style more than once is invalid.", self, mapView.style];
    }

    mapView.mbglMap->addSource(std::move(_pendingSource));
}

- (void)removeFromMapView:(MGLMapView *)mapView
{
    auto removedSource = mapView.mbglMap->removeSource(self.identifier.UTF8String);

    _pendingSource = std::move(reinterpret_cast<std::unique_ptr<mbgl::style::GeoJSONSource> &>(removedSource));
    self.rawSource = _pendingSource.get();
}

- (void)commonInit
{
    auto source = std::make_unique<mbgl::style::GeoJSONSource>(self.identifier.UTF8String, self.geoJSONOptions);
    
    if (self.URL) {
        NSURL *url = self.URL.mgl_URLByStandardizingScheme;
        source->setURL(url.absoluteString.UTF8String);
        _features = nil;
    } else if (self.geoJSONData) {
        NSString *string = [[NSString alloc] initWithData:self.geoJSONData encoding:NSUTF8StringEncoding];
        const auto geojson = mapbox::geojson::parse(string.UTF8String).get<mapbox::geojson::feature_collection>();
        source->setGeoJSON(geojson);
        _features = MGLFeaturesFromMBGLFeatures(geojson);
    } else {
        mbgl::FeatureCollection featureCollection;
        featureCollection.reserve(self.features.count);
        for (id <MGLFeaturePrivate> feature in self.features) {
            featureCollection.push_back([feature mbglFeature]);
        }
        const auto geojson = mbgl::GeoJSON{featureCollection};
        source->setGeoJSON(geojson);
        _features = MGLFeaturesFromMBGLFeatures(featureCollection);
    }
    
    _pendingSource = std::move(source);
    self.rawSource = _pendingSource.get();
}

- (mbgl::style::GeoJSONOptions)geoJSONOptions
{
    auto mbglOptions = mbgl::style::GeoJSONOptions();
    
    if (id value = self.options[MGLGeoJSONSourceOptionMaximumZoomLevel]) {
        [self validateValue:value];
        mbglOptions.maxzoom = [value integerValue];
    }
    
    if (id value = self.options[MGLGeoJSONSourceOptionBuffer]) {
        [self validateValue:value];
        mbglOptions.buffer = [value integerValue];
    }
    
    if (id value = self.options[MGLGeoJSONSourceOptionSimplificationTolerance]) {
        [self validateValue:value];
        mbglOptions.tolerance = [value doubleValue];
    }
    
    if (id value = self.options[MGLGeoJSONSourceOptionClusterRadius]) {
        [self validateValue:value];
        mbglOptions.clusterRadius = [value integerValue];
    }
    
    if (id value = self.options[MGLGeoJSONSourceOptionMaximumZoomLevelForClustering]) {
        [self validateValue:value];
        mbglOptions.clusterMaxZoom = [value integerValue];
    }
    
    if (id value = self.options[MGLGeoJSONSourceOptionClustered]) {
        [self validateValue:value];
        mbglOptions.cluster = [value boolValue];
    }
    
    return mbglOptions;
}

- (void)validateValue:(id)value
{
    if (! [value isKindOfClass:[NSNumber class]])
    {
        [NSException raise:@"Value not handled" format:@"%@ is not an NSNumber", value];
    }
}

- (void)setGeoJSONData:(NSData *)geoJSONData
{
    _geoJSONData = geoJSONData;
    
    if (self.rawSource == NULL)
    {
        [self commonInit];
    }
    
    NSString *string = [[NSString alloc] initWithData:_geoJSONData encoding:NSUTF8StringEncoding];
    const auto geojson = mapbox::geojson::parse(string.UTF8String).get<mapbox::geojson::feature_collection>();
    self.rawSource->setGeoJSON(geojson);
    
    _features = MGLFeaturesFromMBGLFeatures(geojson);
}

- (void)setURL:(NSURL *)URL
{
    _URL = URL;
    
    if (self.rawSource == NULL)
    {
        [self commonInit];
    }
    
    NSURL *url = self.URL.mgl_URLByStandardizingScheme;
    self.rawSource->setURL(url.absoluteString.UTF8String);
}

- (void)setFeatures:(NSArray *)features
{
    if (self.rawSource == NULL)
    {
        [self commonInit];
    }
    
    mbgl::FeatureCollection featureCollection;
    featureCollection.reserve(features.count);
    for (id <MGLFeaturePrivate> feature in features) {
        featureCollection.push_back([feature mbglFeature]);
    }
    const auto geojson = mbgl::GeoJSON{featureCollection};
    self.rawSource->setGeoJSON(geojson);
    
    _features = MGLFeaturesFromMBGLFeatures(featureCollection);
}

- (NSString *)description
{
    return [NSString stringWithFormat:@"<%@: %p; identifier = %@; URL = %@; geoJSONData = %@; features = %@>",
            NSStringFromClass([self class]), (void *)self, self.identifier, self.URL, self.geoJSONData, self.features];
}

@end
