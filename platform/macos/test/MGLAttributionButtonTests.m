#import <Mapbox/Mapbox.h>
#import <XCTest/XCTest.h>

#import "MGLAttributionButton.h"
#import "MGLAttributionInfo.h"

@interface MGLAttributionButtonTests : XCTestCase

@end

@implementation MGLAttributionButtonTests

- (void)testPlainSymbol {
    NSAttributedString *title = [[NSAttributedString alloc] initWithString:@"® & ™ Mapbox" attributes:@{
        NSUnderlineStyleAttributeName: @(NSUnderlineStyleSingle),
    }];
    MGLAttributionInfo *info = [[MGLAttributionInfo alloc] initWithTitle:title URL:nil];
    MGLAttributionButton *button = [[MGLAttributionButton alloc] initWithAttributionInfo:info];

    NSRange symbolUnderlineRange;
    NSNumber *symbolUnderline = [button.attributedTitle attribute:NSUnderlineStyleAttributeName atIndex:0 effectiveRange:&symbolUnderlineRange];
    XCTAssertNil(symbolUnderline);
    XCTAssertEqual(symbolUnderlineRange.length, 6);

    NSRange wordUnderlineRange;
    NSNumber *wordUnderline = [button.attributedTitle attribute:NSUnderlineStyleAttributeName atIndex:6 effectiveRange:&wordUnderlineRange];
    XCTAssertEqualObjects(wordUnderline, @(NSUnderlineStyleSingle));
    XCTAssertEqual(wordUnderlineRange.length, 6);
}

@end
