//
//  ExamplesUITests.m
//  ExamplesUITests
//
//  Created by Jason Wray on 1/26/16.
//  Copyright © 2016 Mapbox. All rights reserved.
//

#import <XCTest/XCTest.h>
#import "Examples.h"

@interface ExamplesUITests : XCTestCase

@end
@implementation ExamplesUITests

- (void)setUp {
    [super setUp];

    // In UI tests it is usually best to stop immediately when a failure occurs.
    self.continueAfterFailure = NO;

    // UI tests must launch the application that they test. Doing this in setup will make sure it happens for each test method.
    XCUIApplication *app = [[XCUIApplication alloc] init];
    app.launchArguments = [app.launchArguments arrayByAddingObject:@"useFastAnimations"];
    [app launch];
}

- (void)tearDown {
    [super tearDown];
}

- (void)testEveryExample {
    XCUIApplication *app = [[XCUIApplication alloc] init];

    for (int i = 0; i < app.tables.cells.count; i++) {
        [app.tables.cells.allElementsBoundByIndex[i] tap];

        // XCTest waits for the app to idle before continuing.

        [app.navigationBars.buttons[@"Back"] tap];
    }
}

@end
