#import <Mapbox/Mapbox.h>

#pragma clang diagnostic ignored "-Wgnu-statement-expression"
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"

#import <XCTest/XCTest.h>

@interface MGLOfflinePackTests : XCTestCase

@end

@implementation MGLOfflinePackTests

- (void)testInvalidation {
    MGLOfflinePack *invalidPack = [[MGLOfflinePack alloc] init];
    
    XCTAssertEqual(invalidPack.state, MGLOfflinePackStateInvalid, @"Offline pack should be invalid when initialized independently of MGLOfflineStorage.");
    
    XCTAssertThrowsSpecificNamed(invalidPack.region, NSException, @"Invalid offline pack", @"Invalid offline pack should raise an exception when accessing its region.");
    XCTAssertThrowsSpecificNamed(invalidPack.context, NSException, @"Invalid offline pack", @"Invalid offline pack should raise an exception when accessing its context.");
    XCTAssertThrowsSpecificNamed([invalidPack resume], NSException, @"Invalid offline pack", @"Invalid offline pack should raise an exception when being resumed.");
    XCTAssertThrowsSpecificNamed([invalidPack suspend], NSException, @"Invalid offline pack", @"Invalid offline pack should raise an exception when being suspended.");
}

@end
