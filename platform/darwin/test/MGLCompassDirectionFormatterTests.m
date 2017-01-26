#import <Mapbox/Mapbox.h>
#import <XCTest/XCTest.h>

@interface MGLCompassDirectionFormatterTests : XCTestCase

@end

@implementation MGLCompassDirectionFormatterTests

- (void)testCompassDirections {
    MGLCompassDirectionFormatter *shortFormatter = [[MGLCompassDirectionFormatter alloc] init];
    shortFormatter.unitStyle = NSFormattingUnitStyleShort;

    MGLCompassDirectionFormatter *mediumFormatter = [[MGLCompassDirectionFormatter alloc] init];
    XCTAssertEqual(mediumFormatter.unitStyle, NSFormattingUnitStyleMedium, @"Unit style should be medium by default.");

    MGLCompassDirectionFormatter *longFormatter = [[MGLCompassDirectionFormatter alloc] init];
    longFormatter.unitStyle = NSFormattingUnitStyleLong;

    CLLocationDirection direction;

    direction = -45;
    XCTAssertEqualObjects(@"NW", [shortFormatter stringFromDirection:direction]);
    XCTAssertEqualObjects(@"northwest", [mediumFormatter stringFromDirection:direction]);
    XCTAssertEqualObjects(@"northwest", [longFormatter stringFromDirection:direction]);

    direction = 0;
    XCTAssertEqualObjects(@"N", [shortFormatter stringFromDirection:direction]);
    XCTAssertEqualObjects(@"north", [mediumFormatter stringFromDirection:direction]);
    XCTAssertEqualObjects(@"north", [longFormatter stringFromDirection:direction]);

    direction = 1;
    XCTAssertEqualObjects(@"N", [shortFormatter stringFromDirection:direction]);
    XCTAssertEqualObjects(@"north", [mediumFormatter stringFromDirection:direction]);
    XCTAssertEqualObjects(@"north", [longFormatter stringFromDirection:direction]);

    direction = 10;
    XCTAssertEqualObjects(@"NbE", [shortFormatter stringFromDirection:direction]);
    XCTAssertEqualObjects(@"north by east", [mediumFormatter stringFromDirection:direction]);
    XCTAssertEqualObjects(@"north by east", [longFormatter stringFromDirection:direction]);

    direction = 20;
    XCTAssertEqualObjects(@"NNE", [shortFormatter stringFromDirection:direction]);
    XCTAssertEqualObjects(@"north-northeast", [mediumFormatter stringFromDirection:direction]);
    XCTAssertEqualObjects(@"north-northeast", [longFormatter stringFromDirection:direction]);

    direction = 45;
    XCTAssertEqualObjects(@"NE", [shortFormatter stringFromDirection:direction]);
    XCTAssertEqualObjects(@"northeast", [mediumFormatter stringFromDirection:direction]);
    XCTAssertEqualObjects(@"northeast", [longFormatter stringFromDirection:direction]);

    direction = 90;
    XCTAssertEqualObjects(@"E", [shortFormatter stringFromDirection:direction]);
    XCTAssertEqualObjects(@"east", [mediumFormatter stringFromDirection:direction]);
    XCTAssertEqualObjects(@"east", [longFormatter stringFromDirection:direction]);

    direction = 180;
    XCTAssertEqualObjects(@"S", [shortFormatter stringFromDirection:direction]);
    XCTAssertEqualObjects(@"south", [mediumFormatter stringFromDirection:direction]);
    XCTAssertEqualObjects(@"south", [longFormatter stringFromDirection:direction]);

    direction = 270;
    XCTAssertEqualObjects(@"W", [shortFormatter stringFromDirection:direction]);
    XCTAssertEqualObjects(@"west", [mediumFormatter stringFromDirection:direction]);
    XCTAssertEqualObjects(@"west", [longFormatter stringFromDirection:direction]);

    direction = 359.34951805867024;
    XCTAssertEqualObjects(@"N", [shortFormatter stringFromDirection:direction]);
    XCTAssertEqualObjects(@"north", [mediumFormatter stringFromDirection:direction]);
    XCTAssertEqualObjects(@"north", [longFormatter stringFromDirection:direction]);

    direction = 360;
    XCTAssertEqualObjects(@"N", [shortFormatter stringFromDirection:direction]);
    XCTAssertEqualObjects(@"north", [mediumFormatter stringFromDirection:direction]);
    XCTAssertEqualObjects(@"north", [longFormatter stringFromDirection:direction]);

    direction = 360.1;
    XCTAssertEqualObjects(@"N", [shortFormatter stringFromDirection:direction]);
    XCTAssertEqualObjects(@"north", [mediumFormatter stringFromDirection:direction]);
    XCTAssertEqualObjects(@"north", [longFormatter stringFromDirection:direction]);

    direction = 720;
    XCTAssertEqualObjects(@"N", [shortFormatter stringFromDirection:direction]);
    XCTAssertEqualObjects(@"north", [mediumFormatter stringFromDirection:direction]);
    XCTAssertEqualObjects(@"north", [longFormatter stringFromDirection:direction]);
}

@end
