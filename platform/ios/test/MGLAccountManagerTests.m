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

#if TARGET_OS_IOS
- (void)testSKU {
    XCTAssertTrue([[MGLAccountManager valueForKeyPath:@"skuToken"] isEqualToString:@"foo"]);
}
#endif

@end
