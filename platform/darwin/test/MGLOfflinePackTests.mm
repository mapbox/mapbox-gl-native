#import <Mapbox/Mapbox.h>
#import <XCTest/XCTest.h>
#import "MGLOfflinePack_Private.h"
#import "MGLTestAssertionHandler.h"

@interface MGLOfflinePackTests : XCTestCase

@end

@implementation MGLOfflinePackTests

- (void)testInvalidation {
    MGLOfflinePack *invalidPack = [[MGLOfflinePack alloc] init];

    XCTAssertEqual(invalidPack.state, MGLOfflinePackStateInvalid, @"Offline pack should be invalid when initialized independently of MGLOfflineStorage.");

    XCTAssertThrowsSpecificNamed(invalidPack.region, NSException, MGLInvalidOfflinePackException, @"Invalid offline pack should raise an exception when accessing its region.");
    XCTAssertThrowsSpecificNamed(invalidPack.context, NSException, MGLInvalidOfflinePackException, @"Invalid offline pack should raise an exception when accessing its context.");
    XCTAssertThrowsSpecificNamed([invalidPack resume], NSException, MGLInvalidOfflinePackException, @"Invalid offline pack should raise an exception when being resumed.");
    XCTAssertThrowsSpecificNamed([invalidPack suspend], NSException, MGLInvalidOfflinePackException, @"Invalid offline pack should raise an exception when being suspended.");
}

- (void)testInvalidatingAnInvalidPack {
    MGLOfflinePack *invalidPack = [[MGLOfflinePack alloc] init];

    XCTAssertThrowsSpecificNamed([invalidPack invalidate], NSException, NSInternalInconsistencyException, @"Invalid offline pack should raise an exception when being invalidated.");

    // Now try again, without asserts
    NSAssertionHandler *oldHandler = [NSAssertionHandler currentHandler];
    MGLTestAssertionHandler *newHandler = [[MGLTestAssertionHandler alloc] initWithTestCase:self];
    [[[NSThread currentThread] threadDictionary] setValue:newHandler forKey:NSAssertionHandlerKey];

    // Make sure this doesn't crash without asserts
    [invalidPack invalidate];
    
    [[[NSThread currentThread] threadDictionary] setValue:oldHandler forKey:NSAssertionHandlerKey];
}

- (void)testProgressBoxing {
    MGLOfflinePackProgress progress = {
        .countOfResourcesCompleted = 3,
        .countOfResourcesExpected = 2,
        .countOfBytesCompleted = 7,
        .countOfTilesCompleted = 1,
        .countOfTileBytesCompleted = 6,
        .maximumResourcesExpected = UINT64_MAX,
    };
    MGLOfflinePackProgress roundTrippedProgress = [NSValue valueWithMGLOfflinePackProgress:progress].MGLOfflinePackProgressValue;

    XCTAssertEqual(progress.countOfResourcesCompleted, roundTrippedProgress.countOfResourcesCompleted, @"Completed resources should round-trip.");
    XCTAssertEqual(progress.countOfResourcesExpected, roundTrippedProgress.countOfResourcesExpected, @"Expected resources should round-trip.");
    XCTAssertEqual(progress.countOfBytesCompleted, roundTrippedProgress.countOfBytesCompleted, @"Completed bytes should round-trip.");
    XCTAssertEqual(progress.countOfTilesCompleted, roundTrippedProgress.countOfTilesCompleted, @"Completed tiles should round-trip.");
    XCTAssertEqual(progress.countOfTileBytesCompleted, roundTrippedProgress.countOfTileBytesCompleted, @"Completed tile bytes should round-trip.");
    XCTAssertEqual(progress.maximumResourcesExpected, roundTrippedProgress.maximumResourcesExpected, @"Maximum expected resources should round-trip.");
}

@end
