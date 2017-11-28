#import "MGLComputedShapeSource.h"

#import "MGLMapView_Private.h"
#import "MGLSource_Private.h"
#import "MGLShape_Private.h"
#import "MGLAbstractShapeSource_Private.h"
#import "MGLGeometry_Private.h"

#include <mbgl/map/map.hpp>
#include <mbgl/style/sources/custom_geometry_source.hpp>
#include <mbgl/tile/tile_id.hpp>
#include <mbgl/util/geojson.hpp>

@interface MGLComputedShapeSource () {
    std::unique_ptr<mbgl::style::CustomGeometrySource> _pendingSource;
}

@property (nonatomic, readwrite) NSDictionary *options;
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
@property (nonatomic, nullable) mbgl::style::CustomGeometrySource *rawSource;

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
    mbgl::style::CustomGeometrySource *rawSource = static_cast<mbgl::style::CustomGeometrySource *>(source.rawSource);
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
        if(![self isCancelled] && self.rawSource) {
            self.rawSource->setTileData(mbgl::CanonicalTileID(self.z, self.x, self.y), geojson);
        }
    }
}

- (void)cancel {
    [super cancel];
    self.rawSource = NULL;
}

@end

@implementation MGLComputedShapeSource

- (instancetype)initWithIdentifier:(NSString *)identifier options:(NS_DICTIONARY_OF(MGLShapeSourceOption, id) *)options {
    NSOperationQueue *requestQueue = [[NSOperationQueue alloc] init];
    requestQueue.name = [NSString stringWithFormat:@"mgl.MGLComputedShapeSource.%@", identifier];
    requestQueue.qualityOfService = NSQualityOfServiceUtility;
    requestQueue.maxConcurrentOperationCount = 4;

    auto sourceOptions  = MBGLCustomGeometrySourceOptionsFromDictionary(options);
    sourceOptions.fetchTileFunction = ^void(const mbgl::CanonicalTileID& tileID) {
        NSOperation *operation = [[MGLComputedShapeSourceFetchOperation alloc] initForSource:self tile:tileID];
        [requestQueue addOperation:operation];
    };
    
    sourceOptions.cancelTileFunction = ^void(const mbgl::CanonicalTileID& tileID) {
        for (MGLComputedShapeSourceFetchOperation *operation in requestQueue.operations) {
            if (operation.x == tileID.x && operation.y == tileID.y && operation.z == tileID.z) {
                [operation cancel];
            }
        }
    };

    auto source = std::make_unique<mbgl::style::CustomGeometrySource>(identifier.UTF8String, sourceOptions);

    if (self = [super initWithPendingSource:std::move(source)]) {
        _requestQueue = requestQueue;
    }
    return self;
}

- (instancetype)initWithIdentifier:(NSString *)identifier dataSource:(id<MGLComputedShapeSourceDataSource>)dataSource options:(NS_DICTIONARY_OF(MGLShapeSourceOption, id) *)options {
    if (self = [self initWithIdentifier:identifier options:options]) {
        [self setDataSource:dataSource];
    }
    return self;
}

- (void)dealloc {
    [self.requestQueue cancelAllOperations];
}

- (void)setFeatures:(NSArray<MGLShape <MGLFeature> *>*)features inTileAtX:(NSUInteger)x y:(NSUInteger)y zoomLevel:(NSUInteger)zoomLevel {
    mbgl::CanonicalTileID tileID = mbgl::CanonicalTileID((uint8_t)zoomLevel, (uint32_t)x, (uint32_t)y);
    mbgl::FeatureCollection featureCollection;
    featureCollection.reserve(features.count);
    for (MGLShape <MGLFeature> * feature in features) {
        mbgl::Feature geoJsonObject = [feature geoJSONObject].get<mbgl::Feature>();
        featureCollection.push_back(geoJsonObject);
    }
    const auto geojson = mbgl::GeoJSON{featureCollection};
    static_cast<mbgl::style::CustomGeometrySource *>(self.rawSource)->setTileData(tileID, geojson);
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

- (void) invalidateBounds:(MGLCoordinateBounds)bounds {
    ((mbgl::style::CustomGeometrySource *)self.rawSource)->invalidateRegion(MGLLatLngBoundsFromCoordinateBounds(bounds));
}

- (void) invalidateTileAtX:(NSUInteger)x y:(NSUInteger)y zoomLevel:(NSUInteger)z {
    ((mbgl::style::CustomGeometrySource *)self.rawSource)->invalidateTile(mbgl::CanonicalTileID(z, (unsigned int)x, (unsigned int)y));
}

@end
