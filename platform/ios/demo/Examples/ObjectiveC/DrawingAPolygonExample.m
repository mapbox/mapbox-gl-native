#import "DrawingAPolygonExample.h"
@import Mapbox;

NSString *const MBXExampleDrawingAPolygon = @"DrawingAPolygonExample";

@interface DrawingAPolygonExample () <MGLMapViewDelegate>
@property (nonatomic) MGLMapView *mapView;
@end

@implementation DrawingAPolygonExample

- (void)viewDidLoad {
    [super viewDidLoad];

    self.mapView = [[MGLMapView alloc] initWithFrame:self.view.bounds];
    self.mapView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;

    // Set the map's center coordinate
    [self.mapView setCenterCoordinate:CLLocationCoordinate2DMake(45.520486, -122.673541)
                            zoomLevel:11
                             animated:NO];

    [self.view addSubview:self.mapView];

    // Set the delegate property of our map view to self after instantiating it
    self.mapView.delegate = self;
}

- (void)viewDidAppear:(BOOL)animated {
    [super viewDidAppear:animated];

    // Draw the polygon after the map has initialized
    [self drawShape];
}

- (void)drawShape {
    // Create a coordinates array to hold all of the coordinates for our shape.
    CLLocationCoordinate2D coordinates[] = {
        CLLocationCoordinate2DMake(45.522585, -122.685699),
        CLLocationCoordinate2DMake(45.534611, -122.708873),
        CLLocationCoordinate2DMake(45.530883, -122.678833),
        CLLocationCoordinate2DMake(45.547115, -122.667503),
        CLLocationCoordinate2DMake(45.530643, -122.660121),
        CLLocationCoordinate2DMake(45.533529, -122.636260),
        CLLocationCoordinate2DMake(45.521743, -122.659091),
        CLLocationCoordinate2DMake(45.510677, -122.648792),
        CLLocationCoordinate2DMake(45.515008, -122.664070),
        CLLocationCoordinate2DMake(45.502496, -122.669048),
        CLLocationCoordinate2DMake(45.515369, -122.678489),
        CLLocationCoordinate2DMake(45.506346, -122.702007),
        CLLocationCoordinate2DMake(45.522585, -122.685699),
    };
    NSUInteger numberOfCoordinates = sizeof(coordinates) / sizeof(CLLocationCoordinate2D);

    // Create our shape with the formatted coordinates array
    MGLPolygon *shape = [MGLPolygon polygonWithCoordinates:coordinates count:numberOfCoordinates];

    // Add the shape to the map
    [self.mapView addAnnotation:shape];
}

- (CGFloat)mapView:(MGLMapView *)mapView alphaForShapeAnnotation:(MGLShape *)annotation {
    // Set the alpha for shape annotations to 0.5 (half opacity)
    return 0.5f;
}

- (UIColor *)mapView:(MGLMapView *)mapView strokeColorForShapeAnnotation:(MGLShape *)annotation {
    // Set the stroke color for shape annotations
    return [UIColor whiteColor];
}

- (UIColor *)mapView:(MGLMapView *)mapView fillColorForPolygonAnnotation:(MGLPolygon *)annotation {
    // Mapbox cyan fill color
    return [UIColor colorWithRed:59.0f/255.0f green:178.0f/255.0f blue:208.0f/255.0f alpha:1.0f];
}

@end
