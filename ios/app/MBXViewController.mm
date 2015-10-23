#import "MBXViewController.h"

#import <mbgl/ios/Mapbox.h>

#import <CoreLocation/CoreLocation.h>

static UIColor *const kTintColor = [UIColor colorWithRed:0.120 green:0.550 blue:0.670 alpha:1.000];

static struct MBXStyle {
    NSString *name;
    NSString *displayName;
} MBXAvailableStyles[] = {
    {@"streets",            @"Streets"},
    {@"emerald",            @"Emerald"},
    {@"light",              @"Light"},
    {@"dark",               @"Dark"},
    {@"satellite",          @"Satellite"},
    {@"satellite-hybrid",   @"Hybrid"},
};

static NSUInteger const kStyleVersion = 8;

@interface MBXViewController () <UIActionSheetDelegate, MGLMapViewDelegate>

@property (nonatomic) MGLMapView *mapView;
@property (nonatomic) NSUInteger styleIndex;

@end

@implementation MBXViewController

#pragma mark - Setup

+ (void)initialize
{
    if (self == [MBXViewController class])
    {
        [[NSUserDefaults standardUserDefaults] registerDefaults:@{
            @"MBXUserTrackingMode": @(MGLUserTrackingModeNone),
            @"MBXShowsUserLocation": @NO,
            @"MBXDebug": @NO,
        }];
    }
}

- (id)init
{
    self = [super init];

    if (self)
    {
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(saveState:) name:UIApplicationDidEnterBackgroundNotification object:nil];
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(restoreState:) name:UIApplicationWillEnterForegroundNotification object:nil];
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(saveState:) name:UIApplicationWillTerminateNotification object:nil];
    }

    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];

    self.mapView = [[MGLMapView alloc] initWithFrame:self.view.bounds];
    self.mapView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
    self.mapView.delegate = self;
    [self.view addSubview:self.mapView];

    self.view.tintColor = kTintColor;
    self.navigationController.navigationBar.tintColor = kTintColor;
    self.mapView.tintColor = kTintColor;

    self.navigationItem.leftBarButtonItem = [[UIBarButtonItem alloc] initWithImage:[UIImage imageNamed:@"settings.png"]
                                                                             style:UIBarButtonItemStylePlain
                                                                            target:self
                                                                            action:@selector(showSettings)];
    
    self.styleIndex = 0;

    UIButton *titleButton = [UIButton buttonWithType:UIButtonTypeCustom];
    [titleButton setFrame:CGRectMake(0, 0, 150, 40)];
    [titleButton setTitle:MBXAvailableStyles[self.styleIndex].displayName forState:UIControlStateNormal];
    [titleButton setTitleColor:kTintColor forState:UIControlStateNormal];
    [titleButton addTarget:self action:@selector(cycleStyles) forControlEvents:UIControlEventTouchUpInside];
    self.navigationItem.titleView = titleButton;

    self.navigationItem.rightBarButtonItem = [[UIBarButtonItem alloc] initWithImage:[UIImage imageNamed:@"TrackingLocationOffMask.png"]
                                                                              style:UIBarButtonItemStylePlain
                                                                             target:self
                                                                             action:@selector(locateUser)];

    [self.mapView addGestureRecognizer:[[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(handleLongPress:)]];

    [self restoreState:nil];
}

- (void)saveState:(__unused NSNotification *)notification
{
    if (self.mapView)
    {
        NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
        NSData *archivedCamera = [NSKeyedArchiver archivedDataWithRootObject:self.mapView.camera];
        [defaults setObject:archivedCamera forKey:@"MBXCamera"];
        [defaults setInteger:self.mapView.userTrackingMode forKey:@"MBXUserTrackingMode"];
        [defaults setBool:self.mapView.showsUserLocation forKey:@"MBXShowsUserLocation"];
        [defaults setBool:self.mapView.debugActive forKey:@"MBXDebug"];
        [defaults synchronize];
    }
}

- (void)restoreState:(__unused NSNotification *)notification
{
    if (self.mapView) {
        NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
        NSData *archivedCamera = [defaults objectForKey:@"MBXCamera"];
        MGLMapCamera *camera = archivedCamera ? [NSKeyedUnarchiver unarchiveObjectWithData:archivedCamera] : nil;
        if (camera)
        {
            self.mapView.camera = camera;
        }
        NSInteger uncheckedTrackingMode = [defaults integerForKey:@"MBXUserTrackingMode"];
        if (uncheckedTrackingMode >= 0 &&
            (NSUInteger)uncheckedTrackingMode >= MGLUserTrackingModeNone &&
            (NSUInteger)uncheckedTrackingMode <= MGLUserTrackingModeFollowWithCourse)
        {
            self.mapView.userTrackingMode = (MGLUserTrackingMode)uncheckedTrackingMode;
        }
        self.mapView.showsUserLocation = [defaults boolForKey:@"MBXShowsUserLocation"];
        self.mapView.debugActive = [defaults boolForKey:@"MBXDebug"];
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

- (void)handleLongPress:(UILongPressGestureRecognizer *)longPress
{
    if (longPress.state == UIGestureRecognizerStateBegan)
    {
        MGLPointAnnotation *point = [MGLPointAnnotation new];
        point.coordinate = [self.mapView convertPoint:[longPress locationInView:longPress.view]
                                 toCoordinateFromView:self.mapView];
        point.title = @"Dropped Marker";
        point.subtitle = [NSString stringWithFormat:@"lat: %.3f, lon: %.3f", point.coordinate.latitude, point.coordinate.longitude];
        [self.mapView addAnnotation:point];
        [self.mapView selectAnnotation:point animated:YES];
    }
}

- (void)cycleStyles
{
    UIButton *titleButton = (UIButton *)self.navigationItem.titleView;

    self.styleIndex = (self.styleIndex + 1) % (sizeof(MBXAvailableStyles) / sizeof(MBXAvailableStyles[0]));

    self.mapView.styleURL = [NSURL URLWithString:
        [NSString stringWithFormat:@"mapbox://styles/mapbox/%@-v%lu",
            MBXAvailableStyles[self.styleIndex].name,
            (unsigned long)kStyleVersion]];

    [titleButton setTitle:MBXAvailableStyles[self.styleIndex].displayName forState:UIControlStateNormal];
}

- (void)locateUser
{
    MGLUserTrackingMode nextMode;
    switch (self.mapView.userTrackingMode) {
        case MGLUserTrackingModeNone:
            nextMode = MGLUserTrackingModeFollow;
            break;
        case MGLUserTrackingModeFollow:
            nextMode = MGLUserTrackingModeFollowWithHeading;
            break;
        case MGLUserTrackingModeFollowWithHeading:
            nextMode = MGLUserTrackingModeFollowWithCourse;
            break;
        case MGLUserTrackingModeFollowWithCourse:
            nextMode = MGLUserTrackingModeNone;
            break;
    }
    self.mapView.userTrackingMode = nextMode;
}

#pragma mark - Destruction

- (void)dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver:self];

    [self saveState:nil];
}

#pragma mark - MGLMapViewDelegate

- (MGLAnnotationImage *)mapView:(MGLMapView * __nonnull)mapView imageForAnnotation:(id <MGLAnnotation> __nonnull)annotation
{
    if ([annotation.title isEqualToString:@"Dropped Marker"]) return nil; // use default marker

    NSString *title = [(MGLPointAnnotation *)annotation title];
    NSString *lastTwoCharacters = [title substringFromIndex:title.length - 2];

    UIColor *color;

    // make every tenth annotation blue
    if ([lastTwoCharacters hasSuffix:@"0"]) {
        color = [UIColor blueColor];
    } else {
        color = [UIColor redColor];
    }

    MGLAnnotationImage *image = [mapView dequeueReusableAnnotationImageWithIdentifier:lastTwoCharacters];

    if ( ! image)
    {
        CGRect rect = CGRectMake(0, 0, 20, 15);

        UIGraphicsBeginImageContextWithOptions(rect.size, NO, [[UIScreen mainScreen] scale]);

        CGContextRef ctx = UIGraphicsGetCurrentContext();

        CGContextSetFillColorWithColor(ctx, [[color colorWithAlphaComponent:0.75] CGColor]);
        CGContextFillRect(ctx, rect);

        CGContextSetStrokeColorWithColor(ctx, [[UIColor blackColor] CGColor]);
        CGContextStrokeRectWithWidth(ctx, rect, 2);

        NSAttributedString *drawString = [[NSAttributedString alloc] initWithString:lastTwoCharacters attributes:@{
            NSFontAttributeName: [UIFont fontWithName:@"Arial-BoldMT" size:12],
            NSForegroundColorAttributeName: [UIColor whiteColor] }];
        CGSize stringSize = drawString.size;
        CGRect stringRect = CGRectMake((rect.size.width - stringSize.width) / 2,
                                       (rect.size.height - stringSize.height) / 2,
                                       stringSize.width,
                                       stringSize.height);
        [drawString drawInRect:stringRect];

        image = [MGLAnnotationImage annotationImageWithImage:UIGraphicsGetImageFromCurrentImageContext() reuseIdentifier:lastTwoCharacters];

        // don't allow touches on blue annotations
        if ([color isEqual:[UIColor blueColor]]) image.enabled = NO;

        UIGraphicsEndImageContext();
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
    NSString *newButtonTitle;

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
        case MGLUserTrackingModeFollowWithCourse:
            newButtonImage = nil;
            newButtonTitle = @"Course";
            break;
    }

    self.navigationItem.rightBarButtonItem.title = newButtonTitle;
    [UIView animateWithDuration:0.25 animations:^{
        self.navigationItem.rightBarButtonItem.image = newButtonImage;
    }];
}

@end
