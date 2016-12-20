#import <Mapbox/Mapbox.h>
#import <XCTest/XCTest.h>

#import "MGLAttributionInfo_Private.h"

@interface MGLAttributionInfoTests : XCTestCase

@end

@implementation MGLAttributionInfoTests

- (void)testParsing {
    static NSString * const htmlStrings[] = {
        @"<a href=\"https://www.mapbox.com/about/maps/\" target=\"_blank\">&copy; Mapbox</a> "
        @"<a href=\"http://www.openstreetmap.org/about/\" target=\"_blank\">©️ OpenStreetMap</a> "
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
    
    XCTAssertEqual(infos.count, 4);
    
    CLLocationCoordinate2D mapbox = CLLocationCoordinate2DMake(12.9810816, 77.6368034);
    XCTAssertEqualObjects(infos[0].title.string, @"© Mapbox");
    XCTAssertEqualObjects(infos[0].URL, [NSURL URLWithString:@"https://www.mapbox.com/about/maps/"]);
    XCTAssertFalse(infos[0].feedbackLink);
    XCTAssertNil([infos[0] feedbackURLAtCenterCoordinate:mapbox zoomLevel:14]);
    
    XCTAssertEqualObjects(infos[1].title.string, @"©️ OpenStreetMap");
    XCTAssertEqualObjects(infos[1].URL, [NSURL URLWithString:@"http://www.openstreetmap.org/about/"]);
    XCTAssertFalse(infos[1].feedbackLink);
    XCTAssertNil([infos[1] feedbackURLAtCenterCoordinate:mapbox zoomLevel:14]);
    
    XCTAssertEqualObjects(infos[2].title.string, @"CC\u00a0BY-SA");
    XCTAssertNil(infos[2].URL);
    XCTAssertFalse(infos[2].feedbackLink);
    XCTAssertNil([infos[2] feedbackURLAtCenterCoordinate:mapbox zoomLevel:14]);
    
    XCTAssertEqualObjects(infos[3].title.string, @"Improve this map");
    XCTAssertEqualObjects(infos[3].URL, [NSURL URLWithString:@"https://www.mapbox.com/map-feedback/"]);
    XCTAssertTrue(infos[3].feedbackLink);
    XCTAssertEqualObjects([infos[3] feedbackURLAtCenterCoordinate:mapbox zoomLevel:14],
                          [NSURL URLWithString:@"https://www.mapbox.com/map-feedback/#/77.63680/12.98108/15"]);
}

- (void)testStyle {
    static NSString * const htmlStrings[] = {
        @"<a href=\"https://www.mapbox.com/\">Mapbox</a>",
    };
    
    CGFloat fontSize = 72;
    MGLColor *color = [MGLColor redColor];
    NS_MUTABLE_ARRAY_OF(MGLAttributionInfo *) *infos = [NSMutableArray array];
    for (NSUInteger i = 0; i < sizeof(htmlStrings) / sizeof(htmlStrings[0]); i++) {
        NSArray *subinfos = [MGLAttributionInfo attributionInfosFromHTMLString:htmlStrings[i]
                                                                      fontSize:72
                                                                     linkColor:color];
        [infos growArrayByAddingAttributionInfosFromArray:subinfos];
    }
    
    XCTAssertEqual(infos.count, 1);
    
    XCTAssertEqualObjects(infos[0].title.string, @"Mapbox");
    XCTAssertEqualObjects([infos[0].title attribute:NSLinkAttributeName atIndex:0 effectiveRange:nil], [NSURL URLWithString:@"https://www.mapbox.com/"]);
    XCTAssertEqualObjects([infos[0].title attribute:NSUnderlineStyleAttributeName atIndex:0 effectiveRange:nil], @(NSUnderlineStyleSingle));
    
#if TARGET_OS_IPHONE
    UIFont *font;
#else
    NSFont *font;
#endif
    font = [infos[0].title attribute:NSFontAttributeName atIndex:0 effectiveRange:nil];
    XCTAssertEqual(font.pointSize, fontSize);
    
    CGFloat r, g, b, a;
    [color getRed:&r green:&g blue:&b alpha:&a];
    MGLColor *linkColor = [infos[0].title attribute:NSForegroundColorAttributeName atIndex:0 effectiveRange:nil];
    CGFloat linkR, linkG, linkB, linkA;
    [linkColor getRed:&linkR green:&linkG blue:&linkB alpha:&linkA];
    XCTAssertEqual(r, linkR);
    XCTAssertEqual(g, linkG);
    XCTAssertEqual(b, linkB);
    XCTAssertEqual(a, linkA);
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
