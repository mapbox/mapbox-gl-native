#import "MGLRasterSource.h"

#import "MGLSource_Private.hpp"

#include <mbgl/style/sources/raster_source.hpp>

@implementation MGLRasterSource

static NSString *MGLRasterSourceType   = @"raster";

- (instancetype)initWithSourceIdentifier:(NSString *)sourceIdentifier URL:(NSURL *)url tileSize:(CGFloat)tileSize {
    if (self = [super initWithSourceIdentifier:sourceIdentifier sourceType:MGLRasterSourceType]) {
        _URL = url;
        _tileSize = tileSize;
    }
    return self;
}

- (std::unique_ptr<mbgl::style::Source>)mbgl_source {
    auto source = std::make_unique<mbgl::style::RasterSource>(self.sourceIdentifier.UTF8String,
                                                              self.URL.absoluteString.UTF8String,
                                                              uint16_t(self.tileSize));
    return std::move(source);
}

@end
