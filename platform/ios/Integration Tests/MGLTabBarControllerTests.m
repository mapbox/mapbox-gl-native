
#import "MGLTabBarControllerTests.h"
// Will lack of access token impact CADisplayLink being called?

@implementation MGLTabBarControllerTests
@synthesize mapView;

- (void)setUp {
    [super setUp];
    UIStoryboard *storyboard = [UIStoryboard storyboardWithName:@"IntegrationTest" bundle:[NSBundle mainBundle]];

    // Keep an eye on how well this plays with other tests that use other root view controllers
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
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, 0 * NSEC_PER_SEC), dispatch_get_main_queue(), ^{
        __block NSInteger counter = 0;
        NSTimer *repeatingTimer = [NSTimer scheduledTimerWithTimeInterval:0.1 repeats:YES block:^(NSTimer * _Nonnull timer) {
            counter++;
            NSLog(@"%li", counter);
            // TODO
            self.tabController.selectedIndex = counter % 2;
            if (counter > 30) {
                // time call to updateDisplayLink to see if it takes longer that
                
                // subclass MGLMapView
                [repeatingTimer invalidate];
                [self.mapView setCenterCoordinate:CLLocationCoordinate2DMake(2, 2) zoomLevel:4 direction:0 animated:NO completionHandler:^{
                    NSLog(@"DONE");
                    [expectation fulfill];
                }];
            }
        }];
    });

    [self waitForExpectationsWithTimeout:10 handler:^(NSError * _Nullable error) {
        if (error) {
            XCTFail(@"Failed with error: %@", error);
        }
    }];
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

