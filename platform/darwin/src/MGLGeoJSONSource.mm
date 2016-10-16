#import "MGLGeoJSONSource.h"

#import "MGLSource_Private.h"
#import "MGLFeature_Private.h"

#import "NSURL+MGLAdditions.h"

#include <mbgl/style/sources/geojson_source.hpp>

NSString * const MGLGeoJSONClusterOption = @"MGLGeoJSONCluster";
NSString * const MGLGeoJSONClusterRadiusOption = @"MGLGeoJSONClusterRadius";
NSString * const MGLGeoJSONClusterMaximumZoomLevelOption = @"MGLGeoJSONClusterMaximumZoomLevel";
NSString * const MGLGeoJSONMaximumZoomLevelOption = @"MGLGeoJSONMaximumZoomLevel";
NSString * const MGLGeoJSONBufferOption = @"MGLGeoJSONBuffer";
NSString * const MGLGeoJSONToleranceOption = @"MGLGeoJSONOptionsClusterTolerance";

@interface MGLGeoJSONSource ()

@property (nonatomic, readwrite) NSDictionary *options;

@end

@implementation MGLGeoJSONSource

- (instancetype)initWithIdentifier:(NSString *)identifier geoJSONData:(NSData *)data options:(NS_DICTIONARY_OF(NSString *, id) *)options
{
    if (self = [super initWithIdentifier:identifier])
    {
        _geoJSONData = data;
        _options = options;
    }
    return self;
}

- (instancetype)initWithIdentifier:(NSString *)identifier URL:(NSURL *)url options:(NS_DICTIONARY_OF(NSString *, id) *)options
{
    if (self = [super initWithIdentifier:identifier])
    {
        _URL = url;
        _options = options;
    }
    return self;
}

- (instancetype)initWithIdentifier:(NSString *)identifier features:(NSArray<id<MGLFeature>> *)features options:(NS_DICTIONARY_OF(NSString *,id) *)options {
    if (self = [super initWithIdentifier:identifier]) {
        _features = features;
        _options = options;
    }
    
    return self;
}

- (mbgl::style::GeoJSONOptions)geoJSONOptions
{
    auto mbglOptions = mbgl::style::GeoJSONOptions();
    
    if (self.options[MGLGeoJSONMaximumZoomLevelOption]) {
        id value = self.options[MGLGeoJSONMaximumZoomLevelOption];
        [self validateValue:value];
        mbglOptions.maxzoom = [value integerValue];
    }
    
    if (self.options[MGLGeoJSONBufferOption]) {
        id value = self.options[MGLGeoJSONBufferOption];
        [self validateValue:value];
        mbglOptions.buffer = [value integerValue];
    }
    
    if (self.options[MGLGeoJSONToleranceOption]) {
        id value = self.options[MGLGeoJSONToleranceOption];
        [self validateValue:value];
        mbglOptions.tolerance = [value doubleValue];
    }
    
    if (self.options[MGLGeoJSONClusterRadiusOption]) {
        id value = self.options[MGLGeoJSONClusterRadiusOption];
        [self validateValue:value];
        mbglOptions.clusterRadius = [value integerValue];
    }
    
    if (self.options[MGLGeoJSONClusterMaximumZoomLevelOption]) {
        id value = self.options[MGLGeoJSONClusterMaximumZoomLevelOption];
        [self validateValue:value];
        mbglOptions.clusterMaxZoom = [value integerValue];
    }
    
    if (self.options[MGLGeoJSONClusterOption]) {
        id value = self.options[MGLGeoJSONClusterOption];
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

- (std::unique_ptr<mbgl::style::Source>)mbglSource
{
    auto source = std::make_unique<mbgl::style::GeoJSONSource>(self.identifier.UTF8String, self.geoJSONOptions);
    
    if (self.URL) {
        NSURL *url = self.URL.mgl_URLByStandardizingScheme;
        source->setURL(url.absoluteString.UTF8String);
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
    
    return std::move(source);
}

@end
