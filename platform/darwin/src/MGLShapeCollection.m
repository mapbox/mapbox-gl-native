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

@end
