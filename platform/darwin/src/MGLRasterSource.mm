#import "MGLRasterSource.h"

#import "MGLSource_Private.h"
#import "MGLTileSet_Private.h"

#include <mbgl/style/sources/raster_source.hpp>

@implementation MGLRasterSource

- (instancetype)initWithSourceIdentifier:(NSString *)sourceIdentifier URL:(NSURL *)url tileSize:(CGFloat)tileSize
{
    if (self = [super initWithSourceIdentifier:sourceIdentifier]) {
        _URL = url;
        _tileSize = tileSize;
    }
    return self;
}

- (instancetype)initWithSourceIdentifier:(NSString *)sourceIdentifier tileSize:(CGFloat)tileSize tileSet:(MGLTileSet *)tileSet;
{
    if (self = [super initWithSourceIdentifier:sourceIdentifier])
    {
        _tileSize = tileSize;
        _tileSet = tileSet;
    }
    return self;
}

- (std::unique_ptr<mbgl::style::Source>)mbglSource
{
    std::unique_ptr<mbgl::style::RasterSource> source;
    
    if (self.URL)
    {
        source = std::make_unique<mbgl::style::RasterSource>(self.sourceIdentifier.UTF8String,
                                                             self.URL.absoluteString.UTF8String,
                                                             uint16_t(self.tileSize));
    }
    else
    {
        source = std::make_unique<mbgl::style::RasterSource>(self.sourceIdentifier.UTF8String, self.tileSet.mbglTileset, uint16_t(self.tileSize));
        
    }
    
    return std::move(source);
}

@end
