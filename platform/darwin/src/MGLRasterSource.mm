#import "MGLRasterSource.h"

#import "MGLSource_Private.hpp"

#include <mbgl/style/sources/raster_source.hpp>

@implementation MGLRasterSource

static NSString *MGLRasterSourceType   = @"raster";

- (instancetype)initWithSourceID:(NSString *)sourceID url:(NSURL *)url tileSize:(CGFloat)tileSize
{
    self = [super initWithSourceID:sourceID sourceType:MGLRasterSourceType];
    if (self == nil) return nil;
    _url = url;
    _tileSize = tileSize;
    return self;
}


- (std::unique_ptr<mbgl::style::Source>)mbgl_source
{
    auto source = std::make_unique<mbgl::style::RasterSource>(self.sourceID.UTF8String,
                                                              self.url.absoluteString.UTF8String,
                                                              uint16_t(self.tileSize));
    return std::move(source);
}

@end
