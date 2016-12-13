#import <XCTest/XCTest.h>

#import <Mapbox/Mapbox.h>
#import "MGLFeature_Private.h"
#import "MGLShapeSource_Private.h"
#import "MGLSource_Private.h"

#include <mbgl/style/sources/geojson_source.hpp>

@interface MGLShapeSourceTests : XCTestCase
@end

@implementation MGLShapeSourceTests

- (void)testMGLShapeSourceWithOptions {
    NSURL *url = [NSURL URLWithString:@"http://www.mapbox.com/source"];
    
    NSDictionary *options = @{MGLShapeSourceOptionClustered: @YES,
                              MGLShapeSourceOptionClusterRadius: @42,
                              MGLShapeSourceOptionMaximumZoomLevelForClustering: @98,
                              MGLShapeSourceOptionMaximumZoomLevel: @99,
                              MGLShapeSourceOptionBuffer: @1976,
                              MGLShapeSourceOptionSimplificationTolerance: @0.42};
    MGLShapeSource *source = [[MGLShapeSource alloc] initWithIdentifier:@"source-id" URL:url options:options];
    
    auto mbglOptions = [source geoJSONOptions];
    XCTAssertTrue(mbglOptions.cluster);
    XCTAssertEqual(mbglOptions.clusterRadius, 42);
    XCTAssertEqual(mbglOptions.clusterMaxZoom, 98);
    XCTAssertEqual(mbglOptions.maxzoom, 99);
    XCTAssertEqual(mbglOptions.buffer, 1976);
    XCTAssertEqual(mbglOptions.tolerance, 0.42);
  
    options = @{MGLShapeSourceOptionClustered: @"number 1"};
    XCTAssertThrows([[MGLShapeSource alloc] initWithIdentifier:@"source-id" URL:url options:options]);
}

- (void)testNilShape {
    MGLShapeSource *source = [[MGLShapeSource alloc] initWithIdentifier:@"id" shape:nil options:nil];
    XCTAssertNil(source.shape);
}

- (void)testMGLShapeSourceWithDataMultipleFeatures {
    
    NSString *geoJSON = @"{\"type\": \"FeatureCollection\",\"features\": [{\"type\": \"Feature\",\"properties\": {},\"geometry\": {\"type\": \"LineString\",\"coordinates\": [[-107.75390625,40.329795743702064],[-104.34814453125,37.64903402157866]]}}]}";
    
    NSData *data = [geoJSON dataUsingEncoding:NSUTF8StringEncoding];
    MGLShapeSource *source = [[MGLShapeSource alloc] initWithIdentifier:@"source-id" geoJSONData:data options:nil];
    
    MGLShapeCollection *collection = (MGLShapeCollection *)source.shape;
    XCTAssertNotNil(collection);
    XCTAssertEqual(collection.shapes.count, 1);
    XCTAssertTrue([collection.shapes.firstObject isMemberOfClass:[MGLPolylineFeature class]]);
}

- (void)testMGLShapeSourceWithSingleGeometry {
    MGLShapeSource *source = [[MGLShapeSource alloc] initWithIdentifier:@"geojson"
                                                                geoJSONData:[@"{\"type\": \"Point\", \"coordinates\": [0, 0]}" dataUsingEncoding:NSUTF8StringEncoding]
                                                                    options:nil];
    XCTAssertNotNil(source.shape);
    XCTAssert([source.shape isKindOfClass:[MGLPointFeature class]]);
}

- (void)testMGLGeoJSONSourceWithSingleFeature {
    NSString *geoJSON = @"{\"type\": \"Feature\", \"properties\": {\"color\": \"green\"}, \"geometry\": { \"type\": \"Point\", \"coordinates\": [ -114.06847000122069, 51.050459433092655 ] }}";
    MGLShapeSource *source = [[MGLShapeSource alloc] initWithIdentifier:@"geojson"
                                                                geoJSONData:[geoJSON dataUsingEncoding:NSUTF8StringEncoding]
                                                                    options:nil];
    XCTAssertNotNil(source.shape);
    XCTAssert([source.shape isKindOfClass:[MGLPointFeature class]]);
    MGLPointFeature *feature = (MGLPointFeature *)source.shape;
    XCTAssert([feature.attributes.allKeys containsObject:@"color"]);
}

- (void)testMGLShapeSourceWithPolylineFeatures {
    CLLocationCoordinate2D coordinates[] = { CLLocationCoordinate2DMake(0, 0), CLLocationCoordinate2DMake(10, 10)};
    MGLPolylineFeature *polylineFeature = [MGLPolylineFeature polylineWithCoordinates:coordinates count:2];
    
    MGLShapeSource *source = [[MGLShapeSource alloc] initWithIdentifier:@"source-id" shape:polylineFeature options:nil];
    
    XCTAssertNotNil(source.shape);
    XCTAssertTrue([source.shape isMemberOfClass:[MGLPolylineFeature class]]);
}

- (void)testMGLShapeSourceWithPolygonFeatures {
    CLLocationCoordinate2D coordinates[] = {
        CLLocationCoordinate2DMake(100.0, 0.0),
        CLLocationCoordinate2DMake(101.0, 0.0),
        CLLocationCoordinate2DMake(101.0, 1.0),
        CLLocationCoordinate2DMake(100.0, 1.0),
        CLLocationCoordinate2DMake(100.0, 0.0)};
    
    MGLPolygonFeature<MGLFeaturePrivate> *polygonFeature = (MGLPolygonFeature<MGLFeaturePrivate> *)[MGLPolygonFeature polygonWithCoordinates:coordinates count:5];
    polygonFeature.identifier = @"feature-id";
    NSString *stringAttribute = @"string";
    NSNumber *boolAttribute = [NSNumber numberWithBool:YES];
    NSNumber *doubleAttribute = [NSNumber numberWithDouble:1.23];
    NSDictionary *nestedDictionaryValue = @{@"nested-key-1": @"nested-string-value"};
    NSArray *arrayValue = @[@"string-value", @2];
    NSDictionary *dictionaryValue = @{@"key-1": @"string-value",
                                      @"key-2": @1,
                                      @"key-3": nestedDictionaryValue,
                                      @"key-4": arrayValue};
    NSArray *arrayOfArrays = @[@[@1, @"string-value", @[@"jagged"]]];
    NSArray *arrayOfDictionaries = @[@{@"key": @"value"}];
    
    polygonFeature.attributes = @{@"name": stringAttribute,
                                  @"bool": boolAttribute,
                                  @"double": doubleAttribute,
                                  @"dictionary-attribute": dictionaryValue,
                                  @"array-attribute": arrayValue,
                                  @"array-of-array-attribute": arrayOfArrays,
                                  @"array-of-dictionary-attribute": arrayOfDictionaries};
    
    MGLShapeSource *source = [[MGLShapeSource alloc] initWithIdentifier:@"source-id" shape:polygonFeature options:nil];
    
    XCTAssertNotNil(source.shape);
    MGLPolygonFeature *expectedPolygonFeature = (MGLPolygonFeature *)source.shape;
    XCTAssertEqualObjects(expectedPolygonFeature.identifier, polygonFeature.identifier);
    XCTAssertTrue([expectedPolygonFeature isMemberOfClass:[MGLPolygonFeature class]]);
    XCTAssertEqualObjects(expectedPolygonFeature.identifier, polygonFeature.identifier);
    XCTAssertEqualObjects(expectedPolygonFeature.attributes[@"name"], stringAttribute);
    XCTAssertEqualObjects(expectedPolygonFeature.attributes[@"bool"], boolAttribute);
    XCTAssertEqualObjects(expectedPolygonFeature.attributes[@"double"], doubleAttribute);
    XCTAssertEqualObjects(expectedPolygonFeature.attributes[@"dictionary-attribute"], dictionaryValue);
    XCTAssertEqualObjects(expectedPolygonFeature.attributes[@"array-attribute"], arrayValue);
    XCTAssertEqualObjects(expectedPolygonFeature.attributes[@"array-of-array-attribute"], arrayOfArrays);
    XCTAssertEqualObjects(expectedPolygonFeature.attributes[@"array-of-dictionary-attribute"], arrayOfDictionaries);
}

- (void)testMGLShapeSourceWithPolygonFeaturesInculdingInteriorPolygons {
    CLLocationCoordinate2D coordinates[] = {
        CLLocationCoordinate2DMake(100.0, 0.0),
        CLLocationCoordinate2DMake(101.0, 0.0),
        CLLocationCoordinate2DMake(101.0, 1.0),
        CLLocationCoordinate2DMake(100.0, 1.0),
        CLLocationCoordinate2DMake(100.0, 0.0)};
    
    CLLocationCoordinate2D interiorCoordinates[] = {
        CLLocationCoordinate2DMake(100.2, 0.2),
        CLLocationCoordinate2DMake(100.8, 0.2),
        CLLocationCoordinate2DMake(100.8, 0.8),
        CLLocationCoordinate2DMake(100.2, 0.8),
        CLLocationCoordinate2DMake(100.2, 0.2)};
    
    MGLPolygon *polygon = [MGLPolygon polygonWithCoordinates:interiorCoordinates count:5];
    
    MGLPolygonFeature *polygonFeature = [MGLPolygonFeature polygonWithCoordinates:coordinates count:5 interiorPolygons:@[polygon]];
    
    MGLShapeSource *source = [[MGLShapeSource alloc] initWithIdentifier:@"source-id" shape:polygonFeature options:nil];
    
    XCTAssertNotNil(source.shape);
    XCTAssertTrue([source.shape isMemberOfClass:[MGLPolygonFeature class]]);
}

- (void)testMGLShapeSourceWithMultiPolylineFeatures {
    CLLocationCoordinate2D firstCoordinates[] = { CLLocationCoordinate2DMake(0, 0), CLLocationCoordinate2DMake(10, 10)};
    MGLPolylineFeature *firstPolylineFeature = [MGLPolylineFeature polylineWithCoordinates:firstCoordinates count:2];
    CLLocationCoordinate2D secondCoordinates[] = { CLLocationCoordinate2DMake(0, 0), CLLocationCoordinate2DMake(10, 10)};
    MGLPolylineFeature *secondPolylineFeature = [MGLPolylineFeature polylineWithCoordinates:secondCoordinates count:2];
    MGLMultiPolylineFeature *multiPolylineFeature = [MGLMultiPolylineFeature multiPolylineWithPolylines:@[firstPolylineFeature, secondPolylineFeature]];
    
    MGLShapeSource *source = [[MGLShapeSource alloc] initWithIdentifier:@"source-id" shape:multiPolylineFeature options:nil];
    
    XCTAssertNotNil(source.shape);
    XCTAssertTrue([source.shape isMemberOfClass:[MGLMultiPolylineFeature class]]);
}

- (void)testMGLShapeSourceWithMultiPolygonFeatures {
    CLLocationCoordinate2D coordinates[] = {
        CLLocationCoordinate2DMake(100.0, 0.0),
        CLLocationCoordinate2DMake(101.0, 0.0),
        CLLocationCoordinate2DMake(101.0, 1.0),
        CLLocationCoordinate2DMake(100.0, 1.0),
        CLLocationCoordinate2DMake(100.0, 0.0)};
    
    CLLocationCoordinate2D interiorCoordinates[] = {
        CLLocationCoordinate2DMake(100.2, 0.2),
        CLLocationCoordinate2DMake(100.8, 0.2),
        CLLocationCoordinate2DMake(100.8, 0.8),
        CLLocationCoordinate2DMake(100.2, 0.8),
        CLLocationCoordinate2DMake(100.2, 0.2)};
    
    MGLPolygon *polygon = [MGLPolygon polygonWithCoordinates:interiorCoordinates count:5];
    
    MGLPolygonFeature *firstPolygon = [MGLPolygonFeature polygonWithCoordinates:coordinates count:5 interiorPolygons:@[polygon]];
    MGLPolygonFeature *secondPolygon = [MGLPolygonFeature polygonWithCoordinates:coordinates count:5 interiorPolygons:@[polygon]];
    
    MGLMultiPolygonFeature *multiPolygonFeature = [MGLMultiPolygonFeature multiPolygonWithPolygons:@[firstPolygon, secondPolygon]];
    
    MGLShapeSource *source = [[MGLShapeSource alloc] initWithIdentifier:@"source-id" shape:multiPolygonFeature options:nil];
    
    XCTAssertNotNil(source.shape);
    XCTAssertTrue([source.shape isMemberOfClass:[MGLMultiPolygonFeature class]]);
}

- (void)testMGLShapeSourceWithPointFeature {
    MGLPointFeature *pointFeature = [MGLPointFeature new];
    pointFeature.coordinate = CLLocationCoordinate2DMake(100.2, 0.2);
    
    MGLShapeSource *source = [[MGLShapeSource alloc] initWithIdentifier:@"souce-id" shape:pointFeature options:nil];
    
    XCTAssertNotNil(source.shape);
    XCTAssertTrue([source.shape isMemberOfClass:[MGLPointFeature class]]);
}

- (void)testMGLShapeSourceWithPointCollectionFeature {
    CLLocationCoordinate2D coordinates[] = {
        CLLocationCoordinate2DMake(100.0, 0.0),
        CLLocationCoordinate2DMake(101.0, 0.0),
        CLLocationCoordinate2DMake(101.0, 1.0),
        CLLocationCoordinate2DMake(100.0, 1.0),
        CLLocationCoordinate2DMake(100.0, 0.0)};
    MGLPointCollectionFeature *pointCollectionFeature = [MGLPointCollectionFeature pointCollectionWithCoordinates:coordinates count:5];
    MGLShapeSource *source = [[MGLShapeSource alloc] initWithIdentifier:@"souce-id" shape:pointCollectionFeature options:nil];

    XCTAssertNotNil(source.shape);
    XCTAssertTrue([source.shape isMemberOfClass:[MGLPointCollectionFeature class]]);
}

- (void)testMGLShapeSourceWithShapeCollectionFeatures {
    CLLocationCoordinate2D coordinates[] = {
        CLLocationCoordinate2DMake(100.0, 0.0),
        CLLocationCoordinate2DMake(101.0, 0.0),
        CLLocationCoordinate2DMake(101.0, 1.0),
        CLLocationCoordinate2DMake(100.0, 1.0),
        CLLocationCoordinate2DMake(100.0, 0.0)};
    
    CLLocationCoordinate2D interiorCoordinates[] = {
        CLLocationCoordinate2DMake(100.2, 0.2),
        CLLocationCoordinate2DMake(100.8, 0.2),
        CLLocationCoordinate2DMake(100.8, 0.8),
        CLLocationCoordinate2DMake(100.2, 0.8),
        CLLocationCoordinate2DMake(100.2, 0.2)};
    
    MGLPolygon *polygon = [MGLPolygon polygonWithCoordinates:interiorCoordinates count:5];
    
    MGLPolygonFeature *polygonFeature = [MGLPolygonFeature polygonWithCoordinates:coordinates count:5 interiorPolygons:@[polygon]];
    
    CLLocationCoordinate2D coordinates_2[] = { CLLocationCoordinate2DMake(0, 0), CLLocationCoordinate2DMake(10, 10)};
    MGLPolylineFeature *polylineFeature = [MGLPolylineFeature polylineWithCoordinates:coordinates_2 count:2];
    
    MGLMultiPolygonFeature *multiPolygonFeature = [MGLMultiPolygonFeature multiPolygonWithPolygons:@[polygonFeature, polygonFeature]];
    
    MGLMultiPolylineFeature *multiPolylineFeature = [MGLMultiPolylineFeature multiPolylineWithPolylines:@[polylineFeature, polylineFeature]];
    
    MGLPointCollectionFeature *pointCollectionFeature = [MGLPointCollectionFeature pointCollectionWithCoordinates:coordinates count:5];
    
    MGLPointFeature *pointFeature = [MGLPointFeature new];
    pointFeature.coordinate = CLLocationCoordinate2DMake(100.2, 0.2);
    
    MGLShapeCollectionFeature *shapeCollectionFeature = [MGLShapeCollectionFeature shapeCollectionWithShapes:@[polygonFeature, polylineFeature, multiPolygonFeature, multiPolylineFeature, pointCollectionFeature, pointFeature]];

    MGLShapeSource *source = [[MGLShapeSource alloc] initWithIdentifier:@"source-id" shape:shapeCollectionFeature options:nil];
    
    MGLShapeCollectionFeature *shape = (MGLShapeCollectionFeature *)source.shape;
    XCTAssertNotNil(shape);
    XCTAssert(shape.shapes.count == 6, @"Shape collection should contain 6 shapes");
}

@end
