#import "MGLStyle.h"

#import <mbgl/util/default_styles.hpp>

#import <XCTest/XCTest.h>

@interface MGLStyleTests : XCTestCase
@end

@implementation MGLStyleTests

- (void)testStyleURLs {
    // Test that all the default styles have publicly-declared MGLStyle class
    // methods and that the URLs are all well-formed.
    XCTAssertEqualObjects([MGLStyle streetsStyleURL].absoluteString, @(mbgl::util::default_styles::streets.url));
    XCTAssertEqualObjects([MGLStyle emeraldStyleURL].absoluteString, @(mbgl::util::default_styles::emerald.url));
    XCTAssertEqualObjects([MGLStyle lightStyleURL].absoluteString, @(mbgl::util::default_styles::light.url));
    XCTAssertEqualObjects([MGLStyle darkStyleURL].absoluteString, @(mbgl::util::default_styles::dark.url));
    XCTAssertEqualObjects([MGLStyle satelliteStyleURL].absoluteString, @(mbgl::util::default_styles::satellite.url));
    XCTAssertEqualObjects([MGLStyle hybridStyleURL].absoluteString, @(mbgl::util::default_styles::hybrid.url));
    
    static_assert(6 == mbgl::util::default_styles::numOrderedStyles,
                  "MGLStyleTests isn’t testing all the styles in mbgl::util::default_styles.");
}

@end
