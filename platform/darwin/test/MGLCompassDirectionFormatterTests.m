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
    
    XCTAssertEqualObjects(@"NW", [shortFormatter stringFromDirection:-45]);
    XCTAssertEqualObjects(@"northwest", [mediumFormatter stringFromDirection:-45]);
    XCTAssertEqualObjects([mediumFormatter stringFromDirection:-45], [longFormatter stringFromDirection:-45]);
    
    XCTAssertEqualObjects(@"N", [shortFormatter stringFromDirection:0]);
    XCTAssertEqualObjects(@"north", [mediumFormatter stringFromDirection:0]);
    XCTAssertEqualObjects([mediumFormatter stringFromDirection:0], [longFormatter stringFromDirection:0]);
    
    XCTAssertEqualObjects(@"N", [shortFormatter stringFromDirection:1]);
    XCTAssertEqualObjects(@"north", [mediumFormatter stringFromDirection:1]);
    XCTAssertEqualObjects([mediumFormatter stringFromDirection:1], [longFormatter stringFromDirection:1]);
    
    XCTAssertEqualObjects(@"N×E", [shortFormatter stringFromDirection:10]);
    XCTAssertEqualObjects(@"north by east", [mediumFormatter stringFromDirection:10]);
    XCTAssertEqualObjects([mediumFormatter stringFromDirection:10], [longFormatter stringFromDirection:10]);
    
    XCTAssertEqualObjects(@"NNE", [shortFormatter stringFromDirection:20]);
    XCTAssertEqualObjects(@"north-northeast", [mediumFormatter stringFromDirection:20]);
    XCTAssertEqualObjects([mediumFormatter stringFromDirection:20], [longFormatter stringFromDirection:20]);
    
    XCTAssertEqualObjects(@"NE", [shortFormatter stringFromDirection:45]);
    XCTAssertEqualObjects(@"northeast", [mediumFormatter stringFromDirection:45]);
    XCTAssertEqualObjects([mediumFormatter stringFromDirection:45], [longFormatter stringFromDirection:45]);
    
    XCTAssertEqualObjects(@"E", [shortFormatter stringFromDirection:90]);
    XCTAssertEqualObjects(@"east", [mediumFormatter stringFromDirection:90]);
    XCTAssertEqualObjects([mediumFormatter stringFromDirection:90], [longFormatter stringFromDirection:90]);
    
    XCTAssertEqualObjects(@"S", [shortFormatter stringFromDirection:180]);
    XCTAssertEqualObjects(@"south", [mediumFormatter stringFromDirection:180]);
    XCTAssertEqualObjects([mediumFormatter stringFromDirection:180], [longFormatter stringFromDirection:180]);
    
    XCTAssertEqualObjects(@"W", [shortFormatter stringFromDirection:270]);
    XCTAssertEqualObjects(@"west", [mediumFormatter stringFromDirection:270]);
    XCTAssertEqualObjects([mediumFormatter stringFromDirection:270], [longFormatter stringFromDirection:270]);
    
    XCTAssertEqualObjects(@"N", [shortFormatter stringFromDirection:360]);
    XCTAssertEqualObjects(@"north", [mediumFormatter stringFromDirection:360]);
    XCTAssertEqualObjects([mediumFormatter stringFromDirection:360], [longFormatter stringFromDirection:360]);
    
    XCTAssertEqualObjects(@"N", [shortFormatter stringFromDirection:720]);
    XCTAssertEqualObjects(@"north", [mediumFormatter stringFromDirection:720]);
    XCTAssertEqualObjects([mediumFormatter stringFromDirection:720], [longFormatter stringFromDirection:720]);
}

@end
