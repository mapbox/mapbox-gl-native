#import "MGLGeoJSONSource.h"

#import "MGLSource_Private.hpp"

#include <mbgl/style/sources/geojson_source.hpp>

@interface MGLGeoJSONSource ()

@property (nonatomic, copy) NSURL *URL;

@end

@implementation MGLGeoJSONSource

- (instancetype)initWithSourceIdentifier:(NSString *)sourceIdentifier URL:(NSURL *)url
{
    if (self = [super initWithSourceIdentifier:sourceIdentifier]) {
        _URL = url;
        if (url.isFileURL) {
            _data = [[NSString alloc] initWithContentsOfURL:url encoding:NSUTF8StringEncoding error:NULL];
        } else {
            _data = url.absoluteString;
        }
    }
    return self;
}

- (std::unique_ptr<mbgl::style::Source>)mbgl_source
{
    auto source = std::make_unique<mbgl::style::GeoJSONSource>(self.sourceIdentifier.UTF8String);
    if (_URL.isFileURL) {
        const auto geojson = mapbox::geojson::parse(self.data.UTF8String).get<mapbox::geojson::feature_collection>();
        source->setGeoJSON(geojson);
    } else {
        source->setURL(self.data.UTF8String);
    }
    return std::move(source);
}

@end
