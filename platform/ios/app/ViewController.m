#import "ViewController.h"
@import Mapbox;

@interface MyCustomPointAnnotation : MGLPointAnnotation
@property (nonatomic, assign) BOOL willUseImage;
@end

@implementation MyCustomPointAnnotation
@end


@interface ViewController () <MGLMapViewDelegate>

@property (nonatomic, strong) MGLMapView *mapView;
@property (nonatomic, strong) NSMutableArray *annotations;
@property (nonatomic, strong) NSTimer *timer;

@end

@implementation ViewController

@synthesize mapView;

- (void)viewDidLoad {
    [super viewDidLoad];
    
    mapView = [[MGLMapView alloc] initWithFrame:self.view.bounds
                                       styleURL:[MGLStyle lightStyleURL]];
    
    mapView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
    
    mapView.centerCoordinate = CLLocationCoordinate2DMake(36.54,-116.97);
    mapView.zoomLevel = 9;
    mapView.delegate = self;
    [self.view addSubview:mapView];
    
    _annotations = [NSMutableArray array];
}

- (int)getRandomNumber:(int)from to:(int)to
{
    return (int)(from + (arc4random() % (to - from + 1)));
}

- (NSString *)getRandomString{
    char random[10];
    for(int i = 0 ; i < 10 ; i++){
        char p = [self getRandomNumber:65 to:122];
        random[i] = p;
    }
    return [NSString stringWithCString:random encoding:NSUTF8StringEncoding];
}

- (void)addAnnotations{
    [mapView removeAnnotations:_annotations];
    [_annotations removeAllObjects];
    
    for(int i = 0 ; i < 100; i++){
        MyCustomPointAnnotation *pointD = [[MyCustomPointAnnotation alloc] init];
        pointD.title = [self getRandomString];
        pointD.coordinate = CLLocationCoordinate2DMake([self getRandomNumber:-90 to:90],[self getRandomNumber:-180 to:180]);
        [_annotations addObject:pointD];
    }
    [mapView addAnnotations:_annotations];
}

- (void)mapViewDidFinishLoadingMap:(MGLMapView *)mapView{
    _timer = [NSTimer scheduledTimerWithTimeInterval:5.0f repeats:YES block:^(NSTimer * _Nonnull timer) {
        [self addAnnotations];
        [mapView setCenterCoordinate:CLLocationCoordinate2DMake([self getRandomNumber:-90 to:90],[self getRandomNumber:-180 to:180]) zoomLevel:[self getRandomNumber:1 to:3] animated:YES];
    }];
}


- (MGLAnnotationView *)mapView:(MGLMapView *)mapView viewForAnnotation:(id <MGLAnnotation>)annotation {
    if ([annotation isKindOfClass:[MyCustomPointAnnotation class]]) {
        MyCustomPointAnnotation *castAnnotation = (MyCustomPointAnnotation *)annotation;
        
        if (castAnnotation.willUseImage) {
            return nil;
        }
    }
    NSString *reuseIdentifier = @"reusableDotView";
    MGLAnnotationView *annotationView = [mapView dequeueReusableAnnotationViewWithIdentifier:reuseIdentifier];
    
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

- (BOOL)mapView:(MGLMapView *)mapView annotationCanShowCallout:(id<MGLAnnotation>)annotation {
    return YES;
}

@end
