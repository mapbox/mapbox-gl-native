#import "DrawingACustomMarkerExample.h"
@import Mapbox;

NSString *const MBXExampleDrawingACustomMarker = @"DrawingACustomMarkerExample";

@interface DrawingACustomMarkerExample () <MGLMapViewDelegate>
@end

@implementation DrawingACustomMarkerExample

- (void)viewDidLoad {
    [super viewDidLoad];
    
    NSURL *styleURL = [MGLStyle lightStyleURLWithVersion:9];
    MGLMapView *mapView = [[MGLMapView alloc] initWithFrame:self.view.bounds styleURL:styleURL];
    mapView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
    mapView.tintColor = [UIColor darkGrayColor];
    
    // Set the map‘s bounds to Pisa, Italy.
    MGLCoordinateBounds bounds = MGLCoordinateBoundsMake(
        CLLocationCoordinate2DMake(43.7115, 10.3725),
        CLLocationCoordinate2DMake(43.7318, 10.4222));
    [mapView setVisibleCoordinateBounds:bounds];
    
    [self.view addSubview:mapView];
    
    // Set the map view‘s delegate property.
    mapView.delegate = self;

    // Initialize and add the point annotation.
    MGLPointAnnotation *pisa = [[MGLPointAnnotation alloc] init];
    pisa.coordinate = CLLocationCoordinate2DMake(43.723, 10.396633);
    pisa.title = @"Leaning Tower of Pisa";
    [mapView addAnnotation:pisa];
}

- (MGLAnnotationImage *)mapView:(MGLMapView *)mapView imageForAnnotation:(id <MGLAnnotation>)annotation {
    // Try to reuse the existing ‘pisa’ annotation image, if it exists.
    MGLAnnotationImage *annotationImage = [mapView dequeueReusableAnnotationImageWithIdentifier:@"pisa"];

    // If the ‘pisa’ annotation image hasn‘t been set yet, initialize it here.
    if (!annotationImage) {
        // Leaning Tower of Pisa by Stefan Spieler from the Noun Project.
        UIImage *image = [UIImage imageNamed:@"pisavector"];

        // The anchor point of an annotation is currently always the center. To
        // shift the anchor point to the bottom of the annotation, the image
        // asset includes transparent bottom padding equal to the original image
        // height.
        //
        // To make this padding non-interactive, we create another image object
        // with a custom alignment rect that excludes the padding.
        image = [image imageWithAlignmentRectInsets:UIEdgeInsetsMake(0, 0, image.size.height/2, 0)];

        // Initialize the ‘pisa’ annotation image with the UIImage we just loaded.
        annotationImage = [MGLAnnotationImage annotationImageWithImage:image reuseIdentifier:@"pisa"];
    }
    
    return annotationImage;
}

- (BOOL)mapView:(MGLMapView *)mapView annotationCanShowCallout:(id <MGLAnnotation>)annotation {
    // Always allow callouts to popup when annotations are tapped.
    return YES;
}

@end
