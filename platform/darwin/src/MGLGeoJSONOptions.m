#import "MGLGeoJSONOptions.h"

@implementation MGLGeoJSONOptions

- (instancetype)init
{
    if (self = [super init]) {
        _maximumZoom = 18;
        _buffer = 128;
        _tolerance = 0.375;
        
        _cluster = NO;
        _clusterRadius = 50;
        _clusterMaximumZoom = 17;
    }
    return self;
}

@end
