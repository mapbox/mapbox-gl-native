#import "MGLComputedShapeSource_Private.h"

#import "MGLMapView_Private.h"
#import "MGLSource_Private.h"
#import "MGLShape_Private.h"
#import "MGLGeometry_Private.h"
#import "MGLShapeCollection.h"

#include <mbgl/map/map.hpp>
#include <mbgl/style/sources/custom_geometry_source.hpp>
#include <mbgl/tile/tile_id.hpp>
#include <mbgl/util/geojson.hpp>

const MGLExceptionName MGLInvalidDatasourceException = @"MGLInvalidDatasourceException";

const MGLShapeSourceOption MGLShapeSourceOptionWrapsCoordinates = @"MGLShapeSourceOptionWrapsCoordinates";
const MGLShapeSourceOption MGLShapeSourceOptionClipsCoordinates = @"MGLShapeSourceOptionClipsCoordinates";

mbgl::style::CustomGeometrySource::Options MBGLCustomGeometrySourceOptionsFromDictionary(NSDictionary<MGLShapeSourceOption, id> *options) {
    mbgl::style::CustomGeometrySource::Options sourceOptions;

    if (NSNumber *value = options[MGLShapeSourceOptionMinimumZoomLevel]) {
        if (![value isKindOfClass:[NSNumber class]]) {
            [NSException raise:NSInvalidArgumentException
                        format:@"MGLShapeSourceOptionMinimumZoomLevel must be an NSNumber."];
        }
        sourceOptions.zoomRange.min = value.integerValue;
    }

    if (NSNumber *value = options[MGLShapeSourceOptionMaximumZoomLevel]) {
        if (![value isKindOfClass:[NSNumber class]]) {
            [NSException raise:NSInvalidArgumentException
                        format:@"MGLShapeSourceOptionMaximumZoomLevel must be an NSNumber."];
        }
        sourceOptions.zoomRange.max = value.integerValue;
    }

    if (NSNumber *value = options[MGLShapeSourceOptionBuffer]) {
        if (![value isKindOfClass:[NSNumber class]]) {
            [NSException raise:NSInvalidArgumentException
                        format:@"MGLShapeSourceOptionBuffer must be an NSNumber."];
        }
        sourceOptions.tileOptions.buffer = value.integerValue;
    }

    if (NSNumber *value = options[MGLShapeSourceOptionSimplificationTolerance]) {
        if (![value isKindOfClass:[NSNumber class]]) {
            [NSException raise:NSInvalidArgumentException
                        format:@"MGLShapeSourceOptionSimplificationTolerance must be an NSNumber."];
        }
        sourceOptions.tileOptions.tolerance = value.doubleValue;
    }

    if (NSNumber *value = options[MGLShapeSourceOptionWrapsCoordinates]) {
        if (![value isKindOfClass:[NSNumber class]]) {
            [NSException raise:NSInvalidArgumentException
                        format:@"MGLShapeSourceOptionWrapsCoordinates must be an NSNumber."];
        }
        sourceOptions.tileOptions.wrap = value.boolValue;
    }

    if (NSNumber *value = options[MGLShapeSourceOptionClipsCoordinates]) {
        if (![value isKindOfClass:[NSNumber class]]) {
            [NSException raise:NSInvalidArgumentException
                        format:@"MGLShapeSourceOptionClipsCoordinates must be an NSNumber."];
        }
        sourceOptions.tileOptions.clip = value.boolValue;
    }

    return sourceOptions;
}

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
            if ([feature isMemberOfClass:[MGLShapeCollection class]]) {
                static dispatch_once_t onceToken;
                dispatch_once(&onceToken, ^{
                    NSLog(@"MGLShapeCollection initialized with MGLFeatures will not retain attributes."
                          @"Use MGLShapeCollectionFeature to retain attributes instead."
                          @"This will be logged only once.");
                });
            }
            mbgl::GeoJSONFeature geoJsonObject = [feature geoJSONObject].get<mbgl::GeoJSONFeature>();
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

- (instancetype)initWithIdentifier:(NSString *)identifier options:(NSDictionary<MGLShapeSourceOption, id> *)options {
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

- (instancetype)initWithIdentifier:(NSString *)identifier dataSource:(id<MGLComputedShapeSourceDataSource>)dataSource options:(NSDictionary<MGLShapeSourceOption, id> *)options {
    if (self = [self initWithIdentifier:identifier options:options]) {
        [self setDataSource:dataSource];
    }
    return self;
}

- (void)dealloc {
    [self.requestQueue cancelAllOperations];
}

- (void)setFeatures:(NSArray<MGLShape <MGLFeature> *>*)features inTileAtX:(NSUInteger)x y:(NSUInteger)y zoomLevel:(NSUInteger)zoomLevel {
    MGLAssertStyleSourceIsValid();
    mbgl::CanonicalTileID tileID = mbgl::CanonicalTileID((uint8_t)zoomLevel, (uint32_t)x, (uint32_t)y);
    mbgl::FeatureCollection featureCollection;
    featureCollection.reserve(features.count);
    for (MGLShape <MGLFeature> * feature in features) {
        mbgl::GeoJSONFeature geoJsonObject = [feature geoJSONObject].get<mbgl::GeoJSONFeature>();
        featureCollection.push_back(geoJsonObject);
        if ([feature isMemberOfClass:[MGLShapeCollection class]]) {
            static dispatch_once_t onceToken;
            dispatch_once(&onceToken, ^{
                NSLog(@"MGLShapeCollection initialized with MGLFeatures will not retain attributes."
                      @"Use MGLShapeCollectionFeature to retain attributes instead."
                      @"This will be logged only once.");
            });
        }
    }
    const auto geojson = mbgl::GeoJSON{featureCollection};
    static_cast<mbgl::style::CustomGeometrySource *>(self.rawSource)->setTileData(tileID, geojson);
}

- (void)setDataSource:(id<MGLComputedShapeSourceDataSource>)dataSource {
    [self.requestQueue cancelAllOperations];
    // Check which method the datasource implements, to avoid having to check for each tile
    self.dataSourceImplementsFeaturesForTile = [dataSource respondsToSelector:@selector(featuresInTileAtX:y:zoomLevel:)];
    self.dataSourceImplementsFeaturesForBounds = [dataSource respondsToSelector:@selector(featuresInCoordinateBounds:zoomLevel:)];

    if (!self.dataSourceImplementsFeaturesForBounds && !self.dataSourceImplementsFeaturesForTile) {
        [NSException raise:MGLInvalidDatasourceException
                    format:@"Datasource does not implement any MGLComputedShapeSourceDataSource methods"];
    } else if (self.dataSourceImplementsFeaturesForBounds && self.dataSourceImplementsFeaturesForTile) {
        [NSException raise:MGLInvalidDatasourceException
                    format:@"Datasource implements multiple MGLComputedShapeSourceDataSource methods"];
    }

    _dataSource = dataSource;
}

- (void) invalidateBounds:(MGLCoordinateBounds)bounds {
    MGLAssertStyleSourceIsValid();
    ((mbgl::style::CustomGeometrySource *)self.rawSource)->invalidateRegion(MGLLatLngBoundsFromCoordinateBounds(bounds));
}

- (void) invalidateTileAtX:(NSUInteger)x y:(NSUInteger)y zoomLevel:(NSUInteger)z {
    MGLAssertStyleSourceIsValid();
    ((mbgl::style::CustomGeometrySource *)self.rawSource)->invalidateTile(mbgl::CanonicalTileID(z, (unsigned int)x, (unsigned int)y));
}

@end
