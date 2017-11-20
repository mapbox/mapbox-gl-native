#import <XCTest/XCTest.h>

#import <Mapbox/Mapbox.h>


@interface MGLComputedShapeSourceTests : XCTestCase
@end

@implementation MGLComputedShapeSourceTests

- (void)testInitializer {
    MGLComputedShapeSource *source = [[MGLComputedShapeSource alloc] initWithIdentifier:@"id" options:@{}];
    XCTAssertNotNil(source);
    XCTAssertNotNil(source.requestQueue);
    XCTAssertNil(source.dataSource);
}

- (void)testNilOptions {
    MGLComputedShapeSource *source = [[MGLComputedShapeSource alloc] initWithIdentifier:@"id" options:nil];
    XCTAssertNotNil(source);
}


@end
