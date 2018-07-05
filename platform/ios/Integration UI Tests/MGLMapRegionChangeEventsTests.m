#import <XCTest/XCTest.h>

#import "MGLTestingSupport.h"

@interface MGLMapRegionChangeEventsTests : XCTestCase

@property (nonatomic) XCUIApplication *app;

@end

@implementation MGLMapRegionChangeEventsTests

- (void)setUp {
    [super setUp];

    // In UI tests it is usually best to stop immediately when a failure occurs.
    self.continueAfterFailure = NO;
    // UI tests must launch the application that they test. Doing this in setup will make sure it happens for each test method.
    self.app = [[XCUIApplication alloc] init];
    self.app.launchArguments = [self.app.launchArguments arrayByAddingObject:@"useFastAnimations"];
    [self.app launch];
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

- (void)testMapViewRegionChangeEvents {

    __block XCUIElement *mapViewElement;
    __block NSMutableArray *regionEvents = [NSMutableArray array];
    
    [XCTContext runActivityNamed:@"Wait for initial render" block:^(id<XCTActivity>  _Nonnull activity) {
        XCUIElementQuery *allQuery = [self.app descendantsMatchingType:XCUIElementTypeAny];
        mapViewElement = [allQuery elementMatchingType:XCUIElementTypeAny identifier:MGLTestingSupportMapViewID];
        XCTDarwinNotificationExpectation *expectation = [[XCTDarwinNotificationExpectation alloc] initWithNotificationName:MGLTestingSupportNotificationMapViewRendered];
        [self waitForExpectations:@[expectation] timeout:15.0];
    }];

    [XCTContext runActivityNamed:@"Perform guestures" block:^(id<XCTActivity>  _Nonnull activity) {
        XCTDarwinNotificationExpectation *willChangeExpectation = [[XCTDarwinNotificationExpectation alloc] initWithNotificationName:MGLTestingSupportNotificationMapViewRegionWillChange];
        willChangeExpectation.handler = ^BOOL{
            // Expect the RegionWillChange event occoured 2 times
            [regionEvents addObject:MGLTestingSupportNotificationMapViewRegionWillChange];
            NSArray *events = [regionEvents filteredArrayUsingPredicate:[NSPredicate predicateWithFormat:@"SELF = %@",
                                                                         MGLTestingSupportNotificationMapViewRegionWillChange]];
            return events.count == 2;
        };
        
        XCTDarwinNotificationExpectation *isChangingExpectation = [[XCTDarwinNotificationExpectation alloc] initWithNotificationName:MGLTestingSupportNotificationMapViewRegionIsChanging];
        isChangingExpectation.handler = ^BOOL{
            [regionEvents addObject:MGLTestingSupportNotificationMapViewRegionIsChanging];
            NSArray *events = [regionEvents filteredArrayUsingPredicate:[NSPredicate predicateWithFormat:@"SELF = %@",
                                                                         MGLTestingSupportNotificationMapViewRegionIsChanging]];
            // Expect the RegionIsChanging event occoured > 2 times
            return events.count >= 2;
        };
        
        XCTDarwinNotificationExpectation *didChangeExpectation = [[XCTDarwinNotificationExpectation alloc] initWithNotificationName:MGLTestingSupportNotificationMapViewRegionDidChanged];
        didChangeExpectation.handler = ^BOOL{
            [regionEvents addObject:MGLTestingSupportNotificationMapViewRegionDidChanged];
            NSArray *events = [regionEvents filteredArrayUsingPredicate:[NSPredicate predicateWithFormat:@"SELF = %@",
                                                                         MGLTestingSupportNotificationMapViewRegionDidChanged]];
            // Expect the RegionDidChanged event occoured 2 times
            return events.count == 2;
        };
        
        [mapViewElement swipeUp];
        // TODO: Seems the swipe down guesture performed after previous guesture finished,
        // Tried to find a method which could interrupt previous guesture and start an new guesture
        [mapViewElement swipeDown];
        [self waitForExpectations:@[willChangeExpectation,
                                    isChangingExpectation,
                                    didChangeExpectation]
                          timeout:10];
    }];
    
    /*
     Verify the events occured as the following order.
     
     -mapView:regionWillChangeAnimated:
     -mapViewIsChangingAnimated: many times
     -mapView:regionDidChangeAnimated:
     -mapView:regionWillChangeAnimated:
     -mapViewIsChangingAnimated: many times
     -mapView:regionDidChangeAnimated:
     */
    
    NSIndexSet *willChangePoints = [regionEvents indexesOfObjectsPassingTest:^BOOL(id  _Nonnull obj, NSUInteger idx, BOOL * _Nonnull stop) {
        return [obj isEqualToString:MGLTestingSupportNotificationMapViewRegionWillChange];
    }];
    
    NSIndexSet *didChangedPoints = [regionEvents indexesOfObjectsPassingTest:^BOOL(id  _Nonnull obj, NSUInteger idx, BOOL * _Nonnull stop) {
        return [obj isEqualToString:MGLTestingSupportNotificationMapViewRegionDidChanged];
    }];
    
    [XCTContext runActivityNamed:@"Verify events triggered by swipe up" block:^(id<XCTActivity>  _Nonnull activity) {
        [self verifyOrderWithEvents:[regionEvents subarrayWithRange:NSMakeRange(willChangePoints.firstIndex,
                                                                                didChangedPoints.firstIndex - willChangePoints.firstIndex + 1)]];
    }];
    
    [XCTContext runActivityNamed:@"Verify events triggered by swipe down" block:^(id<XCTActivity>  _Nonnull activity) {
        [self verifyOrderWithEvents:[regionEvents subarrayWithRange:NSMakeRange(willChangePoints.lastIndex,
                                                                                didChangedPoints.lastIndex - willChangePoints.lastIndex + 1)]];
    }];
}

- (void)verifyOrderWithEvents:(NSArray *)events {
    // All the evets's count should >= 3
    XCTAssertGreaterThanOrEqual(events.count, 3);
    // The RegionWillChange event should occoured at first
    XCTAssertEqual(events.firstObject, MGLTestingSupportNotificationMapViewRegionWillChange);
    // All the RegionIsChanging event occoured in the process
    for (NSInteger i = 1; i < events.count - 2; i ++) {
        XCTAssertEqual(events[i], MGLTestingSupportNotificationMapViewRegionIsChanging);
    }
    // The RegionDidChanged event should occoured at end
    XCTAssertEqual(events.lastObject, MGLTestingSupportNotificationMapViewRegionDidChanged);
}

@end
