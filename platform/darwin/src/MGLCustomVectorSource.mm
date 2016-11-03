#import "MGLCustomVectorSource.h"

#import "MGLMapView_Private.h"
#import "MGLSource_Private.h"
#import "MGLFeature_Private.h"

#include <mbgl/style/sources/custom_vector_source.hpp>

@interface MGLCustomVectorSource () {
    std::unique_ptr<mbgl::style::CustomVectorSource> _pendingSource;
}

@property (nonnull) mbgl::style::CustomVectorSource *rawSource;

@end

@implementation MGLCustomVectorSource

- (instancetype)initWithIdentifier:(NSString *)identifier minimumZoomLevel:(NSUInteger)minimumZoomLevel maximumZoomLevel:(NSUInteger)maximumZoomLevel dataSource:(NSObject<MGLCustomVectorSourceDataSource>*)dataSource
{
    if (self = [super initWithIdentifier:identifier])
    {
        _maximumZoomLevel = @(maximumZoomLevel);
        _minimumZoomLevel = @(minimumZoomLevel);
        _dataSource = dataSource;
    
        auto source = std::make_unique<mbgl::style::CustomVectorSource>(self.identifier.UTF8String, self.customSourceOptions,
                                                                         ^void(uint8_t z, uint32_t x, uint32_t y)
                                                                         {
                                                                             [self.dataSource getTileForZoom:z
                                                                                                           x:x
                                                                                                           y:y
                                                                                                    callback:
                                                                              ^(NS_ARRAY_OF(id <MGLFeature>) *features)
                                                                              {
                                                                                  [self processData:features forTile:z x:x y:y];
//                                                                                  NSLog(@"Got tile data into callback%@", tiledata);
                                                                              }];
                                                                         });
        
        _pendingSource = std::move(source);
        self.rawSource = _pendingSource.get();

    }
    return self;
}

- (mbgl::style::CustomVectorSourceOptions)customSourceOptions
{
    auto options = mbgl::style::CustomVectorSourceOptions();
    options.minzoom = self.minimumZoomLevel.integerValue;
    options.maxzoom = self.maximumZoomLevel.integerValue;
    return options;
}

- (void)addToMapView:(MGLMapView *)mapView
{
    mapView.mbglMap->addSource(std::move(_pendingSource));
}

- (void)setSourceLoaded
{
    _rawSource->setSourceLoaded();
}

- (void)processData:(NS_ARRAY_OF(id <MGLFeature>)*)features forTile:(uint8_t)z x:(uint32_t)x y:(uint32_t)y
{
    NSLog(@"processData %li/%li/%li", (long)z, (long)x, (long)y);
}


@end
