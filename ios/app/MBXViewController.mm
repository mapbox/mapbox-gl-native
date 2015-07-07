#import "MBXViewController.h"

#import <mbgl/ios/MapboxGL.h>

#import <mbgl/platform/darwin/settings_nsuserdefaults.hpp>

#import <CoreLocation/CoreLocation.h>

static UIColor *const kTintColor = [UIColor colorWithRed:0.120 green:0.550 blue:0.670 alpha:1.000];

static NSArray *const kStyleNames = @[
    @"Mapbox Streets",
    @"Emerald",
    @"Light",
    @"Dark",
    @"Bright",
    @"Basic",
    @"Outdoors",
    @"Satellite",
];

static NSString *const kStyleVersion = @"7";

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

    self.mapView = [[MGLMapView alloc] initWithFrame:self.view.bounds];
    self.mapView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
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
    [titleButton setFrame:CGRectMake(0, 0, 150, 40)];
    [titleButton setTitle:[kStyleNames firstObject] forState:UIControlStateNormal];
    [titleButton setTitleColor:kTintColor forState:UIControlStateNormal];
    [titleButton addTarget:self action:@selector(cycleStyles) forControlEvents:UIControlEventTouchUpInside];
    self.navigationItem.titleView = titleButton;

    self.navigationItem.rightBarButtonItem = [[UIBarButtonItem alloc] initWithImage:[UIImage imageNamed:@"TrackingLocationOffMask.png"]
                                                                              style:UIBarButtonItemStylePlain
                                                                             target:self
                                                                             action:@selector(locateUser)];

    settings = new mbgl::Settings_NSUserDefaults();
    [self restoreState:nil];
}

- (void)saveState:(__unused NSNotification *)notification
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

- (void)restoreState:(__unused NSNotification *)notification
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
                                                                @"Empty Memory",
                                                                @"Add 100 Points",
                                                                @"Add 1,000 Points",
                                                                @"Add 10,000 Points",
                                                                @"Add Test Shapes",
                                                                @"Remove Annotations",
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
        [self.mapView emptyMemoryCache];
    }
    else if (buttonIndex == actionSheet.firstOtherButtonIndex + 4)
    {
        [self parseFeaturesAddingCount:100];
    }
    else if (buttonIndex == actionSheet.firstOtherButtonIndex + 5)
    {
        [self parseFeaturesAddingCount:1000];
    }
    else if (buttonIndex == actionSheet.firstOtherButtonIndex + 6)
    {
        [self parseFeaturesAddingCount:10000];
    }
    else if (buttonIndex == actionSheet.firstOtherButtonIndex + 7)
    {
        // PNW triangle
        //
        CLLocationCoordinate2D triangleCoordinates[3] =
        {
            CLLocationCoordinate2DMake(44, -122),
            CLLocationCoordinate2DMake(46, -122),
            CLLocationCoordinate2DMake(46, -121)
        };

        MGLPolygon *triangle = [MGLPolygon polygonWithCoordinates:triangleCoordinates count:3];

        [self.mapView addAnnotation:triangle];

        // Orcas Island hike
        //
        NSDictionary *hike = [NSJSONSerialization JSONObjectWithData:
                                 [NSData dataWithContentsOfFile:
                                     [[NSBundle mainBundle] pathForResource:@"polyline" ofType:@"geojson"]]
                                                             options:0
                                                               error:nil];

        NSArray *hikeCoordinatePairs = hike[@"features"][0][@"geometry"][@"coordinates"];

        CLLocationCoordinate2D *polylineCoordinates = (CLLocationCoordinate2D *)malloc([hikeCoordinatePairs count] * sizeof(CLLocationCoordinate2D));

        for (NSUInteger i = 0; i < [hikeCoordinatePairs count]; i++)
        {
            polylineCoordinates[i] = CLLocationCoordinate2DMake([hikeCoordinatePairs[i][1] doubleValue], [hikeCoordinatePairs[i][0] doubleValue]);
        }

        MGLPolyline *polyline = [MGLPolyline polylineWithCoordinates:polylineCoordinates
                                                               count:[hikeCoordinatePairs count]];

        [self.mapView addAnnotation:polyline];

        free(polylineCoordinates);

        // PA/NJ/DE polys
        //
        NSDictionary *threestates = [NSJSONSerialization JSONObjectWithData:
                              [NSData dataWithContentsOfFile:
                               [[NSBundle mainBundle] pathForResource:@"threestates" ofType:@"geojson"]]
                                                             options:0
                                                               error:nil];

        for (NSDictionary *feature in threestates[@"features"])
        {
            NSArray *stateCoordinatePairs = feature[@"geometry"][@"coordinates"];

            while ([stateCoordinatePairs count] == 1) stateCoordinatePairs = stateCoordinatePairs[0];

            CLLocationCoordinate2D *polygonCoordinates = (CLLocationCoordinate2D *)malloc([stateCoordinatePairs count] * sizeof(CLLocationCoordinate2D));

            for (NSUInteger i = 0; i < [stateCoordinatePairs count]; i++)
            {
                polygonCoordinates[i] = CLLocationCoordinate2DMake([stateCoordinatePairs[i][1] doubleValue], [stateCoordinatePairs[i][0] doubleValue]);
            }

            MGLPolygon *polygon = [MGLPolygon polygonWithCoordinates:polygonCoordinates count:[stateCoordinatePairs count]];

            [self.mapView addAnnotation:polygon];

            free(polygonCoordinates);
        }
    }
    else if (buttonIndex == actionSheet.firstOtherButtonIndex + 8)
    {
        [self.mapView removeAnnotations:self.mapView.annotations];
    }
}

- (void)parseFeaturesAddingCount:(NSUInteger)featuresCount
{
    [self.mapView removeAnnotations:self.mapView.annotations];

    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^
    {
        NSData *featuresData = [NSData dataWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"points" ofType:@"geojson"]];

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

                MGLPointAnnotation *annotation = [MGLPointAnnotation new];
                annotation.coordinate = coordinate;
                annotation.title = title;

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

    self.mapView.styleURL = [NSURL URLWithString:
        [NSString stringWithFormat:@"asset://styles/%@-v%@.json",
            [[styleName lowercaseString] stringByReplacingOccurrencesOfString:@" " withString:@"-"],
            kStyleVersion]];

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

#pragma mark - MGLMapViewDelegate

- (MGLAnnotationImage *)mapView:(MGLMapView * __nonnull)mapView imageForAnnotation:(id <MGLAnnotation> __nonnull)annotation
{
    if ([[((MGLPointAnnotation *)annotation) title] hasSuffix:@"7"])
    {
        NSLog(@"iOS: %p annotation title ends in '7'; using default sprite", annotation);

        return nil;
    }

    static NSString *identifier = @"red_square";

    MGLAnnotationImage *image = [mapView dequeueReusableAnnotationImageWithIdentifier:identifier];

    if ( ! image)
    {
        NSLog(@"iOS: creating sprite for %p / %@", annotation, identifier);

        CGRect rect = CGRectMake(0, 0, 20, 20);
        UIGraphicsBeginImageContextWithOptions(rect.size, NO, [[UIScreen mainScreen] scale]);
        CGContextRef ctx = UIGraphicsGetCurrentContext();
        CGContextSetFillColorWithColor(ctx, [[[UIColor redColor] colorWithAlphaComponent:0.25] CGColor]);
        CGContextFillRect(ctx, rect);
        image = [MGLAnnotationImage annotationImageWithImage:UIGraphicsGetImageFromCurrentImageContext() reuseIdentifier:identifier];
        UIGraphicsEndImageContext();
    }
    else
    {
        NSLog(@"iOS: reusing sprite for %p / %@", annotation, identifier);
    }

    return image;
}

- (BOOL)mapView:(__unused MGLMapView *)mapView annotationCanShowCallout:(__unused id <MGLAnnotation>)annotation
{
    return YES;
}

- (CGFloat)mapView:(__unused MGLMapView *)mapView alphaForShapeAnnotation:(MGLShape *)annotation
{
    return ([annotation isKindOfClass:[MGLPolygon class]] ? 0.5 : 1.0);
}

- (UIColor *)mapView:(__unused MGLMapView *)mapView strokeColorForShapeAnnotation:(MGLShape *)annotation
{
    return ([annotation isKindOfClass:[MGLPolyline class]] ? [UIColor purpleColor] : [UIColor blackColor]);
}

- (UIColor *)mapView:(__unused MGLMapView *)mapView fillColorForPolygonAnnotation:(__unused MGLPolygon *)annotation
{
    return (annotation.pointCount > 3 ? [UIColor greenColor] : [UIColor redColor]);
}

- (void)mapView:(__unused MGLMapView *)mapView didChangeUserTrackingMode:(MGLUserTrackingMode)mode animated:(__unused BOOL)animated
{
    UIImage *newButtonImage;
    
    switch (mode) {
        case MGLUserTrackingModeNone:
            newButtonImage = [UIImage imageNamed:@"TrackingLocationOffMask.png"];
            break;
            
        case MGLUserTrackingModeFollow:
            newButtonImage = [UIImage imageNamed:@"TrackingLocationMask.png"];
            break;
            
        case MGLUserTrackingModeFollowWithHeading:
            newButtonImage = [UIImage imageNamed:@"TrackingHeadingMask.png"];
            break;
    }
    
    [UIView animateWithDuration:0.25 animations:^{
        self.navigationItem.rightBarButtonItem.image = newButtonImage;
    }];
}

@end
