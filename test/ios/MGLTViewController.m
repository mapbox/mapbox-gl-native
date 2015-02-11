#import "MGLTViewController.h"
#import "MGLMapView.h"

@implementation MGLTViewController

- (void)viewDidLoad
{
    [super viewDidLoad];

    MGLMapView *mapView = [[MGLMapView alloc] initWithFrame:self.view.bounds
                                                accessToken:@"pk.eyJ1IjoianVzdGluIiwiYSI6Ik9RX3RRQzAifQ.dmOg_BAp1ywuDZMM7YsXRg"];
    mapView.viewControllerForLayoutGuides = self;
    mapView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;

    [self.view addSubview:mapView];
}

@end
