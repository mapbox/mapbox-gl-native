#import "MGLCustomVectorSource.h"

#import "MGLMapView_Private.h"
#import "MGLSource_Private.h"
#import "MGLFeature_Private.h"
#import "MGLShape_Private.h"
#import "MGLGeoJSONSourceBase_Private.h"
#import "MGLGeometry_Private.h"

#include <mbgl/style/sources/custom_vector_source.hpp>
#include <mbgl/util/geojson.hpp>

@interface MGLCustomVectorSource () {
    std::unique_ptr<mbgl::style::CustomVectorSource> _pendingSource;
}

@property (nonatomic, readwrite) NSDictionary *options;
@property (nonnull) mbgl::style::CustomVectorSource *rawSource;

@end

@implementation MGLCustomVectorSource

- (instancetype)initWithIdentifier:(NSString *)identifier dataSource:(NSObject<MGLCustomVectorSourceDataSource>*)dataSource options:(NS_DICTIONARY_OF(MGLGeoJSONSourceOption, id) *)options
{
    if (self = [super initWithIdentifier:identifier options:options])
    {
        _dataSource = dataSource;
        
        _requestQueue = [[NSOperationQueue alloc] init];
        self.requestQueue.name = [NSString stringWithFormat:@"mgl.CustomVectorSource.%@", identifier];
        auto source = std::make_unique<mbgl::style::CustomVectorSource>(self.identifier.UTF8String, self.geoJSONOptions,
                                                                        ^void(uint8_t z, uint32_t x, uint32_t y)
                                                                        {
                                                                            [self.requestQueue addOperationWithBlock:
                                                                             ^{
                                                                                 [self processData:
                                                                                  [self.dataSource getTileForZoomLevel:z
                                                                                                                     x:x
                                                                                                                     y:y]
                                                                                           forTile:z x:x y:y];
                                                                             }];
                                                                        });
        
        _pendingSource = std::move(source);
        self.rawSource = _pendingSource.get();
        
    }
    return self;
}

- (void)addToMapView:(MGLMapView *)mapView
{
    mapView.mbglMap->addSource(std::move(_pendingSource));
}

- (void)processData:(NS_ARRAY_OF(id <MGLFeature>)*)features forTile:(uint8_t)z x:(uint32_t)x y:(uint32_t)y
{
    mbgl::FeatureCollection featureCollection;
    featureCollection.reserve(features.count);
    for (id <MGLFeaturePrivate> feature in features)
    {
        featureCollection.push_back([feature mbglFeature]);
    }
    const auto geojson = mbgl::GeoJSON{featureCollection};
    dispatch_async(dispatch_get_main_queue(), ^{
        self.rawSource->setTileData(z, x, y, geojson);
    });
}

- (void)reloadTileInCoordinateBounds:(MGLCoordinateBounds)bounds zoomLevel:(NSUInteger)zoomLevel
{
    self.rawSource->reloadRegion(MGLLatLngBoundsFromCoordinateBounds(bounds), (uint8_t)zoomLevel);
}

- (void)setNeedsUpdateAtZoomLevel:(NSUInteger)z x:(NSUInteger)x y:(NSUInteger)y
{
    self.rawSource->updateTile((uint8_t)z, (uint32_t)x, (uint32_t)y);
}

- (void)reloadData
{
    self.rawSource->reload();
}

@end
