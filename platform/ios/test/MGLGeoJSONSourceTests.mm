#import <XCTest/XCTest.h>

#import <Mapbox/Mapbox.h>
#import "MGLFeature_Private.h"
#import "MGLGeoJSONSource_Private.h"
#import "MGLSource_Private.h"

#include <mbgl/style/sources/geojson_source.hpp>

@interface MGLGeoJSONSourceTests : XCTestCase
@end

@interface MGLPolygonFeature (Test)

@property (nonatomic, copy, readwrite) NS_DICTIONARY_OF(NSString *, id) *attributes;

@end

@implementation MGLGeoJSONSourceTests

- (void)testMGLGeoJSONSourceWithOptions {
    NSURL *url = [NSURL URLWithString:@"http://www.mapbox.com/source"];
    
    NSDictionary *options = @{MGLGeoJSONClusterOption: @(YES),
                              MGLGeoJSONClusterRadiusOption: @42,
                              MGLGeoJSONClusterMaximumZoomLevelOption: @98,
                              MGLGeoJSONMaximumZoomLevelOption: @99,
                              MGLGeoJSONBufferOption: @1976,
                              MGLGeoJSONToleranceOption: @0.42};
    MGLGeoJSONSource *source = [[MGLGeoJSONSource alloc] initWithIdentifier:@"source-id" URL:url options:options];
    
    auto mbglOptions = [source geoJSONOptions];
    XCTAssertTrue(mbglOptions.cluster);
    XCTAssertEqual(mbglOptions.clusterRadius, 42);
    XCTAssertEqual(mbglOptions.clusterMaxZoom, 98);
    XCTAssertEqual(mbglOptions.maxzoom, 99);
    XCTAssertEqual(mbglOptions.buffer, 1976);
    XCTAssertEqual(mbglOptions.tolerance, 0.42);
  
    options = @{MGLGeoJSONClusterOption: @"number 1"};
    source = [[MGLGeoJSONSource alloc] initWithIdentifier:@"source-id" URL:url options:options];
    XCTAssertThrows([source geoJSONOptions]);
}

- (void)testMGLGeoJSONSourceWithData {
    
    NSString *geoJSON = @"{\"type\": \"FeatureCollection\",\"features\": [{\"type\": \"Feature\",\"properties\": {},\"geometry\": {\"type\": \"LineString\",\"coordinates\": [[-107.75390625,40.329795743702064],[-104.34814453125,37.64903402157866]]}}]}";
    
    NSData *data = [geoJSON dataUsingEncoding:NSUTF8StringEncoding];
    MGLGeoJSONSource *source = [[MGLGeoJSONSource alloc] initWithIdentifier:@"source-id" geoJSONData:data options:nil];
    
    [source mbglSource];
    
    XCTAssertNotNil(source.features);
    XCTAssertEqual(source.features.count, 1);
    XCTAssertTrue([source.features.firstObject isMemberOfClass:[MGLPolylineFeature class]]);
}

- (void)testMGLGeoJSONSourceWithPolylineFeatures {
    CLLocationCoordinate2D coordinates[] = { CLLocationCoordinate2DMake(0, 0), CLLocationCoordinate2DMake(10, 10)};
    MGLPolylineFeature *polylineFeature = [MGLPolylineFeature polylineWithCoordinates:coordinates count:2];
    
    MGLGeoJSONSource *source = [[MGLGeoJSONSource alloc] initWithIdentifier:@"source-id" features:@[polylineFeature] options:nil];
    
    std::unique_ptr<mbgl::style::Source> mbglSource = [source mbglSource];
    
    XCTAssertNotNil(source.features);
    XCTAssertEqual(source.features.count, 1);
    XCTAssertTrue([source.features.firstObject isMemberOfClass:[MGLPolylineFeature class]]);
}

- (void)testMGLGeoJSONSourceWithPolygonFeatures {
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
    
    MGLGeoJSONSource *source = [[MGLGeoJSONSource alloc] initWithIdentifier:@"source-id" features:@[polygonFeature] options:nil];
    
    std::unique_ptr<mbgl::style::Source> mbglSource = [source mbglSource];
    
    XCTAssertNotNil(source.features);
    XCTAssertEqual(source.features.count, 1);
    MGLPolygonFeature *expectedPolygonFeature = (MGLPolygonFeature *)source.features.firstObject;
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

- (void)testMGLGeoJSONSourceWithPolygonFeaturesInculdingInteriorPolygons {
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
    
    MGLGeoJSONSource *source = [[MGLGeoJSONSource alloc] initWithIdentifier:@"source-id" features:@[polygonFeature] options:nil];
    
    std::unique_ptr<mbgl::style::Source> mbglSource = [source mbglSource];
    
    XCTAssertNotNil(source.features);
    XCTAssertEqual(source.features.count, 1);
    XCTAssertTrue([source.features.firstObject isMemberOfClass:[MGLPolygonFeature class]]);
}


- (void)testMGLGeoJSONSourceWithMultiPointFeaturesUsingPolylines {
    CLLocationCoordinate2D coordinates[] = {
        CLLocationCoordinate2DMake(100.0, 0.0),
        CLLocationCoordinate2DMake(101.0, 0.0),
        CLLocationCoordinate2DMake(101.0, 1.0),
        CLLocationCoordinate2DMake(100.0, 1.0),
        CLLocationCoordinate2DMake(100.0, 0.0)};
    
    MGLMultiPointFeature *multiPointFeature = [MGLMultiPointFeature multiPointWithCoordinates:coordinates count:5];
    
    MGLGeoJSONSource *source = [[MGLGeoJSONSource alloc] initWithIdentifier:@"source-id" features:@[multiPointFeature] options:nil];
    
    std::unique_ptr<mbgl::style::Source> mbglSource = [source mbglSource];
    
    XCTAssertNotNil(source.features);
    XCTAssertEqual(source.features.count, 1);
    XCTAssertTrue([source.features.firstObject isMemberOfClass:[MGLMultiPointFeature class]]);
}

- (void)testMGLGeoJSONSourceWithMultiPointFeaturesUsingPolygons {
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
    
    MGLMultiPointFeature *multiPointFeature = (MGLMultiPointFeature *)[MGLPolygonFeature polygonWithCoordinates:coordinates count:5 interiorPolygons:@[polygon]];
    
    MGLGeoJSONSource *source = [[MGLGeoJSONSource alloc] initWithIdentifier:@"source-id" features:@[multiPointFeature] options:nil];
    
    std::unique_ptr<mbgl::style::Source> mbglSource = [source mbglSource];
    
    XCTAssertNotNil(source.features);
    XCTAssertEqual(source.features.count, 1);
    XCTAssertTrue([source.features.firstObject isMemberOfClass:[MGLPolygonFeature class]]);
}

- (void)testMGLGeoJSONSourceWithMultiPolylineFeatures {
    CLLocationCoordinate2D firstCoordinates[] = { CLLocationCoordinate2DMake(0, 0), CLLocationCoordinate2DMake(10, 10)};
    MGLPolylineFeature *firstPolylineFeature = [MGLPolylineFeature polylineWithCoordinates:firstCoordinates count:2];
    CLLocationCoordinate2D secondCoordinates[] = { CLLocationCoordinate2DMake(0, 0), CLLocationCoordinate2DMake(10, 10)};
    MGLPolylineFeature *secondPolylineFeature = [MGLPolylineFeature polylineWithCoordinates:secondCoordinates count:2];
    MGLMultiPolylineFeature *multiPolylineFeature = [MGLMultiPolylineFeature multiPolylineWithPolylines:@[firstPolylineFeature, secondPolylineFeature]];
    
    MGLGeoJSONSource *source = [[MGLGeoJSONSource alloc] initWithIdentifier:@"source-id" features:@[multiPolylineFeature] options:nil];
    
    std::unique_ptr<mbgl::style::Source> mbglSource = [source mbglSource];
    
    XCTAssertNotNil(source.features);
    XCTAssertEqual(source.features.count, 1);
    XCTAssertTrue([source.features.firstObject isMemberOfClass:[MGLMultiPolylineFeature class]]);
}

- (void)testMGLGeoJSONSourceWithMultiPolygonFeatures {
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
    
    MGLGeoJSONSource *source = [[MGLGeoJSONSource alloc] initWithIdentifier:@"source-id" features:@[multiPolygonFeature] options:nil];
    
    std::unique_ptr<mbgl::style::Source> mbglSource = [source mbglSource];
    
    XCTAssertNotNil(source.features);
    XCTAssertEqual(source.features.count, 1);
    XCTAssertTrue([source.features.firstObject isMemberOfClass:[MGLMultiPolygonFeature class]]);

}

- (void)testMGLGeoJSONSourceWithPointFeature {
    MGLPointFeature *pointFeature = [MGLPointFeature new];
    pointFeature.coordinate = CLLocationCoordinate2DMake(100.2, 0.2);
    
    MGLGeoJSONSource *source = [[MGLGeoJSONSource alloc] initWithIdentifier:@"souce-id" features:@[pointFeature] options:nil];
    
    std::unique_ptr<mbgl::style::Source> mbglSource = [source mbglSource];
    
    XCTAssertNotNil(source.features);
    XCTAssertEqual(source.features.count, 1);
    XCTAssertTrue([source.features.firstObject isMemberOfClass:[MGLPointFeature class]]);
}

- (void)testMGLGeoJSONSourceWithShapeCollectionFeatures {
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
    
    MGLMultiPointFeature *multiPointFeature = (MGLMultiPointFeature *)[MGLPolygonFeature polygonWithCoordinates:coordinates count:5 interiorPolygons:@[polygon]];
    
    MGLPointFeature *pointFeature = [MGLPointFeature new];
    pointFeature.coordinate = CLLocationCoordinate2DMake(100.2, 0.2);
    
    MGLShapeCollectionFeature *shapeCollectionFeature = [MGLShapeCollectionFeature shapeCollectionWithShapes:@[polygonFeature, polylineFeature, multiPolygonFeature, multiPolylineFeature, multiPointFeature, pointFeature]];

    MGLShapeCollectionFeature *shapeCollectionFeature_1 = [MGLShapeCollectionFeature shapeCollectionWithShapes:@[polygonFeature, polylineFeature, multiPolygonFeature, multiPolylineFeature, multiPointFeature, pointFeature, shapeCollectionFeature]];

    MGLGeoJSONSource *source = [[MGLGeoJSONSource alloc] initWithIdentifier:@"" features:@[shapeCollectionFeature_1] options:nil];
    
    XCTAssertThrowsSpecificNamed([source mbglSource], NSException, @"Method unavailable");
}

@end
