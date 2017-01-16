#import <Mapbox/Mapbox.h>
#import <XCTest/XCTest.h>

#if TARGET_OS_IPHONE
#import "MGLUserLocation_Private.h"
#endif

@interface MGLCodingTests : XCTestCase
@end

@implementation MGLCodingTests

- (NSString *)temporaryFilePathForClass:(Class)clazz {
    return [NSTemporaryDirectory() stringByAppendingPathComponent:NSStringFromClass(clazz)];
}

- (void)testPointAnnotation {
    MGLPointAnnotation *annotation = [[MGLPointAnnotation alloc] init];
    annotation.coordinate = CLLocationCoordinate2DMake(0.5, 0.5);
    annotation.title = @"title";
    annotation.subtitle = @"subtitle";
    
    NSString *filePath = [self temporaryFilePathForClass:MGLPointAnnotation.class];
    [NSKeyedArchiver archiveRootObject:annotation toFile:filePath];
    MGLPointAnnotation *unarchivedAnnotation = [NSKeyedUnarchiver unarchiveObjectWithFile:filePath];
    
    XCTAssertEqualObjects(annotation, unarchivedAnnotation);
}

- (void)testPointFeature {
    MGLPointFeature *pointFeature = [[MGLPointFeature alloc] init];
    pointFeature.title = @"title";
    pointFeature.subtitle = @"subtitle";
    pointFeature.identifier = @(123);
    pointFeature.attributes = @{@"bbox": @[@1, @2, @3, @4]};
    
    NSString *filePath = [self temporaryFilePathForClass:MGLPointFeature.class];
    [NSKeyedArchiver archiveRootObject:pointFeature toFile:filePath];
    MGLPointFeature *unarchivedPointFeature = [NSKeyedUnarchiver unarchiveObjectWithFile:filePath];
    
    XCTAssertEqualObjects(pointFeature, unarchivedPointFeature);
}

- (void)testPolyline {
    CLLocationCoordinate2D coordinates[] = {
        CLLocationCoordinate2DMake(0.129631234123, 1.7812739312551),
        CLLocationCoordinate2DMake(2.532083092342, 3.5216418292392)
    };
    
    NSUInteger numberOfCoordinates = sizeof(coordinates) / sizeof(CLLocationCoordinate2D);
    
    MGLPolyline *polyline = [MGLPolyline polylineWithCoordinates:coordinates count:numberOfCoordinates];
    polyline.title = @"title";
    polyline.subtitle = @"subtitle";
    
    NSString *filePath = [self temporaryFilePathForClass:[MGLPolyline class]];
    [NSKeyedArchiver archiveRootObject:polyline toFile:filePath];
    MGLPolyline *unarchivedPolyline = [NSKeyedUnarchiver unarchiveObjectWithFile:filePath];
    
    XCTAssertEqualObjects(polyline, unarchivedPolyline);
    
    CLLocationCoordinate2D otherCoordinates[] = {
        CLLocationCoordinate2DMake(-1, -2)
    };
    
    [unarchivedPolyline replaceCoordinatesInRange:NSMakeRange(0, 1) withCoordinates:otherCoordinates];
    
    XCTAssertNotEqualObjects(polyline, unarchivedPolyline);
}

- (void)testPolygon {
    CLLocationCoordinate2D coordinates[] = {
        CLLocationCoordinate2DMake(0.664482398, 1.8865675),
        CLLocationCoordinate2DMake(2.13224687, 3.9984632)
    };
    
    NSUInteger numberOfCoordinates = sizeof(coordinates) / sizeof(CLLocationCoordinate2D);
    
    MGLPolygon *polygon = [MGLPolygon polygonWithCoordinates:coordinates count:numberOfCoordinates];
    polygon.title = nil;
    polygon.subtitle = @"subtitle";
    
    NSString *filePath = [self temporaryFilePathForClass:[MGLPolygon class]];
    [NSKeyedArchiver archiveRootObject:polygon toFile:filePath];
    
    MGLPolygon *unarchivedPolygon = [NSKeyedUnarchiver unarchiveObjectWithFile:filePath];
    
    XCTAssertEqualObjects(polygon, unarchivedPolygon);
}

- (void)testPolygonWithInteriorPolygons {
    CLLocationCoordinate2D coordinates[] = {
        CLLocationCoordinate2DMake(0, 1),
        CLLocationCoordinate2DMake(10, 20)
    };
    
    NSUInteger numberOfCoordinates = sizeof(coordinates) / sizeof(CLLocationCoordinate2D);
    
    CLLocationCoordinate2D interiorCoordinates[] = {
        CLLocationCoordinate2DMake(4, 4),
        CLLocationCoordinate2DMake(6, 6)
    };
    
    NSUInteger numberOfInteriorCoordinates = sizeof(interiorCoordinates) / sizeof(CLLocationCoordinate2D);
    
    MGLPolygon *interiorPolygon = [MGLPolygon polygonWithCoordinates:interiorCoordinates count:numberOfInteriorCoordinates];
    MGLPolygon *polygon = [MGLPolygon polygonWithCoordinates:coordinates count:numberOfCoordinates interiorPolygons:@[interiorPolygon]];
    
    NSString *filePath = [self temporaryFilePathForClass:[MGLPolygon class]];
    [NSKeyedArchiver archiveRootObject:polygon toFile:filePath];
    
    MGLPolygon *unarchivedPolygon = [NSKeyedUnarchiver unarchiveObjectWithFile:filePath];
    
    XCTAssertEqualObjects(polygon, unarchivedPolygon);
}

- (void)testPolylineFeature {
    CLLocationCoordinate2D coordinates[] = {
        CLLocationCoordinate2DMake(0, 1),
        CLLocationCoordinate2DMake(10, 20)
    };
    
    NSUInteger numberOfCoordinates = sizeof(coordinates) / sizeof(CLLocationCoordinate2D);
    MGLPolylineFeature *polylineFeature = [MGLPolylineFeature polylineWithCoordinates:coordinates count:numberOfCoordinates];
    polylineFeature.attributes = @{@"bbox": @[@0, @1, @2, @3]};
    polylineFeature.identifier = @"identifier";
    
    NSString *filePath = [self temporaryFilePathForClass:[MGLPolylineFeature class]];
    [NSKeyedArchiver archiveRootObject:polylineFeature toFile:filePath];
    
    MGLPolylineFeature *unarchivedPolylineFeature = [NSKeyedUnarchiver unarchiveObjectWithFile:filePath];
    
    XCTAssertEqualObjects(polylineFeature, unarchivedPolylineFeature);
    
    unarchivedPolylineFeature.attributes = @{@"bbox": @[@4, @3, @2, @1]};
    
    XCTAssertNotEqualObjects(polylineFeature, unarchivedPolylineFeature);
}

- (void)testPolygonFeature {
    CLLocationCoordinate2D coordinates[] = {
        CLLocationCoordinate2DMake(0, 1),
        CLLocationCoordinate2DMake(10, 20)
    };
    
    NSUInteger numberOfCoordinates = sizeof(coordinates) / sizeof(CLLocationCoordinate2D);
    MGLPolygonFeature *polygonFeature = [MGLPolygonFeature polygonWithCoordinates:coordinates count:numberOfCoordinates];
    
    NSString *filePath = [self temporaryFilePathForClass:[MGLPolygonFeature class]];
    [NSKeyedArchiver archiveRootObject:polygonFeature toFile:filePath];
    
    MGLPolygonFeature *unarchivedPolygonFeature = [NSKeyedUnarchiver unarchiveObjectWithFile:filePath];
    
    XCTAssertEqualObjects(polygonFeature, unarchivedPolygonFeature);
    
    unarchivedPolygonFeature.identifier = @"test";
    
    XCTAssertNotEqualObjects(polygonFeature, unarchivedPolygonFeature);
}

- (void)testPointCollection {
    CLLocationCoordinate2D coordinates[] = {
        CLLocationCoordinate2DMake(0, 1),
        CLLocationCoordinate2DMake(10, 11),
        CLLocationCoordinate2DMake(20, 21),
        CLLocationCoordinate2DMake(30, 31),
    };
    
    NSUInteger numberOfCoordinates = sizeof(coordinates) / sizeof(CLLocationCoordinate2D);
    
    MGLPointCollection *pointCollection = [MGLPointCollection pointCollectionWithCoordinates:coordinates count:numberOfCoordinates];
    NSString *filePath = [self temporaryFilePathForClass:[MGLPointCollection class]];
    [NSKeyedArchiver archiveRootObject:pointCollection toFile:filePath];
    
    MGLPointCollection *unarchivedPointCollection = [NSKeyedUnarchiver unarchiveObjectWithFile:filePath];
    
    XCTAssertEqualObjects(pointCollection, unarchivedPointCollection);
}

- (void)testPointCollectionFeature {
    NSMutableArray *features = [NSMutableArray array];
    for (NSUInteger i = 0; i < 100; i++) {
        MGLPointFeature *feature = [[MGLPointFeature alloc] init];
        feature.coordinate = CLLocationCoordinate2DMake(arc4random() % 90, arc4random() % 180);
        [features addObject:feature];
    }
    
    CLLocationCoordinate2D coordinates[] = {
        CLLocationCoordinate2DMake(0, 1),
        CLLocationCoordinate2DMake(10, 11),
        CLLocationCoordinate2DMake(20, 21),
        CLLocationCoordinate2DMake(30, 31),
    };
    
    NSUInteger numberOfCoordinates = sizeof(coordinates) / sizeof(CLLocationCoordinate2D);
    
    MGLPointCollectionFeature *collection = [MGLPointCollectionFeature pointCollectionWithCoordinates:coordinates count:numberOfCoordinates];
    collection.identifier = @"identifier";
    collection.attributes = @{@"bbox": @[@1, @2, @3, @4]};
    
    NSString *filePath = [self temporaryFilePathForClass:[MGLPointCollectionFeature class]];
    [NSKeyedArchiver archiveRootObject:collection toFile:filePath];
    
    MGLPointCollectionFeature *unarchivedCollection = [NSKeyedUnarchiver unarchiveObjectWithFile:filePath];
    
    XCTAssertEqualObjects(collection, unarchivedCollection);
    
    unarchivedCollection.identifier = @"newIdentifier";
    
    XCTAssertNotEqualObjects(collection, unarchivedCollection);
}

- (void)testMultiPolyline {
    
    CLLocationCoordinate2D coordinates[] = {
        CLLocationCoordinate2DMake(0, 1),
        CLLocationCoordinate2DMake(10, 11),
        CLLocationCoordinate2DMake(20, 21),
        CLLocationCoordinate2DMake(30, 31),
    };
    
    NSUInteger numberOfCoordinates = sizeof(coordinates) / sizeof(CLLocationCoordinate2D);
    
    NSMutableArray *polylines = [NSMutableArray array];
    
    for (NSUInteger i = 0; i < 100; i++) {
        MGLPolyline *polyline = [MGLPolyline polylineWithCoordinates:coordinates count:numberOfCoordinates];
        [polylines addObject:polyline];
    }
    
    MGLMultiPolyline *multiPolyline = [MGLMultiPolyline multiPolylineWithPolylines:polylines];
    
    NSString *filePath = [self temporaryFilePathForClass:[MGLMultiPolyline class]];
    [NSKeyedArchiver archiveRootObject:multiPolyline toFile:filePath];
    
    MGLMultiPolyline *unarchivedMultiPolyline = [NSKeyedUnarchiver unarchiveObjectWithFile:filePath];
    MGLMultiPolyline *anotherMultipolyline = [MGLMultiPolyline multiPolylineWithPolylines:[polylines subarrayWithRange:NSMakeRange(0, polylines.count/2)]];
    
    XCTAssertEqualObjects(multiPolyline, unarchivedMultiPolyline);
    XCTAssertNotEqualObjects(unarchivedMultiPolyline, anotherMultipolyline);
}

- (void)testMultiPolygon {
    
    CLLocationCoordinate2D coordinates[] = {
        CLLocationCoordinate2DMake(0, 1),
        CLLocationCoordinate2DMake(10, 11),
        CLLocationCoordinate2DMake(20, 21),
        CLLocationCoordinate2DMake(30, 31),
    };
    
    NSUInteger numberOfCoordinates = sizeof(coordinates) / sizeof(CLLocationCoordinate2D);
    
    NSMutableArray *polygons = [NSMutableArray array];
    
    for (NSUInteger i = 0; i < 100; i++) {
        MGLPolygon *polygon = [MGLPolygon polygonWithCoordinates:coordinates count:numberOfCoordinates];
        [polygons addObject:polygon];
    }
    
    MGLMultiPolygon *multiPolygon = [MGLMultiPolygon multiPolygonWithPolygons:polygons];
    
    NSString *filePath = [self temporaryFilePathForClass:[MGLMultiPolygon class]];
    [NSKeyedArchiver archiveRootObject:multiPolygon toFile:filePath];
    
    MGLMultiPolygon *unarchivedMultiPolygon = [NSKeyedUnarchiver unarchiveObjectWithFile:filePath];
    MGLMultiPolygon *anotherMultiPolygon = [MGLMultiPolygon multiPolygonWithPolygons:[polygons subarrayWithRange:NSMakeRange(0, polygons.count/2)]];
    
    XCTAssertEqualObjects(multiPolygon, unarchivedMultiPolygon);
    XCTAssertNotEqualObjects(anotherMultiPolygon, unarchivedMultiPolygon);
}

- (void)testShapeCollection {
    CLLocationCoordinate2D coordinates[] = {
        CLLocationCoordinate2DMake(10.12315786, 11.23451186),
        CLLocationCoordinate2DMake(20.91836515, 21.93689215),
        CLLocationCoordinate2DMake(30.55697246, 31.33988123),
    };
    
    NSUInteger numberOfCoordinates = sizeof(coordinates) / sizeof(CLLocationCoordinate2D);
    
    MGLPolyline *polyline = [MGLPolyline polylineWithCoordinates:coordinates count:numberOfCoordinates];
    MGLPolygon *polygon = [MGLPolygon polygonWithCoordinates:coordinates count:numberOfCoordinates];
    
    MGLShapeCollection *shapeCollection = [MGLShapeCollection shapeCollectionWithShapes:@[polyline, polygon]];
    
    NSString *filePath = [self temporaryFilePathForClass:[MGLShapeCollection class]];
    [NSKeyedArchiver archiveRootObject:shapeCollection toFile:filePath];
    
    MGLShapeCollection *unarchivedShapeCollection = [NSKeyedUnarchiver unarchiveObjectWithFile:filePath];
    MGLShapeCollection *anotherShapeCollection = [MGLShapeCollection shapeCollectionWithShapes:@[polygon]];
    
    XCTAssertEqualObjects(shapeCollection, unarchivedShapeCollection);
    XCTAssertNotEqualObjects(shapeCollection, anotherShapeCollection);
}

- (void)testMultiPolylineFeature {
    CLLocationCoordinate2D coordinates[] = {
        CLLocationCoordinate2DMake(10.12315786, 11.23451186),
        CLLocationCoordinate2DMake(20.91836515, 21.93689215),
        CLLocationCoordinate2DMake(30.55697246, 31.33988123),
    };
    
    NSUInteger numberOfCoordinates = sizeof(coordinates) / sizeof(CLLocationCoordinate2D);
    
    NSMutableArray *polylines = [NSMutableArray array];
    for (NSUInteger i = 0; i < 100; i++) {
        MGLPolylineFeature *polylineFeature = [MGLPolylineFeature polylineWithCoordinates:coordinates count:numberOfCoordinates];
        polylineFeature.identifier = @(arc4random() % 100).stringValue;
        [polylines addObject:polylineFeature];
    }
    
    MGLMultiPolylineFeature *multiPolylineFeature = [MGLMultiPolylineFeature multiPolylineWithPolylines:polylines];
    multiPolylineFeature.attributes = @{@"bbox": @[@4, @3, @2, @1]};
    
    NSString *filePath = [self temporaryFilePathForClass:[MGLMultiPolylineFeature class]];
    [NSKeyedArchiver archiveRootObject:multiPolylineFeature toFile:filePath];
    
    MGLMultiPolylineFeature *unarchivedMultiPolylineFeature = [NSKeyedUnarchiver unarchiveObjectWithFile:filePath];
    MGLMultiPolylineFeature *anotherMultiPolylineFeature = [MGLMultiPolylineFeature multiPolylineWithPolylines:[polylines subarrayWithRange:NSMakeRange(0, polylines.count/2)]];
    
    XCTAssertEqualObjects(multiPolylineFeature, unarchivedMultiPolylineFeature);
    XCTAssertNotEqualObjects(unarchivedMultiPolylineFeature, anotherMultiPolylineFeature);
}

- (void)testMultiPolygonFeature {
    CLLocationCoordinate2D coordinates[] = {
        CLLocationCoordinate2DMake(10.12315786, 11.23451185),
        CLLocationCoordinate2DMake(20.88471238, 21.93684215),
        CLLocationCoordinate2DMake(30.15697236, 31.32988123),
    };
    
    NSUInteger numberOfCoordinates = sizeof(coordinates) / sizeof(CLLocationCoordinate2D);
    
    NSMutableArray *polygons = [NSMutableArray array];
    for (NSUInteger i = 0; i < 100; i++ ) {
        MGLPolygonFeature *polygonFeature = [MGLPolygonFeature polygonWithCoordinates:coordinates count:numberOfCoordinates];
        polygonFeature.identifier = @(arc4random_uniform(100)).stringValue;
        [polygons addObject:polygonFeature];
    }
    
    MGLMultiPolygonFeature *multiPolygonFeature = [MGLMultiPolygonFeature multiPolygonWithPolygons:polygons];
    multiPolygonFeature.attributes = @{@"bbox": @[@(arc4random_uniform(100)),
                                                  @(arc4random_uniform(100)),
                                                  @(arc4random_uniform(100)),
                                                  @(arc4random_uniform(100))]};
    
    NSString *filePath = [self temporaryFilePathForClass:[MGLMultiPolylineFeature class]];
    [NSKeyedArchiver archiveRootObject:multiPolygonFeature toFile:filePath];
    
    MGLMultiPolygonFeature *unarchivedMultiPolygonFeature = [NSKeyedUnarchiver unarchiveObjectWithFile:filePath];
    MGLMultiPolygonFeature *anotherMultiPolygonFeature = [MGLMultiPolygonFeature multiPolygonWithPolygons:[polygons subarrayWithRange:NSMakeRange(0, polygons.count/2)]];
    
    XCTAssertEqualObjects(multiPolygonFeature, unarchivedMultiPolygonFeature);
    XCTAssertNotEqualObjects(anotherMultiPolygonFeature, unarchivedMultiPolygonFeature);
}

- (void)testShapeCollectionFeature {
    CLLocationCoordinate2D coordinates[] = {
        CLLocationCoordinate2DMake(10.12315786, 11.23451186),
        CLLocationCoordinate2DMake(20.91836515, 21.93689215),
        CLLocationCoordinate2DMake(30.55697246, 31.33988123),
    };
    
    NSUInteger numberOfCoordinates = sizeof(coordinates) / sizeof(CLLocationCoordinate2D);
    
    MGLPolylineFeature *polyline = [MGLPolylineFeature polylineWithCoordinates:coordinates count:numberOfCoordinates];
    MGLPolygonFeature *polygon = [MGLPolygonFeature polygonWithCoordinates:coordinates count:numberOfCoordinates];
    
    MGLShapeCollectionFeature *shapeCollectionFeature = [MGLShapeCollectionFeature shapeCollectionWithShapes:@[polyline, polygon]];
    shapeCollectionFeature.identifier = @(arc4random_uniform(100)).stringValue;
    shapeCollectionFeature.attributes = @{@"bbox":@[@(arc4random_uniform(100)),
                                                    @(arc4random_uniform(100)),
                                                    @(arc4random_uniform(100)),
                                                    @(arc4random_uniform(100))]};
    
    NSString *filePath = [self temporaryFilePathForClass:[MGLShapeCollectionFeature class]];
    [NSKeyedArchiver archiveRootObject:shapeCollectionFeature toFile:filePath];
    
    MGLShapeCollectionFeature *unarchivedShapeCollectionFeature = [NSKeyedUnarchiver unarchiveObjectWithFile:filePath];
    
    XCTAssertEqualObjects(shapeCollectionFeature, unarchivedShapeCollectionFeature);
}

- (void)testAnnotationImage {
#if TARGET_OS_IPHONE
    UIGraphicsBeginImageContext(CGSizeMake(10, 10));
    [[UIColor redColor] setFill];
    UIRectFill(CGRectMake(0, 0, 10, 10));
    UIImage *image = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
#else
    NSImage *image = [[NSImage alloc] initWithSize:CGSizeMake(10, 10)];
    [image lockFocus];
    [[NSColor redColor] drawSwatchInRect:CGRectMake(0, 0, 10, 10)];
    [image unlockFocus];
#endif
    
    MGLAnnotationImage *annotationImage = [MGLAnnotationImage annotationImageWithImage:image reuseIdentifier:@(arc4random_uniform(100)).stringValue];
    
    NSString *filePath = [self temporaryFilePathForClass:[MGLAnnotationImage class]];
    [NSKeyedArchiver archiveRootObject:annotationImage toFile:filePath];
    
    MGLAnnotationImage *unarchivedAnnotationImage = [NSKeyedUnarchiver unarchiveObjectWithFile:filePath];
    
    XCTAssertEqualObjects(annotationImage, unarchivedAnnotationImage);
}

#if TARGET_OS_IPHONE
- (void)testAnnotationView {
    MGLAnnotationView *annotationView = [[MGLAnnotationView alloc] initWithReuseIdentifier:@"id"];
    annotationView.enabled = NO;
    annotationView.selected = YES;
    annotationView.draggable = YES;
    annotationView.centerOffset = CGVectorMake(10, 10);
    annotationView.scalesWithViewingDistance = NO;
    
    NSString *filePath = [self temporaryFilePathForClass:[MGLAnnotationView class]];
    [NSKeyedArchiver archiveRootObject:annotationView toFile:filePath];
    
    MGLAnnotationView *unarchivedAnnotationView = [NSKeyedUnarchiver unarchiveObjectWithFile:filePath];
    
    XCTAssertEqual(annotationView.enabled, unarchivedAnnotationView.enabled);
    XCTAssertEqual(annotationView.selected, unarchivedAnnotationView.selected);
    XCTAssertEqual(annotationView.draggable, unarchivedAnnotationView.draggable);
    XCTAssertEqualObjects(NSStringFromCGVector(annotationView.centerOffset), NSStringFromCGVector(unarchivedAnnotationView.centerOffset));
    XCTAssertEqual(annotationView.scalesWithViewingDistance, unarchivedAnnotationView.scalesWithViewingDistance);
}
#endif

#if TARGET_OS_IPHONE
- (void)testUserLocation {
    MGLUserLocation *userLocation = [[MGLUserLocation alloc] init];
    userLocation.location = [[CLLocation alloc] initWithLatitude:1 longitude:1];
    
    NSString *filePath = [self temporaryFilePathForClass:[MGLUserLocation class]];
    [NSKeyedArchiver archiveRootObject:userLocation toFile:filePath];
    
    MGLUserLocation *unarchivedUserLocation = [NSKeyedUnarchiver unarchiveObjectWithFile:filePath];
    
    XCTAssertEqualObjects(userLocation, unarchivedUserLocation);
    unarchivedUserLocation.location = [[CLLocation alloc] initWithLatitude:10 longitude:10];
    XCTAssertNotEqualObjects(userLocation, unarchivedUserLocation);
}
#endif

#if TARGET_OS_IPHONE
- (void)testUserLocationAnnotationView {
    MGLUserLocationAnnotationView *annotationView = [[MGLUserLocationAnnotationView alloc] init];
    annotationView.enabled = NO;
    annotationView.selected = YES;
    annotationView.draggable = YES;
    annotationView.centerOffset = CGVectorMake(10, 10);
    annotationView.scalesWithViewingDistance = NO;
    
    NSString *filePath = [self temporaryFilePathForClass:[MGLUserLocationAnnotationView class]];
    [NSKeyedArchiver archiveRootObject:annotationView toFile:filePath];
    
    MGLUserLocationAnnotationView *unarchivedAnnotationView = [NSKeyedUnarchiver unarchiveObjectWithFile:filePath];
    
    XCTAssertEqual(annotationView.enabled, unarchivedAnnotationView.enabled);
    XCTAssertEqual(annotationView.selected, unarchivedAnnotationView.selected);
    XCTAssertEqual(annotationView.draggable, unarchivedAnnotationView.draggable);
    XCTAssertEqualObjects(NSStringFromCGVector(annotationView.centerOffset), NSStringFromCGVector(unarchivedAnnotationView.centerOffset));
    XCTAssertEqual(annotationView.scalesWithViewingDistance, unarchivedAnnotationView.scalesWithViewingDistance);
}
#endif

@end
