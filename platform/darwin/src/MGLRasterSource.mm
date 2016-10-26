#import "MGLRasterSource.h"

#import "MGLSource_Private.h"
#import "MGLTileSet_Private.h"
#import "NSURL+MGLAdditions.h"

#include <mbgl/style/sources/raster_source.hpp>

@implementation MGLRasterSource

- (instancetype)initWithIdentifier:(NSString *)identifier URL:(NSURL *)url tileSize:(CGFloat)tileSize
{
    if (self = [super initWithIdentifier:identifier]) {
        _URL = url;
        _tileSize = tileSize;
    }
    return self;
}

- (instancetype)initWithIdentifier:(NSString *)identifier tileSet:(MGLTileSet *)tileSet tileSize:(CGFloat)tileSize;
{
    if (self = [super initWithIdentifier:identifier])
    {
        _tileSet = tileSet;
        _tileSize = tileSize;
    }
    return self;
}

- (std::unique_ptr<mbgl::style::Source>)mbglSource
{
    std::unique_ptr<mbgl::style::RasterSource> source;
    
    if (self.URL)
    {
        source = std::make_unique<mbgl::style::RasterSource>(self.identifier.UTF8String,
                                                             self.URL.mgl_URLByStandardizingScheme.absoluteString.UTF8String,
                                                             uint16_t(self.tileSize));
    }
    else
    {
        source = std::make_unique<mbgl::style::RasterSource>(self.identifier.UTF8String,
                                                             self.tileSet.mbglTileset,
                                                             uint16_t(self.tileSize));
        
    }
    
    return std::move(source);
}

@end
