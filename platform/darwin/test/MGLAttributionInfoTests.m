#import <Mapbox/Mapbox.h>
#import <XCTest/XCTest.h>

#import "MGLAttributionInfo.h"

static NSString * const HTMLAttributionStrings[] = {
    @"World",
    @"Hello World",
    @"Another Source",
    @"Hello",
    @"Hello World",
};

@interface MGLAttributionInfoTests : XCTestCase

@end

@implementation MGLAttributionInfoTests

- (void)testDedupe {
    NS_MUTABLE_ARRAY_OF(MGLAttributionInfo *) *infos = [NSMutableArray array];
    for (NSUInteger i = 0; i < sizeof(HTMLAttributionStrings) / sizeof(HTMLAttributionStrings[0]); i++) {
        NSArray *subinfos = [MGLAttributionInfo attributionInfosFromHTMLString:HTMLAttributionStrings[i]
                                                                      fontSize:0
                                                                     linkColor:nil];
        [infos growArrayByAddingAttributionInfosFromArray:subinfos];
    }
    
    XCTAssertEqual(infos.count, 2);
    XCTAssertEqualObjects(infos[0].title.string, @"Hello World");
    XCTAssertEqualObjects(infos[1].title.string, @"Another Source");
}

@end
