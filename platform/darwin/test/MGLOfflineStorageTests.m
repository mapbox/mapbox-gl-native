#import <Mapbox/Mapbox.h>

#import <XCTest/XCTest.h>

@interface MGLOfflineStorageTests : XCTestCase

@end

@implementation MGLOfflineStorageTests

- (void)testSharedObject {
    XCTAssertEqual([MGLOfflineStorage sharedOfflineStorage], [MGLOfflineStorage sharedOfflineStorage], @"There should only be one shared offline storage object.");
}

// This test needs to come first so it can test the initial loading of packs.
- (void)testAAALoadPacks {
    [self keyValueObservingExpectationForObject:[MGLOfflineStorage sharedOfflineStorage] keyPath:@"packs" handler:^BOOL(id _Nonnull observedObject, NSDictionary * _Nonnull change) {
        NSKeyValueChange changeKind = [change[NSKeyValueChangeKindKey] unsignedIntegerValue];
        return changeKind = NSKeyValueChangeSetting;
    }];
    
    [self waitForExpectationsWithTimeout:1 handler:nil];
    
    XCTAssertNotNil([MGLOfflineStorage sharedOfflineStorage].packs, @"Shared offline storage object should have a non-nil collection of packs by this point.");
}

- (void)testAddPack {
    NSUInteger countOfPacks = [MGLOfflineStorage sharedOfflineStorage].packs.count;
    
    NSURL *styleURL = [MGLStyle lightStyleURLWithVersion:8];
    /// Somewhere near Grape Grove, Ohio, United States.
    MGLCoordinateBounds bounds = {
        { .latitude = 39.70358155855172, .longitude = -83.69506472545841 },
        { .latitude = 39.703818870225376, .longitude = -83.69420641857361 },
    };
    double zoomLevel = 20;
    MGLTilePyramidOfflineRegion *region = [[MGLTilePyramidOfflineRegion alloc] initWithStyleURL:styleURL bounds:bounds fromZoomLevel:zoomLevel toZoomLevel:zoomLevel];
    
    NSString *nameKey = @"Name";
    NSString *name = @"🍇 Grape Grove";
    
    NSData *context = [NSKeyedArchiver archivedDataWithRootObject:@{
        nameKey: name,
    }];
    
    __block MGLOfflinePack *pack;
    [self keyValueObservingExpectationForObject:[MGLOfflineStorage sharedOfflineStorage] keyPath:@"packs" handler:^BOOL(id _Nonnull observedObject, NSDictionary * _Nonnull change) {
        NSKeyValueChange changeKind = [change[NSKeyValueChangeKindKey] unsignedIntegerValue];
        NSIndexSet *indices = change[NSKeyValueChangeIndexesKey];
        return changeKind == NSKeyValueChangeInsertion && indices.count == 1;
    }];
    XCTestExpectation *additionCompletionHandlerExpectation = [self expectationWithDescription:@"add pack completion handler"];
    [[MGLOfflineStorage sharedOfflineStorage] addPackForRegion:region withContext:context completionHandler:^(MGLOfflinePack * _Nullable completionHandlerPack, NSError * _Nullable error) {
        XCTAssertNotNil(completionHandlerPack, @"Added pack should exist.");
        XCTAssertEqual(completionHandlerPack.state, MGLOfflinePackStateInactive, @"New pack should initially have inactive state.");
        pack = completionHandlerPack;
        [additionCompletionHandlerExpectation fulfill];
    }];
    [self waitForExpectationsWithTimeout:1 handler:nil];
    
    XCTAssertEqual([MGLOfflineStorage sharedOfflineStorage].packs.count, countOfPacks + 1, @"Added pack should have been added to the canonical collection of packs owned by the shared offline storage object. This assertion can fail if this test is run before -testAAALoadPacks.");
    
    XCTAssertEqual(pack, [MGLOfflineStorage sharedOfflineStorage].packs.lastObject, @"Pack should be appended to end of packs array.");
    
    XCTAssertEqualObjects(pack.region, region, @"Added pack’s region has changed.");
    
    NSDictionary *userInfo = [NSKeyedUnarchiver unarchiveObjectWithData:pack.context];
    XCTAssert([userInfo isKindOfClass:[NSDictionary class]], @"Context of offline pack isn’t a dictionary.");
    XCTAssert([userInfo[nameKey] isKindOfClass:[NSString class]], @"Name of offline pack isn’t a string.");
    XCTAssertEqualObjects(userInfo[nameKey], name, @"Name of offline pack has changed.");
    
    XCTAssertEqual(pack.state, MGLOfflinePackStateInactive, @"New pack should initially have inactive state.");
    
    [self keyValueObservingExpectationForObject:pack keyPath:@"state" handler:^BOOL(id _Nonnull observedObject, NSDictionary * _Nonnull change) {
        NSKeyValueChange changeKind = [change[NSKeyValueChangeKindKey] unsignedIntegerValue];
        MGLOfflinePackState state = [change[NSKeyValueChangeNewKey] integerValue];
        return changeKind == NSKeyValueChangeSetting && state == MGLOfflinePackStateInactive;
    }];
    [self expectationForNotification:MGLOfflinePackProgressChangedNotification object:pack handler:^BOOL(NSNotification * _Nonnull notification) {
        MGLOfflinePack *notificationPack = notification.object;
        XCTAssert([notificationPack isKindOfClass:[MGLOfflinePack class]], @"Object of notification should be an MGLOfflinePack.");
        
        NSDictionary *userInfo = notification.userInfo;
        XCTAssertNotNil(userInfo, @"Progress change notification should have a userInfo dictionary.");
        
        NSNumber *stateNumber = userInfo[MGLOfflinePackStateUserInfoKey];
        XCTAssert([stateNumber isKindOfClass:[NSNumber class]], @"Progress change notification’s state should be an NSNumber.");
        XCTAssertEqual(stateNumber.integerValue, pack.state, @"State in a progress change notification should match the pack’s state.");
        
        NSValue *progressValue = userInfo[MGLOfflinePackProgressUserInfoKey];
        XCTAssert([progressValue isKindOfClass:[NSValue class]], @"Progress change notification’s progress should be an NSValue.");
        XCTAssertEqualObjects(progressValue, [NSValue valueWithMGLOfflinePackProgress:pack.progress], @"Progress change notification’s progress should match pack’s progress.");
        
        return notificationPack == pack && pack.state == MGLOfflinePackStateInactive;
    }];
    [pack requestProgress];
    [self waitForExpectationsWithTimeout:1 handler:nil];
}

- (void)testBackupExclusion {
    NSURL *cacheDirectoryURL = [[NSFileManager defaultManager] URLForDirectory:NSApplicationSupportDirectory
                                                                      inDomain:NSUserDomainMask
                                                             appropriateForURL:nil
                                                                        create:NO
                                                                         error:nil];
    // Unit tests don't use the main bundle; use com.mapbox.ios.sdk instead.
    NSString *bundleIdentifier = [NSBundle bundleForClass:[MGLMapView class]].bundleIdentifier;
    cacheDirectoryURL = [cacheDirectoryURL URLByAppendingPathComponent:bundleIdentifier];
    cacheDirectoryURL = [cacheDirectoryURL URLByAppendingPathComponent:@".mapbox"];
    XCTAssertTrue([[NSFileManager defaultManager] fileExistsAtPath:cacheDirectoryURL.path], @"Cache subdirectory should exist.");

    NSURL *cacheURL = [cacheDirectoryURL URLByAppendingPathComponent:@"cache.db"];
    XCTAssertTrue([[NSFileManager defaultManager] fileExistsAtPath:cacheURL.path], @"Cache database should exist.");

    NSError *error = nil;
    NSNumber *exclusionFlag = nil;
    [cacheDirectoryURL getResourceValue:&exclusionFlag
                                 forKey:NSURLIsExcludedFromBackupKey
                                  error:&error];
    XCTAssertTrue(exclusionFlag && [exclusionFlag boolValue], @"Backup exclusion flag should be set for the directory containing the cache database.");
    XCTAssertNil(error, @"No errors should be returned when checking backup exclusion flag.");
}

- (void)testRemovePack {
    NSUInteger countOfPacks = [MGLOfflineStorage sharedOfflineStorage].packs.count;
    
    MGLOfflinePack *pack = [MGLOfflineStorage sharedOfflineStorage].packs.lastObject;
    XCTAssertNotNil(pack, @"Added pack should still exist.");
    
    [self keyValueObservingExpectationForObject:[MGLOfflineStorage sharedOfflineStorage] keyPath:@"packs" handler:^BOOL(id _Nonnull observedObject, NSDictionary * _Nonnull change) {
        NSKeyValueChange changeKind = [change[NSKeyValueChangeKindKey] unsignedIntegerValue];
        NSIndexSet *indices = change[NSKeyValueChangeIndexesKey];
        return changeKind = NSKeyValueChangeRemoval && indices.count == 1;
    }];
    XCTestExpectation *completionHandlerExpectation = [self expectationWithDescription:@"remove pack completion handler"];
    [[MGLOfflineStorage sharedOfflineStorage] removePack:pack withCompletionHandler:^(NSError * _Nullable error) {
        XCTAssertEqual(pack.state, MGLOfflinePackStateInvalid, @"Removed pack should be invalid in the completion handler.");
        [completionHandlerExpectation fulfill];
    }];
    [self waitForExpectationsWithTimeout:1 handler:nil];
    
    XCTAssertEqual(pack.state, MGLOfflinePackStateInvalid, @"Removed pack should have been invalidated synchronously.");
    
    XCTAssertEqual([MGLOfflineStorage sharedOfflineStorage].packs.count, countOfPacks - 1, @"Removed pack should have been removed from the canonical collection of packs owned by the shared offline storage object. This assertion can fail if this test is run before -testAAALoadPacks or -testAddPack.");
}

@end
