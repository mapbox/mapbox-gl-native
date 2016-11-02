#import "MGLCustomVectorSource.h"

#import "MGLSource_Private.h"
#include <mbgl/style/sources/custom_vector_source.hpp>

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

- (mbgl::style::CustomVectorSourceOptions)customSourceOptions {
    auto options = mbgl::style::CustomVectorSourceOptions();
    options.minzoom = self.minimumZoomLevel.integerValue;
    options.maxzoom = self.maximumZoomLevel.integerValue;
    return options;
}

- (std::unique_ptr<mbgl::style::Source>)mbglSource {
    auto source = std::make_unique<mbgl::style::CustomVectorSource>(self.identifier.UTF8String, self.customSourceOptions,
                                                                    ^void(uint8_t z, uint32_t x, uint32_t y){
                                                                        [self.dataSource getTileForZoom:z
                                                                                                      x:x
                                                                                                      y:y
                                                                                               callback:^(NSDictionary * _Nullable) {
                                                                        
                                                                    }];
                                                                    });
    return std::move(source);
}



@end
