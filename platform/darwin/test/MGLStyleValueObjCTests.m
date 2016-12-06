#import <XCTest/XCTest.h>
#import <Mapbox/Mapbox.h>

@interface MGLStyleValueObjCTests : XCTestCase
@end

@implementation MGLStyleValueObjCTests

- (void)testStoplessFunction {
    XCTAssertThrowsSpecificNamed([MGLStyleValue<NSNumber *> valueWithStops:@{}], NSException, @"NSInvalidArgumentException", @"Stopless function should raise an exception");
}

@end
