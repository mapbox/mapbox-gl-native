#import <Mapbox/Mapbox.h>
#import <XCTest/XCTest.h>

#import "MGLAttributionInfo.h"

@interface MGLAttributionInfoTests : XCTestCase

@end

@implementation MGLAttributionInfoTests

- (void)testParsing {
    static NSString * const htmlStrings[] = {
        @"<a href=\"https://www.mapbox.com/about/maps/\" target=\"_blank\">&copy; Mapbox</a> "
        @"<a href=\"http://www.openstreetmap.org/about/\" target=\"_blank\">&copy; OpenStreetMap</a> "
        @"CC&nbsp;BY-SA "
        @"<a class=\"mapbox-improve-map\" href=\"https://www.mapbox.com/map-feedback/\" target=\"_blank\">Improve this map</a>",
    };
    
    NS_MUTABLE_ARRAY_OF(MGLAttributionInfo *) *infos = [NSMutableArray array];
    for (NSUInteger i = 0; i < sizeof(htmlStrings) / sizeof(htmlStrings[0]); i++) {
        NSArray *subinfos = [MGLAttributionInfo attributionInfosFromHTMLString:htmlStrings[i]
                                                                      fontSize:0
                                                                     linkColor:nil];
        [infos growArrayByAddingAttributionInfosFromArray:subinfos];
    }
    
    XCTAssertEqual(infos.count, 3);
    
    XCTAssertEqualObjects(infos[0].title.string, @"© Mapbox");
    XCTAssertEqualObjects(infos[0].URL, [NSURL URLWithString:@"https://www.mapbox.com/about/maps/"]);
    
    XCTAssertEqualObjects(infos[1].title.string, @"© OpenStreetMap");
    XCTAssertEqualObjects(infos[1].URL, [NSURL URLWithString:@"http://www.openstreetmap.org/about/"]);
    
    XCTAssertEqualObjects(infos[2].title.string, @"CC\u00a0BY-SA");
    XCTAssertNil(infos[2].URL);
}

- (void)testDedupe {
    static NSString * const htmlStrings[] = {
        @"World",
        @"Hello World",
        @"Another Source",
        @"Hello",
        @"Hello World",
    };
    
    NS_MUTABLE_ARRAY_OF(MGLAttributionInfo *) *infos = [NSMutableArray array];
    for (NSUInteger i = 0; i < sizeof(htmlStrings) / sizeof(htmlStrings[0]); i++) {
        NSArray *subinfos = [MGLAttributionInfo attributionInfosFromHTMLString:htmlStrings[i]
                                                                      fontSize:0
                                                                     linkColor:nil];
        [infos growArrayByAddingAttributionInfosFromArray:subinfos];
    }
    
    XCTAssertEqual(infos.count, 2);
    XCTAssertEqualObjects(infos[0].title.string, @"Hello World");
    XCTAssertEqualObjects(infos[1].title.string, @"Another Source");
}

@end
