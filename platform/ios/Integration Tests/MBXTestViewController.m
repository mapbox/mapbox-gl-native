
#import "MBXTestViewController.h"

@implementation MBXTestViewController

- (void)setUp {
    [super setUp];
    UIStoryboard *storyboard = [UIStoryboard storyboardWithName:@"IntegrationTest" bundle:[NSBundle mainBundle]];

    self.tabController = (UITabBarController *)[storyboard instantiateViewControllerWithIdentifier:@"TabController"];
    self.viewController = self.tabController.childViewControllers[0];
    MGLMapView *mapView = [[MGLMapView alloc] initWithFrame:self.viewController.view.bounds];
    mapView.delegate = self;
    [self.viewController.view addSubview:mapView];
    
    
    MGLPointAnnotation *annot = [[MGLPointAnnotation alloc] init];
    annot.coordinate = mapView.centerCoordinate;
    [mapView addAnnotation:annot];
    
    NSLog(@"%@", self.tabController.childViewControllers);
}
    
- (void)testViewControllerNil {
    XCTAssertNotNil(self.viewController);
}

- (void)testSwitchingTabs {
    self.counter = 0;
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, 5 * NSEC_PER_SEC), dispatch_get_main_queue(), ^{
        NSTimer *repeatingTimer = [NSTimer scheduledTimerWithTimeInterval:0.1 repeats:YES block:^(NSTimer * _Nonnull timer) {
            self.counter++;
            self.tabController.tabBarController.selectedIndex = self.tabController.tabBarController.selectedIndex == 1 ? 0 : 1;
        }];
        if (self.counter > 10) {
            [repeatingTimer invalidate];
        }
    });

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

