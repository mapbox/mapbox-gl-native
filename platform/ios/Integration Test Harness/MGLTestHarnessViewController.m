#import "Mapbox.h"
#import "MGLMapViewDelegate.h"

#import "MGLTestHarnessViewController.h"
#import "MGLTestingSupport.h"

@interface MGLTestHarnessViewController ()<MGLMapViewDelegate>

@property (nonatomic) MGLMapView *mapView;

@end

@implementation MGLTestHarnessViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    self.mapView = [[MGLMapView alloc] initWithFrame:self.view.bounds];
    
    self.mapView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
    self.mapView.accessibilityIdentifier = MGLTestingSupportMapViewID;
    [self.mapView setCenterCoordinate:CLLocationCoordinate2DMake(31, -100) zoomLevel:3 animated:NO];
    self.mapView.delegate = self;
    
    [self.view addSubview:self.mapView];
}


#pragma mark - mapview delgate handles

- (void)mapViewDidFinishRenderingMap:(MGLMapView *)mapView fullyRendered:(BOOL)fullyRendered {
    testingSupportPostNotification(MGLTestingSupportNotificationMapViewRendered);
}

- (void)mapView:(MGLMapView *)mapView didFinishLoadingStyle:(MGLStyle *)style {
    testingSupportPostNotification(MGLTestingSupportNotificationMapViewStyleLoaded);
}

- (void)mapView:(MGLMapView *)mapView regionWillChangeAnimated:(BOOL)animated {
    testingSupportPostNotification(MGLTestingSupportNotificationMapViewRegionWillChange);
}

- (void)mapView:(MGLMapView *)mapView regionIsChangingWithReason:(MGLCameraChangeReason)reason {
    testingSupportPostNotification(MGLTestingSupportNotificationMapViewRegionIsChanging);
}

- (void)mapView:(MGLMapView *)mapView regionDidChangeAnimated:(BOOL)animated {
    testingSupportPostNotification(MGLTestingSupportNotificationMapViewRegionDidChanged);
}


@end
