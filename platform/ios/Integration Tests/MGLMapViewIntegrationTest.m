#import "MGLMapViewIntegrationTest.h"

@interface MGLMapView (MGLMapViewIntegrationTest)
- (void)updateFromDisplayLink;
@end

@implementation MGLMapViewIntegrationTest

- (void)invokeTest {
    NSString *selector = NSStringFromSelector(self.invocation.selector);
    BOOL isPendingTest = [selector hasSuffix:@"PENDING"];
    
    if (isPendingTest) {
        NSString *runPendingTests = [[NSProcessInfo processInfo] environment][@"MAPBOX_RUN_PENDING_TESTS"];
        if (![runPendingTests boolValue]) {
            printf("warning: '%s' is a pending test - skipping\n", selector.UTF8String);
            return;
        }
    }

    [super invokeTest];
}

- (NSString*)validAccessToken {
    NSString *accessToken = [[NSProcessInfo processInfo] environment][@"MAPBOX_ACCESS_TOKEN"];
    if (!accessToken) {
        printf("warning: MAPBOX_ACCESS_TOKEN env var is required for this test - skipping.\n");
        return nil;
    }

    [MGLAccountManager setAccessToken:accessToken];
    return accessToken;
}

- (void)setUp {
    [super setUp];

    [MGLAccountManager setAccessToken:@"pk.feedcafedeadbeefbadebede"];
    NSURL *styleURL = [[NSBundle bundleForClass:[self class]] URLForResource:@"one-liner" withExtension:@"json"];

    self.mapView = [[MGLMapView alloc] initWithFrame:UIScreen.mainScreen.bounds styleURL:styleURL];
    self.mapView.delegate = self;

    UIView *superView = [[UIView alloc] initWithFrame:UIScreen.mainScreen.bounds];
    [superView addSubview:self.mapView];
    UIWindow *window = [[UIWindow alloc] initWithFrame:UIScreen.mainScreen.bounds];
    [window addSubview:superView];
    [window makeKeyAndVisible];

    if (!self.mapView.style) {
        [self waitForMapViewToFinishLoadingStyleWithTimeout:5];
    }
}

- (void)tearDown {
    self.styleLoadingExpectation = nil;
    self.renderFinishedExpectation = nil;
    self.mapView = nil;
    self.style = nil;

    [super tearDown];
}

#pragma mark - MGLMapViewDelegate

- (MGLAnnotationView*)mapView:(MGLMapView *)mapView viewForAnnotation:(id<MGLAnnotation>)annotation {
    if (self.viewForAnnotation) {
        return self.viewForAnnotation(mapView, annotation);
    }
    
    return nil;
}

- (void)mapView:(MGLMapView *)mapView didFinishLoadingStyle:(MGLStyle *)style {
    XCTAssertNotNil(mapView.style);
    XCTAssertEqual(mapView.style, style);

    [self.styleLoadingExpectation fulfill];
}

- (void)mapViewDidFinishRenderingFrame:(MGLMapView *)mapView fullyRendered:(__unused BOOL)fullyRendered {
    [self.renderFinishedExpectation fulfill];
    self.renderFinishedExpectation = nil;
}

- (void)mapView:(MGLMapView *)mapView regionWillChangeAnimated:(BOOL)animated {
    if (self.regionWillChange) {
        self.regionWillChange(mapView, animated);
    }
}

- (void)mapViewRegionIsChanging:(MGLMapView *)mapView {
    if (self.regionIsChanging) {
        self.regionIsChanging(mapView);
    }
}

- (void)mapView:(MGLMapView *)mapView regionDidChangeWithReason:(MGLCameraChangeReason)reason animated:(BOOL)animated {
    if (self.regionDidChange) {
        self.regionDidChange(mapView, reason, animated);
    }
}

#pragma mark - Utilities

- (void)waitForMapViewToFinishLoadingStyleWithTimeout:(NSTimeInterval)timeout {
    XCTAssertNil(self.styleLoadingExpectation);
    self.styleLoadingExpectation = [self expectationWithDescription:@"Map view should finish loading style."];
    [self waitForExpectations:@[self.styleLoadingExpectation] timeout:timeout];
}

- (void)waitForMapViewToBeRenderedWithTimeout:(NSTimeInterval)timeout {
    XCTAssertNil(self.renderFinishedExpectation);
    [self.mapView setNeedsDisplay];
    self.renderFinishedExpectation = [self expectationWithDescription:@"Map view should be rendered"];
    [self waitForExpectations:@[self.renderFinishedExpectation] timeout:timeout];
}

- (void)waitForExpectations:(NSArray<XCTestExpectation *> *)expectations timeout:(NSTimeInterval)seconds {

    NSTimer *timer;

    if (@available(iOS 10.0, *)) {
        // We're good.
    }
    else if (self.mapView) {
        // Before iOS 10 it seems that the display link is not called during the
        // waitForExpectations below
        timer = [NSTimer scheduledTimerWithTimeInterval:1.0/30.0
                                                 target:self.mapView
                                               selector:@selector(updateFromDisplayLink)
                                               userInfo:nil
                                                repeats:YES];
    }

    [super waitForExpectations:expectations timeout:seconds];
    [timer invalidate];
}

- (MGLStyle *)style {
    return self.mapView.style;
}

@end
