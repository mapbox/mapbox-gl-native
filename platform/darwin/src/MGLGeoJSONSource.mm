#import "MGLGeoJSONSource.h"

#import "MGLSource_Private.hpp"

#include <mbgl/style/sources/geojson_source.hpp>

@interface MGLGeoJSONSource()
@property (nonatomic, copy) NSURL *url;
@end

@implementation MGLGeoJSONSource

static NSString *MGLGeoJSONSourceType   = @"geojson";
static NSString *MGLGeoJSONDataKey      = @"data";

- (instancetype)initWithSourceID:(NSString *)sourceID url:(NSURL *)url
{
    self = [super initWithSourceID:sourceID sourceType:MGLGeoJSONSourceType];
    if (self == nil) return nil;
    _data = url.absoluteString;
    return self;
}


- (std::unique_ptr<mbgl::style::Source>)mbgl_source
{
    auto source = std::make_unique<mbgl::style::GeoJSONSource>(self.sourceID.UTF8String);
    source->setURL(self.data.UTF8String);
    return std::move(source);
}

@end
