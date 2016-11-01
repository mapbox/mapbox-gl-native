#import "MGLCustomVectorSource.h"

#import "MGLSource_Private.h"

@implementation MGLCustomVectorSource

- (instancetype)initWithIdentifier:(NSString *)identifier minimumZoomLevel:(NSUInteger)minimumZoomLevel maximumZoomLevel:(NSUInteger)maximumZoomLevel dataSource:(NSObject<MGLCustomVectorSourceDataSource>*)dataSource
{
    if (self = [super initWithIdentifier:identifier])
    {
        _maximumZoomLevel = @(maximumZoomLevel);
        _minimumZoomLevel = @(minimumZoomLevel);
        _dataSource = dataSource;
    }
    return self;
}

- (std::unique_ptr<mbgl::style::Source>)mbglSource {
    return nil;
}

@end
