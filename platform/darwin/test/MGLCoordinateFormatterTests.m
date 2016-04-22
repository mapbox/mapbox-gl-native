#import <Mapbox/Mapbox.h>
#import <XCTest/XCTest.h>

@interface MGLCoordinateFormatterTests : XCTestCase

@end

@implementation MGLCoordinateFormatterTests

- (void)testStrings {
    MGLCoordinateFormatter *formatter = [[MGLCoordinateFormatter alloc] init];
    XCTAssertEqualObjects([formatter stringFromCoordinate:CLLocationCoordinate2DMake(38.9131982, -77.0325453144239)], @"38°54′48″N, 77°1′57″W");
}

@end
