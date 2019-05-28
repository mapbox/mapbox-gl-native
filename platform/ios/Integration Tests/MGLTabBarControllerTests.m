
#import "MGLTabBarControllerTests.h"
// Will lack of access token impact CADisplayLink being called?

@implementation MGLTabBarControllerTests

- (void)setUp {
    [super setUp];
    UIStoryboard *storyboard = [UIStoryboard storyboardWithName:@"IntegrationTest" bundle:[NSBundle mainBundle]];
//    [MGLAccountManager setAccessToken:@"pk.eyJ1Ijoiam9yZGFua2lsZXkiLCJhIjoiY2o1c3V0dG53MWJqczJxcGxmcXF5bGpmZyJ9.jgslVC7xNSQ2Rxayio_9Iw"];
    // Keep an eye on how well this plays with other tests that use other root view controllers.
    [self.window setRootViewController:(UITabBarController *)[storyboard instantiateViewControllerWithIdentifier:@"TabController"]];
    
    self.tabController = (UITabBarController *)self.window.rootViewController;
    self.viewController = self.tabController.childViewControllers[0];

    [self.viewController.view addSubview:self.mapView];

    MGLPointAnnotation *annot = [[MGLPointAnnotation alloc] init];
    annot.coordinate = self.mapView.centerCoordinate;
    [self.mapView addAnnotation:annot];
}

- (void)testViewControllerNil {
    XCTAssertNotNil(self.viewController);
}

- (void)testSwitchingTabs {
    
    XCTestExpectation *expectation = [self expectationWithDescription:@"Able to switch the tabs repeatedly without lag"];
    // TODO: Add animated map transition, or call setNeedGLDisplay if animation
        __block NSInteger counter = 0;
        
        __block NSTimer *repeatingTimer = [NSTimer scheduledTimerWithTimeInterval:0.1 repeats:YES block:^(NSTimer * _Nonnull timer) {
            
            counter++;
            NSLog(@"%li", counter);
            self.tabController.selectedIndex = counter % 2;
            if (counter > 41) {
                // time call to updateDisplayLink to see if it takes longer that
                
                // subclass MGLMapView
                [repeatingTimer invalidate];
                repeatingTimer = nil;
                
                MGLMapCamera *camera = [MGLMapCamera cameraLookingAtCenterCoordinate:CLLocationCoordinate2DMake(10, 10) altitude:100 pitch:0 heading:0];
                [self.mapView flyToCamera:camera withDuration:4 completionHandler:^{
                    [expectation fulfill];
                }];
                
                // I get an exception here because it is called multiple times
//                [self measureBlock:^{
//                [self measureMetrics:@[XCTPerformanceMetric_WallClockTime] automaticallyStartMeasuring:NO forBlock:^{
//                    [self startMeasuring];
//                    [self.mapView updateFromDisplayLink:nil];
                
//                    [self stopMeasuring];
//                }];
            }
        }];

    
    [self waitForExpectations:@[expectation] timeout:10];
    
}

- (void)switchTabsRepeatedly {
    
}

- (MGLAnnotationView *)mapView:(MGLMapView *)mapView viewForAnnotation:(id <MGLAnnotation>)annotation {

    MGLAnnotationView *annotView = [mapView dequeueReusableAnnotationViewWithIdentifier:@"annot-view"];
        if (!annotView) {
            annotView = [[MGLAnnotationView alloc] initWithReuseIdentifier:@"annot-view"];
        annotView.frame = CGRectMake(0, 0, 10, 10);
    }

    return annotView;
}

@end

