#import <XCTest/XCTest.h>

#import "NSString+MGLAdditions.h"

@interface MGLNSStringAdditionsTests : XCTestCase

@end

@implementation MGLNSStringAdditionsTests

- (void)testTitleCasedString {
    NSLocale *locale = [NSLocale currentLocale];
    
    XCTAssertEqualObjects([@"© OpenStreetMap" mgl_titleCasedStringWithLocale:locale], @"© OpenStreetMap");
    XCTAssertEqualObjects([@"© OSM" mgl_titleCasedStringWithLocale:locale], @"© OSM");
    
    XCTAssertEqualObjects([@"Improve this map" mgl_titleCasedStringWithLocale:locale], @"Improve This Map");
    XCTAssertEqualObjects([@"Improve This Map" mgl_titleCasedStringWithLocale:locale], @"Improve This Map");
    
    XCTAssertEqualObjects([@"Improve the map" mgl_titleCasedStringWithLocale:locale], @"Improve the Map");
    XCTAssertEqualObjects([@"Improve The Map" mgl_titleCasedStringWithLocale:locale], @"Improve The Map");
    
    XCTAssertEqualObjects([@"Improve a map" mgl_titleCasedStringWithLocale:locale], @"Improve a Map");
    XCTAssertEqualObjects([@"Improve A Map" mgl_titleCasedStringWithLocale:locale], @"Improve A Map");
    
    XCTAssertEqualObjects([@"Improve for the map" mgl_titleCasedStringWithLocale:locale], @"Improve for the Map");
    XCTAssertEqualObjects([@"Improve For The Map" mgl_titleCasedStringWithLocale:locale], @"Improve For The Map");
    
    XCTAssertEqualObjects([@"Improve and map" mgl_titleCasedStringWithLocale:locale], @"Improve and Map");
    XCTAssertEqualObjects([@"Improve And Map" mgl_titleCasedStringWithLocale:locale], @"Improve And Map");
    
    XCTAssertEqualObjects([@"Improve while mapping" mgl_titleCasedStringWithLocale:locale], @"Improve While Mapping");
    XCTAssertEqualObjects([@"Improve While Mapping" mgl_titleCasedStringWithLocale:locale], @"Improve While Mapping");
    
    XCTAssertEqualObjects([@"Improve with the map" mgl_titleCasedStringWithLocale:locale], @"Improve With the Map");
    XCTAssertEqualObjects([@"Improve With The Map" mgl_titleCasedStringWithLocale:locale], @"Improve With The Map");
    
    XCTAssertEqualObjects([@"Improve this iPhone" mgl_titleCasedStringWithLocale:locale], @"Improve This iPhone");
    XCTAssertEqualObjects([@"Improve This iPhone" mgl_titleCasedStringWithLocale:locale], @"Improve This iPhone");
}

@end
