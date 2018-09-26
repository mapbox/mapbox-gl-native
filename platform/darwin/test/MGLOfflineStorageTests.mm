#import <Mapbox/Mapbox.h>

#import "MGLOfflineStorage_Private.h"

#import <XCTest/XCTest.h>

#include <mbgl/util/run_loop.hpp>

@interface MGLOfflineStorageTests : XCTestCase <MGLOfflineStorageDelegate>

@end

@implementation MGLOfflineStorageTests

+ (void)tearDown {
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
    
    [[NSFileManager defaultManager] removeItemAtURL:cacheURL error:nil];
    XCTAssertFalse([[NSFileManager defaultManager] fileExistsAtPath:cacheURL.path], @"Cache subdirectory should not exist.");
}

- (void)setUp {
    [super setUp];

    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        XCTestExpectation *expectation = [self keyValueObservingExpectationForObject:[MGLOfflineStorage sharedOfflineStorage] keyPath:@"packs" handler:^BOOL(id _Nonnull observedObject, NSDictionary * _Nonnull change) {
            const auto changeKind = static_cast<NSKeyValueChange>([change[NSKeyValueChangeKindKey] unsignedLongValue]);
            return changeKind == NSKeyValueChangeSetting;
        }];
        if ([MGLOfflineStorage sharedOfflineStorage].packs) {
            [expectation fulfill];
            [self waitForExpectationsWithTimeout:0 handler:nil];
        } else {
            [self waitForExpectationsWithTimeout:10 handler:nil];
        }

        XCTAssertNotNil([MGLOfflineStorage sharedOfflineStorage].packs, @"Shared offline storage object should have a non-nil collection of packs by this point.");
    });
}

- (void)testSharedObject {
    XCTAssertEqual([MGLOfflineStorage sharedOfflineStorage], [MGLOfflineStorage sharedOfflineStorage], @"There should only be one shared offline storage object.");
}

- (void)testAddPackForBounds {
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
        const auto changeKind = static_cast<NSKeyValueChange>([change[NSKeyValueChangeKindKey] unsignedLongValue]);
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
    [self waitForExpectationsWithTimeout:5 handler:nil];

    XCTAssertEqual([MGLOfflineStorage sharedOfflineStorage].packs.count, countOfPacks + 1, @"Added pack should have been added to the canonical collection of packs owned by the shared offline storage object. This assertion can fail if this test is run before -testAAALoadPacks.");

    XCTAssertEqual(pack, [MGLOfflineStorage sharedOfflineStorage].packs.lastObject, @"Pack should be appended to end of packs array.");

    XCTAssertEqualObjects(pack.region, region, @"Added pack’s region has changed.");

    NSDictionary *userInfo = [NSKeyedUnarchiver unarchiveObjectWithData:pack.context];
    XCTAssert([userInfo isKindOfClass:[NSDictionary class]], @"Context of offline pack isn’t a dictionary.");
    XCTAssert([userInfo[nameKey] isKindOfClass:[NSString class]], @"Name of offline pack isn’t a string.");
    XCTAssertEqualObjects(userInfo[nameKey], name, @"Name of offline pack has changed.");

    XCTAssertEqual(pack.state, MGLOfflinePackStateInactive, @"New pack should initially have inactive state.");

    [self keyValueObservingExpectationForObject:pack keyPath:@"state" handler:^BOOL(id _Nonnull observedObject, NSDictionary * _Nonnull change) {
        const auto changeKind = static_cast<NSKeyValueChange>([change[NSKeyValueChangeKindKey] unsignedLongValue]);
        const auto state = static_cast<MGLOfflinePackState>([change[NSKeyValueChangeNewKey] longValue]);
        return changeKind == NSKeyValueChangeSetting && state == MGLOfflinePackStateInactive;
    }];
    [self expectationForNotification:MGLOfflinePackProgressChangedNotification object:pack handler:^BOOL(NSNotification * _Nonnull notification) {
        MGLOfflinePack *notificationPack = notification.object;
        XCTAssert([notificationPack isKindOfClass:[MGLOfflinePack class]], @"Object of notification should be an MGLOfflinePack.");

        NSDictionary *userInfo = notification.userInfo;
        XCTAssertNotNil(userInfo, @"Progress change notification should have a userInfo dictionary.");

        NSNumber *stateNumber = userInfo[MGLOfflinePackUserInfoKeyState];
        XCTAssert([stateNumber isKindOfClass:[NSNumber class]], @"Progress change notification’s state should be an NSNumber.");
        XCTAssertEqual(stateNumber.integerValue, pack.state, @"State in a progress change notification should match the pack’s state.");

        NSValue *progressValue = userInfo[MGLOfflinePackUserInfoKeyProgress];
        XCTAssert([progressValue isKindOfClass:[NSValue class]], @"Progress change notification’s progress should be an NSValue.");
        XCTAssertEqualObjects(progressValue, [NSValue valueWithMGLOfflinePackProgress:pack.progress], @"Progress change notification’s progress should match pack’s progress.");

        return notificationPack == pack && pack.state == MGLOfflinePackStateInactive;
    }];
    [pack requestProgress];
    [self waitForExpectationsWithTimeout:5 handler:nil];
}

- (void)testAddPackForGeometry {
    NSUInteger countOfPacks = [MGLOfflineStorage sharedOfflineStorage].packs.count;
    
    NSURL *styleURL = [MGLStyle lightStyleURLWithVersion:8];
    double zoomLevel = 20;
    NSString *geojson = @"{ \"type\": \"Polygon\", \"coordinates\": [ [ [ 5.1299285888671875, 52.10365839097971 ], [ 5.103063583374023, 52.110037078604236 ], [ 5.080232620239258, 52.09548601177304 ], [ 5.106925964355469, 52.07987524347506 ], [ 5.1299285888671875, 52.10365839097971 ] ] ]}";
    NSError *error;
    MGLShape *shape = [MGLShape shapeWithData: [geojson dataUsingEncoding:NSUTF8StringEncoding] encoding: NSUTF8StringEncoding error:&error];
    XCTAssertNil(error);
    MGLShapeOfflineRegion *region = [[MGLShapeOfflineRegion alloc] initWithStyleURL:styleURL shape:shape fromZoomLevel:zoomLevel toZoomLevel:zoomLevel];
    
    
    NSString *nameKey = @"Name";
    NSString *name = @"Utrecht centrum";
    
    NSData *context = [NSKeyedArchiver archivedDataWithRootObject:@{nameKey: name}];
    
    __block MGLOfflinePack *pack;
    [self keyValueObservingExpectationForObject:[MGLOfflineStorage sharedOfflineStorage] keyPath:@"packs" handler:^BOOL(id _Nonnull observedObject, NSDictionary * _Nonnull change) {
        const auto changeKind = static_cast<NSKeyValueChange>([change[NSKeyValueChangeKindKey] unsignedLongValue]);
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
    [self waitForExpectationsWithTimeout:5 handler:nil];
    
    XCTAssertEqual([MGLOfflineStorage sharedOfflineStorage].packs.count, countOfPacks + 1, @"Added pack should have been added to the canonical collection of packs owned by the shared offline storage object. This assertion can fail if this test is run before -testAAALoadPacks.");
    
    XCTAssertEqual(pack, [MGLOfflineStorage sharedOfflineStorage].packs.lastObject, @"Pack should be appended to end of packs array.");
    
    XCTAssertEqualObjects(pack.region, region, @"Added pack’s region has changed.");
    
    NSDictionary *userInfo = [NSKeyedUnarchiver unarchiveObjectWithData:pack.context];
    XCTAssert([userInfo isKindOfClass:[NSDictionary class]], @"Context of offline pack isn’t a dictionary.");
    XCTAssert([userInfo[nameKey] isKindOfClass:[NSString class]], @"Name of offline pack isn’t a string.");
    XCTAssertEqualObjects(userInfo[nameKey], name, @"Name of offline pack has changed.");
    
    XCTAssertEqual(pack.state, MGLOfflinePackStateInactive, @"New pack should initially have inactive state.");
    
    [self keyValueObservingExpectationForObject:pack keyPath:@"state" handler:^BOOL(id _Nonnull observedObject, NSDictionary * _Nonnull change) {
        const auto changeKind = static_cast<NSKeyValueChange>([change[NSKeyValueChangeKindKey] unsignedLongValue]);
        const auto state = static_cast<MGLOfflinePackState>([change[NSKeyValueChangeNewKey] longValue]);
        return changeKind == NSKeyValueChangeSetting && state == MGLOfflinePackStateInactive;
    }];
    [self expectationForNotification:MGLOfflinePackProgressChangedNotification object:pack handler:^BOOL(NSNotification * _Nonnull notification) {
        MGLOfflinePack *notificationPack = notification.object;
        XCTAssert([notificationPack isKindOfClass:[MGLOfflinePack class]], @"Object of notification should be an MGLOfflinePack.");
        
        NSDictionary *userInfo = notification.userInfo;
        XCTAssertNotNil(userInfo, @"Progress change notification should have a userInfo dictionary.");
        
        NSNumber *stateNumber = userInfo[MGLOfflinePackUserInfoKeyState];
        XCTAssert([stateNumber isKindOfClass:[NSNumber class]], @"Progress change notification’s state should be an NSNumber.");
        XCTAssertEqual(stateNumber.integerValue, pack.state, @"State in a progress change notification should match the pack’s state.");
        
        NSValue *progressValue = userInfo[MGLOfflinePackUserInfoKeyProgress];
        XCTAssert([progressValue isKindOfClass:[NSValue class]], @"Progress change notification’s progress should be an NSValue.");
        XCTAssertEqualObjects(progressValue, [NSValue valueWithMGLOfflinePackProgress:pack.progress], @"Progress change notification’s progress should match pack’s progress.");
        
        return notificationPack == pack && pack.state == MGLOfflinePackStateInactive;
    }];
    [pack requestProgress];
    [self waitForExpectationsWithTimeout:5 handler:nil];
    pack = nil;
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
        const auto changeKind = static_cast<NSKeyValueChange>([change[NSKeyValueChangeKindKey] unsignedLongValue]);
        NSIndexSet *indices = change[NSKeyValueChangeIndexesKey];
        return changeKind == NSKeyValueChangeRemoval && indices.count == 1;
    }];
    XCTestExpectation *completionHandlerExpectation = [self expectationWithDescription:@"remove pack completion handler"];
    [[MGLOfflineStorage sharedOfflineStorage] removePack:pack withCompletionHandler:^(NSError * _Nullable error) {
        XCTAssertEqual(pack.state, MGLOfflinePackStateInvalid, @"Removed pack should be invalid in the completion handler.");
        [completionHandlerExpectation fulfill];
    }];
    [self waitForExpectationsWithTimeout:5 handler:nil];

    XCTAssertEqual(pack.state, MGLOfflinePackStateInvalid, @"Removed pack should have been invalidated synchronously.");

    XCTAssertEqual([MGLOfflineStorage sharedOfflineStorage].packs.count, countOfPacks - 1, @"Removed pack should have been removed from the canonical collection of packs owned by the shared offline storage object. This assertion can fail if this test is run before -testAAALoadPacks or -testAddPack.");
}

- (void)testCountOfBytesCompleted {
    XCTAssertGreaterThan([MGLOfflineStorage sharedOfflineStorage].countOfBytesCompleted, 0);
}

- (NSURL *)offlineStorage:(MGLOfflineStorage *)storage
     URLForResourceOfKind:(MGLResourceKind)kind
                  withURL:(NSURL *)url {
    if ([url.scheme isEqual: @"test"] && [url.host isEqual: @"api"]) {
        return [NSURL URLWithString:@"https://api.mapbox.com"];
    } else {
        return url;
    }
}

- (void)testResourceTransform {
    MGLOfflineStorage *os = [MGLOfflineStorage sharedOfflineStorage];
    [os setDelegate:self];

    auto fs = os.mbglFileSource;

    // Delegate returns "https://api.mapbox.com" as a replacement URL.
    const mbgl::Resource resource { mbgl::Resource::Unknown, "test://api" };
    std::unique_ptr<mbgl::AsyncRequest> req;
    req = fs->request(resource, [&](mbgl::Response res) {
        req.reset();
        XCTAssertFalse(res.error.get(), @"Request should not return an error");
        XCTAssertTrue(res.data.get(), @"Request should return data");
        XCTAssertEqual("{\"api\":\"mapbox\"}", *res.data, @"Request did not return expected data");
        CFRunLoopStop(CFRunLoopGetCurrent());
    });

    CFRunLoopRun();

    [os setDelegate:nil];
}

- (void)testAddFileContent {

    // Valid database
    {
        NSURL *resourceURL = [NSURL fileURLWithPath:[[NSBundle bundleForClass:[self class]] pathForResource:@"barcelona" ofType:@"db"]];
        NSArray *documentPaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString *documentDir = [documentPaths objectAtIndex:0];
        NSString *filePath = [documentDir stringByAppendingPathComponent:@"barcelona.db"];
        NSFileManager *fileManager = [NSFileManager defaultManager];
        
        BOOL directoryExists = [fileManager fileExistsAtPath:documentDir];
        if (!directoryExists) {
            [fileManager createDirectoryAtPath:documentDir withIntermediateDirectories:YES attributes:nil error:nil];
        }
        
        BOOL databaseExists = [fileManager fileExistsAtPath:filePath];
        if (databaseExists) {
            [fileManager removeItemAtPath:filePath error:nil];
        }
        
        NSError *error;
        [fileManager moveItemAtURL:resourceURL toURL:[NSURL fileURLWithPath:filePath] error:&error];
        NSDictionary *atributes = @{ NSFilePosixPermissions: @0777 };
        error = nil;
        [fileManager setAttributes:atributes ofItemAtPath:filePath error:&error];
        XCTAssertNil(error, @"Changing the file's permissions:%@ should not return an error.", filePath);
        error = nil;
        NSDictionary *fileAttributes = [fileManager attributesOfItemAtPath:filePath error:&error];
        
        NSNumber *fileSizeNumber = [fileAttributes objectForKey:NSFileSize];
        long long fileSize = [fileSizeNumber longLongValue];
        long long databaseFileSize = 32391168;
        // Merging databases creates an empty file if the file does not exist at the given path.
        XCTAssertEqual(fileSize, databaseFileSize, @"The database file size must be:%lld actual size:%lld", databaseFileSize, fileSize);
        
        NSUInteger countOfPacks = [MGLOfflineStorage sharedOfflineStorage].packs.count;
        
        [self keyValueObservingExpectationForObject:[MGLOfflineStorage sharedOfflineStorage] keyPath:@"packs" handler:^BOOL(id _Nonnull observedObject, NSDictionary * _Nonnull change) {
            const auto changeKind = static_cast<NSKeyValueChange>([change[NSKeyValueChangeKindKey] unsignedLongValue]);
            NSIndexSet *indices = change[NSKeyValueChangeIndexesKey];
            return changeKind == NSKeyValueChangeInsertion && indices.count == 1;
        }];
        
        XCTestExpectation *fileAdditionCompletionHandlerExpectation = [self expectationWithDescription:@"add database content completion handler"];
        MGLOfflineStorage *os = [MGLOfflineStorage sharedOfflineStorage];
        [os addContentsOfFile:filePath withCompletionHandler:^(NSURL *fileURL, NSArray<MGLOfflinePack *> * _Nullable packs, NSError * _Nullable error) {
            XCTAssertNotNil(fileURL, @"The fileURL should not be nil.");
            XCTAssertNotNil(packs, @"Adding the contents of the barcelona.db should update one pack.");
            XCTAssertNil(error, @"Adding contents to a file should not return an error.");
            for (MGLOfflinePack *pack in [MGLOfflineStorage sharedOfflineStorage].packs) {
                NSLog(@"PACK:%@", pack);
            }
            [fileAdditionCompletionHandlerExpectation fulfill];
        }];
        [self waitForExpectationsWithTimeout:5 handler:nil];
        // Depending on the database it may update or add a pack. For this case specifically the offline database adds one pack.
        XCTAssertEqual([MGLOfflineStorage sharedOfflineStorage].packs.count, countOfPacks + 1, @"Adding contents of barcelona.db should add one pack.");
    }
    // Invalid database type
    {
        NSURL *resourceURL = [NSURL fileURLWithPath:[[NSBundle bundleForClass:[self class]] pathForResource:@"one-liner" ofType:@"json"]];
        NSArray *documentPaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString *documentDir = [documentPaths objectAtIndex:0];
        NSString *filePath = [documentDir stringByAppendingPathComponent:@"on-liner-copy.json"];
        NSFileManager *fileManager = [NSFileManager defaultManager];

        BOOL exists = [fileManager fileExistsAtPath:filePath];
        if (exists) {
            [fileManager removeItemAtPath:filePath error:nil];
        }

        NSError *error;
        [fileManager copyItemAtURL:resourceURL toURL:[NSURL fileURLWithPath:filePath] error:&error];
        
        XCTestExpectation *invalidFileCompletionHandlerExpectation = [self expectationWithDescription:@"invalid content database completion handler"];
        MGLOfflineStorage *os = [MGLOfflineStorage sharedOfflineStorage];
        [os addContentsOfFile:filePath withCompletionHandler:^(NSURL *fileURL, NSArray<MGLOfflinePack *> * _Nullable packs, NSError * _Nullable error) {
            XCTAssertNotNil(error, @"Passing an invalid offline database file should return an error.");
            XCTAssertNil(packs, @"Passing an invalid offline database file should not add packs to the offline database.");
            [invalidFileCompletionHandlerExpectation fulfill];
        }];
        [self waitForExpectationsWithTimeout:5 handler:nil];
    }
    // File non existent
    {
        NSArray *documentPaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString *documentDir = [documentPaths objectAtIndex:0];
        NSString *filePath = [documentDir stringByAppendingPathComponent:@"nonexistent.db"];
        NSFileManager *fileManager = [NSFileManager defaultManager];
        
        BOOL exists = [fileManager fileExistsAtPath:filePath];
        if (exists) {
            [fileManager removeItemAtPath:filePath error:nil];
        }
        
        MGLOfflineStorage *os = [MGLOfflineStorage sharedOfflineStorage];
        XCTAssertThrowsSpecificNamed([os addContentsOfFile:filePath withCompletionHandler:nil], NSException, NSInvalidArgumentException, "MGLOfflineStorage should rise an exception if an invalid database file is passed.");

    }
    // URL to a non-file
    {
        NSURL *resourceURL = [NSURL URLWithString:@"https://www.mapbox.com"];
        
        MGLOfflineStorage *os = [MGLOfflineStorage sharedOfflineStorage];
        XCTAssertThrowsSpecificNamed([os addContentsOfURL:resourceURL withCompletionHandler:nil], NSException, NSInvalidArgumentException, "MGLOfflineStorage should rise an exception if an invalid URL file is passed.");
        
    }
    
}

@end
