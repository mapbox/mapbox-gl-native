#import <Mapbox/Mapbox.h>

#import <XCTest/XCTest.h>

@interface MGLVersionTests : XCTestCase

@end

@implementation MGLVersionTests

- (void)testVersionNumber {
    XCTAssertEqual(1, MapboxVersionNumber);
}

@end
