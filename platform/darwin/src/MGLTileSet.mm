#import "MGLTileSet.h"

#include <mbgl/util/tileset.hpp>

@implementation MGLTileSet
{
    BOOL _minimumZoomLevelIsSet;
    BOOL _maximumZoomLevelIsSet;
}

- (instancetype)initWithTileURLTemplates:(NS_ARRAY_OF(NSString *) *)tileURLTemplates
{
    if (self = [super init])
    {
        _tileURLTemplates = tileURLTemplates;
    }
    return self;
}

- (instancetype)initWithTileURLTemplates:(NS_ARRAY_OF(NSString *) *)tileURLTemplates minimumZoomLevel:(NSUInteger)minimumZoomLevel maximumZoomLevel:(NSUInteger)maximumZoomLevel
{
    if (self = [super init])
    {
        _tileURLTemplates = tileURLTemplates;
        _minimumZoomLevel = minimumZoomLevel;
        _minimumZoomLevelIsSet = YES;
        _maximumZoomLevel = maximumZoomLevel;
        _maximumZoomLevelIsSet = YES;
    }
    return self;
}

- (void)setMinimumZoomLevel:(NSUInteger)minimumZoomLevel
{
    _minimumZoomLevel = minimumZoomLevel;
    _minimumZoomLevelIsSet = YES;
}

- (void)setMaximumZoomLevel:(NSUInteger)maximumZoomLevel
{
    _maximumZoomLevel = maximumZoomLevel;
    _maximumZoomLevelIsSet = YES;
}

- (mbgl::Tileset)mbglTileset
{
    mbgl::Tileset tileset;
    
    for (NSString *tileURLTemplate in self.tileURLTemplates)
    {
        tileset.tiles.push_back(tileURLTemplate.UTF8String);
    }
   
    // set the minimum / maximum zoom range to the values specified by this class if they
    // were set. otherwise, use the core objects default values
    uint8_t minimumZoomLevel = _minimumZoomLevelIsSet ? self.minimumZoomLevel : tileset.zoomRange.min;
    uint8_t maximumZoomLevel = _maximumZoomLevelIsSet ? self.maximumZoomLevel : tileset.zoomRange.max;
    tileset.zoomRange = mbgl::Range<uint8_t>(minimumZoomLevel, maximumZoomLevel);
   
    if (self.attribution)
    {
        tileset.attribution = self.attribution.UTF8String;
    }
    
    if (self.scheme == MGLTileSetSchemeTMS) {
        tileset.scheme = mbgl::Tileset::Scheme::TMS;
    }
    
    return tileset;
}

@end
