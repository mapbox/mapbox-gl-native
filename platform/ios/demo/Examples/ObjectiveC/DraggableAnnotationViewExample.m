#import "DraggableAnnotationViewExample.h"
@import Mapbox;

NSString *const MBXExampleDraggableAnnotationView = @"DraggableAnnotationViewExample";

// MGLAnnotationView subclass
@interface DraggableAnnotationView : MGLAnnotationView
@end
@implementation DraggableAnnotationView

- (instancetype)initWithReuseIdentifier:(nullable NSString *)reuseIdentifier size:(CGFloat)size {
    self = [self initWithReuseIdentifier:reuseIdentifier];
    if (self)
    {
        // `draggable` is a property of MGLAnnotationView, disabled by default.
        self.draggable = true;

        // This property prevents the annotation from changing size when the map is tilted.
        self.scalesWithViewingDistance = false;

        // Begin setting up the view.
        self.frame = CGRectMake(0, 0, size, size);

        self.backgroundColor = [UIColor darkGrayColor];

        // Use CALayer’s corner radius to turn this view into a circle.
        self.layer.cornerRadius = size / 2;
        self.layer.borderWidth = 1;
        self.layer.borderColor = [UIColor whiteColor].CGColor;
        self.layer.shadowColor = [UIColor blackColor].CGColor;
        self.layer.shadowOpacity = 0.1;
    }
    return self;
}

- (void)setDragState:(MGLAnnotationViewDragState)dragState animated:(BOOL)animated {
    [super setDragState:dragState animated:animated];

    switch (dragState) {
        case MGLAnnotationViewDragStateStarting:
            printf("Starting");
            [self startDragging];
            break;

        case MGLAnnotationViewDragStateDragging:
            printf(".");
            break;

        case MGLAnnotationViewDragStateEnding:
        case MGLAnnotationViewDragStateCanceling:
            printf("Ending\n");
            [self endDragging];
            break;

        case MGLAnnotationViewDragStateNone:
            return;
    }
}

// When the user interacts with an annotation, animate opacity and scale changes.
- (void)startDragging {
    [UIView animateWithDuration:0.3 delay:0 usingSpringWithDamping:0.5 initialSpringVelocity:0 options:0 animations:^{
        self.layer.opacity = 0.8;
        self.transform = CGAffineTransformScale(CGAffineTransformIdentity, 1.5, 1.5);
    } completion:nil];
}

- (void)endDragging {
    self.transform = CGAffineTransformScale(CGAffineTransformIdentity, 1.5, 1.5);
    [UIView animateWithDuration:0.3 delay:0 usingSpringWithDamping:0.5 initialSpringVelocity:0 options:0 animations:^{
        self.layer.opacity = 1;
        self.transform = CGAffineTransformScale(CGAffineTransformIdentity, 1, 1);
    } completion:nil];
}

@end


//
// Example view controller
@interface DraggableAnnotationViewExample () <MGLMapViewDelegate>
@end
@implementation DraggableAnnotationViewExample

- (void)viewDidLoad {
    [super viewDidLoad];

    MGLMapView *mapView = [[MGLMapView alloc] initWithFrame:self.view.bounds];
    mapView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
    mapView.styleURL = [MGLStyle lightStyleURLWithVersion:9];
    mapView.tintColor = [UIColor darkGrayColor];
    mapView.zoomLevel = 1;
    mapView.delegate = self;
    [self.view addSubview:mapView];

    // Specify coordinates for our annotations.
    CLLocationCoordinate2D coordinates[] = {
        CLLocationCoordinate2DMake(0, -70),
        CLLocationCoordinate2DMake(0, -35),
        CLLocationCoordinate2DMake(0,  0),
        CLLocationCoordinate2DMake(0, 35),
        CLLocationCoordinate2DMake(0, 70),
    };
    NSUInteger numberOfCoordinates = sizeof(coordinates) / sizeof(CLLocationCoordinate2D);

    // Fill an array with point annotations and add it to the map.
    NSMutableArray *pointAnnotations = [NSMutableArray arrayWithCapacity:numberOfCoordinates];
    for (NSUInteger i = 0; i < numberOfCoordinates; i++) {
        CLLocationCoordinate2D coordinate = coordinates[i];
        MGLPointAnnotation *point = [[MGLPointAnnotation alloc] init];
        point.coordinate = coordinate;
        point.title = @"To drag this annotation, first tap and hold.";
        [pointAnnotations addObject:point];
    }

    [mapView addAnnotations:pointAnnotations];
}

#pragma mark - MGLMapViewDelegate methods

// This delegate method is where you tell the map to load a view for a specific annotation. To load a static MGLAnnotationImage, you would use `-mapView:imageForAnnotation:`.
- (MGLAnnotationView *)mapView:(MGLMapView *)mapView viewForAnnotation:(id <MGLAnnotation>)annotation {
    // This example is only concerned with point annotations.
    if (![annotation isKindOfClass:[MGLPointAnnotation class]]) {
        return nil;
    }

    // For better performance, always try to reuse existing annotations. To use multiple different annotation views, change the reuse identifier for each.
    DraggableAnnotationView *annotationView = [mapView dequeueReusableAnnotationViewWithIdentifier:@"draggablePoint"];

    // If there’s no reusable annotation view available, initialize a new one.
    if (!annotationView) {
        annotationView = [[DraggableAnnotationView alloc] initWithReuseIdentifier:@"draggablePoint" size:50];
    }

    return annotationView;
}

- (BOOL)mapView:(MGLMapView *)mapView annotationCanShowCallout:(id<MGLAnnotation>)annotation {
    return YES;
}

@end
