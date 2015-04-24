#import "MGLTViewController.h"
#import "MapboxGL.h"

@implementation MGLTViewController
{
    MGLMapView *_mapView;
}

- (void)viewDidLoad
{
    [super viewDidLoad];

    _mapView = [[MGLMapView alloc] initWithFrame:self.view.bounds
                                                accessToken:@"pk.eyJ1IjoianVzdGluIiwiYSI6Ik9RX3RRQzAifQ.dmOg_BAp1ywuDZMM7YsXRg"];
    _mapView.viewControllerForLayoutGuides = self;
    _mapView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;

    [self.view addSubview:_mapView];
}

- (void)insetMapView
{
    _mapView.frame = CGRectInset(_mapView.frame, 50, 50);
}

@end
