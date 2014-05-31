#import "MBXViewController.h"

#import "MVKMapView.h"

#import "../common/settings_nsuserdefaults.hpp"

#import <CoreLocation/CoreLocation.h>

@interface MBXViewController () <CLLocationManagerDelegate>

@property (nonatomic) MVKMapView *mapView;
@property (nonatomic) BOOL debug;
@property (nonatomic) UIView *palette;
@property (nonatomic) CLLocationManager *locationManager;

@end

@implementation MBXViewController

llmr::Settings_NSUserDefaults *settings = nullptr;

#pragma mark - Setup

- (id)init
{
    self = [super init];

    if (self)
    {
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(saveState:) name:UIApplicationDidEnterBackgroundNotification object:nil];
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(restoreState:) name:UIApplicationWillEnterForegroundNotification object:nil];
    }

    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];

    self.mapView = [[MVKMapView alloc] initWithFrame:self.view.bounds];
    self.mapView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
    [self.view addSubview:self.mapView];

    settings = new llmr::Settings_NSUserDefaults();
    [self restoreState:nil];

    [self setupDebugUI];

    self.locationManager = [CLLocationManager new];
    self.locationManager.delegate = self;
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"

- (void)saveState:(NSNotification *)notification
{
    if (self.mapView && settings)
    {
        settings->longitude = self.mapView.centerCoordinate.longitude;
        settings->latitude = self.mapView.centerCoordinate.latitude;
        settings->zoom = self.mapView.zoomLevel;
        settings->angle = self.mapView.direction;
        settings->debug = self.mapView.isDebugActive;
        settings->save();
    }
}

- (void)restoreState:(NSNotification *)notification
{
    if (self.mapView && settings) {
        settings->load();
        [self.mapView setCenterCoordinate:CLLocationCoordinate2DMake(settings->latitude, settings->longitude) zoomLevel:settings->zoom animated:NO];
        self.mapView.direction = settings->angle;
        [self.mapView setDebugActive:settings->debug];
    }
}

#pragma clang diagnostic pop

- (void)setupDebugUI
{
    UITapGestureRecognizer *singleTap = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(handleSingleTapGesture:)];
    singleTap.numberOfTapsRequired = 1;
    [self.mapView addGestureRecognizer:singleTap];

    NSArray *selectorNames = @[ @"unrotate", @"resetPosition", @"toggleDebug", @"toggleStyle", @"locateUser" ];
    CGFloat buttonSize  = 40;
    CGFloat bufferSize  = 20;
    CGFloat alpha       = 0.75;
    CGFloat paletteWidth  = buttonSize + (2 * bufferSize);
    CGFloat paletteHeight = [selectorNames count] * (buttonSize + bufferSize) + bufferSize;
    self.palette = [[UIView alloc] initWithFrame:CGRectMake(self.view.bounds.size.width - paletteWidth,
                                                            (self.view.bounds.size.height - paletteHeight) / 2,
                                                            paletteWidth,
                                                            paletteHeight)];
    self.palette.backgroundColor = [UIColor colorWithWhite:0 alpha:alpha];
    self.palette.autoresizingMask = UIViewAutoresizingFlexibleLeftMargin | UIViewAutoresizingFlexibleTopMargin | UIViewAutoresizingFlexibleBottomMargin;
    self.palette.layer.cornerRadius = bufferSize;
    [self.view addSubview:self.palette];
    for (NSString *selectorName in selectorNames)
    {
        UIButton *button = [UIButton buttonWithType:UIButtonTypeCustom];
        button.frame = CGRectMake(bufferSize,
                                  ([selectorNames indexOfObject:selectorName] * (buttonSize + bufferSize)) + bufferSize,
                                  buttonSize,
                                  buttonSize);
        [button setImage:[UIImage imageNamed:[selectorName stringByAppendingString:@".png"]] forState:UIControlStateNormal];
        [button addTarget:self action:NSSelectorFromString(selectorName) forControlEvents:UIControlEventTouchUpInside];
        [self.palette addSubview:button];
    }
}

- (NSUInteger)supportedInterfaceOrientations
{
    return UIInterfaceOrientationMaskAll;
}

#pragma mark - Debugging UI

- (void)handleSingleTapGesture:(UITapGestureRecognizer *)singleTap
{
    if (singleTap.state == UIGestureRecognizerStateEnded)
    {
        [self togglePalette];
    }
}

- (void)togglePalette
{
    if (self.palette.alpha < 1)
    {
        self.palette.userInteractionEnabled = YES;

        [UIView animateWithDuration:0.25 animations:^(void)
        {
            self.palette.alpha = 1;
        }];
    }
    else
    {
        self.palette.userInteractionEnabled = NO;

        [UIView animateWithDuration:0.25 animations:^(void)
        {
            self.palette.alpha = 0;
        }];
    }
}

- (void)unrotate
{
    [self.mapView resetNorth];
}

- (void)resetPosition
{
    [self.mapView resetPosition];
}

- (void)toggleDebug
{
    [self.mapView toggleDebug];

    self.debug = ! self.debug;
}

- (void)toggleStyle
{
    [self.mapView toggleStyle];
}

- (void)locateUser
{
    [NSObject cancelPreviousPerformRequestsWithTarget:self.locationManager selector:@selector(stopUpdatingLocation) object:nil];

    [self.locationManager startUpdatingLocation];

    [self.locationManager performSelector:@selector(stopUpdatingLocation) withObject:nil afterDelay:5.0];
}

#pragma mark - Destruction

- (void)dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver:self];

    if (settings)
    {
        [self saveState:nil];
        delete settings;
        settings = nullptr;
    }
}

#pragma mark - User location

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"

- (void)locationManager:(CLLocationManager *)manager didUpdateLocations:(NSArray *)locations
{
    CLLocation *latestLocation = locations.lastObject;

    [self.mapView setCenterCoordinate:CLLocationCoordinate2DMake(latestLocation.coordinate.latitude, latestLocation.coordinate.longitude) zoomLevel:17 animated:YES];
}

#pragma clang diagnostic pop

@end
