#import "PointConversionExample.h"
@import Mapbox;

NSString *const MBXExamplePointConversion = @"PointConversionExample";

@interface PointConversionExample ()
@property (nonatomic) MGLMapView *mapView;
@end

@implementation PointConversionExample

- (void)viewDidLoad {
    [super viewDidLoad];

    self.mapView = [[MGLMapView alloc] initWithFrame:self.view.bounds];
    self.mapView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;

    [self.view addSubview:self.mapView];

    // Double tapping zooms the map, so ensure that can still happen.
    UITapGestureRecognizer *doubleTap = [[UITapGestureRecognizer alloc] initWithTarget:self action:nil];
    doubleTap.numberOfTapsRequired = 2;
    [self.mapView addGestureRecognizer:doubleTap];

    // Delay single tap recognition until it is clearly not a double.
    UITapGestureRecognizer *singleTap = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(handleSingleTap:)];
    [singleTap requireGestureRecognizerToFail:doubleTap];
    [self.mapView addGestureRecognizer:singleTap];

    // Convert `mapView.centerCoordinate` (CLLocationCoordinate2D)
    // to screen location (CGPoint).
    CGPoint centerScreenPoint = [self.mapView convertCoordinate:self.mapView.centerCoordinate
                                                  toPointToView:self.mapView];

    NSLog(@"Screen center: %@ = %@",
          NSStringFromCGPoint(centerScreenPoint),
          NSStringFromCGPoint(self.mapView.center));
}

- (void)handleSingleTap:(UITapGestureRecognizer *)tap {
    // Convert tap location (CGPoint)
    // to geographic coordinates (CLLocationCoordinate2D).
    CLLocationCoordinate2D location = [self.mapView convertPoint:[tap locationInView:self.mapView]
                                            toCoordinateFromView:self.mapView];

    NSLog(@"You tapped at: %.5f, %.5f", location.latitude, location.longitude);

    // Create an array of coordinates for our polyline.
    CLLocationCoordinate2D coordinates[] = {
        self.mapView.centerCoordinate,
        location
    };
    NSUInteger numberOfCoordinates = sizeof(coordinates) / sizeof(CLLocationCoordinate2D);

    // Remove existing polyline from the map, (re)add polyline with coordinates.
    if (self.mapView.annotations.count) {
        [self.mapView removeAnnotations:self.mapView.annotations];
    }
    MGLPolyline *polyline = [MGLPolyline polylineWithCoordinates:coordinates
                                                           count:numberOfCoordinates];
    [self.mapView addAnnotation:polyline];
}

@end
