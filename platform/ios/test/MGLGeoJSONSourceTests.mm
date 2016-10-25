#import <XCTest/XCTest.h>

#import <Mapbox/Mapbox.h>
#import "MGLGeoJSONSource_Private.h"

#include <mbgl/style/sources/geojson_source.hpp>

@interface MGLGeoJSONSourceTests : XCTestCase

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
    MGLGeoJSONSource *source = [[MGLGeoJSONSource alloc] initWithSourceIdentifier:@"source-id" URL:url options:options];
    
    auto mbglOptions = [source geoJSONOptions];
    XCTAssertTrue(mbglOptions.cluster);
    XCTAssertEqual(mbglOptions.clusterRadius, 42);
    XCTAssertEqual(mbglOptions.clusterMaxZoom, 98);
    XCTAssertEqual(mbglOptions.maxzoom, 99);
    XCTAssertEqual(mbglOptions.buffer, 1976);
    XCTAssertEqual(mbglOptions.tolerance, 0.42);
  
    // when the supplied option cluster value is not of the correct type
    options = @{MGLGeoJSONClusterOption: @"number 1"};
    source = [[MGLGeoJSONSource alloc] initWithSourceIdentifier:@"source-id" URL:url options:options];
    XCTAssertThrows([source geoJSONOptions]);
}

@end
