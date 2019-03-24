#import "MGLMapViewIntegrationTest.h"
#import "MGLMockApplication.h"

@interface MGLMapView (BackgroundTests)
@property (nonatomic) id<MGLApplication> application;
@property (nonatomic) BOOL rendererWasFlushed;
@property (nonatomic, getter=isDormant) BOOL dormant;
@property (nonatomic) CADisplayLink *displayLink;
@end

@protocol MGLApplication;

typedef void (^MGLNotificationBlock)(NSNotification*);

@interface MGLBackgroundIntegrationTest : MGLMapViewIntegrationTest
@property (nonatomic) id<MGLApplication> oldApplication;
@property (nonatomic) MGLMockApplication *mockApplication;
@property (nonatomic, copy) MGLNotificationBlock willEnterForeground;
@property (nonatomic, copy) MGLNotificationBlock didEnterBackground;
@end

@implementation MGLBackgroundIntegrationTest

- (void)setUp {

    self.mockApplication = [[MGLMockApplication alloc] init];

    // Register notifications
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(willEnterForeground:) name:UIApplicationWillEnterForegroundNotification object:self.mockApplication];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(didEnterBackground:) name:UIApplicationDidEnterBackgroundNotification object:self.mockApplication];
    
    [super setUp];
    
    // Setup MGLMapView to use our new mocked application
    // Change notification handling here.
    self.oldApplication = self.mapView.application;
    self.mapView.application = self.mockApplication;
}

- (void)tearDown {

    [[NSNotificationCenter defaultCenter] removeObserver:self];
    
    // Swap back
    self.mapView.application = self.oldApplication;
    self.oldApplication = nil;
    self.mockApplication = nil;
    
    [super tearDown];
}

#pragma mark - Notifications

- (void)willEnterForeground:(NSNotification*)notification {
    NSLog(@"Test willEnterForeground");
    if (self.willEnterForeground) {
        self.willEnterForeground(notification);
    }
}

- (void)didEnterBackground:(NSNotification*)notification {
    NSLog(@"Test didEnterBackground");
    if (self.didEnterBackground) {
        self.didEnterBackground(notification);
    }
}

#pragma mark - Tests

- (void)testRendererWhenGoingIntoBackground {
    
    XCTAssertFalse(self.mapView.isDormant);
    XCTAssertFalse(self.mapView.displayLink.isPaused);
    XCTAssert(self.mapView.application.applicationState == UIApplicationStateActive);
    XCTAssertFalse(self.mapView.rendererWasFlushed);

    __weak MGLMapView *weakMapView = self.mapView;

    //
    // Enter background
    //
    
    XCTestExpectation *didEnterBackgroundExpectation = [self expectationWithDescription:@"didEnterBackground"];
    didEnterBackgroundExpectation.expectedFulfillmentCount = 1;
    didEnterBackgroundExpectation.assertForOverFulfill = YES;

    self.didEnterBackground = ^(__unused NSNotification *notification){
        MGLMapView *strongMapView = weakMapView;
        
        // Remove the map view, and re-add to try and force a bad situation
        UIView *parentView = strongMapView.superview;
        
        NSLog(@"Removing MGLMapView from super view");
        [strongMapView removeFromSuperview];
        
        // Re-add
        NSLog(@"Re-adding MGLMapView as child");
        [parentView addSubview:strongMapView];
        [strongMapView.topAnchor constraintEqualToAnchor:parentView.topAnchor].active = YES;
        [strongMapView.leftAnchor constraintEqualToAnchor:parentView.leftAnchor].active = YES;
        [strongMapView.rightAnchor constraintEqualToAnchor:parentView.rightAnchor].active = YES;
        [strongMapView.bottomAnchor constraintEqualToAnchor:parentView.bottomAnchor].active = YES;
    };
    
    [self.mockApplication enterBackground];
    [self waitForExpectations:@[didEnterBackgroundExpectation] timeout:2.0];
    
    XCTAssert(self.mapView.isDormant);
    XCTAssert(self.mapView.displayLink.isPaused);
    XCTAssert(self.mapView.application.applicationState == UIApplicationStateBackground);
    XCTAssert(self.mapView.rendererWasFlushed);

    //
    // Enter foreground
    //

    XCTestExpectation *willEnterForegroundExpectation = [self expectationWithDescription:@"willEnterForeground"];
    willEnterForegroundExpectation.expectedFulfillmentCount = 1;
    willEnterForegroundExpectation.assertForOverFulfill = YES;

    self.willEnterForeground = ^(NSNotification *notification) {
        [willEnterForegroundExpectation fulfill];
    };
    
    [self.mockApplication enterForeground];
    [self waitForExpectations:@[willEnterForegroundExpectation] timeout:2.0];
    
    XCTAssertFalse(self.mapView.isDormant);
    XCTAssertFalse(self.mapView.displayLink.isPaused);
    XCTAssert(self.mapView.application.applicationState == UIApplicationStateActive);
    XCTAssert(self.mapView.rendererWasFlushed);
}

@end
