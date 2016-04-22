#import <Mapbox/Mapbox.h>
#import <XCTest/XCTest.h>

static NSString * const MGLTestLocaleIdentifier = @"en-US";

@interface MGLDirectionFormatterTests : XCTestCase

@end

@implementation MGLDirectionFormatterTests

- (void)testAbsoluteDirections {
    MGLDirectionFormatter *shortFormatter = [[MGLDirectionFormatter alloc] init];
    XCTAssertEqual(shortFormatter.origin, MGLDirectionFormatterOriginNorth, @"Reference point should be north by default.");
    shortFormatter.unitStyle = NSFormattingUnitStyleShort;
    shortFormatter.locale = [NSLocale localeWithLocaleIdentifier:MGLTestLocaleIdentifier];
    
    MGLDirectionFormatter *mediumFormatter = [[MGLDirectionFormatter alloc] init];
    XCTAssertEqual(mediumFormatter.unitStyle, NSFormattingUnitStyleMedium, @"Unit style should be medium by default.");
    mediumFormatter.locale = [NSLocale localeWithLocaleIdentifier:MGLTestLocaleIdentifier];
    
    MGLDirectionFormatter *longFormatter = [[MGLDirectionFormatter alloc] init];
    longFormatter.unitStyle = NSFormattingUnitStyleLong;
    longFormatter.locale = [NSLocale localeWithLocaleIdentifier:MGLTestLocaleIdentifier];
    
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

- (void)testRelativeDirections {
    MGLDirectionFormatter *shortFormatter = [[MGLDirectionFormatter alloc] init];
    shortFormatter.origin = MGLDirectionFormatterOriginStraightAhead;
    shortFormatter.unitStyle = NSFormattingUnitStyleShort;
    shortFormatter.locale = [NSLocale localeWithLocaleIdentifier:MGLTestLocaleIdentifier];
    
    MGLDirectionFormatter *mediumFormatter = [[MGLDirectionFormatter alloc] init];
    mediumFormatter.origin = MGLDirectionFormatterOriginStraightAhead;
    mediumFormatter.locale = [NSLocale localeWithLocaleIdentifier:MGLTestLocaleIdentifier];
    
    MGLDirectionFormatter *longFormatter = [[MGLDirectionFormatter alloc] init];
    longFormatter.origin = MGLDirectionFormatterOriginStraightAhead;
    longFormatter.unitStyle = NSFormattingUnitStyleLong;
    longFormatter.locale = [NSLocale localeWithLocaleIdentifier:MGLTestLocaleIdentifier];
    
    XCTAssertEqualObjects(@"9:00", [shortFormatter stringFromDirection:-90]);
    XCTAssertEqualObjects(@"9 o’clock", [mediumFormatter stringFromDirection:-90]);
    XCTAssertEqualObjects(@"nine o’clock", [longFormatter stringFromDirection:-90]);
    
    XCTAssertEqualObjects(@"12:00", [shortFormatter stringFromDirection:0]);
    XCTAssertEqualObjects(@"12 o’clock", [mediumFormatter stringFromDirection:0]);
    XCTAssertEqualObjects(@"twelve o’clock", [longFormatter stringFromDirection:0]);
    
    XCTAssertEqualObjects(@"2:00", [shortFormatter stringFromDirection:45]);
    XCTAssertEqualObjects(@"2 o’clock", [mediumFormatter stringFromDirection:45]);
    XCTAssertEqualObjects(@"two o’clock", [longFormatter stringFromDirection:45]);
    
    XCTAssertEqualObjects(@"3:00", [shortFormatter stringFromDirection:90]);
    XCTAssertEqualObjects(@"3 o’clock", [mediumFormatter stringFromDirection:90]);
    XCTAssertEqualObjects(@"three o’clock", [longFormatter stringFromDirection:90]);
    
    XCTAssertEqualObjects(@"6:00", [shortFormatter stringFromDirection:180]);
    XCTAssertEqualObjects(@"6 o’clock", [mediumFormatter stringFromDirection:180]);
    XCTAssertEqualObjects(@"six o’clock", [longFormatter stringFromDirection:180]);
    
    XCTAssertEqualObjects(@"9:00", [shortFormatter stringFromDirection:270]);
    XCTAssertEqualObjects(@"9 o’clock", [mediumFormatter stringFromDirection:270]);
    XCTAssertEqualObjects(@"nine o’clock", [longFormatter stringFromDirection:270]);
}

@end
