#import "MBXViewController.h"

#import <mbgl/ios/MapboxGL.h>

#import <mbgl/platform/darwin/settings_nsuserdefaults.hpp>

#import <CoreLocation/CoreLocation.h>

#import "MBXAnnotation.h"

static UIColor *const kTintColor = [UIColor colorWithRed:0.120 green:0.550 blue:0.670 alpha:1.000];

static NSArray *const kStyleNames = @[
    @"Emerald",
    @"Bright",
    @"Basic",
    @"Outdoors",
    @"Satellite",
    @"Hybrid",
];

static NSString *const kStyleVersion = @"v7";

@interface MBXViewController () <UIActionSheetDelegate, MGLMapViewDelegate>

@property (nonatomic) MGLMapView *mapView;

@end

@implementation MBXViewController

mbgl::Settings_NSUserDefaults *settings = nullptr;

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

    NSString *accessToken = [[NSProcessInfo processInfo] environment][@"MAPBOX_ACCESS_TOKEN"];
    if (accessToken) {
        // Store to preferences so that we can launch the app later on without having to specify
        // token.
        [[NSUserDefaults standardUserDefaults] setObject:accessToken forKey:@"access_token"];
    } else {
        // Try to retrieve from preferences, maybe we've stored them there previously and can reuse
        // the token.
        accessToken = [[NSUserDefaults standardUserDefaults] objectForKey:@"access_token"];
    }

    if ( ! accessToken) NSLog(@"No access token set. Mapbox vector tiles won't work.");

    self.mapView = [[MGLMapView alloc] initWithFrame:self.view.bounds accessToken:accessToken];
    self.mapView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
    self.mapView.viewControllerForLayoutGuides = self;
    self.mapView.showsUserLocation = YES;
    self.mapView.delegate = self;
    [self.view addSubview:self.mapView];

    self.view.tintColor = kTintColor;
    self.navigationController.navigationBar.tintColor = kTintColor;
    self.mapView.tintColor = kTintColor;

    self.navigationItem.leftBarButtonItem = [[UIBarButtonItem alloc] initWithImage:[UIImage imageNamed:@"settings.png"]
                                                                             style:UIBarButtonItemStylePlain
                                                                            target:self
                                                                            action:@selector(showSettings)];

    UIButton *titleButton = [UIButton buttonWithType:UIButtonTypeCustom];
    [titleButton setFrame:CGRectMake(0, 0, 120, 40)];
    [titleButton setTitle:[kStyleNames firstObject] forState:UIControlStateNormal];
    [titleButton setTitleColor:kTintColor forState:UIControlStateNormal];
    [titleButton addTarget:self action:@selector(cycleStyles) forControlEvents:UIControlEventTouchUpInside];
    self.navigationItem.titleView = titleButton;

    self.navigationItem.rightBarButtonItem = [[UIBarButtonItem alloc] initWithImage:[UIImage imageNamed:@"locateUser.png"]
                                                                              style:UIBarButtonItemStylePlain
                                                                             target:self
                                                                             action:@selector(locateUser)];

    settings = new mbgl::Settings_NSUserDefaults();
    [self restoreState:nil];
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
        settings->bearing = self.mapView.direction;
        settings->debug = self.mapView.isDebugActive;
        settings->userTrackingMode = self.mapView.userTrackingMode;
        settings->showsUserLocation = self.mapView.showsUserLocation;
        settings->save();
    }
}

- (void)restoreState:(NSNotification *)notification
{
    if (self.mapView && settings) {
        settings->load();
        [self.mapView setCenterCoordinate:CLLocationCoordinate2DMake(settings->latitude, settings->longitude) zoomLevel:settings->zoom animated:NO];
        self.mapView.direction = settings->bearing;
        self.mapView.userTrackingMode = settings->userTrackingMode;
        self.mapView.showsUserLocation = settings->showsUserLocation;
        [self.mapView setDebugActive:settings->debug];
    }
}

#pragma clang diagnostic pop

- (NSUInteger)supportedInterfaceOrientations
{
    return UIInterfaceOrientationMaskAll;
}

#pragma mark - Actions

- (void)showSettings
{
    UIActionSheet *sheet = [[UIActionSheet alloc] initWithTitle:@"Map Settings"
                                                       delegate:self
                                              cancelButtonTitle:@"Cancel"
                                         destructiveButtonTitle:nil
                                              otherButtonTitles:@"Reset North",
                                                                @"Reset Position",
                                                                @"Toggle Debug",
                                                                @"Add 100 Points",
                                                                @"Add 1,000 Points",
                                                                @"Add 10,000 Points",
                                                                @"Remove Points",
                                                                nil];

    [sheet showFromBarButtonItem:self.navigationItem.leftBarButtonItem animated:YES];
}

- (void)actionSheet:(UIActionSheet *)actionSheet didDismissWithButtonIndex:(NSInteger)buttonIndex
{
    if (buttonIndex == actionSheet.firstOtherButtonIndex)
    {
        [self.mapView resetNorth];
    }
    else if (buttonIndex == actionSheet.firstOtherButtonIndex + 1)
    {
        [self.mapView resetPosition];
    }
    else if (buttonIndex == actionSheet.firstOtherButtonIndex + 2)
    {
        [self.mapView toggleDebug];
    }
    else if (buttonIndex == actionSheet.firstOtherButtonIndex + 3)
    {
        [self parseFeaturesAddingCount:100];
    }
    else if (buttonIndex == actionSheet.firstOtherButtonIndex + 4)
    {
        [self parseFeaturesAddingCount:1000];
    }
    else if (buttonIndex == actionSheet.firstOtherButtonIndex + 5)
    {
        [self parseFeaturesAddingCount:10000];
    }
    else if (buttonIndex == actionSheet.firstOtherButtonIndex + 6)
    {
        [self.mapView removeAnnotations:self.mapView.annotations];
    }
}

- (void)parseFeaturesAddingCount:(NSUInteger)featuresCount
{
    [self.mapView removeAnnotations:self.mapView.annotations];

    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^
    {
        NSData *featuresData = [NSData dataWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"features" ofType:@"geojson"]];

        id features = [NSJSONSerialization JSONObjectWithData:featuresData
                                                      options:0
                                                        error:nil];

        if ([features isKindOfClass:[NSDictionary class]])
        {
            NSMutableArray *annotations = [NSMutableArray array];

            for (NSDictionary *feature in features[@"features"])
            {
                CLLocationCoordinate2D coordinate = CLLocationCoordinate2DMake([feature[@"geometry"][@"coordinates"][1] doubleValue],
                                                                               [feature[@"geometry"][@"coordinates"][0] doubleValue]);
                NSString *title = feature[@"properties"][@"NAME"];

                MBXAnnotation *annotation = [MBXAnnotation annotationWithLocation:coordinate
                                                                            title:title
                                                                         subtitle:nil];

                [annotations addObject:annotation];

                if (annotations.count == featuresCount) break;
            }

            dispatch_async(dispatch_get_main_queue(), ^
            {
                [self.mapView addAnnotations:annotations];

                [self.mapView setCenterCoordinate:CLLocationCoordinate2DMake(38.904722, -77.016389)
                                        zoomLevel:10
                                         animated:NO];

                [self.mapView setDirection:0];
            });
        }
    });
}

- (void)cycleStyles
{
    UIButton *titleButton = (UIButton *)self.navigationItem.titleView;

    NSString *styleName = [titleButton titleForState:UIControlStateNormal];

    if ( ! styleName)
    {
        styleName = [kStyleNames firstObject];
    }
    else
    {
        NSUInteger index = [kStyleNames indexOfObject:styleName] + 1;
        if (index == [kStyleNames count]) index = 0;
        styleName = [kStyleNames objectAtIndex:index];
    }

    [self.mapView useBundledStyleNamed:
        [[[styleName lowercaseString]
        stringByAppendingString:@"-"]
        stringByAppendingString:kStyleVersion]];

    [titleButton setTitle:styleName forState:UIControlStateNormal];
}

- (void)locateUser
{
    if (self.mapView.userTrackingMode == MGLUserTrackingModeNone)
    {
        self.mapView.userTrackingMode = MGLUserTrackingModeFollow;
    }
    else if (self.mapView.userTrackingMode == MGLUserTrackingModeFollow)
    {
        self.mapView.userTrackingMode = MGLUserTrackingModeFollowWithHeading;
    }
    else
    {
        self.mapView.userTrackingMode = MGLUserTrackingModeNone;
    }
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

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"

#pragma mark - MGLMapViewDelegate

- (BOOL)mapView:(MGLMapView *)mapView annotationCanShowCallout:(id <MGLAnnotation>)annotation
{
    return YES;
}

#pragma clang diagnostic pop

@end
