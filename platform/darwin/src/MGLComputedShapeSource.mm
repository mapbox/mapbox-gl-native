#import "MGLComputedShapeSource.h"

#import "MGLMapView_Private.h"
#import "MGLSource_Private.h"
#import "MGLShape_Private.h"
#import "MGLAbstractShapeSource_Private.h"
#import "MGLGeometry_Private.h"

#include <mbgl/map/map.hpp>
#include <mbgl/style/sources/custom_vector_source.hpp>
#include <mbgl/util/geojson.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/tile/tile_id.hpp>

@interface MGLComputedShapeSource () {
    std::unique_ptr<mbgl::style::CustomVectorSource> _pendingSource;
}

@property (nonatomic, readwrite) NSDictionary *options;
@property (nonnull) mbgl::style::CustomVectorSource *rawSource;
@property (nonatomic, assign) BOOL dataSourceImplementsFeaturesForTile;
@property (nonatomic, assign) BOOL dataSourceImplementsFeaturesForBounds;

@end

@implementation MGLComputedShapeSource

- (instancetype)initWithIdentifier:(NSString *)identifier options:(NS_DICTIONARY_OF(MGLShapeSourceOption, id) *)options {
    if (self = [super initWithIdentifier:identifier]) {
        _requestQueue = [[NSOperationQueue alloc] init];
        self.requestQueue.name = [NSString stringWithFormat:@"mgl.MGLComputedShapeSource.%@", identifier];
        auto geoJSONOptions = MGLGeoJSONOptionsFromDictionary(options);
        auto source = std::make_unique<mbgl::style::CustomVectorSource>
        (self.identifier.UTF8String, geoJSONOptions,
         ^void(uint8_t z, uint32_t x, uint32_t y)
         {
             [self.requestQueue addOperationWithBlock:
              ^{
                  NSArray<MGLShape <MGLFeature> *> *data;
                  if(!self.dataSource) {
                      data = nil;
                  } else if(self.dataSourceImplementsFeaturesForTile) {
                      data = [self.dataSource featuresInTileAtX:x
                                                              y:y
                                                      zoomLevel:z];
                  } else {
                      mbgl::CanonicalTileID tileID = mbgl::CanonicalTileID(z, x, y);
                      mbgl::LatLngBounds tileBounds = mbgl::LatLngBounds(tileID);
                      data = [self.dataSource featuresInCoordinateBounds:MGLCoordinateBoundsFromLatLngBounds(tileBounds)
                                                               zoomLevel:z];
                  }
                  [self processData:data
                            forTile:z x:x y:y];
              }];
         });
        
        _pendingSource = std::move(source);
        self.rawSource = _pendingSource.get();
        
    }
    return self;
}

- (void)setDataSource:(id<MGLComputedShapeSourceDataSource>)dataSource {
    //Check which method the datasource implements, to avoid having to check for each tile
    self.dataSourceImplementsFeaturesForTile = [dataSource respondsToSelector:@selector(featuresInTileAtX:y:zoomLevel:)];
    self.dataSourceImplementsFeaturesForBounds = [dataSource respondsToSelector:@selector(featuresInCoordinateBounds:zoomLevel:)];
    
    if(!self.dataSourceImplementsFeaturesForBounds && !self.dataSourceImplementsFeaturesForTile) {
        [NSException raise:@"Invalid Datasource" format:@"Datasource does not implement any MGLComputedShapeSourceDataSource methods"];
    } else if(self.dataSourceImplementsFeaturesForBounds && self.dataSourceImplementsFeaturesForTile) {
        [NSException raise:@"Invalid Datasource" format:@"Datasource implements multiple MGLComputedShapeSourceDataSource methods"];
    }

    _dataSource = dataSource;
}

- (void)addToMapView:(MGLMapView *)mapView {
    if (_pendingSource == nullptr) {
        [NSException raise:@"MGLRedundantSourceException"
                    format:@"This instance %@ was already added to %@. Adding the same source instance " \
         "to the style more than once is invalid.", self, mapView.style];
    }
    
    mapView.mbglMap->addSource(std::move(_pendingSource));
}

- (void)removeFromMapView:(MGLMapView *)mapView {
    auto removedSource = mapView.mbglMap->removeSource(self.identifier.UTF8String);
    
    _pendingSource = std::move(reinterpret_cast<std::unique_ptr<mbgl::style::CustomVectorSource> &>(removedSource));
    self.rawSource = _pendingSource.get();
}

- (void)processData:(NS_ARRAY_OF(MGLShape <MGLFeature> *)*)features forTile:(uint8_t)z x:(uint32_t)x y:(uint32_t)y {
    mbgl::FeatureCollection featureCollection;
    featureCollection.reserve(features.count);
    for (MGLShape <MGLFeature> * feature in features) {
        mbgl::Feature geoJsonObject = [feature geoJSONObject].get<mbgl::Feature>();
        featureCollection.push_back(geoJsonObject);
    }
    const auto geojson = mbgl::GeoJSON{featureCollection};
    dispatch_async(dispatch_get_main_queue(), ^{
        self.rawSource->setTileData(z, x, y, geojson);
    });
}

- (void)reloadTileInCoordinateBounds:(MGLCoordinateBounds)bounds zoomLevel:(NSUInteger)zoomLevel {
    self.rawSource->reloadRegion(MGLLatLngBoundsFromCoordinateBounds(bounds), (uint8_t)zoomLevel);
}

- (void)setNeedsUpdateAtZoomLevel:(NSUInteger)z x:(NSUInteger)x y:(NSUInteger)y {
    self.rawSource->updateTile((uint8_t)z, (uint32_t)x, (uint32_t)y);
}

- (void)reloadData {
    self.rawSource->reload();
}

@end
