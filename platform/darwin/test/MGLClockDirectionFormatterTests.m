#import <Mapbox/Mapbox.h>
#import <XCTest/XCTest.h>

static NSString * const MGLTestLocaleIdentifier = @"en-US";

@interface MGLClockDirectionFormatterTests : XCTestCase

@end

@implementation MGLClockDirectionFormatterTests

- (void)testClockDirections {
    MGLClockDirectionFormatter *shortFormatter = [[MGLClockDirectionFormatter alloc] init];
    shortFormatter.unitStyle = NSFormattingUnitStyleShort;
    shortFormatter.locale = [NSLocale localeWithLocaleIdentifier:MGLTestLocaleIdentifier];
    
    MGLClockDirectionFormatter *mediumFormatter = [[MGLClockDirectionFormatter alloc] init];
    mediumFormatter.locale = [NSLocale localeWithLocaleIdentifier:MGLTestLocaleIdentifier];
    
    MGLClockDirectionFormatter *longFormatter = [[MGLClockDirectionFormatter alloc] init];
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
