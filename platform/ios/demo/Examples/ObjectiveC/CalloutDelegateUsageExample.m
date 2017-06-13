#import "CalloutDelegateUsageExample.h"
@import Mapbox;

NSString *const MBXExampleCalloutDelegateUsage = @"CalloutDelegateUsageExample";

@interface CalloutDelegateUsageExample () <MGLMapViewDelegate>
@property MGLMapView *mapView;
@end

@implementation CalloutDelegateUsageExample

- (void)viewDidLoad
{
    [super viewDidLoad];

    self.mapView = [[MGLMapView alloc] initWithFrame:self.view.bounds];
    self.mapView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
    [self.view addSubview:self.mapView];

    // Remember to set the delegate.
    self.mapView.delegate = self;

    [self addAnnotation];
}

- (void)addAnnotation
{
    MGLPointAnnotation *annotation = [[MGLPointAnnotation alloc] init];
    annotation.coordinate = CLLocationCoordinate2DMake(35.03946, 135.72956);
    annotation.title = @"Kinkaku-ji";
    annotation.subtitle = [NSString stringWithFormat:@"%.5f, %.5f", annotation.coordinate.latitude, annotation.coordinate.longitude];

    [self.mapView addAnnotation:annotation];

    // Center the map on the annotation.
    [self.mapView setCenterCoordinate:annotation.coordinate zoomLevel:17 animated:NO];

    // Pop-up the callout view.
    [self.mapView selectAnnotation:annotation animated:YES];
}

- (BOOL)mapView:(MGLMapView *)mapView annotationCanShowCallout:(id<MGLAnnotation>)annotation
{
    return true;
}

- (UIView *)mapView:(MGLMapView *)mapView leftCalloutAccessoryViewForAnnotation:(id<MGLAnnotation>)annotation
{
    if ([annotation.title isEqualToString:@"Kinkaku-ji"])
    {
        // Callout height is fixed; width expands to fit its content.
        UILabel *label = [[UILabel alloc] initWithFrame:CGRectMake(0, 0, 60.f, 50.f)];
        label.textAlignment = NSTextAlignmentRight;
        label.textColor = [UIColor colorWithRed:0.81f green:0.71f blue:0.23f alpha:1.f];
        label.text = @"金閣寺";

        return label;
    }

    return nil;
}

- (UIView *)mapView:(MGLMapView *)mapView rightCalloutAccessoryViewForAnnotation:(id<MGLAnnotation>)annotation
{
    return [UIButton buttonWithType:UIButtonTypeDetailDisclosure];
}

- (void)mapView:(MGLMapView *)mapView annotation:(id<MGLAnnotation>)annotation calloutAccessoryControlTapped:(UIControl *)control
{
    // Hide the callout view.
    [self.mapView deselectAnnotation:annotation animated:NO];
    
    // Show an alert containing the annotation's details
    UIAlertController *alert = [UIAlertController alertControllerWithTitle:annotation.title
                                                                   message:@"A lovely (if touristy) place."
                                                            preferredStyle:UIAlertControllerStyleAlert];
    
    [alert addAction:[UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleDefault handler:nil]];
    
    [self presentViewController:alert animated:YES completion:nil];

}

@end
