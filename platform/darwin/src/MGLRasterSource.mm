#import "MGLRasterSource.h"

#import "MGLMapView_Private.h"
#import "MGLSource_Private.h"
#import "MGLTileSet_Private.h"
#import "NSURL+MGLAdditions.h"

#include <mbgl/style/sources/raster_source.hpp>

@interface MGLRasterSource ()

@property (nonatomic) mbgl::style::RasterSource *rawSource;

@end

@implementation MGLRasterSource
{
    std::unique_ptr<mbgl::style::RasterSource> _pendingSource;
}

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
    
    _pendingSource = std::move(source);
    self.rawSource = _pendingSource.get();
}

- (void)addToMapView:(MGLMapView *)mapView
{
    mapView.mbglMap->addSource(std::move(_pendingSource));
}

@end
