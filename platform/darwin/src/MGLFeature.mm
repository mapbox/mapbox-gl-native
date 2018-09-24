#import "MGLFeature_Private.h"

#import "MGLPointAnnotation.h"
#import "MGLPolyline.h"
#import "MGLPolygon.h"
#import "MGLValueEvaluator.h"

#import "MGLShape_Private.h"
#import "MGLPointCollection_Private.h"
#import "MGLPolyline_Private.h"
#import "MGLPolygon_Private.h"

#import "NSDictionary+MGLAdditions.h"
#import "NSArray+MGLAdditions.h"
#import "NSExpression+MGLPrivateAdditions.h"
#import "MGLLoggingConfiguration_Private.h"

#import <mbgl/util/geometry.hpp>
#import <mbgl/style/conversion/geojson.hpp>
#import <mapbox/feature.hpp>

@interface MGLEmptyFeature ()
@end

@implementation MGLEmptyFeature

@synthesize identifier;
@synthesize attributes = _attributes;

MGL_DEFINE_FEATURE_INIT_WITH_CODER();
MGL_DEFINE_FEATURE_ENCODE();
MGL_DEFINE_FEATURE_IS_EQUAL();
MGL_DEFINE_FEATURE_ATTRIBUTES_GETTER();

- (id)attributeForKey:(NSString *)key {
    MGLLogDebug(@"Retrieving attributeForKey: %@", key);
    return self.attributes[key];
}

- (NSDictionary *)geoJSONDictionary {
    return NSDictionaryFeatureForGeometry([super geoJSONDictionary], self.attributes, self.identifier);
}

- (mbgl::GeoJSON)geoJSONObject {
    return mbglFeature({[self geometryObject]}, identifier, self.attributes);
}

- (NSString *)description
{
    return [NSString stringWithFormat:@"<%@: %p; identifier = %@, attributes = %@>",
            NSStringFromClass([self class]), (void *)self,
            self.identifier ? [NSString stringWithFormat:@"\"%@\"", self.identifier] : self.identifier,
            self.attributes.count ? self.attributes : @"none"];
}

@end

NSString * const MGLPointClusterBoolKey              = @"cluster";
NSString * const MGLPointClusterIdKey                = @"cluster_id";
NSString * const MGLPointClusterCountKey             = @"point_count";
NSString * const MGLPointClusterCountAbbreviationKey = @"cluster_abbreviated";

// TODO: privately conform to MGLPointCluster, so we can return the same object OR create a *new*
// object, or even have a subclass of MGLPointFeature?
@interface MGLPointFeature () <MGLPointCluster>
@end

@implementation MGLPointFeature

@synthesize identifier;
@synthesize attributes = _attributes;

MGL_DEFINE_FEATURE_INIT_WITH_CODER();
MGL_DEFINE_FEATURE_ENCODE();
MGL_DEFINE_FEATURE_IS_EQUAL();
MGL_DEFINE_FEATURE_ATTRIBUTES_GETTER();

- (id)attributeForKey:(NSString *)key {
    MGLLogDebug(@"Retrieving attributeForKey: %@", key);
    return self.attributes[key];
}

- (NSDictionary *)geoJSONDictionary {
    return NSDictionaryFeatureForGeometry([super geoJSONDictionary], self.attributes, self.identifier);
}

- (mbgl::GeoJSON)geoJSONObject {
    return mbglFeature({[self geometryObject]}, identifier, self.attributes);
}

- (NSString *)description
{
    return [NSString stringWithFormat:@"<%@: %p; identifier = %@, coordinate = %f, %f, attributes = %@>",
            NSStringFromClass([self class]), (void *)self,
            self.identifier ? [NSString stringWithFormat:@"\"%@\"", self.identifier] : self.identifier,
            self.coordinate.latitude, self.coordinate.longitude,
            self.attributes.count ? self.attributes : @"none"];
}

- (id<MGLPointCluster>)cluster {
    return self.cluster ? self : nil;
}

#pragma mark - MGLPointCluster

- (BOOL)isCluster {
    NSNumber *isCluster = MGL_OBJC_DYNAMIC_CAST(self.attributes[MGLPointClusterBoolKey], NSNumber);
    return [isCluster boolValue];
}

- (uint32_t)clusterId {
    NSNumber *clusterNumber = MGL_OBJC_DYNAMIC_CAST(self.attributes[MGLPointClusterIdKey], NSNumber);
    
    NSAssert(clusterNumber, @"Clusters should have a cluster_id");
    if (!clusterNumber) {
        return -1;
    }

    NSUInteger clusterId = [clusterNumber unsignedIntegerValue];

    NSAssert(clusterId < (1UL << 32), @"Cluster IDs are 32bit");
    NSAssert([self.identifier isKindOfClass:[NSNumber class]], @"The identifier should be an NSNumber");
    NSAssert(clusterId == [self.identifier unsignedIntegerValue], @"The cluster id should match the feature's identifier.");
    
    return (uint32_t)clusterId;
}

- (NSUInteger)pointCount {
    NSNumber *pointCount = MGL_OBJC_DYNAMIC_CAST(self.attributes[MGLPointClusterCountKey], NSNumber);
    
    NSAssert(pointCount, @"Clusters should have a point_count");
    if (!pointCount) {
        return -1;
    }

    return [pointCount unsignedIntegerValue];
}

- (NSString*)clusterAbbreviated {
    NSString *abbreviation = MGL_OBJC_DYNAMIC_CAST(self.attributes[MGLPointClusterCountAbbreviationKey], NSString);

    NSAssert(abbreviation, @"Clusters should have a cluster_abbreviated");
    if (!abbreviation) {
        return @"<invalid>";
    }

    return abbreviation;
}

@end

@interface MGLPolylineFeature ()
@end

@implementation MGLPolylineFeature

@synthesize identifier;
@synthesize attributes = _attributes;

MGL_DEFINE_FEATURE_INIT_WITH_CODER();
MGL_DEFINE_FEATURE_ENCODE();
MGL_DEFINE_FEATURE_IS_EQUAL();
MGL_DEFINE_FEATURE_ATTRIBUTES_GETTER();

- (id)attributeForKey:(NSString *)key {
    MGLLogDebug(@"Retrieving attributeForKey: %@", key);
    return self.attributes[key];
}

- (NSDictionary *)geoJSONDictionary {
    return NSDictionaryFeatureForGeometry([super geoJSONDictionary], self.attributes, self.identifier);
}

- (mbgl::GeoJSON)geoJSONObject {
    return mbglFeature({[self geometryObject]}, identifier, self.attributes);
}

- (NSString *)description
{
    return [NSString stringWithFormat:@"<%@: %p; identifier = %@, count = %lu, bounds = %@, attributes = %@>",
            NSStringFromClass([self class]), (void *)self,
            self.identifier ? [NSString stringWithFormat:@"\"%@\"", self.identifier] : self.identifier,
            (unsigned long)[self pointCount],
            MGLStringFromCoordinateBounds(self.overlayBounds),
            self.attributes.count ? self.attributes : @"none"];
}

@end

@interface MGLPolygonFeature ()
@end

@implementation MGLPolygonFeature

@synthesize identifier;
@synthesize attributes = _attributes;

MGL_DEFINE_FEATURE_INIT_WITH_CODER();
MGL_DEFINE_FEATURE_ENCODE();
MGL_DEFINE_FEATURE_IS_EQUAL();
MGL_DEFINE_FEATURE_ATTRIBUTES_GETTER();

- (id)attributeForKey:(NSString *)key {
    MGLLogDebug(@"Retrieving attributeForKey: %@", key);
    return self.attributes[key];
}

- (NSDictionary *)geoJSONDictionary {
    return NSDictionaryFeatureForGeometry([super geoJSONDictionary], self.attributes, self.identifier);
}

- (mbgl::GeoJSON)geoJSONObject {
    return mbglFeature({[self geometryObject]}, identifier, self.attributes);
}

- (NSString *)description
{
    return [NSString stringWithFormat:@"<%@: %p; identifier = %@, count = %lu, bounds = %@, attributes = %@>",
            NSStringFromClass([self class]), (void *)self,
            self.identifier ? [NSString stringWithFormat:@"\"%@\"", self.identifier] : self.identifier,
            (unsigned long)[self pointCount],
            MGLStringFromCoordinateBounds(self.overlayBounds),
            self.attributes.count ? self.attributes : @"none"];
}

@end

@interface MGLPointCollectionFeature ()
@end

@implementation MGLPointCollectionFeature

@synthesize identifier;
@synthesize attributes = _attributes;

MGL_DEFINE_FEATURE_INIT_WITH_CODER();
MGL_DEFINE_FEATURE_ENCODE();
MGL_DEFINE_FEATURE_IS_EQUAL();
MGL_DEFINE_FEATURE_ATTRIBUTES_GETTER();

- (id)attributeForKey:(NSString *)key {
    MGLLogDebug(@"Retrieving attributeForKey: %@", key);
    return self.attributes[key];
}

- (NSDictionary *)geoJSONDictionary {
    return NSDictionaryFeatureForGeometry([super geoJSONDictionary], self.attributes, self.identifier);
}

- (mbgl::GeoJSON)geoJSONObject {
    return mbglFeature({[self geometryObject]}, identifier, self.attributes);
}

@end

@interface MGLMultiPolylineFeature ()
@end

@implementation MGLMultiPolylineFeature

@synthesize identifier;
@synthesize attributes = _attributes;

MGL_DEFINE_FEATURE_INIT_WITH_CODER();
MGL_DEFINE_FEATURE_ENCODE();
MGL_DEFINE_FEATURE_IS_EQUAL();
MGL_DEFINE_FEATURE_ATTRIBUTES_GETTER();

- (id)attributeForKey:(NSString *)key {
    MGLLogDebug(@"Retrieving attributeForKey: %@", key);
    return self.attributes[key];
}

- (NSDictionary *)geoJSONDictionary {
    return NSDictionaryFeatureForGeometry([super geoJSONDictionary], self.attributes, self.identifier);
}

- (mbgl::GeoJSON)geoJSONObject {
    return mbglFeature({[self geometryObject]}, identifier, self.attributes);
}

- (NSString *)description
{
    return [NSString stringWithFormat:@"<%@: %p; identifier = %@, count = %lu, bounds = %@, attributes = %@>",
            NSStringFromClass([self class]), (void *)self,
            self.identifier ? [NSString stringWithFormat:@"\"%@\"", self.identifier] : self.identifier,
            (unsigned long)self.polylines.count,
            MGLStringFromCoordinateBounds(self.overlayBounds),
            self.attributes.count ? self.attributes : @"none"];
}

@end

@interface MGLMultiPolygonFeature ()
@end

@implementation MGLMultiPolygonFeature

@synthesize identifier;
@synthesize attributes = _attributes;

MGL_DEFINE_FEATURE_INIT_WITH_CODER();
MGL_DEFINE_FEATURE_ENCODE();
MGL_DEFINE_FEATURE_IS_EQUAL();
MGL_DEFINE_FEATURE_ATTRIBUTES_GETTER();

- (id)attributeForKey:(NSString *)key {
    MGLLogDebug(@"Retrieving attributeForKey: %@", key);
    return self.attributes[key];
}

- (NSDictionary *)geoJSONDictionary {
    return NSDictionaryFeatureForGeometry([super geoJSONDictionary], self.attributes, self.identifier);
}

- (mbgl::GeoJSON)geoJSONObject {
    return mbglFeature({[self geometryObject]}, identifier, self.attributes);
}

- (NSString *)description
{
    return [NSString stringWithFormat:@"<%@: %p; identifier = %@, count = %lu, bounds = %@, attributes = %@>",
            NSStringFromClass([self class]), (void *)self,
            self.identifier ? [NSString stringWithFormat:@"\"%@\"", self.identifier] : self.identifier,
            (unsigned long)self.polygons.count,
            MGLStringFromCoordinateBounds(self.overlayBounds),
            self.attributes.count ? self.attributes : @"none"];
}

@end

@interface MGLShapeCollectionFeature ()
@end

@implementation MGLShapeCollectionFeature

@synthesize identifier;
@synthesize attributes = _attributes;

@dynamic shapes;

+ (instancetype)shapeCollectionWithShapes:(NSArray<MGLShape<MGLFeature> *> *)shapes {
    return [super shapeCollectionWithShapes:shapes];
}

MGL_DEFINE_FEATURE_INIT_WITH_CODER();
MGL_DEFINE_FEATURE_ENCODE();
MGL_DEFINE_FEATURE_IS_EQUAL();
MGL_DEFINE_FEATURE_ATTRIBUTES_GETTER();

- (id)attributeForKey:(NSString *)key {
    return self.attributes[key];
}

- (NSDictionary *)geoJSONDictionary {
    return NSDictionaryFeatureForGeometry([super geoJSONDictionary], self.attributes, self.identifier);
}

- (mbgl::GeoJSON)geoJSONObject {
    mbgl::FeatureCollection featureCollection;
    featureCollection.reserve(self.shapes.count);
    for (MGLShape <MGLFeature> *feature in self.shapes) {
        auto geoJSONObject = feature.geoJSONObject;
        MGLAssert(geoJSONObject.is<mbgl::Feature>(), @"Feature collection must only contain features.");
        featureCollection.push_back(geoJSONObject.get<mbgl::Feature>());
    }
    return featureCollection;
}

@end

/**
 Transforms an `mbgl::geometry::geometry` type into an instance of the
 corresponding Objective-C geometry class.
 */
template <typename T>
class GeometryEvaluator {
public:
    MGLShape <MGLFeature> * operator()(const mbgl::EmptyGeometry &) const {
        MGLEmptyFeature *feature = [[MGLEmptyFeature alloc] init];
        return feature;
    }

    MGLShape <MGLFeature> * operator()(const mbgl::Point<T> &geometry) const {
        MGLPointFeature *feature = [[MGLPointFeature alloc] init];
        feature.coordinate = toLocationCoordinate2D(geometry);
        return feature;
    }

    MGLShape <MGLFeature> * operator()(const mbgl::LineString<T> &geometry) const {
        std::vector<CLLocationCoordinate2D> coordinates = toLocationCoordinates2D(geometry);
        return [MGLPolylineFeature polylineWithCoordinates:&coordinates[0] count:coordinates.size()];
    }

    MGLShape <MGLFeature> * operator()(const mbgl::Polygon<T> &geometry) const {
        return toShape<MGLPolygonFeature>(geometry);
    }

    MGLShape <MGLFeature> * operator()(const mbgl::MultiPoint<T> &geometry) const {
        std::vector<CLLocationCoordinate2D> coordinates = toLocationCoordinates2D(geometry);
        return [[MGLPointCollectionFeature alloc] initWithCoordinates:&coordinates[0] count:coordinates.size()];
    }

    MGLShape <MGLFeature> * operator()(const mbgl::MultiLineString<T> &geometry) const {
        NSMutableArray *polylines = [NSMutableArray arrayWithCapacity:geometry.size()];
        for (auto &lineString : geometry) {
            std::vector<CLLocationCoordinate2D> coordinates = toLocationCoordinates2D(lineString);
            MGLPolyline *polyline = [MGLPolyline polylineWithCoordinates:&coordinates[0] count:coordinates.size()];
            [polylines addObject:polyline];
        }

        return [MGLMultiPolylineFeature multiPolylineWithPolylines:polylines];
    }

    MGLShape <MGLFeature> * operator()(const mbgl::MultiPolygon<T> &geometry) const {
        NSMutableArray *polygons = [NSMutableArray arrayWithCapacity:geometry.size()];
        for (auto &polygon : geometry) {
            [polygons addObject:toShape(polygon)];
        }

        return [MGLMultiPolygonFeature multiPolygonWithPolygons:polygons];
    }

    MGLShape <MGLFeature> * operator()(const mapbox::geometry::geometry_collection<T> &collection) const {
        NSMutableArray *shapes = [NSMutableArray arrayWithCapacity:collection.size()];
        for (auto &geometry : collection) {
            // This is very much like the transformation that happens in MGLFeaturesFromMBGLFeatures(), but these are raw geometries with no associated feature IDs or attributes.
            MGLShape <MGLFeature> *shape = mapbox::geometry::geometry<T>::visit(geometry, *this);
            [shapes addObject:shape];
        }
        return [MGLShapeCollectionFeature shapeCollectionWithShapes:shapes];
    }

private:
    static CLLocationCoordinate2D toLocationCoordinate2D(const mbgl::Point<T> &point) {
        return CLLocationCoordinate2DMake(point.y, point.x);
    }

    static std::vector<CLLocationCoordinate2D> toLocationCoordinates2D(const std::vector<mbgl::Point<T>> &points) {
        std::vector<CLLocationCoordinate2D> coordinates;
        coordinates.reserve(points.size());
        std::transform(points.begin(), points.end(), std::back_inserter(coordinates), toLocationCoordinate2D);
        return coordinates;
    }

    template<typename U = MGLPolygon>
    static U *toShape(const mbgl::Polygon<T> &geometry) {
        auto &linearRing = geometry.front();
        std::vector<CLLocationCoordinate2D> coordinates = toLocationCoordinates2D(linearRing);
        NSMutableArray *innerPolygons;
        if (geometry.size() > 1) {
            innerPolygons = [NSMutableArray arrayWithCapacity:geometry.size() - 1];
            for (auto iter = geometry.begin() + 1; iter != geometry.end(); iter++) {
                auto &innerRing = *iter;
                std::vector<CLLocationCoordinate2D> innerCoordinates = toLocationCoordinates2D(innerRing);
                MGLPolygon *innerPolygon = [MGLPolygon polygonWithCoordinates:&innerCoordinates[0] count:innerCoordinates.size()];
                [innerPolygons addObject:innerPolygon];
            }
        }

        return [U polygonWithCoordinates:&coordinates[0] count:coordinates.size() interiorPolygons:innerPolygons];
    }
};

template <typename T>
class GeoJSONEvaluator {
public:
    MGLShape <MGLFeature> * operator()(const mbgl::Geometry<T> &geometry) const {
        GeometryEvaluator<T> evaluator;
        MGLShape <MGLFeature> *shape = mapbox::geometry::geometry<T>::visit(geometry, evaluator);
        return shape;
    }

    MGLShape <MGLFeature> * operator()(const mbgl::Feature &feature) const {
        MGLShape <MGLFeature> *shape = (MGLShape <MGLFeature> *)MGLFeatureFromMBGLFeature(feature);
        return shape;
    }

    MGLShape <MGLFeature> * operator()(const mbgl::FeatureCollection &collection) const {
        NSMutableArray *shapes = [NSMutableArray arrayWithCapacity:collection.size()];
        for (const auto &feature : collection) {
            [shapes addObject:MGLFeatureFromMBGLFeature(feature)];
        }
        return [MGLShapeCollectionFeature shapeCollectionWithShapes:shapes];
    }
};

NSArray<MGLShape <MGLFeature> *> *MGLFeaturesFromMBGLFeatures(const std::vector<mbgl::Feature> &features) {
    NSMutableArray *shapes = [NSMutableArray arrayWithCapacity:features.size()];
    for (const auto &feature : features) {
        [shapes addObject:MGLFeatureFromMBGLFeature(feature)];
    }
    return shapes;
}

id <MGLFeature> MGLFeatureFromMBGLFeature(const mbgl::Feature &feature) {
    NSMutableDictionary *attributes = [NSMutableDictionary dictionaryWithCapacity:feature.properties.size()];
    for (auto &pair : feature.properties) {
        auto &value = pair.second;
        ValueEvaluator evaluator;
        attributes[@(pair.first.c_str())] = mbgl::Value::visit(value, evaluator);
    }
    GeometryEvaluator<double> evaluator;
    MGLShape <MGLFeature> *shape = mapbox::geometry::geometry<double>::visit(feature.geometry, evaluator);
    if (!feature.id.is<mapbox::feature::null_value_t>()) {
        shape.identifier = mbgl::FeatureIdentifier::visit(feature.id, ValueEvaluator());
    }
    shape.attributes = attributes;

    return shape;
}

MGLShape* MGLShapeFromGeoJSON(const mapbox::geojson::geojson &geojson) {
    GeoJSONEvaluator<double> evaluator;
    MGLShape *shape = mapbox::geojson::geojson::visit(geojson, evaluator);
    return shape;
}

mbgl::Feature mbglFeature(mbgl::Feature feature, id identifier, NSDictionary *attributes)
{
    if (identifier) {
        NSExpression *identifierExpression = [NSExpression expressionForConstantValue:identifier];
        feature.id = [identifierExpression mgl_featureIdentifier];
    }
    feature.properties = [attributes mgl_propertyMap];
    return feature;
}

NSDictionary<NSString *, id> *NSDictionaryFeatureForGeometry(NSDictionary *geometry, NSDictionary *attributes, id identifier) {
    NSMutableDictionary *feature = [@{@"type": @"Feature",
                                      @"properties": attributes,
                                      @"geometry": geometry} mutableCopy];
    feature[@"id"] = identifier;
    return [feature copy];
}
