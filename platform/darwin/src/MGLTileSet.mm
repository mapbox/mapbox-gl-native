#import "MGLTileSet.h"

#include <mbgl/util/tileset.hpp>

@implementation MGLTileSet

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
    if (minimumZoomLevel > maximumZoomLevel)
    {
        [[NSException exceptionWithName:@"Invalid minimumZoomLevel"
                                 reason:@"minimumZoomLevel must be less than maximumZoomLevel"
                               userInfo:nil] raise];
        return nil;
    }
    
    if (self = [super init])
    {
        _tileURLTemplates = tileURLTemplates;
        _minimumZoomLevel = @(minimumZoomLevel);
        _maximumZoomLevel = @(maximumZoomLevel);
    }
    return self;
}

- (void)setMinimumZoomLevel:(NSNumber *)minimumZoomLevel
{
    if (self.maximumZoomLevel && [minimumZoomLevel integerValue] > [self.maximumZoomLevel integerValue])
    {
        [[NSException exceptionWithName:@"Invalid minimumZoomLevel"
                                 reason:@"minimumZoomLevel must be less than maximumZoomLevel"
                               userInfo:nil] raise];
        return;
    }
    
    _minimumZoomLevel = minimumZoomLevel;
}

- (void)setMaximumZoomLevel:(NSNumber *)maximumZoomLevel
{
    if (self.maximumZoomLevel && [maximumZoomLevel integerValue] < [self.maximumZoomLevel integerValue])
    {
        [[NSException exceptionWithName:@"Invalid minimumZoomLevel"
                                 reason:@"minimumZoomLevel must be less than maximumZoomLevel"
                               userInfo:nil] raise];
    }
    
    _maximumZoomLevel = maximumZoomLevel;
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
    uint8_t minimumZoomLevel = self.minimumZoomLevel ? [self.minimumZoomLevel unsignedIntegerValue] : tileset.zoomRange.min;
    uint8_t maximumZoomLevel = self.minimumZoomLevel ? [self.maximumZoomLevel unsignedIntegerValue] : tileset.zoomRange.max;
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

- (NSString *)description
{
    return [NSString stringWithFormat:@"<%@: %p; tileURLTemplates = %@>",
            NSStringFromClass([self class]), (void *)self, self.tileURLTemplates];
}

@end
