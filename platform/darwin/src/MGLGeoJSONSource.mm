#import "MGLGeoJSONSource.h"

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
    _url = url;
    return self;
}

- (NSString *)urlString
{
    return self.url.absoluteString;
}


@end
