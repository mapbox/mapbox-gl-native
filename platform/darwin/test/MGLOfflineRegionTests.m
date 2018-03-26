#import <Mapbox/Mapbox.h>

#import <XCTest/XCTest.h>

@interface MGLOfflineRegionTests : XCTestCase

@end

@implementation MGLOfflineRegionTests

- (void)testStyleURLs {
    MGLCoordinateBounds bounds = MGLCoordinateBoundsMake(kCLLocationCoordinate2DInvalid, kCLLocationCoordinate2DInvalid);
    MGLTilePyramidOfflineRegion *region = [[MGLTilePyramidOfflineRegion alloc] initWithStyleURL:nil bounds:bounds fromZoomLevel:0 toZoomLevel:DBL_MAX];
    XCTAssertEqualObjects(region.styleURL, [MGLStyle streetsStyleURLWithVersion:MGLStyleDefaultVersion], @"Streets isn’t the default style.");

    NSURL *localURL = [NSURL URLWithString:@"beautiful.style"];
    XCTAssertThrowsSpecificNamed([[MGLTilePyramidOfflineRegion alloc] initWithStyleURL:localURL bounds:bounds fromZoomLevel:0 toZoomLevel:DBL_MAX], NSException, MGLInvalidStyleURLException, @"No exception raised when initializing region with a local file URL as the style URL.");
}

- (void)testTilePyramidRegionEquality {
    MGLCoordinateBounds bounds = MGLCoordinateBoundsMake(kCLLocationCoordinate2DInvalid, kCLLocationCoordinate2DInvalid);
    MGLTilePyramidOfflineRegion *original = [[MGLTilePyramidOfflineRegion alloc] initWithStyleURL:[MGLStyle lightStyleURLWithVersion:MGLStyleDefaultVersion] bounds:bounds fromZoomLevel:5 toZoomLevel:10];
    MGLTilePyramidOfflineRegion *copy = [original copy];
    XCTAssertEqualObjects(original, copy, @"Tile pyramid region should be equal to its copy.");

    XCTAssertEqualObjects(original.styleURL, copy.styleURL, @"Style URL has changed.");
    XCTAssert(MGLCoordinateBoundsEqualToCoordinateBounds(original.bounds, copy.bounds), @"Bounds have changed.");
    XCTAssertEqual(original.minimumZoomLevel, original.minimumZoomLevel, @"Minimum zoom level has changed.");
    XCTAssertEqual(original.maximumZoomLevel, original.maximumZoomLevel, @"Maximum zoom level has changed.");
}

- (void)testGeometryRegionEquality {
    NSString *geojson = @"{\"type\": \"Point\", \"coordinates\": [-3.8671874999999996, 52.482780222078226] }";
    NSError *error;
    MGLShape *shape = [MGLShape shapeWithData: [geojson dataUsingEncoding:NSUTF8StringEncoding] encoding: NSUTF8StringEncoding error:&error];
    XCTAssertNil(error);
    
    MGLShapeOfflineRegion *original = [[MGLShapeOfflineRegion alloc] initWithStyleURL:[MGLStyle lightStyleURLWithVersion:MGLStyleDefaultVersion] shape:shape fromZoomLevel:5 toZoomLevel:10];
    MGLShapeOfflineRegion *copy = [original copy];
    XCTAssertEqualObjects(original, copy, @"Shape region should be equal to its copy.");
    
    XCTAssertEqualObjects(original.styleURL, copy.styleURL, @"Style URL has changed.");
    XCTAssertEqualObjects(original.shape, copy.shape, @"Geometry has changed.");
    XCTAssertEqual(original.minimumZoomLevel, original.minimumZoomLevel, @"Minimum zoom level has changed.");
    XCTAssertEqual(original.maximumZoomLevel, original.maximumZoomLevel, @"Maximum zoom level has changed.");
}

@end
