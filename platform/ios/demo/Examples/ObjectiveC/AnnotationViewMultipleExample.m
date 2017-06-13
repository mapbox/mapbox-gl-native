#import "AnnotationViewMultipleExample.h"
@import Mapbox;

NSString *const MBXExampleAnnotationViewMultiple = @"AnnotationViewMultipleExample";

// MGLPointAnnotation subclass
@interface MyCustomPointAnnotation : MGLPointAnnotation
@property (nonatomic, assign) BOOL willUseImage;
@end

@implementation MyCustomPointAnnotation
@end
// end MGLPointAnnotation subclass

@interface AnnotationViewMultipleExample () <MGLMapViewDelegate>
@end

@implementation AnnotationViewMultipleExample

- (void)viewDidLoad {
    [super viewDidLoad];
    
    // Create a new map view using the Mapbox Light style.
    MGLMapView *mapView = [[MGLMapView alloc] initWithFrame:self.view.bounds
        styleURL:[MGLStyle lightStyleURLWithVersion:9]];
    
    mapView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
    
    // Set the map's center coordinate and zoom level.
    mapView.centerCoordinate = CLLocationCoordinate2DMake(36.54,-116.97);
    mapView.zoomLevel = 9;
    mapView.delegate = self;
    [self.view addSubview:mapView];
    
    // Create four new point annotations with specified coordinates and titles.
    MyCustomPointAnnotation *pointA = [[MyCustomPointAnnotation alloc] init];
    pointA.title = @"Stovepipe Wells";
    pointA.coordinate = CLLocationCoordinate2DMake(36.4623,-116.8656);
    pointA.willUseImage = YES;
    
    MyCustomPointAnnotation *pointB = [[MyCustomPointAnnotation alloc] init];
    pointB.title = @"Furnace Creek";
    pointB.coordinate = CLLocationCoordinate2DMake(36.6071,-117.1458);
    pointB.willUseImage = YES;
    
    MyCustomPointAnnotation *pointC = [[MyCustomPointAnnotation alloc] init];
    pointC.title = @"Zabriskie Point";
    pointC.coordinate = CLLocationCoordinate2DMake(36.4208,-116.8101);
    
    MyCustomPointAnnotation *pointD = [[MyCustomPointAnnotation alloc] init];
    pointD.title = @"Mesquite Flat Sand Dunes";
    pointD.coordinate = CLLocationCoordinate2DMake(36.6836,-117.1005);
    
    // Fill an array with four point annotations.
    NSArray *myPlaces = @[pointA, pointB, pointC, pointD];
    
    // Add all annotations to the map all at once, instead of individually.
    [mapView addAnnotations:myPlaces];
}

// This delegate method is where you tell the map to load a view for a specific annotation based on the willUseImage property of the custom subclass.
- (MGLAnnotationView *)mapView:(MGLMapView *)mapView viewForAnnotation:(id <MGLAnnotation>)annotation {
    if ([annotation isKindOfClass:[MyCustomPointAnnotation class]]) {
        MyCustomPointAnnotation *castAnnotation = (MyCustomPointAnnotation *)annotation;
        
        if (castAnnotation.willUseImage) {
            return nil;
        }
    }
    
    // Assign a reuse identifier to be used by both of the annotation views, taking advantage of their similarities.
    NSString *reuseIdentifier = @"reusableDotView";
    
    // For better performance, always try to reuse existing annotations.
    MGLAnnotationView *annotationView = [mapView dequeueReusableAnnotationViewWithIdentifier:reuseIdentifier];
    
    // If there’s no reusable annotation view available, initialize a new one.
    if (!annotationView) {
        annotationView = [[MGLAnnotationView alloc] initWithReuseIdentifier:reuseIdentifier];
        annotationView.frame = CGRectMake(0, 0, 30, 30);
        annotationView.layer.cornerRadius = annotationView.frame.size.width / 2;
        annotationView.layer.borderColor = [UIColor whiteColor].CGColor;
        annotationView.layer.borderWidth = 4.0;
        annotationView.backgroundColor = [UIColor colorWithRed:0.03 green:0.80 blue:0.69 alpha:1.0];
    }
    
    return annotationView;
}

// This delegate method is where you tell the map to load an image for a specific annotation based on the willUseImage property of the custom subclass.
- (MGLAnnotationImage *)mapView:(MGLMapView *)mapView imageForAnnotation:(id <MGLAnnotation>)annotation {

    if ([annotation isKindOfClass:[MyCustomPointAnnotation class]]) {
        MyCustomPointAnnotation *castAnnotation = (MyCustomPointAnnotation *)annotation;
        
        if (!castAnnotation.willUseImage) {
            return nil;
        }
    }
    
    // For better performance, always try to reuse existing annotations.
    MGLAnnotationImage *annotationImage = [mapView dequeueReusableAnnotationImageWithIdentifier:@"camera"];
    
    // If there is no reusable annotation image available, initialize a new one.
    if (!annotationImage) {
        UIImage *image = [UIImage imageNamed:@"camera"];
        image = [image imageWithAlignmentRectInsets:UIEdgeInsetsMake(0, 0, image.size.height/2, 0)];
        annotationImage = [MGLAnnotationImage annotationImageWithImage:image reuseIdentifier:@"camera"];
    }
    
    return annotationImage;
}

- (BOOL)mapView:(MGLMapView *)mapView annotationCanShowCallout:(id<MGLAnnotation>)annotation {
    // Always allow callouts to popup when annotations are tapped.
    return YES;
}

@end
