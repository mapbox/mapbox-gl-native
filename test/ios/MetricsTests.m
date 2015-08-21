#import "MGLMapboxEvents.h"

#import "KIFTestActor+MapboxGL.h"

#import <KIF/KIF.h>
#import <OCMock/OCMock.h>

#import "Mapbox.h"
#import "OHHTTPStubs.h"

const NSUInteger MGLMaximumEventsPerFlush = 20;
const NSTimeInterval MGLFlushInterval = 60;

@interface MGLMapboxEvents (Testing)

- (NSString *)appBundleId;
- (NSString *)token;
- (BOOL)isPaused;
- (NSURLSession *)session;
- (void)pushEvent:(NSString *)event withAttributes:(NSDictionary *)attributeDictionary;
- (NSMutableArray *)eventQueue;
- (void)postEvents:(NSArray *)events;
- (NSTimer *)timer;
- (void)flush;
- (void)startTimer;

@end

@interface MetricsTests : KIFTestCase

@end

@implementation MetricsTests

- (void)beforeAll {
    [tester acknowledgeSystemAlert];
}

- (void)beforeEach {
    [[NSUserDefaults standardUserDefaults] setBool:YES forKey:@"MGLMapboxMetricsEnabled"];
    [MGLMapboxEvents resumeMetricsCollection];

    [MGLAccountManager setAccessToken:@"pk.eyJ1IjoianVzdGluIiwiYSI6IlpDbUJLSUEifQ.4mG8vhelFMju6HpIY-Hi5A"];
}

- (void)afterEach {
    [MGLMapboxEvents pauseMetricsCollection];
}

- (void)pushFakeEvent {
    [MGLMapboxEvents pushEvent:@"test_event" withAttributes:@{ @"foo" : @"one", @"bar" : @(2) }];
}

- (void)testAppBundleID {
    XCTAssertEqualObjects([[MGLMapboxEvents sharedManager] appBundleId], @"com.mapbox.Mapbox-GL-Tests");
}

- (void)testResumeMetricsCollection {
    [MGLMapboxEvents resumeMetricsCollection];

    XCTAssertFalse([[MGLMapboxEvents sharedManager] isPaused]);
    XCTAssertNotNil([[MGLMapboxEvents sharedManager] session]);
}

- (void)testPauseMetricsCollection {
    [MGLMapboxEvents pauseMetricsCollection];

    XCTAssertTrue([[MGLMapboxEvents sharedManager] isPaused]);
    XCTAssertNil([[MGLMapboxEvents sharedManager] session]);
}

- (void)testFlushAtThreshold {
    NSUInteger startCount = [[[MGLMapboxEvents sharedManager] eventQueue] count];

    XCTestExpectation *queueItemsExpectation = [self expectationWithDescription:@"queue should contain events"];

    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        while (true) {
            if ([[[MGLMapboxEvents sharedManager] eventQueue] count] > 0) {
                [queueItemsExpectation fulfill];
                break;
            }
        }
    });

    for (NSUInteger i = 0; i < (MGLMaximumEventsPerFlush - startCount - 1); i++) {
        [self pushFakeEvent];
    }

    [self waitForExpectationsWithTimeout:1.0 handler:nil];

    XCTestExpectation *emptyQueueExpectation = [self expectationWithDescription:@"queue should be empty"];

    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        while (true) {
            if ([[[MGLMapboxEvents sharedManager] eventQueue] count] == 0) {
                [emptyQueueExpectation fulfill];
                break;
            }
        }
    });

    [self pushFakeEvent];

    [self waitForExpectationsWithTimeout:2.0 handler:nil];
}

- (void)testTimerDestroyedAfterFlush {
    XCTestExpectation *emptyQueueExpectation = [self expectationWithDescription:@"queue should be empty"];

    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        while (true) {
            if ([[[MGLMapboxEvents sharedManager] eventQueue] count] == 0) {
                [emptyQueueExpectation fulfill];
                break;
            }
        }
    });

    [[MGLMapboxEvents sharedManager] flush];

    [self waitForExpectationsWithTimeout:1.0 handler:nil];

    XCTAssertNil([[MGLMapboxEvents sharedManager] timer]);
}

- (void)testTimerCreatedAfterEventPushed {
    XCTestExpectation *emptyQueueExpectation = [self expectationWithDescription:@"queue should be empty"];

    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        while (true) {
            if ([[[MGLMapboxEvents sharedManager] eventQueue] count] == 0) {
                [emptyQueueExpectation fulfill];
                break;
            }
        }
    });

    [[MGLMapboxEvents sharedManager] flush];

    [self waitForExpectationsWithTimeout:2.0 handler:nil];

    id eventsMock = [OCMockObject partialMockForObject:[MGLMapboxEvents sharedManager]];
    [[[eventsMock expect] andForwardToRealObject] startTimer];
    [self pushFakeEvent];
    [eventsMock verifyWithDelay:0.5];

    XCTAssertEqual([[[MGLMapboxEvents sharedManager] eventQueue] count], 1);
    XCTAssertNotNil([[MGLMapboxEvents sharedManager] timer]);
    XCTAssertEqual(MGLFlushInterval, [[[MGLMapboxEvents sharedManager] timer] timeInterval]);
}

- (void)testTimerFiresFlush {
    XCTestExpectation *emptyQueueExpectation = [self expectationWithDescription:@"queue should be empty"];

    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        while (true) {
            if ([[[MGLMapboxEvents sharedManager] eventQueue] count] == 0) {
                [emptyQueueExpectation fulfill];
                break;
            }
        }
    });

    [[MGLMapboxEvents sharedManager] flush];

    [self waitForExpectationsWithTimeout:1.0 handler:nil];

    [self pushFakeEvent];
    id eventsMock = [OCMockObject partialMockForObject:[MGLMapboxEvents sharedManager]];
    [[eventsMock expect] flush];
    [eventsMock verifyWithDelay:MGLFlushInterval];
}

- (void)testFlushPostsEvents {
    NSArray *events = [NSArray arrayWithArray:[[MGLMapboxEvents sharedManager] eventQueue]];

    id eventsMock = [OCMockObject partialMockForObject:[MGLMapboxEvents sharedManager]];
    [[eventsMock expect] postEvents:events];
    [[MGLMapboxEvents sharedManager] flush];
    [eventsMock verifyWithDelay:1.0];

    XCTAssertTrue([[[MGLMapboxEvents sharedManager] eventQueue] count] == 0);
}

- (void)testPostEventsNetworkRequest {
    XCTestExpectation *requestExpectation = [self expectationWithDescription:@"posting events generates network request"];
    XCTestExpectation *contentTypeExpectation = [self expectationWithDescription:@"events are posted as JSON"];
    XCTestExpectation *userAgentExpectation = [self expectationWithDescription:@"events are posted with proper user agent"];

    [OHHTTPStubs stubRequestsPassingTest:^BOOL(NSURLRequest *request) {
        return [request.URL.path hasPrefix:@"/events/v1"];
    } withStubResponse:^OHHTTPStubsResponse *(NSURLRequest *request) {
        if ([request.URL.absoluteString isEqualToString:[@"https://api.tiles.mapbox.com/events/v1?access_token=" stringByAppendingString:[MGLAccountManager accessToken]]]) {
            [requestExpectation fulfill];
        }

        if ([request.allHTTPHeaderFields[@"Content-Type"] isEqualToString:@"application/json"]) {
            [contentTypeExpectation fulfill];
        }

        if ([request.allHTTPHeaderFields[@"User-Agent"] isEqualToString:[NSString stringWithFormat:@"%@/1.0/1.0 MapboxEventsiOS/1.0", [[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleName"]]]) {
            [userAgentExpectation fulfill];
        }

        return nil;
    }];

    [self pushFakeEvent];
    [MGLMapboxEvents flush];

    [self waitForExpectationsWithTimeout:1.0 handler:nil];
}

- (void)testPushEventAddsToEventQueue {
    XCTestExpectation *queueSizeIncreaseExpectation = [self expectationWithDescription:@"event queue size increase"];

    NSUInteger beforeCount = [[[MGLMapboxEvents sharedManager] eventQueue] count];

    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        while (true) {
            if ([[[MGLMapboxEvents sharedManager] eventQueue] count] == (beforeCount + 1)) {
                [queueSizeIncreaseExpectation fulfill];
                break;
            }
        }
    });

    [self pushFakeEvent];

    [self waitForExpectationsWithTimeout:2.0 handler:nil];
}

- (void)testOptOutUpfrontDisablesMetrics {
    [[NSUserDefaults standardUserDefaults] setBool:NO forKey:@"MGLMapboxMetricsEnabled"];

    NSUInteger startingCount = [[[MGLMapboxEvents sharedManager] eventQueue] count];

    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        while (true) {
            if ([[[MGLMapboxEvents sharedManager] eventQueue] count] > startingCount) {
                XCTFail(@"queue should not receive events when opted-out");
                break;
            }
        }
    });

    [self pushFakeEvent];

    [tester waitForTimeInterval:1.0];
}

- (void)testOptOutAtRuntimeDisablesMetrics {
    XCTAssertTrue([[NSUserDefaults standardUserDefaults] boolForKey:@"MGLMapboxMetricsEnabled"]);

    [OHHTTPStubs stubRequestsPassingTest:^BOOL(NSURLRequest *request) {
        return [request.URL.path hasPrefix:@"/events/v1"];
    } withStubResponse:^OHHTTPStubsResponse *(NSURLRequest *request) {
        XCTFail(@"opt-out should not post events");
        return nil;
    }];

    [[NSUserDefaults standardUserDefaults] setBool:NO forKey:@"MGLMapboxMetricsEnabled"];

    [self pushFakeEvent];

    [[MGLMapboxEvents sharedManager] flush];

    [tester waitForTimeInterval:2.0];
}

@end
