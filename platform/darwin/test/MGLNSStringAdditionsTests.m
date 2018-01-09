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

- (void)testTransliteratedString {
    if (@available(iOS 9.0, *)) {
        XCTAssertEqualObjects([@"Portland" mgl_stringByTransliteratingIntoScript:@"Latn"], @"Portland");
        XCTAssertEqualObjects([@"Portland" mgl_stringByTransliteratingIntoScript:@"Hans"], @"Portland");
        XCTAssertEqualObjects([@"Portland" mgl_stringByTransliteratingIntoScript:@"Cyrl"], @"Портланд");
        XCTAssertEqualObjects([@"Portland" mgl_stringByTransliteratingIntoScript:@"Arab"], @"پُرتلَند");
        XCTAssertEqualObjects([@"Portland" mgl_stringByTransliteratingIntoScript:@"Fake"], @"Portland");

        XCTAssertEqualObjects([@"北京" mgl_stringByTransliteratingIntoScript:@"Latn"], @"běi jīng");
        XCTAssertEqualObjects([@"北京" mgl_stringByTransliteratingIntoScript:@"Hans"], @"北京");
        XCTAssertEqualObjects([@"北京" mgl_stringByTransliteratingIntoScript:@"Cyrl"], @"бе̌и йӣнг");
        XCTAssertEqualObjects([@"北京" mgl_stringByTransliteratingIntoScript:@"Arab"], @"بِِ̌ جِينگ");
        XCTAssertEqualObjects([@"北京" mgl_stringByTransliteratingIntoScript:@"Fake"], @"北京");

        XCTAssertEqualObjects([@"Mосква" mgl_stringByTransliteratingIntoScript:@"Latn"], @"Moskva");
        XCTAssertEqualObjects([@"Mосква" mgl_stringByTransliteratingIntoScript:@"Hans"], @"Mосква");
        XCTAssertEqualObjects([@"Mосква" mgl_stringByTransliteratingIntoScript:@"Cyrl"], @"Москва");
        XCTAssertEqualObjects([@"Mосква" mgl_stringByTransliteratingIntoScript:@"Arab"], @"مُسكڤَ");
        XCTAssertEqualObjects([@"Mосква" mgl_stringByTransliteratingIntoScript:@"Fake"], @"Mосква");

        XCTAssertEqualObjects([@"ロンドン" mgl_stringByTransliteratingIntoScript:@"Latn"], @"rondon");
        XCTAssertEqualObjects([@"ロンドン" mgl_stringByTransliteratingIntoScript:@"Hans"], @"ロンドン");
        XCTAssertEqualObjects([@"ロンドン" mgl_stringByTransliteratingIntoScript:@"Cyrl"], @"рондон");
        XCTAssertEqualObjects([@"ロンドン" mgl_stringByTransliteratingIntoScript:@"Arab"], @"رُندُن");
        XCTAssertEqualObjects([@"ロンドン" mgl_stringByTransliteratingIntoScript:@"Fake"], @"ロンドン");
    } else {
        XCTAssertEqualObjects([@"Made-up Place" mgl_stringByTransliteratingIntoScript:@"Latn"], @"Made-up Place");
        XCTAssertEqualObjects([@"Made-up Place" mgl_stringByTransliteratingIntoScript:@"Hans"], @"Made-up Place");
        XCTAssertEqualObjects([@"Made-up Place" mgl_stringByTransliteratingIntoScript:@"Cyrl"], @"Made-up Place");
        XCTAssertEqualObjects([@"Made-up Place" mgl_stringByTransliteratingIntoScript:@"Arab"], @"Made-up Place");
        XCTAssertEqualObjects([@"Made-up Place" mgl_stringByTransliteratingIntoScript:@"Fake"], @"Made-up Place");
    }
}

@end
