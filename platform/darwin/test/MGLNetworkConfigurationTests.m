#import <XCTest/XCTest.h>

#import "../../darwin/src/NSBundle+MGLAdditions.h"
#import "../../darwin/src/MGLNetworkConfiguration.h"

@interface MGLNetworkConfigurationTests : XCTestCase

@property (nonatomic) NSBundle *testBundle;

@end

@implementation MGLNetworkConfigurationTests

- (void)setUp {
    [super setUp];
    self.testBundle = [NSBundle bundleWithIdentifier:@"com.mapbox.test"];
    XCTAssertNil([self.testBundle objectForInfoDictionaryKey:@"MGLMapboxAPIBaseURL"]);
}

- (void)tearDown {
    [super tearDown];
    XCTAssertNil([self.testBundle objectForInfoDictionaryKey:@"MGLMapboxAPIBaseURL"]);
}

- (void)testNullableAPIBaseURL {
    XCTAssertNil([MGLNetworkConfiguration apiBaseURL]);
}

@end
