#import "MGLShapeCollection.h"

@implementation MGLShapeCollection

+ (instancetype)shapeCollectionWithShapes:(NS_ARRAY_OF(MGLShape *) *)shapes {
    return [[self alloc] initWithShapes:shapes];
}

- (instancetype)initWithShapes:(NS_ARRAY_OF(MGLShape *) *)shapes {
    if (self = [super init]) {
        NSAssert(shapes.count, @"Cannot create an empty shape collection");
        _shapes = shapes.copy;
    }
    return self;
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

@end
