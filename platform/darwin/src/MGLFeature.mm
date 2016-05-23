#import "MGLFeature_Private.h"

#import "MGLPointAnnotation.h"
#import "MGLPolyline.h"
#import "MGLPolygon.h"

#import "MGLMultiPoint_Private.h"

class PropertyValueEvaluator {
public:
    PropertyValueEvaluator() {}
    
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
        std::transform(values.begin(), values.end(), std::back_inserter(objects), ^id (const mbgl::Value &value) {
            PropertyValueEvaluator evaluator;
            return mbgl::Value::visit(value, evaluator);
        });
        return [NSArray arrayWithObjects:&objects[0] count:objects.size()];
    }
    
    id operator()(const std::unordered_map<std::string, mbgl::Value> &items) const {
        std::vector<NSString *> keys;
        keys.reserve(items.size());
        std::vector<id> objects;
        objects.reserve(items.size());
        for (auto &item : items) {
            keys.push_back(@(item.first.c_str()));
            PropertyValueEvaluator evaluator;
            objects.push_back(mbgl::Value::visit(item.second, evaluator));
        }
        return [NSDictionary dictionaryWithObjects:&objects[0] forKeys:&keys[0] count:keys.size()];
    }
};

template <typename T>
class GeometryEvaluator {
public:
    GeometryEvaluator()
    : attributes([NSMutableDictionary dictionary]) {}
    
    GeometryEvaluator(const mbgl::Feature &feature)
    : tag(feature.id ? @(*feature.id) : nullptr),
    attributes([NSMutableDictionary dictionaryWithCapacity:feature.properties.size()]) {
        for (auto &pair : feature.properties) {
            auto &value = pair.second;
            PropertyValueEvaluator evaluator;
            attributes[@(pair.first.c_str())] = mbgl::Value::visit(value, evaluator);
        }
    }
    
    MGLShape <MGLFeature> * operator()(const mapbox::geometry::point<T> &geometry) const {
        MGLPointFeature *feature = [[MGLPointFeature alloc] init];
        feature.coordinate = coordinateFromPoint(geometry);
        feature.identifier = tag;
        feature.attributes = attributes;
        return feature;
    }
    
    MGLShape <MGLFeature> * operator()(const mapbox::geometry::line_string<T> &geometry) const {
        std::vector<CLLocationCoordinate2D> coordinates;
        coordinates.reserve(geometry.size());
        std::transform(geometry.begin(), geometry.end(), std::back_inserter(coordinates), coordinateFromPoint);
        
        MGLPolylineFeature *feature = [[MGLPolylineFeature alloc] initWithCoordinates:&coordinates[0] count:coordinates.size()];
        feature.identifier = tag;
        feature.attributes = attributes;
        return feature;
    }
    
    MGLShape <MGLFeature> * operator()(const mapbox::geometry::polygon<T> &geometry) const {
        // TODO: MGLPolygon doesn’t support holes, so what to do?
        auto &linearRing = geometry.front();
        
        std::vector<CLLocationCoordinate2D> coordinates;
        coordinates.reserve(linearRing.size());
        std::transform(linearRing.begin(), linearRing.end(), std::back_inserter(coordinates), coordinateFromPoint);
        
        MGLPolygonFeature *feature = [[MGLPolygonFeature alloc] initWithCoordinates:&coordinates[0] count:coordinates.size()];
        feature.identifier = tag;
        feature.attributes = attributes;
        return feature;
    }
    
    MGLShape <MGLFeature> * operator()(const mapbox::geometry::multi_point<T> &geometry) const {
        std::vector<CLLocationCoordinate2D> coordinates;
        coordinates.reserve(geometry.size());
        std::transform(geometry.begin(), geometry.end(), std::back_inserter(coordinates), coordinateFromPoint);
        
        MGLMultiPointFeature *feature = [[MGLMultiPointFeature alloc] initWithCoordinates:&coordinates[0] count:coordinates.size()];
        feature.identifier = tag;
        feature.attributes = attributes;
        return feature;
    }
    
    MGLShape <MGLFeature> * operator()(const mapbox::geometry::multi_line_string<T> &geometry) const {
        NSMutableArray *polylines = [NSMutableArray arrayWithCapacity:geometry.size()];
        for (auto &lineString : geometry) {
            std::vector<CLLocationCoordinate2D> coordinates;
            coordinates.reserve(lineString.size());
            std::transform(lineString.begin(), lineString.end(), std::back_inserter(coordinates), coordinateFromPoint);
            
            MGLPolyline *polyline = [MGLPolyline polylineWithCoordinates:&coordinates[0] count:coordinates.size()];
            [polylines addObject:polyline];
        }
        
        MGLMultiPolylineFeature *feature = [MGLMultiPolylineFeature multiPolylineWithPolylines:polylines];
        feature.identifier = tag;
        feature.attributes = attributes;
        return feature;
    }
    
    MGLShape <MGLFeature> * operator()(const mapbox::geometry::multi_polygon<T> &geometry) const {
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
        
        MGLMultiPolygonFeature *feature = [MGLMultiPolygonFeature multiPolygonWithPolygons:polygons];
        feature.identifier = tag;
        feature.attributes = attributes;
        return feature;
    }
    
    MGLShape <MGLFeature> * operator()(const mapbox::geometry::geometry_collection<T> &collection) const {
        NSMutableArray *shapes = [NSMutableArray arrayWithCapacity:collection.size()];
        for (auto &geometry : collection) {
            GeometryEvaluator<T> evaluator;
            id <MGLFeature> feature = mapbox::geometry::geometry<T>::visit(geometry, evaluator);
            [shapes addObject:feature];
        }
        MGLShapeCollectionFeature *feature = [MGLShapeCollectionFeature shapeCollectionWithShapes:shapes];
        feature.identifier = tag;
        feature.attributes = attributes;
        return feature;
    }
    
private:
    NSNumber *tag = nullptr;
    NS_MUTABLE_DICTIONARY_OF(NSString *, id) *attributes;
    
    static CLLocationCoordinate2D coordinateFromPoint(const mapbox::geometry::point<T> &point) {
        return CLLocationCoordinate2DMake(point.y, point.x);
    }
};

NS_ARRAY_OF(MGLShape <MGLFeature> *) *MGLFeaturesFromMBGLFeatures(const std::vector<mbgl::Feature> &features) {
    std::vector<MGLShape <MGLFeature> *> shapes;
    shapes.reserve(features.size());
    std::transform(features.begin(), features.end(), std::back_inserter(shapes), ^MGLShape <MGLFeature> * (const mbgl::Feature &feature) {
        GeometryEvaluator<double> evaluator(feature);
        return mapbox::geometry::geometry<double>::visit(feature.geometry, evaluator);
    });
    return [NSArray arrayWithObjects:&shapes[0] count:shapes.size()];
}

@implementation MGLPointFeature

- (id)attributeForKey:(NSString *)key {
    return self.attributes[key];
}

@end

@implementation MGLPolylineFeature

- (id)attributeForKey:(NSString *)key {
    return self.attributes[key];
}

@end

@implementation MGLPolygonFeature

- (id)attributeForKey:(NSString *)key {
    return self.attributes[key];
}

@end

@implementation MGLMultiPointFeature

- (id)attributeForKey:(NSString *)key {
    return self.attributes[key];
}

@end

@implementation MGLMultiPolylineFeature

- (id)attributeForKey:(NSString *)key {
    return self.attributes[key];
}

@end

@implementation MGLMultiPolygonFeature

- (id)attributeForKey:(NSString *)key {
    return self.attributes[key];
}

@end

@implementation MGLShapeCollectionFeature

- (id)attributeForKey:(NSString *)key {
    return self.attributes[key];
}

@end
