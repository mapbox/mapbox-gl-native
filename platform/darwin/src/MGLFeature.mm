#import "MGLFeature_Private.h"

#import "MGLPointAnnotation.h"
#import "MGLPolyline.h"
#import "MGLPolygon.h"

#import "MGLMultiPoint_Private.h"

@protocol MGLFeaturePrivate <MGLFeature>

@property (nonatomic, copy, nullable, readwrite) id identifier;
@property (nonatomic, copy, readwrite) NS_DICTIONARY_OF(NSString *, id) *attributes;

@end

@interface MGLPointFeature () <MGLFeaturePrivate>
@end

@implementation MGLPointFeature

@synthesize identifier;
@synthesize attributes;

- (id)attributeForKey:(NSString *)key {
    return self.attributes[key];
}

@end

@interface MGLPolylineFeature () <MGLFeaturePrivate>
@end

@implementation MGLPolylineFeature

@synthesize identifier;
@synthesize attributes;

- (id)attributeForKey:(NSString *)key {
    return self.attributes[key];
}

@end

@interface MGLPolygonFeature () <MGLFeaturePrivate>
@end

@implementation MGLPolygonFeature

@synthesize identifier;
@synthesize attributes;

- (id)attributeForKey:(NSString *)key {
    return self.attributes[key];
}

@end

@interface MGLMultiPointFeature () <MGLFeaturePrivate>
@end

@implementation MGLMultiPointFeature

@synthesize identifier;
@synthesize attributes;

- (id)attributeForKey:(NSString *)key {
    return self.attributes[key];
}

@end

@interface MGLMultiPolylineFeature () <MGLFeaturePrivate>
@end

@implementation MGLMultiPolylineFeature

@synthesize identifier;
@synthesize attributes;

- (id)attributeForKey:(NSString *)key {
    return self.attributes[key];
}

@end

@interface MGLMultiPolygonFeature () <MGLFeaturePrivate>
@end

@implementation MGLMultiPolygonFeature

@synthesize identifier;
@synthesize attributes;

- (id)attributeForKey:(NSString *)key {
    return self.attributes[key];
}

@end

@interface MGLShapeCollectionFeature () <MGLFeaturePrivate>
@end

@implementation MGLShapeCollectionFeature

@synthesize identifier;
@synthesize attributes;

- (id)attributeForKey:(NSString *)key {
    return self.attributes[key];
}

@end

/**
 Recursively transforms a C++ type into the corresponding Foundation type.
 */
class PropertyValueEvaluator {
public:
    id operator()(const std::nullptr_t &) const {
        return [NSNull null];
    }
    
    id operator()(const bool &value) const {
        return value ? @YES : @NO;
    }
    
    id operator()(const uint64_t &value) const {
        return @(value);
    }
    
    id operator()(const int64_t &value) const {
        return @(value);
    }
    
    id operator()(const double &value) const {
        return @(value);
    }
    
    id operator()(const std::string &value) const {
        return @(value.c_str());
    }
    
    id operator()(const std::vector<mbgl::Value> &values) const {
        std::vector<id> objects;
        objects.reserve(values.size());
        for (const auto &v : values) {
            objects.push_back(mbgl::Value::visit(v, *this));
        }
        return [NSArray arrayWithObjects:&objects[0] count:objects.size()];
    }
    
    id operator()(const std::unordered_map<std::string, mbgl::Value> &items) const {
        std::vector<NSString *> keys;
        keys.reserve(items.size());
        std::vector<id> objects;
        objects.reserve(items.size());
        for (auto &item : items) {
            keys.push_back(@(item.first.c_str()));
            objects.push_back(mbgl::Value::visit(item.second, *this));
        }
        return [NSDictionary dictionaryWithObjects:&objects[0] forKeys:&keys[0] count:keys.size()];
    }
};

/**
 Transforms an `mbgl::geometry::geometry` type into an instance of the
 corresponding Objective-C geometry class.
 */
template <typename T>
class GeometryEvaluator {
public:
    MGLShape <MGLFeaturePrivate> * operator()(const mapbox::geometry::point<T> &geometry) const {
        MGLPointFeature *feature = [[MGLPointFeature alloc] init];
        feature.coordinate = coordinateFromPoint(geometry);
        return feature;
    }
    
    MGLShape <MGLFeaturePrivate> * operator()(const mapbox::geometry::line_string<T> &geometry) const {
        std::vector<CLLocationCoordinate2D> coordinates;
        coordinates.reserve(geometry.size());
        std::transform(geometry.begin(), geometry.end(), std::back_inserter(coordinates), coordinateFromPoint);
        
        return [[MGLPolylineFeature alloc] initWithCoordinates:&coordinates[0] count:coordinates.size()];
    }
    
    MGLShape <MGLFeaturePrivate> * operator()(const mapbox::geometry::polygon<T> &geometry) const {
        // TODO: MGLPolygon doesn’t support holes, so what to do?
        auto &linearRing = geometry.front();
        
        std::vector<CLLocationCoordinate2D> coordinates;
        coordinates.reserve(linearRing.size());
        std::transform(linearRing.begin(), linearRing.end(), std::back_inserter(coordinates), coordinateFromPoint);
        
        return [[MGLPolygonFeature alloc] initWithCoordinates:&coordinates[0] count:coordinates.size()];
    }
    
    MGLShape <MGLFeaturePrivate> * operator()(const mapbox::geometry::multi_point<T> &geometry) const {
        std::vector<CLLocationCoordinate2D> coordinates;
        coordinates.reserve(geometry.size());
        std::transform(geometry.begin(), geometry.end(), std::back_inserter(coordinates), coordinateFromPoint);
        
        return [[MGLMultiPointFeature alloc] initWithCoordinates:&coordinates[0] count:coordinates.size()];
    }
    
    MGLShape <MGLFeaturePrivate> * operator()(const mapbox::geometry::multi_line_string<T> &geometry) const {
        NSMutableArray *polylines = [NSMutableArray arrayWithCapacity:geometry.size()];
        for (auto &lineString : geometry) {
            std::vector<CLLocationCoordinate2D> coordinates;
            coordinates.reserve(lineString.size());
            std::transform(lineString.begin(), lineString.end(), std::back_inserter(coordinates), coordinateFromPoint);
            
            MGLPolyline *polyline = [MGLPolyline polylineWithCoordinates:&coordinates[0] count:coordinates.size()];
            [polylines addObject:polyline];
        }
        
        return [MGLMultiPolylineFeature multiPolylineWithPolylines:polylines];
    }
    
    MGLShape <MGLFeaturePrivate> * operator()(const mapbox::geometry::multi_polygon<T> &geometry) const {
        NSMutableArray *polygons = [NSMutableArray arrayWithCapacity:geometry.size()];
        for (auto &polygon : geometry) {
            // TODO: MGLPolygon doesn’t support holes, so what to do?
            auto &linearRing = polygon.front();
            
            std::vector<CLLocationCoordinate2D> coordinates;
            coordinates.reserve(linearRing.size());
            std::transform(linearRing.begin(), linearRing.end(), std::back_inserter(coordinates), coordinateFromPoint);
            
            MGLPolygon *polygonObject = [MGLPolygon polygonWithCoordinates:&coordinates[0] count:coordinates.size()];
            [polygons addObject:polygonObject];
        }
        
        return [MGLMultiPolygonFeature multiPolygonWithPolygons:polygons];
    }
    
    MGLShape <MGLFeaturePrivate> * operator()(const mapbox::geometry::geometry_collection<T> &collection) const {
        NSMutableArray *shapes = [NSMutableArray arrayWithCapacity:collection.size()];
        for (auto &geometry : collection) {
            // This is very much like the transformation that happens in MGLFeaturesFromMBGLFeatures(), but these are raw geometries with no associated feature IDs or attributes.
            GeometryEvaluator<T> evaluator;
            MGLShape <MGLFeaturePrivate> *shape = mapbox::geometry::geometry<T>::visit(geometry, evaluator);
            [shapes addObject:shape];
        }
        return [MGLShapeCollectionFeature shapeCollectionWithShapes:shapes];
    }
    
private:
    static CLLocationCoordinate2D coordinateFromPoint(const mapbox::geometry::point<T> &point) {
        return CLLocationCoordinate2DMake(point.y, point.x);
    }
};

NS_ARRAY_OF(MGLShape <MGLFeature> *) *MGLFeaturesFromMBGLFeatures(const std::vector<mbgl::Feature> &features) {
    std::vector<MGLShape <MGLFeature> *> shapes;
    shapes.reserve(features.size());
    std::transform(features.begin(), features.end(), std::back_inserter(shapes), ^MGLShape <MGLFeature> * (const mbgl::Feature &feature) {
        NSMutableDictionary *attributes = [NSMutableDictionary dictionaryWithCapacity:feature.properties.size()];
        for (auto &pair : feature.properties) {
            auto &value = pair.second;
            PropertyValueEvaluator evaluator;
            attributes[@(pair.first.c_str())] = mbgl::Value::visit(value, evaluator);
        }
        
        GeometryEvaluator<double> evaluator;
        MGLShape <MGLFeaturePrivate> *shape = mapbox::geometry::geometry<double>::visit(feature.geometry, evaluator);
        if (feature.id) {
            shape.identifier = @(*feature.id);
        }
        shape.attributes = attributes;
        return shape;
    });
    return [NSArray arrayWithObjects:&shapes[0] count:shapes.size()];
}
