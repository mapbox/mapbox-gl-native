#import <Mapbox/Mapbox.h>

#import "../src/MGLMapView_Experimental.h"
#import "MBXFrameTimeGraphView.h"
#import "MBXTestViewController.h"

@interface CustomAnnotationView : MGLAnnotationView
@end

@implementation CustomAnnotationView

- (void)layoutSubviews {
    [super layoutSubviews];
    
    // Use CALayer’s corner radius to turn this view into a circle.
    self.layer.cornerRadius = self.bounds.size.width / 2;
    self.layer.borderWidth = 2;
    self.layer.borderColor = [UIColor whiteColor].CGColor;
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated {
    [super setSelected:selected animated:animated];
    
    // Animate the border width in/out, creating an iris effect.
    CABasicAnimation *animation = [CABasicAnimation animationWithKeyPath:@"borderWidth"];
    animation.duration = 0.1;
    self.layer.borderWidth = selected ? self.bounds.size.width / 4 : 2;
    [self.layer addAnimation:animation forKey:@"borderWidth"];
}

@end


//
// Example view controller
@interface MBXTestViewController () <MGLMapViewDelegate>
@property (nonatomic) MGLMapView *mapView;
@property (nonatomic) NSTimer *timer;
@end

@implementation MBXTestViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    self.mapView = [[MGLMapView alloc] initWithFrame:self.view.bounds];
    self.mapView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
    self.mapView.styleURL = [MGLStyle darkStyleURL];
    self.mapView.tintColor = [UIColor lightGrayColor];
    self.mapView.centerCoordinate = CLLocationCoordinate2DMake(0, 66);
    self.mapView.zoomLevel = 2;
    self.mapView.delegate = self;
    self.mapView.showsUserLocation = YES;
    self.mapView.userTrackingMode = MGLUserTrackingModeFollow;
    [self.view addSubview:self.mapView];
    
    // Specify coordinates for our annotations.
    CLLocationCoordinate2D coordinates[] = {
        CLLocationCoordinate2DMake(0, 33),
        CLLocationCoordinate2DMake(0, 66),
        CLLocationCoordinate2DMake(0, 99),
    };
    NSUInteger numberOfCoordinates = sizeof(coordinates) / sizeof(CLLocationCoordinate2D);
    
    // Fill an array with point annotations and add it to the map.
    NSMutableArray *pointAnnotations = [NSMutableArray arrayWithCapacity:numberOfCoordinates];
    for (NSUInteger i = 0; i < numberOfCoordinates; i++) {
        CLLocationCoordinate2D coordinate = coordinates[i];
        MGLPointAnnotation *point = [[MGLPointAnnotation alloc] init];
        point.coordinate = coordinate;
        point.title = [NSString stringWithFormat:@"%.f, %.f", coordinate.latitude, coordinate.longitude];
        [pointAnnotations addObject:point];
    }
    [self.mapView addAnnotations:pointAnnotations];
//    [_timer invalidate];
//    _timer = [NSTimer scheduledTimerWithTimeInterval:5 target:self selector:@selector(updateCenter) userInfo:nil repeats:YES];
}

- (void)updateCenter {
    //    CLLocationCoordinate2D coord = CLLocationCoordinate2DMake(arc4random_uniform(180) - 90, arc4random_uniform(360) - 180);
    //    self.mapView.centerCoordinate = coord;
}
#pragma mark - MGLMapViewDelegate methods

// This delegate method is where you tell the map to load a view for a specific annotation. To load a static MGLAnnotationImage, you would use `-mapView:imageForAnnotation:`.
//- (MGLAnnotationView *)mapView:(MGLMapView *)mapView viewForAnnotation:(id <MGLAnnotation>)annotation {
//    // This example is only concerned with point annotations.
//    if (![annotation isKindOfClass:[MGLPointAnnotation class]]) {
//        return nil;
//    }
//
//    // Use the point annotation’s longitude value (as a string) as the reuse identifier for its view.
//    NSString *reuseIdentifier = [NSString stringWithFormat:@"%f", annotation.coordinate.longitude];
//
//    // For better performance, always try to reuse existing annotations.
//    CustomAnnotationView *annotationView = [mapView dequeueReusableAnnotationViewWithIdentifier:reuseIdentifier];
//
//    // If there’s no reusable annotation view available, initialize a new one.
//    if (!annotationView) {
//        annotationView = [[CustomAnnotationView alloc] initWithReuseIdentifier:reuseIdentifier];
//        annotationView.bounds = CGRectMake(0, 0, 40, 40);
//
//        // Set the annotation view’s background color to a value determined by its longitude.
//        CGFloat hue = (CGFloat)annotation.coordinate.longitude / 100;
//        annotationView.backgroundColor = [UIColor colorWithHue:hue saturation:0.5 brightness:1 alpha:1];
//    }
////    [annotationView setHidden:YES];
//    return annotationView;
//}

- (BOOL)mapView:(MGLMapView *)mapView annotationCanShowCallout:(id<MGLAnnotation>)annotation {
    return YES;
}

@end
