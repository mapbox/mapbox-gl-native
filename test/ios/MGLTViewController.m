#import "MGLTViewController.h"
#import "Mapbox.h"

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

- (void)resetMapView
{
    _mapView.frame = self.view.bounds;
}

@end
