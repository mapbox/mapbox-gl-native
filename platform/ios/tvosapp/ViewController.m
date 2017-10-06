#import "ViewController.h"

#import <Mapbox/Mapbox.h>

static const CLLocationCoordinate2D WorldTourDestinations[] = {
    { .latitude = 38.8999418, .longitude = -77.033996 },
    { .latitude = 37.7884307, .longitude = -122.3998631 },
    { .latitude = 52.5003103, .longitude = 13.4197763 },
    { .latitude = 60.1712627, .longitude = 24.9378866 },
    { .latitude = 53.8948782, .longitude = 27.5558476 },
};

@interface ViewController () <MGLMapViewDelegate>

@property (weak, nonatomic) IBOutlet MGLMapView *mapView;

@end

@implementation ViewController {
    BOOL _isTouringWorld;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
}


- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (IBAction)startWorldTour:(id)sender {
    _isTouringWorld = YES;

    [self.mapView removeAnnotations:self.mapView.annotations];
    NSUInteger numberOfAnnotations = sizeof(WorldTourDestinations) / sizeof(WorldTourDestinations[0]);
    NSMutableArray *annotations = [NSMutableArray arrayWithCapacity:numberOfAnnotations];
    for (NSUInteger i = 0; i < numberOfAnnotations; i++) {
        MGLPointAnnotation *annotation = [[MGLPointAnnotation alloc] init];
        annotation.coordinate = WorldTourDestinations[i];
        [annotations addObject:annotation];
    }
    [self.mapView addAnnotations:annotations];
    [self continueWorldTourWithRemainingAnnotations:annotations];
}

- (void)continueWorldTourWithRemainingAnnotations:(NSMutableArray<MGLPointAnnotation *> *)annotations {
    MGLPointAnnotation *nextAnnotation = annotations.firstObject;
    if (!nextAnnotation || !_isTouringWorld) {
        _isTouringWorld = NO;
        return;
    }
    
    [annotations removeObjectAtIndex:0];
    MGLMapCamera *camera = [MGLMapCamera cameraLookingAtCenterCoordinate:nextAnnotation.coordinate
                                                          acrossDistance:10
                                                                   pitch:arc4random_uniform(60)
                                                                 heading:arc4random_uniform(360)];
    __weak ViewController *weakSelf = self;
    [self.mapView flyToCamera:camera completionHandler:^{
        ViewController *strongSelf = weakSelf;
        [strongSelf performSelector:@selector(continueWorldTourWithRemainingAnnotations:)
                         withObject:annotations
                         afterDelay:2];
    }];
}

#pragma mark MGLMapViewDelegate methods

- (void)mapView:(MGLMapView *)mapView didFinishLoadingStyle:(MGLStyle *)style {
    [style localizeLabelsIntoLocale:nil];
}

@end
