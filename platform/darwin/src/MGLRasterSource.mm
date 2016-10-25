#import "MGLRasterSource.h"

#import "MGLSource_Private.h"
#import "MGLTileSet_Private.h"
#import "NSURL+MGLAdditions.h"

#include <mbgl/style/sources/raster_source.hpp>

@interface MGLRasterSource ()

@property (nonatomic) mbgl::style::RasterSource *rawSource;

@end

@implementation MGLRasterSource

- (instancetype)initWithIdentifier:(NSString *)identifier URL:(NSURL *)url tileSize:(CGFloat)tileSize
{
    if (self = [super initWithIdentifier:identifier]) {
        _URL = url;
        _tileSize = tileSize;
        [self commonInit];
    }
    return self;
}

- (instancetype)initWithIdentifier:(NSString *)identifier tileSet:(MGLTileSet *)tileSet tileSize:(CGFloat)tileSize;
{
    if (self = [super initWithIdentifier:identifier])
    {
        _tileSet = tileSet;
        _tileSize = tileSize;
        [self commonInit];
    }
    return self;
}

- (void)commonInit
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
    
    self.pendingSource = std::move(source);
}

@end
