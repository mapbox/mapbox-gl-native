@import Mapbox;

#import "MBXCustomLocationViewController.h"

@interface MBXCustomLocationManager : NSObject<MGLLocationManager>
@end

@interface MBXCustomLocationManager()

@property (nonatomic) CLLocationManager *locationManager;
@property (nonatomic, strong) NSTimer *locationUpdateTimer;
@property (nonatomic) NSUInteger index;
@property (strong, nonatomic) NSDictionary *routeCoordinates;
@property (strong, nonatomic) NSArray *coordinates;

@end

@implementation MBXCustomLocationManager

@synthesize delegate;

- (instancetype)init
{
    if (self = [super init]) {
        _locationManager = [[CLLocationManager alloc] init];
        _index = 0;
    }
    return self;
}

- (CLAuthorizationStatus)authorizationStatus
{
    return [CLLocationManager authorizationStatus];
}

- (void)setHeadingOrientation:(CLDeviceOrientation)headingOrientation
{
    _locationManager.headingOrientation = headingOrientation;
}

- (CLDeviceOrientation)headingOrientation
{
    return _locationManager.headingOrientation;
}

- (void)requestAlwaysAuthorization
{
    [self.locationManager requestAlwaysAuthorization];
}

- (void)requestWhenInUseAuthorization
{
    [self.locationManager requestWhenInUseAuthorization];
}

- (void)startUpdatingHeading
{
    [self.locationManager startUpdatingHeading];
}

- (void)startUpdatingLocation
{
    [self loadRouteCoordinates];
    self.locationUpdateTimer = [NSTimer scheduledTimerWithTimeInterval:0.8
                                                                target:self
                                                              selector:@selector(updateLocation)
                                                              userInfo:nil
                                                               repeats:YES];
}

- (void)stopUpdatingHeading
{
    [self.locationManager stopUpdatingHeading];
}

- (void)stopUpdatingLocation
{
    [self.locationUpdateTimer invalidate];
    self.locationUpdateTimer = nil;
}

- (void)dismissHeadingCalibrationDisplay
{
    [self.locationManager dismissHeadingCalibrationDisplay];
}

- (void)dealloc
{
    [self.locationManager stopUpdatingLocation];
    [self.locationManager stopUpdatingHeading];
    self.delegate = nil;
}

#pragma mark - Location Updates

- (void)loadRouteCoordinates
{
    NSString *filePath = [[NSBundle bundleForClass:self.class] pathForResource:@"simple_route" ofType:@"json"];
    NSData *data = [NSData dataWithContentsOfFile:filePath];
    _routeCoordinates = [NSJSONSerialization JSONObjectWithData:data options:kNilOptions error:nil];
    _coordinates = [self.routeCoordinates objectForKey:@"coordinates"];
}

- (void)updateLocation
{
    if ([self.delegate respondsToSelector:@selector(locationManager:didUpdateLocations:)]) {
        
        if (self.index >= [self.coordinates count] ) {
            self.index = 0;
            self.coordinates = [[self.coordinates reverseObjectEnumerator] allObjects];
        }
        NSArray *loc = self.coordinates[self.index];
        CLLocationDegrees latitude = [[loc objectAtIndex:1] doubleValue];
        CLLocationDegrees longitude = [[loc objectAtIndex:0] doubleValue];
        CLLocation *location = [[CLLocation alloc] initWithLatitude:latitude longitude:longitude];
        self.index++;
        [self.delegate locationManager:self didUpdateLocations:@[location]];
    }
}

- (void)locationManager:(CLLocationManager *)manager didUpdateHeading:(CLHeading *)newHeading
{
    if ([self.delegate respondsToSelector:@selector(locationManager:didUpdateHeading:)]) {
        [self.delegate locationManager:self didUpdateHeading:newHeading];
    }
}

- (BOOL)locationManagerShouldDisplayHeadingCalibration:(CLLocationManager *)manager
{
    if ([self.delegate respondsToSelector:@selector(locationManagerShouldDisplayHeadingCalibration:)]) {
        return [self.delegate locationManagerShouldDisplayHeadingCalibration:self];
    }
    
    return NO;
}

- (void)locationManager:(CLLocationManager *)locationManager didFailWithError:(nonnull NSError *)error {
    if ([self.delegate respondsToSelector:@selector(locationManager:didFailWithError:)]) {
        [self.delegate locationManager:self didFailWithError:error];
    }
}

@end

@interface MBXCustomLocationViewController ()

@property (strong, nonatomic) MGLMapView *mapView;

@end

@implementation MBXCustomLocationViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    self.mapView = [[MGLMapView alloc] initWithFrame:self.view.bounds];
    
    self.mapView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
    MBXCustomLocationManager *mbxLocationManager = [[MBXCustomLocationManager alloc] init];
    self.mapView.locationManager = mbxLocationManager;
    // Set the map’s center coordinate and zoom level.
    [self.mapView setCenterCoordinate:CLLocationCoordinate2DMake(37.788380, -122.400121)
                            zoomLevel:13
                        animated:NO];
    
    [self.view addSubview:self.mapView];
    self.mapView.showsUserLocation = YES;
}

- (void)viewWillDisappear:(BOOL)animated {
    [super viewWillDisappear:animated];
    self.mapView.showsUserLocation = NO;
}

@end
