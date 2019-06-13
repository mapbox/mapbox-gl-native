#import <XCTest/XCTest.h>
#import <Mapbox/Mapbox.h>

@interface MBXAccounts: NSObject
@property (class, nonatomic, readonly) NSString *skuToken;
@end

@implementation MBXAccounts

+ (NSString *)skuToken {
    return @"foo";
}

@end

@interface MGLAccountManagerTests : XCTestCase
@end

@implementation MGLAccountManagerTests

- (void)testSKU {
    XCTAssertTrue([[MGLAccountManager valueForKeyPath:@"skuToken"] isEqualToString:@"foo"]);
}

@end
