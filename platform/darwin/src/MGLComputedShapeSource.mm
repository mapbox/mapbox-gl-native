#import "MGLComputedShapeSource.h"

#import "MGLMapView_Private.h"
#import "MGLSource_Private.h"
#import "MGLShape_Private.h"
#import "MGLAbstractShapeSource_Private.h"
#import "MGLGeometry_Private.h"

#include <mbgl/map/map.hpp>
#include <mbgl/style/sources/custom_vector_source.hpp>
#include <mbgl/tile/tile_id.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/geojson.hpp>

@interface MGLComputedShapeSource () {
    std::unique_ptr<mbgl::style::CustomVectorSource> _pendingSource;
}

@property (nonatomic, readwrite) NSDictionary *options;
@property (nonnull) mbgl::style::CustomVectorSource *rawSource;
@property (nonatomic, assign) BOOL dataSourceImplementsFeaturesForTile;
@property (nonatomic, assign) BOOL dataSourceImplementsFeaturesForBounds;

@end

@interface MGLComputedShapeSourceFetchOperation : NSOperation

@property (nonatomic, readonly) uint8_t z;
@property (nonatomic, readonly) uint32_t x;
@property (nonatomic, readonly) uint32_t y;
@property (nonatomic, assign) BOOL dataSourceImplementsFeaturesForTile;
@property (nonatomic, assign) BOOL dataSourceImplementsFeaturesForBounds;
@property (nonatomic, weak, nullable) id<MGLComputedShapeSourceDataSource> dataSource;
@property (nonatomic, nullable) mbgl::style::CustomVectorSource *rawSource;

- (instancetype)initForSource:(MGLComputedShapeSource*)source tile:(const mbgl::CanonicalTileID&)tileId;

@end

@implementation MGLComputedShapeSourceFetchOperation

- (instancetype)initForSource:(MGLComputedShapeSource*)source tile:(const mbgl::CanonicalTileID&)tileID {
    self = [super init];
    _z = tileID.z;
    _x = tileID.x;
    _y = tileID.y;
    _dataSourceImplementsFeaturesForTile = source.dataSourceImplementsFeaturesForTile;
    _dataSourceImplementsFeaturesForBounds = source.dataSourceImplementsFeaturesForBounds;
    _dataSource = source.dataSource;
    mbgl::style::CustomVectorSource *rawSource = (mbgl::style::CustomVectorSource *)source.rawSource;
    _rawSource = rawSource;
    return self;
}

- (void)main {
    if ([self isCancelled]) {
        return;
    }

    NSArray<MGLShape <MGLFeature> *> *data;
    if(!self.dataSource) {
        data = nil;
    } else if(self.dataSourceImplementsFeaturesForTile) {
        data = [self.dataSource featuresInTileAtX:self.x
                                                y:self.y
                                        zoomLevel:self.z];
    } else {
        mbgl::CanonicalTileID tileID = mbgl::CanonicalTileID(self.z, self.x, self.y);
        mbgl::LatLngBounds tileBounds = mbgl::LatLngBounds(tileID);
        data = [self.dataSource featuresInCoordinateBounds:MGLCoordinateBoundsFromLatLngBounds(tileBounds)
                                                 zoomLevel:self.z];
    }

    if(![self isCancelled]) {
        mbgl::FeatureCollection featureCollection;
        featureCollection.reserve(data.count);
        for (MGLShape <MGLFeature> * feature in data) {
            mbgl::Feature geoJsonObject = [feature geoJSONObject].get<mbgl::Feature>();
            featureCollection.push_back(geoJsonObject);
        }
        const auto geojson = mbgl::GeoJSON{featureCollection};
        dispatch_sync(dispatch_get_main_queue(), ^{
            if(![self isCancelled] && self.rawSource) {
                self.rawSource->setTileData(mbgl::CanonicalTileID(self.z, self.x, self.y), geojson);
            }
        });
    }
}

- (void)cancel {
    [super cancel];
    self.rawSource = NULL;
}

@end

@implementation MGLComputedShapeSource

- (instancetype)initWithIdentifier:(NSString *)identifier options:(NS_DICTIONARY_OF(MGLShapeSourceOption, id) *)options {
    if (self = [super initWithIdentifier:identifier]) {
        _requestQueue = [[NSOperationQueue alloc] init];
        self.requestQueue.name = [NSString stringWithFormat:@"mgl.MGLComputedShapeSource.%@", identifier];
        auto geoJSONOptions = MGLGeoJSONOptionsFromDictionary(options);
        auto source = std::make_unique<mbgl::style::CustomVectorSource>
        (self.identifier.UTF8String, geoJSONOptions,
         ^void(const mbgl::CanonicalTileID& tileID)
         {
             NSOperation *operation = [[MGLComputedShapeSourceFetchOperation alloc] initForSource:self tile:tileID];
             [self.requestQueue addOperation:operation];
         });

        _pendingSource = std::move(source);
        self.rawSource = _pendingSource.get();
    }
    return self;
}

- (void)dealloc {
    [self.requestQueue cancelAllOperations];
}

- (void)setDataSource:(id<MGLComputedShapeSourceDataSource>)dataSource {
    [self.requestQueue cancelAllOperations];
    // Check which method the datasource implements, to avoid having to check for each tile
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
    [self.requestQueue cancelAllOperations];
    if (self.rawSource != mapView.mbglMap->getSource(self.identifier.UTF8String)) {
        return;
    }

    auto removedSource = mapView.mbglMap->removeSource(self.identifier.UTF8String);

    mbgl::style::CustomVectorSource *source = dynamic_cast<mbgl::style::CustomVectorSource *>(removedSource.get());
    if (!source) {
        return;
    }

    removedSource.release();

    _pendingSource = std::unique_ptr<mbgl::style::CustomVectorSource>(source);
    self.rawSource = _pendingSource.get();
}

- (void)reloadTileInCoordinateBounds:(MGLCoordinateBounds)bounds zoomLevel:(NSUInteger)zoomLevel {
    self.rawSource->reloadRegion(MGLLatLngBoundsFromCoordinateBounds(bounds), (uint8_t)zoomLevel);
}

- (void)setNeedsUpdateAtZoomLevel:(NSUInteger)z x:(NSUInteger)x y:(NSUInteger)y {
    mbgl::CanonicalTileID tileID = mbgl::CanonicalTileID((uint8_t)z, (uint32_t)x, (uint32_t)y);
    self.rawSource->reloadTile(tileID);
}

- (void)reloadData {
    [self.requestQueue cancelAllOperations];
    self.rawSource->reload();
}

@end
