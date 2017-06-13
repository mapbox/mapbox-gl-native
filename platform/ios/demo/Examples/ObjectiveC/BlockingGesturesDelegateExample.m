#import "BlockingGesturesDelegateExample.h"
@import Mapbox;

NSString *const MBXExampleBlockingGesturesDelegate = @"BlockingGesturesDelegateExample";

@interface BlockingGesturesDelegateExample () <MGLMapViewDelegate>
@property (nonatomic) MGLCoordinateBounds colorado;
@end

@implementation BlockingGesturesDelegateExample

- (void)viewDidLoad {
    [super viewDidLoad];
    
    MGLMapView *mapView = [[MGLMapView alloc] initWithFrame:self.view.bounds];
    mapView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
    mapView.delegate = self;
    mapView.styleURL = [MGLStyle outdoorsStyleURLWithVersion:9];
    
    // Denver, Colorado
    CLLocationCoordinate2D center = CLLocationCoordinate2DMake(39.748947, -104.995882);
    
    // Starting point
    [mapView setCenterCoordinate:center zoomLevel:10 direction:0 animated:NO];
    
    // Colorado's bounds
    CLLocationCoordinate2D ne = CLLocationCoordinate2DMake(40.989329, -102.062592);
    CLLocationCoordinate2D sw = CLLocationCoordinate2DMake(36.986207, -109.049896);
    self.colorado = MGLCoordinateBoundsMake(sw, ne);
    
    [self.view addSubview:mapView];
}

// This example uses Colorado's boundaries to restrict
// the camera movement.

- (BOOL)mapView:(MGLMapView *)mapView shouldChangeFromCamera:(MGLMapCamera *)oldCamera toCamera:(MGLMapCamera *)newCamera
{
    // Get the current camera to restore it after
    MGLMapCamera *currentCamera = mapView.camera;
    
    // From the new camera obtain the center to test
    // if it's inside the boundaries
    CLLocationCoordinate2D newCameraCenter = newCamera.centerCoordinate;
    
    
    // Set mapView to newCamera to project the
    // new boundaries
    mapView.camera = newCamera;
    MGLCoordinateBounds newVisibleCoordinates = mapView.visibleCoordinateBounds;
    
    // Revert the camera
    mapView.camera = currentCamera;
    
    // Test if the newCameraCenter and newVisibleCoordinates
    // are inside self.colorado
    BOOL inside = MGLCoordinateInCoordinateBounds(newCameraCenter, self.colorado);
    BOOL intersects = MGLCoordinateInCoordinateBounds(newVisibleCoordinates.ne, self.colorado) && MGLCoordinateInCoordinateBounds(newVisibleCoordinates.sw, self.colorado);
    
    return inside && intersects;

}

@end
