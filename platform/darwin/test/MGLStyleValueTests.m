#import <XCTest/XCTest.h>
#import <Mapbox/Mapbox.h>

@interface MGLStyleValueTests : XCTestCase
@end

@implementation MGLStyleValueTests

- (void)testStoplessFunction {
    XCTAssertThrowsSpecificNamed([MGLStyleValue<NSNumber *> valueWithStops:@{}], NSException, NSInvalidArgumentException, @"Stopless function should raise an exception");
}

@end
