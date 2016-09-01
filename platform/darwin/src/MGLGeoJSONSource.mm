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

- (instancetype)initWithSourceIdentifier:(NSString *)sourceIdentifier geoJSONData:(NSData *)data
{
    if (self = [super initWithSourceIdentifier:sourceIdentifier])
    {
        _geoJSONData = data;
    }
    return self;
}

- (instancetype)initWithSourceIdentifier:(NSString *)sourceIdentifier geoJSONData:(NSData *)data options:(NS_DICTIONARY_OF(NSString *, id) *)options
{
    if (self = [super initWithSourceIdentifier:sourceIdentifier])
    {
        _geoJSONData = data;
        _options = options;
    }
    return self;
}

- (instancetype)initWithSourceIdentifier:(NSString *)sourceIdentifier URL:(NSURL *)url
{
    if (self = [super initWithSourceIdentifier:sourceIdentifier])
    {
        _URL = url;
    }
    return self;
}

- (instancetype)initWithSourceIdentifier:(NSString *)sourceIdentifier URL:(NSURL *)url options:(NS_DICTIONARY_OF(NSString *, id) *)options
{
    if (self = [super initWithSourceIdentifier:sourceIdentifier])
    {
        _URL = url;
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
    auto source = std::make_unique<mbgl::style::GeoJSONSource>(self.sourceIdentifier.UTF8String, [self geoJSONOptions]);
    
    if (self.URL) {
        NSURL *url = self.URL.mgl_URLByStandardizingScheme;
        source->setURL(url.absoluteString.UTF8String);
    } else {
        NSString *string = [[NSString alloc] initWithData:self.geoJSONData encoding:NSUTF8StringEncoding];
        const auto geojson = mapbox::geojson::parse(string.UTF8String).get<mapbox::geojson::feature_collection>();
        source->setGeoJSON(geojson);
        _features = MGLFeaturesFromMBGLFeatures(geojson);
    }
    
    return std::move(source);
}

@end
