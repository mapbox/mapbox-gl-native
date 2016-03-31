#import "MGLTViewController.h"
#import <Mapbox/Mapbox.h>

@implementation MGLTViewController
{
    MGLMapView *_mapView;
}

- (void)viewDidLoad
{
    [super viewDidLoad];

    _mapView = [[MGLMapView alloc] initWithFrame:self.view.bounds];
    _mapView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;

    [self.view addSubview:_mapView];
}

- (void)insetMapView
{
    _mapView.frame = CGRectInset(_mapView.frame, 50, 50);
}

- (void)tinyMapView
{
    _mapView.frame = CGRectMake(20, self.topLayoutGuide.length, self.view.frame.size.width / 2, self.view.frame.size.height / 2);
}

- (void)resetMapView
{
    _mapView.frame = self.view.bounds;
}

@end
