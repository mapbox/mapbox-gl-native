@import Mapbox;
#import "CameraFlyToExample.h"

NSString const *MBXExampleCameraFlyTo = @"CameraFlyToExample";

@interface CameraFlyToExample ()
@end

@implementation CameraFlyToExample

- (void)viewDidLoad {
    [super viewDidLoad];
    
    MGLMapView *mapView = [[MGLMapView alloc] initWithFrame:self.view.bounds];
    mapView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
    
    // Sets Honolulu, Hawaii as the camera's starting point.
    CLLocation *honolulu = [[CLLocation alloc] initWithLatitude:21.3069 longitude:-157.8583];
    [mapView setCenterCoordinate:honolulu.coordinate
                       zoomLevel:14 animated:NO];
    
    mapView.delegate = self;
    [self.view addSubview:mapView];
}

- (void)mapViewDidFinishLoadingMap:(MGLMapView *)mapView {
    
    // Waits for the mapView to finish loading before setting up the camera.
    // Defines the destination camera as Hawaii Island.
    MGLMapCamera *camera = [MGLMapCamera cameraLookingAtCenterCoordinate:CLLocationCoordinate2DMake(19.784213, -155.784605) fromDistance:35000 pitch:70 heading:90];
    
    // Goes from Honolulu to destination camera.
    [mapView flyToCamera:camera withDuration:4.0 peakAltitude:3000 completionHandler:nil];
    // To use default duration and peak altitudes:
    //    [mapView flyToCamera:camera completionHandler:nil];
    // To use default peak altitude:
    //    [mapView flyToCamera:camera withDuration:4 completionHandler:nil];
}
@end
