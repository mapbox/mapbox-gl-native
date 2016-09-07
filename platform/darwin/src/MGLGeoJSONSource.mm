#import "MGLGeoJSONSource.h"

#import "MGLSource_Private.h"
#import "MGLFeature_Private.h"

#import "NSURL+MGLAdditions.h"

#include <mbgl/style/sources/geojson_source.hpp>

@implementation MGLGeoJSONSource

- (instancetype)initWithSourceIdentifier:(NSString *)sourceIdentifier geoJSONData:(NSData *)data {
    if (self = [super initWithSourceIdentifier:sourceIdentifier]) {
        _geoJSONData = data;
    }
    return self;
}

- (instancetype)initWithSourceIdentifier:(NSString *)sourceIdentifier URL:(NSURL *)url {
    if (self = [super initWithSourceIdentifier:sourceIdentifier]) {
        _URL = url;
    }
    return self;
}

- (std::unique_ptr<mbgl::style::Source>)mbgl_source
{
    auto source = std::make_unique<mbgl::style::GeoJSONSource>(self.sourceIdentifier.UTF8String);
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
