#import "MGLRasterSource.h"

#import "MGLSource_Private.h"

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

- (instancetype)initWithSourceIdentifier:(NSString *)sourceIdentifier tileURLTemplates:(NS_ARRAY_OF(NSString *) *)tileURLTemplates minimumZoomLevel:(NSUInteger)minimumZoomLevel maximumZoomLevel:(NSUInteger)maximumZoomLevel
{
    if (self = [super initWithSourceIdentifier:sourceIdentifier])
    {
        _tileURLTemplates = tileURLTemplates;
        _minimumZoomLevel = minimumZoomLevel;
        _maximumZoomLevel = maximumZoomLevel;
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
        mbgl::Tileset tileSource;
        tileSource.zoomRange = mbgl::Range<uint8_t>(self.minimumZoomLevel, self.maximumZoomLevel);
        for (NSString *tileURLTemplate in self.tileURLTemplates)
        {
            tileSource.tiles.push_back(tileURLTemplate.UTF8String);
        }
        source = std::make_unique<mbgl::style::RasterSource>(self.sourceIdentifier.UTF8String, tileSource, uint16_t(self.tileSize));
        
    }
    
    return std::move(source);
}

@end
