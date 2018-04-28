#import <Mapbox/Mapbox.h>
#import <XCTest/XCTest.h>

@interface MGLCoordinateFormatterTests : XCTestCase

@end

@implementation MGLCoordinateFormatterTests

- (void)testStrings {
    MGLCoordinateFormatter *shortFormatter = [[MGLCoordinateFormatter alloc] init];
    shortFormatter.unitStyle = NSFormattingUnitStyleShort;
    XCTAssertTrue(shortFormatter.allowsSeconds, @"Arcseconds should be allowed by default.");
    XCTAssertTrue(shortFormatter.allowsMinutes, @"Arcminutes should be allowed by default.");

    MGLCoordinateFormatter *mediumFormatter = [[MGLCoordinateFormatter alloc] init];
    XCTAssertEqual(mediumFormatter.unitStyle, NSFormattingUnitStyleMedium, @"Unit style should be medium by default.");

    MGLCoordinateFormatter *longFormatter = [[MGLCoordinateFormatter alloc] init];
    longFormatter.unitStyle = NSFormattingUnitStyleLong;

    CLLocationCoordinate2D coordinate;

    coordinate = CLLocationCoordinate2DMake(38.9131982, -77.0325453144239);
    XCTAssertEqualObjects([shortFormatter stringFromCoordinate:coordinate], @"38°54′48″N, 77°1′57″W");
    XCTAssertEqualObjects([mediumFormatter stringFromCoordinate:coordinate], @"38°54′48″ north, 77°1′57″ west");
    XCTAssertEqualObjects([longFormatter stringFromCoordinate:coordinate], @"38 degrees, 54 minutes, and 48 seconds north by 77 degrees, 1 minute, and 57 seconds west");

    shortFormatter.allowsSeconds = NO;
    mediumFormatter.allowsSeconds = NO;
    longFormatter.allowsSeconds = NO;

    coordinate = CLLocationCoordinate2DMake(38.9131982, -77.0325453144239);
    XCTAssertEqualObjects([shortFormatter stringFromCoordinate:coordinate], @"38°55′N, 77°2′W");
    XCTAssertEqualObjects([mediumFormatter stringFromCoordinate:coordinate], @"38°55′ north, 77°2′ west");
    XCTAssertEqualObjects([longFormatter stringFromCoordinate:coordinate], @"38 degrees and 55 minutes north by 77 degrees and 2 minutes west");

    shortFormatter.allowsMinutes = NO;
    mediumFormatter.allowsMinutes = NO;
    longFormatter.allowsMinutes = NO;

    coordinate = CLLocationCoordinate2DMake(38.9131982, -77.0325453144239);
    XCTAssertEqualObjects([shortFormatter stringFromCoordinate:coordinate], @"39°N, 77°W");
    XCTAssertEqualObjects([mediumFormatter stringFromCoordinate:coordinate], @"39° north, 77° west");
    XCTAssertEqualObjects([longFormatter stringFromCoordinate:coordinate], @"39 degrees north by 77 degrees west");
}

@end
