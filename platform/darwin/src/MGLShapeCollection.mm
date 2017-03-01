#import "MGLShapeCollection.h"

#import "MGLShape_Private.h"

#import <mbgl/style/conversion/geojson.hpp>

@implementation MGLShapeCollection

+ (instancetype)shapeCollectionWithShapes:(NS_ARRAY_OF(MGLShape *) *)shapes {
    return [[self alloc] initWithShapes:shapes];
}

- (instancetype)initWithShapes:(NS_ARRAY_OF(MGLShape *) *)shapes {
    if (self = [super init]) {
        _shapes = shapes.copy;
    }
    return self;
}

- (instancetype)initWithCoder:(NSCoder *)decoder {
    if (self = [super initWithCoder:decoder]) {
        _shapes = [decoder decodeObjectOfClass:[NSArray class] forKey:@"shapes"];
    }
    return self;
}

- (void)encodeWithCoder:(NSCoder *)coder {
    [super encodeWithCoder:coder];
    [coder encodeObject:_shapes forKey:@"shapes"];
}

- (BOOL)isEqual:(id)other {
    if (self == other) return YES;
    if (![other isKindOfClass:[MGLShapeCollection class]]) return NO;

    MGLShapeCollection *otherShapeCollection = other;
    return [super isEqual:otherShapeCollection]
    && [_shapes isEqualToArray:otherShapeCollection.shapes];
}

- (NSUInteger)hash {
    NSUInteger hash = [super hash];
    for (MGLShape *shape in _shapes) {
        hash += [shape hash];
    }
    return hash;
}

- (CLLocationCoordinate2D)coordinate {
    return _shapes.firstObject.coordinate;
}

- (NSDictionary *)geoJSONDictionary {
    return @{@"type": @"GeometryCollection",
             @"geometries": [self geometryCollection]};
}

- (NSArray *)geometryCollection {
    NSMutableArray *geometries = [[NSMutableArray alloc] initWithCapacity:self.shapes.count];
    for (id shape in self.shapes) {
        NSDictionary *geometry = [shape geoJSONDictionary];
        [geometries addObject:geometry];
    }
    return [geometries copy];
}

- (mbgl::Geometry<double>)geometryObject {
    mapbox::geojson::geometry_collection collection;
    collection.reserve(self.shapes.count);
    for (MGLShape *shape in self.shapes) {
        collection.push_back([shape geometryObject]);
    }
    return collection;
}

@end
