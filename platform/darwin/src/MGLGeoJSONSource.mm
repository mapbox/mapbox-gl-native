#import "MGLGeoJSONSource.h"

#import "MGLSource_Private.hpp"

#include <mbgl/style/sources/geojson_source.hpp>

@interface MGLGeoJSONSource ()

@property (nonatomic, copy) NSURL *URL;

@end

@implementation MGLGeoJSONSource

static NSString *MGLGeoJSONSourceType   = @"geojson";
static NSString *MGLGeoJSONDataKey      = @"data";

- (instancetype)initWithSourceIdentifier:(NSString *)sourceIdentifier URL:(NSURL *)url
{
    if (self = [super initWithSourceIdentifier:sourceIdentifier sourceType:MGLGeoJSONSourceType]) {
        _data = url.absoluteString;
    }
    return self;
}


- (std::unique_ptr<mbgl::style::Source>)mbgl_source
{
    auto source = std::make_unique<mbgl::style::GeoJSONSource>(self.sourceIdentifier.UTF8String);
    source->setURL(self.data.UTF8String);
    return std::move(source);
}

@end
