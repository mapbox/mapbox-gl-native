#import "CustomCalloutViewExample.h"
#import "CustomCalloutView.h"
@import Mapbox;

NSString *const MBXExampleCustomCalloutView = @"CustomCalloutViewExample";

@interface CustomCalloutViewExample () <MGLMapViewDelegate>
@end

@implementation CustomCalloutViewExample

- (void)viewDidLoad {
    [super viewDidLoad];

    MGLMapView *mapView = [[MGLMapView alloc] initWithFrame:self.view.bounds styleURL:[MGLStyle lightStyleURLWithVersion:9]];
    mapView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
    mapView.tintColor = [UIColor darkGrayColor];
    [self.view addSubview:mapView];

    // Set the map view‘s delegate property
    mapView.delegate = self;

    // Initialize and add the marker annotation
    MGLPointAnnotation *marker = [[MGLPointAnnotation alloc] init];
    marker.coordinate = CLLocationCoordinate2DMake(0, 0);
    marker.title = @"Hello world!";

    // This custom callout example does not implement subtitles
    //marker.subtitle = @"Welcome to my marker";

    // Add marker to the map
    [mapView addAnnotation:marker];
}

- (BOOL)mapView:(MGLMapView *)mapView annotationCanShowCallout:(id <MGLAnnotation>)annotation {
    // Always allow callouts to popup when annotations are tapped
    return YES;
}

- (UIView<MGLCalloutView> *)mapView:(__unused MGLMapView *)mapView calloutViewForAnnotation:(id<MGLAnnotation>)annotation
{
    // Only show callouts for `Hello world!` annotation
    if ([annotation respondsToSelector:@selector(title)]
        && [annotation.title isEqualToString:@"Hello world!"])
    {
        // Instantiate and return our custom callout view
        CustomCalloutView *calloutView = [[CustomCalloutView alloc] init];
        calloutView.representedObject = annotation;
        return calloutView;
    }
    return nil;
}

- (void)mapView:(MGLMapView *)mapView tapOnCalloutForAnnotation:(id<MGLAnnotation>)annotation
{
    // Optionally handle taps on the callout
    NSLog(@"Tapped the callout for: %@", annotation);

    // Hide the callout
    [mapView deselectAnnotation:annotation animated:YES];
}

@end
