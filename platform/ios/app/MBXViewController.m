#import "MBXViewController.h"

#import "MBXAppDelegate.h"
#import "MBXCustomCalloutView.h"
#import "MBXOfflinePacksTableViewController.h"
#import "MBXAnnotationView.h"

#import <Mapbox/Mapbox.h>

#import <objc/runtime.h>

static const CLLocationCoordinate2D WorldTourDestinations[] = {
    { .latitude = 38.9131982, .longitude = -77.0325453144239 },
    { .latitude = 37.7757368, .longitude = -122.4135302 },
    { .latitude = 12.9810816, .longitude = 77.6368034 },
    { .latitude = -13.15589555, .longitude = -74.2178961777998 },
};

static NSString * const MBXViewControllerAnnotationViewReuseIdentifer = @"MBXViewControllerAnnotationViewReuseIdentifer";

@interface MBXDroppedPinAnnotation : MGLPointAnnotation
@end

@implementation MBXDroppedPinAnnotation
@end

@interface MBXCustomCalloutAnnotation : MGLPointAnnotation
@end

@implementation MBXCustomCalloutAnnotation
@end

@interface MBXViewController () <UIActionSheetDelegate, MGLMapViewDelegate>

@property (nonatomic) IBOutlet MGLMapView *mapView;
@property (nonatomic) NSInteger styleIndex;
@property (nonatomic) BOOL debugLoggingEnabled;

@end

@implementation MBXViewController
{
    BOOL _isTouringWorld;
}

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

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(saveState:) name:UIApplicationDidEnterBackgroundNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(restoreState:) name:UIApplicationWillEnterForegroundNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(saveState:) name:UIApplicationWillTerminateNotification object:nil];

    [self restoreState:nil];

    self.debugLoggingEnabled = [[NSUserDefaults standardUserDefaults] boolForKey:@"MGLMapboxMetricsDebugLoggingEnabled"];
}

- (void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
    
    if ([MGLAccountManager accessToken].length)
    {
        self.styleIndex = -1;
        [self cycleStyles:self];
    }
    else
    {
        UIAlertController *alertController = [UIAlertController alertControllerWithTitle:@"Access Token" message:@"Enter your Mapbox access token to load Mapbox-hosted tiles and styles:" preferredStyle:UIAlertControllerStyleAlert];
        [alertController addTextFieldWithConfigurationHandler:^(UITextField * _Nonnull textField)
         {
             textField.keyboardType = UIKeyboardTypeURL;
             textField.autocorrectionType = UITextAutocorrectionTypeNo;
             textField.autocapitalizationType = UITextAutocapitalizationTypeNone;
         }];
        
        [alertController addAction:[UIAlertAction actionWithTitle:@"Cancel" style:UIAlertActionStyleCancel handler:nil]];
        UIAlertAction *OKAction = [UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action)
        {
            UITextField *textField = alertController.textFields.firstObject;
            NSString *accessToken = textField.text;
            [[NSUserDefaults standardUserDefaults] setObject:accessToken forKey:MBXMapboxAccessTokenDefaultsKey];
            [MGLAccountManager setAccessToken:accessToken];
            
            self.styleIndex = -1;
            [self cycleStyles:self];
            [self.mapView reloadStyle:self];
        }];
        [alertController addAction:OKAction];
        
        if ([alertController respondsToSelector:@selector(setPreferredAction:)])
        {
            alertController.preferredAction = OKAction;
        }
        [self presentViewController:alertController animated:YES completion:nil];
    }
}

- (void)saveState:(__unused NSNotification *)notification
{
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSData *archivedCamera = [NSKeyedArchiver archivedDataWithRootObject:self.mapView.camera];
    [defaults setObject:archivedCamera forKey:@"MBXCamera"];
    [defaults setInteger:self.mapView.userTrackingMode forKey:@"MBXUserTrackingMode"];
    [defaults setBool:self.mapView.showsUserLocation forKey:@"MBXShowsUserLocation"];
    [defaults setInteger:self.mapView.debugMask forKey:@"MBXDebugMask"];
    [defaults synchronize];
}

- (void)restoreState:(__unused NSNotification *)notification
{
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
    NSInteger uncheckedDebugMask = [defaults integerForKey:@"MBXDebugMask"];
    if (uncheckedDebugMask >= 0)
    {
        self.mapView.debugMask = (MGLMapDebugMaskOptions)uncheckedDebugMask;
    }
}

- (UIInterfaceOrientationMask)supportedInterfaceOrientations
{
    return UIInterfaceOrientationMaskAll;
}

- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(__unused id)sender {
    if ([segue.identifier isEqualToString:@"ShowOfflinePacks"]) {
        MBXOfflinePacksTableViewController *controller = [segue destinationViewController];
        controller.mapView = self.mapView;
    }
}

#pragma mark - Actions

- (IBAction)showSettings:(__unused id)sender
{
    MGLMapDebugMaskOptions debugMask = self.mapView.debugMask;
    UIActionSheet *sheet = [[UIActionSheet alloc] initWithTitle:@"Map Settings"
                                                       delegate:self
                                              cancelButtonTitle:@"Cancel"
                                         destructiveButtonTitle:nil
                                              otherButtonTitles:
        @"Reset Position",
        ((debugMask & MGLMapDebugTileBoundariesMask)
         ? @"Hide Tile Boundaries"
         : @"Show Tile Boundaries"),
        ((debugMask & MGLMapDebugTileInfoMask)
         ? @"Hide Tile Info"
         : @"Show Tile Info"),
        ((debugMask & MGLMapDebugTimestampsMask)
         ? @"Hide Tile Timestamps"
         : @"Show Tile Timestamps"),
        ((debugMask & MGLMapDebugCollisionBoxesMask)
         ? @"Hide Collision Boxes"
         : @"Show Collision Boxes"),
        ((debugMask & MGLMapDebugWireframesMask)
         ? @"Hide Wireframes"
         : @"Show Wireframes"),
        @"Add 100 Points",
        @"Add 1,000 Points",
        @"Add 10,000 Points",
        @"Add Test Shapes",
        @"Start World Tour",
        @"Add Custom Callout Point",
        @"Remove Annotations",
        nil];

    if (self.debugLoggingEnabled)
    {
        [sheet addButtonWithTitle:@"Print Telemetry Logfile"];
        [sheet addButtonWithTitle:@"Delete Telemetry Logfile"];
    }

    [sheet showFromBarButtonItem:self.navigationItem.leftBarButtonItem animated:YES];
}

- (void)actionSheet:(UIActionSheet *)actionSheet didDismissWithButtonIndex:(NSInteger)buttonIndex
{
    if (buttonIndex == actionSheet.firstOtherButtonIndex)
    {
        [self.mapView resetPosition];
    }
    else if (buttonIndex == actionSheet.firstOtherButtonIndex + 1)
    {
        self.mapView.debugMask ^= MGLMapDebugTileBoundariesMask;
    }
    else if (buttonIndex == actionSheet.firstOtherButtonIndex + 2)
    {
        self.mapView.debugMask ^= MGLMapDebugTileInfoMask;
    }
    else if (buttonIndex == actionSheet.firstOtherButtonIndex + 3)
    {
        self.mapView.debugMask ^= MGLMapDebugTimestampsMask;
    }
    else if (buttonIndex == actionSheet.firstOtherButtonIndex + 4)
    {
        self.mapView.debugMask ^= MGLMapDebugCollisionBoxesMask;
    }
    else if (buttonIndex == actionSheet.firstOtherButtonIndex + 5)
    {
        self.mapView.debugMask ^= MGLMapDebugWireframesMask;
    }
    else if (buttonIndex == actionSheet.firstOtherButtonIndex + 6)
    {
        [self parseFeaturesAddingCount:100];
    }
    else if (buttonIndex == actionSheet.firstOtherButtonIndex + 7)
    {
        [self parseFeaturesAddingCount:1000];
    }
    else if (buttonIndex == actionSheet.firstOtherButtonIndex + 8)
    {
        [self parseFeaturesAddingCount:10000];
    }
    else if (buttonIndex == actionSheet.firstOtherButtonIndex + 9)
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
        
        CLLocationCoordinate2D innerCoordinates[] = {
            CLLocationCoordinate2DMake(-5, -5),
            CLLocationCoordinate2DMake(-5, 5),
            CLLocationCoordinate2DMake(5, 5),
            CLLocationCoordinate2DMake(5, -5),
        };
        MGLPolygon *innerPolygon = [MGLPolygon polygonWithCoordinates:innerCoordinates count:sizeof(innerCoordinates) / sizeof(innerCoordinates[0])];
        CLLocationCoordinate2D outerCoordinates[] = {
            CLLocationCoordinate2DMake(-10, -20),
            CLLocationCoordinate2DMake(-10, 10),
            CLLocationCoordinate2DMake(10, 10),
            CLLocationCoordinate2DMake(10, -10),
        };
        MGLPolygon *outerPolygon = [MGLPolygon polygonWithCoordinates:outerCoordinates count:sizeof(outerCoordinates) / sizeof(outerCoordinates[0]) interiorPolygons:@[innerPolygon]];
        [self.mapView addAnnotation:outerPolygon];
    }
    else if (buttonIndex == actionSheet.firstOtherButtonIndex + 10)
    {
        [self startWorldTour:actionSheet];
    }
    else if (buttonIndex == actionSheet.firstOtherButtonIndex + 11)
    {
        [self presentAnnotationWithCustomCallout];
    }
    else if (buttonIndex == actionSheet.firstOtherButtonIndex + 12)
    {
        [self.mapView removeAnnotations:self.mapView.annotations];
    }
    else if (buttonIndex == actionSheet.numberOfButtons - 2 && self.debugLoggingEnabled)
    {
        NSString *fileContents = [NSString stringWithContentsOfFile:[self telemetryDebugLogfilePath] encoding:NSUTF8StringEncoding error:nil];
        NSLog(@"%@", fileContents);
    }
    else if (buttonIndex == actionSheet.numberOfButtons - 1 && self.debugLoggingEnabled)
    {
        NSString *filePath = [self telemetryDebugLogfilePath];
        if ([[NSFileManager defaultManager] isDeletableFileAtPath:filePath]) {
            NSError *error;
            BOOL success = [[NSFileManager defaultManager] removeItemAtPath:filePath error:&error];
            if (success) {
                NSLog(@"Deleted telemetry log.");
            } else {
                NSLog(@"Error deleting telemetry log: %@", error.localizedDescription);
            }
        }
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
                [self.mapView showAnnotations:annotations animated:YES];
            });
        }
    });
}

- (void)presentAnnotationWithCustomCallout
{
    [self.mapView removeAnnotations:self.mapView.annotations];
    
    MBXCustomCalloutAnnotation *annotation = [[MBXCustomCalloutAnnotation alloc] init];
    annotation.coordinate = CLLocationCoordinate2DMake(48.8533940, 2.3775439);
    annotation.title = @"Custom Callout";
    
    [self.mapView addAnnotation:annotation];
    [self.mapView showAnnotations:@[annotation] animated:YES];
}

- (IBAction)handleLongPress:(UILongPressGestureRecognizer *)longPress
{
    if (longPress.state == UIGestureRecognizerStateBegan)
    {
        /*
        CGPoint point = [longPress locationInView:longPress.view];
        NSArray *features = [self.mapView visibleFeaturesAtPoint:point];
        NSString *title;
        for (id <MGLFeature> feature in features) {
            if (!title) {
                title = [feature attributeForKey:@"name_en"] ?: [feature attributeForKey:@"name"];
            }
        }
        
        MBXDroppedPinAnnotation *pin = [[MBXDroppedPinAnnotation alloc] init];
        pin.coordinate = [self.mapView convertPoint:point
                                 toCoordinateFromView:self.mapView];
        pin.title = title ?: @"Dropped Pin";
        pin.subtitle = [[[MGLCoordinateFormatter alloc] init] stringFromCoordinate:pin.coordinate];
        // Calling `addAnnotation:` on mapView is not required since `selectAnnotation:animated` has the side effect of adding the annotation if required
        [self.mapView selectAnnotation:pin animated:YES];
        */
    }
}

- (IBAction)cycleStyles:(__unused id)sender
{
    static NSArray *styleNames;
    static NSArray *styleURLs;
    
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        styleNames = @[
            @"Streets",
            @"Outdoors",
            @"Light",
            @"Dark",
            @"Satellite",
            @"Satellite Streets",
        ];
        styleURLs = @[
            [MGLStyle streetsStyleURLWithVersion:MGLStyleDefaultVersion],
            [MGLStyle outdoorsStyleURLWithVersion:MGLStyleDefaultVersion],
            [MGLStyle lightStyleURLWithVersion:MGLStyleDefaultVersion],
            [MGLStyle darkStyleURLWithVersion:MGLStyleDefaultVersion],
            [MGLStyle satelliteStyleURLWithVersion:MGLStyleDefaultVersion],
            [MGLStyle satelliteStreetsStyleURLWithVersion:MGLStyleDefaultVersion],
        ];
        NSAssert(styleNames.count == styleURLs.count, @"Style names and URLs don’t match.");
        
        // Make sure defaultStyleURLs is up-to-date.
        unsigned numMethods = 0;
        Method *methods = class_copyMethodList(object_getClass([MGLStyle class]), &numMethods);
        unsigned numStyleURLMethods = 0;
        for (NSUInteger i = 0; i < numMethods; i++) {
            Method method = methods[i];
            if (method_getNumberOfArguments(method) == 3 /* _cmd, self, version */) {
                SEL selector = method_getName(method);
                NSString *name = @(sel_getName(selector));
                if ([name hasSuffix:@"StyleURLWithVersion:"]) {
                    numStyleURLMethods += 1;
                }
            }
        }
        NSAssert(numStyleURLMethods == styleNames.count,
                 @"MGLStyle provides %u default styles but iosapp only knows about %lu of them.",
                 numStyleURLMethods, (unsigned long)styleNames.count);
    });
    
    self.styleIndex = (self.styleIndex + 1) % styleNames.count;

    self.mapView.styleURL = styleURLs[self.styleIndex];
    
    UIButton *titleButton = (UIButton *)self.navigationItem.titleView;
    [titleButton setTitle:styleNames[self.styleIndex] forState:UIControlStateNormal];
}

- (IBAction)locateUser:(id)sender
{
    MGLUserTrackingMode nextMode;
    NSString *nextAccessibilityValue;
    switch (self.mapView.userTrackingMode) {
        case MGLUserTrackingModeNone:
            nextMode = MGLUserTrackingModeFollow;
            nextAccessibilityValue = @"Follow location";
            break;
        case MGLUserTrackingModeFollow:
            nextMode = MGLUserTrackingModeFollowWithHeading;
            nextAccessibilityValue = @"Follow location and heading";
            break;
        case MGLUserTrackingModeFollowWithHeading:
            nextMode = MGLUserTrackingModeFollowWithCourse;
            nextAccessibilityValue = @"Follow course";
            break;
        case MGLUserTrackingModeFollowWithCourse:
            nextMode = MGLUserTrackingModeNone;
            nextAccessibilityValue = @"Off";
            break;
    }
    self.mapView.userTrackingMode = nextMode;
    [sender setAccessibilityValue:nextAccessibilityValue];
}

- (IBAction)startWorldTour:(__unused id)sender
{
    _isTouringWorld = YES;
    
    [self.mapView removeAnnotations:self.mapView.annotations];
    NSUInteger numberOfAnnotations = sizeof(WorldTourDestinations) / sizeof(WorldTourDestinations[0]);
    NSMutableArray *annotations = [NSMutableArray arrayWithCapacity:numberOfAnnotations];
    for (NSUInteger i = 0; i < numberOfAnnotations; i++)
    {
        MBXDroppedPinAnnotation *annotation = [[MBXDroppedPinAnnotation alloc] init];
        annotation.coordinate = WorldTourDestinations[i];
        [annotations addObject:annotation];
    }
    [self.mapView addAnnotations:annotations];
    [self continueWorldTourWithRemainingAnnotations:annotations];
}

- (void)continueWorldTourWithRemainingAnnotations:(NS_MUTABLE_ARRAY_OF(MGLPointAnnotation *) *)annotations
{
    MGLPointAnnotation *nextAnnotation = annotations.firstObject;
    if (!nextAnnotation || !_isTouringWorld)
    {
        _isTouringWorld = NO;
        return;
    }
    
    [annotations removeObjectAtIndex:0];
    MGLMapCamera *camera = [MGLMapCamera cameraLookingAtCenterCoordinate:nextAnnotation.coordinate
                                                            fromDistance:10
                                                                   pitch:arc4random_uniform(60)
                                                                 heading:arc4random_uniform(360)];
    __weak MBXViewController *weakSelf = self;
    [self.mapView flyToCamera:camera completionHandler:^{
        MBXViewController *strongSelf = weakSelf;
        [strongSelf performSelector:@selector(continueWorldTourWithRemainingAnnotations:)
                         withObject:annotations
                         afterDelay:2];
    }];
}

- (NSString *)telemetryDebugLogfilePath
{
    NSDateFormatter *dateFormatter = [[NSDateFormatter alloc] init];
    [dateFormatter setDateFormat:@"yyyy'-'MM'-'dd"];
    [dateFormatter setTimeZone:[NSTimeZone systemTimeZone]];
    NSString *filePath = [[NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) firstObject] stringByAppendingPathComponent:[NSString stringWithFormat:@"telemetry_log-%@.json", [dateFormatter stringFromDate:[NSDate date]]]];

    return filePath;
}

- (IBAction)unwindToMapViewController:(__unused UIStoryboardSegue *)sender {
}

#pragma mark - Destruction

- (void)dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver:self];

    [self saveState:nil];
}

#pragma mark - MGLMapViewDelegate

- (MGLAnnotationView *)mapView:(MGLMapView *)mapView viewForAnnotation:(id<MGLAnnotation>)annotation
{
    // Use GL backed pins for dropped pin annotations
    if ([annotation isKindOfClass:[MBXDroppedPinAnnotation class]])
    {
        return nil;
    }
    
    MBXAnnotationView *annotationView = (MBXAnnotationView *)[mapView dequeueReusableAnnotationViewWithIdentifier:MBXViewControllerAnnotationViewReuseIdentifer];
    if (!annotationView)
    {
        annotationView = [[MBXAnnotationView alloc] initWithReuseIdentifier:MBXViewControllerAnnotationViewReuseIdentifer];
        annotationView.frame = CGRectMake(0, 0, 10, 10);
        annotationView.centerColor = [UIColor whiteColor];
       
        // uncomment to flatten the annotation view against the map when the map is tilted
        // this currently causes severe performance issues when more than 2k annotations are visible
        // annotationView.flat = YES;
        
        // uncomment to make the annotation view draggable
        // also note that having two long press gesture recognizers on overlapping views (`self.view` & `annotationView`) will cause weird behaviour
        // comment out the pin dropping functionality in the handleLongPress: method in this class to make draggable annotation views play nice
        annotationView.draggable = YES;

       
        // uncomment to force annotation view to maintain a constant size when the map is tilted
        // by default, annotation views will shrink and grow as the move towards and away from the
        // horizon. Relatedly, annotations backed by GL sprites ONLY scale with viewing distance currently.
        // annotationView.scalesWithViewingDistance = NO;
        
    } else {
        // orange indicates that the annotation view was reused
        annotationView.centerColor = [UIColor orangeColor];
    }
    return annotationView;
}

- (void)mapView:(MGLMapView *)mapView didDragAnnotationView:(nonnull MGLAnnotationView *)annotationView toCoordinate:(CLLocationCoordinate2D)coordinate
{
    MGLPointAnnotation *annotation = (MGLPointAnnotation *)annotationView.annotation;
    annotation.coordinate = coordinate;
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

- (UIView<MGLCalloutView> *)mapView:(__unused MGLMapView *)mapView calloutViewForAnnotation:(id<MGLAnnotation>)annotation
{
    if ([annotation respondsToSelector:@selector(title)]
        && [annotation isKindOfClass:[MBXCustomCalloutAnnotation class]])
    {
        MBXCustomCalloutView *calloutView = [[MBXCustomCalloutView alloc] init];
        calloutView.representedObject = annotation;
        return calloutView;
    }
    return nil;
}

- (UIView *)mapView:(__unused MGLMapView *)mapView leftCalloutAccessoryViewForAnnotation:(__unused id<MGLAnnotation>)annotation
{
    UIButton *button = [UIButton buttonWithType:UIButtonTypeSystem];
    button.frame = CGRectZero;
    [button setTitle:@"Left" forState:UIControlStateNormal];
    [button sizeToFit];
    return button;
}

- (UIView *)mapView:(__unused MGLMapView *)mapView rightCalloutAccessoryViewForAnnotation:(__unused id<MGLAnnotation>)annotation
{
    UIButton *button = [UIButton buttonWithType:UIButtonTypeSystem];
    button.frame = CGRectZero;
    [button setTitle:@"Right" forState:UIControlStateNormal];
    [button sizeToFit];
    return button;
}

- (void)mapView:(MGLMapView *)mapView tapOnCalloutForAnnotation:(id <MGLAnnotation>)annotation
{
    if ( ! [annotation isKindOfClass:[MGLPointAnnotation class]])
    {
        return;
    }
    
    MGLPointAnnotation *point = annotation;
    point.coordinate = [self.mapView convertPoint:self.mapView.center toCoordinateFromView:self.mapView];
}

@end
