#import "MGLFoundation_Private.h"
#import "MGLFeature_Private.h"
#import "MGLCluster.h"

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

// Cluster constants
static NSString * const MGLClusterIdentifierKey = @"cluster_id";
static NSString * const MGLClusterCountKey = @"point_count";
const NSUInteger MGLClusterIdentifierInvalid = NSUIntegerMax;

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

@interface MGLPointFeature ()
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

@end

@implementation MGLPointFeatureCluster

- (NSUInteger)clusterIdentifier {
    NSNumber *clusterNumber = MGL_OBJC_DYNAMIC_CAST([self attributeForKey:MGLClusterIdentifierKey], NSNumber);
    MGLAssert(clusterNumber, @"Clusters should have a cluster_id");
    
    if (!clusterNumber) {
        return MGLClusterIdentifierInvalid;
    }
    
    NSUInteger clusterIdentifier = [clusterNumber unsignedIntegerValue];
    MGLAssert(clusterIdentifier <= UINT32_MAX, @"Cluster identifiers are 32bit");
    
    return clusterIdentifier;
}

- (NSUInteger)clusterPointCount {
    NSNumber *count = MGL_OBJC_DYNAMIC_CAST([self attributeForKey:MGLClusterCountKey], NSNumber);
    MGLAssert(count, @"Clusters should have a point_count");
    
    return [count unsignedIntegerValue];
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
        MGLAssert(geoJSONObject.is<mbgl::GeoJSONFeature>(), @"Feature collection must only contain features.");
        featureCollection.push_back(geoJSONObject.get<mbgl::GeoJSONFeature>());
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
private:
    const mbgl::PropertyMap *shared_properties;
    
public:
    GeometryEvaluator(const mbgl::PropertyMap *properties = nullptr):
        shared_properties(properties)
    {}
    
    MGLShape <MGLFeature> * operator()(const mbgl::EmptyGeometry &) const {
        MGLEmptyFeature *feature = [[MGLEmptyFeature alloc] init];
        return feature;
    }

    MGLShape <MGLFeature> * operator()(const mbgl::Point<T> &geometry) const {
        Class pointFeatureClass = [MGLPointFeature class];
        
        // If we're dealing with a cluster, we should change the class type.
        // This could be generic and build the subclass at runtime if it turns
        // out we need to support more than point clusters.
        if (shared_properties) {
            auto clusterIt = shared_properties->find("cluster");
            if (clusterIt != shared_properties->end()) {
                auto clusterValue = clusterIt->second;
                if (clusterValue.template is<bool>()) {
                    if (clusterValue.template get<bool>()) {
                        pointFeatureClass = [MGLPointFeatureCluster class];
                    }
                }
            }
        }
        
        MGLPointFeature *feature = [[pointFeatureClass alloc] init];
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

    MGLShape <MGLFeature> * operator()(const mbgl::GeoJSONFeature &feature) const {
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
        [shapes addObject:MGLFeatureFromMBGLFeature(static_cast<mbgl::GeoJSONFeature>(feature))];
    }
    return shapes;
}

NSArray<MGLShape <MGLFeature> *> *MGLFeaturesFromMBGLFeatures(const std::vector<mbgl::GeoJSONFeature> &features) {
    NSMutableArray *shapes = [NSMutableArray arrayWithCapacity:features.size()];
    for (const auto &feature : features) {
        [shapes addObject:MGLFeatureFromMBGLFeature(feature)];
    }
    return shapes;
}

id <MGLFeature> MGLFeatureFromMBGLFeature(const mbgl::GeoJSONFeature &feature) {
    NSMutableDictionary *attributes = [NSMutableDictionary dictionaryWithCapacity:feature.properties.size()];
    for (auto &pair : feature.properties) {
        auto &value = pair.second;
        ValueEvaluator evaluator;
        attributes[@(pair.first.c_str())] = mbgl::Value::visit(value, evaluator);
    }
    GeometryEvaluator<double> evaluator(&feature.properties);
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

mbgl::GeoJSONFeature mbglFeature(mbgl::GeoJSONFeature feature, id identifier, NSDictionary *attributes)
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
