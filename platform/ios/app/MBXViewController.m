#import "MBXViewController.h"

#import "MBXAppDelegate.h"
#import "MBXCustomCalloutView.h"
#import "MBXOfflinePacksTableViewController.h"
#import "MBXAnnotationView.h"
#import "MBXUserLocationAnnotationView.h"

#import "MGLFillStyleLayer.h"

#import <Mapbox/Mapbox.h>

#import <objc/runtime.h>

static const CLLocationCoordinate2D WorldTourDestinations[] = {
    { .latitude = 38.9131982, .longitude = -77.0325453144239 },
    { .latitude = 37.7757368, .longitude = -122.4135302 },
    { .latitude = 12.9810816, .longitude = 77.6368034 },
    { .latitude = -13.15589555, .longitude = -74.2178961777998 },
};

static NSString * const MBXViewControllerAnnotationViewReuseIdentifer = @"MBXViewControllerAnnotationViewReuseIdentifer";

typedef NS_ENUM(NSInteger, MBXSettingsSections) {
    MBXSettingsCoreRendering = 0,
    MBXSettingsAnnotations,
    MBXSettingsRuntimeStyling,
    MBXSettingsMiscellaneous,
};

typedef NS_ENUM(NSInteger, MBXSettingsCoreRenderingRows) {
    MBXSettingsCoreRenderingResetPosition = 0,
    MBXSettingsCoreRenderingTileBoundaries,
    MBXSettingsCoreRenderingTileInfo,
    MBXSettingsCoreRenderingTimestamps,
    MBXSettingsCoreRenderingCollisionBoxes,
    MBXSettingsCoreRenderingOverdrawVisualization,
};

typedef NS_ENUM(NSInteger, MBXSettingsAnnotationsRows) {
    MBXSettingsAnnotations100Views = 0,
    MBXSettingsAnnotations1000Views,
    MBXSettingsAnnotations10000Views,
    MBXSettingsAnnotations100Sprites,
    MBXSettingsAnnotations1000Sprites,
    MBXSettingsAnnotations10000Sprites,
    MBXSettingsAnnotationsTestShapes,
    MBXSettingsAnnotationsCustomCallout,
    MBXSettingsAnnotationsQueryAnnotations,
    MBXSettingsAnnotationsRemoveAnnotations,
};

typedef NS_ENUM(NSInteger, MBXSettingsRuntimeStylingRows) {
    MBXSettingsRuntimeStylingWater = 0,
    MBXSettingsRuntimeStylingRoads,
    MBXSettingsRuntimeStylingRaster,
    MBXSettingsRuntimeStylingGeoJSON,
    MBXSettingsRuntimeStylingSymbols,
    MBXSettingsRuntimeStylingBuildings,
    MBXSettingsRuntimeStylingFerry,
    MBXSettingsRuntimeStylingParks,
    MBXSettingsRuntimeStylingFilteredFill,
    MBXSettingsRuntimeStylingFilteredLines,
    MBXSettingsRuntimeStylingNumericFilteredFill,
    MBXSettingsRuntimeStylingStyleQuery,
    MBXSettingsRuntimeStylingFeatureSource,
    MBXSettingsRuntimeStylingPointCollection,
    MBXSettingsRuntimeStylingUpdateGeoJSONSourceData,
    MBXSettingsRuntimeStylingUpdateGeoJSONSourceURL,
    MBXSettingsRuntimeStylingUpdateGeoJSONSourceFeatures,
    MBXSettingsRuntimeStylingVectorSource,
    MBXSettingsRuntimeStylingRasterSource,
    MBXSettingsRuntimeStylingCountryLabels,
    MBXSettingsRuntimeStylingRouteLine,
};

typedef NS_ENUM(NSInteger, MBXSettingsMiscellaneousRows) {
    MBXSettingsMiscellaneousShowReuseQueueStats = 0,
    MBXSettingsMiscellaneousWorldTour,
    MBXSettingsMiscellaneousCustomUserDot,
    MBXSettingsMiscellaneousPrintLogFile,
    MBXSettingsMiscellaneousDeleteLogFile,
};

@interface MBXDroppedPinAnnotation : MGLPointAnnotation
@end

@implementation MBXDroppedPinAnnotation
@end

@interface MBXCustomCalloutAnnotation : MGLPointAnnotation
@end

@implementation MBXCustomCalloutAnnotation
@end

@interface MBXSpriteBackedAnnotation : MGLPointAnnotation
@end

@implementation MBXSpriteBackedAnnotation
@end

@interface MBXViewController () <UITableViewDelegate,
                                 UITableViewDataSource,
                                 MGLMapViewDelegate>


@property (nonatomic) IBOutlet MGLMapView *mapView;
@property (weak, nonatomic) IBOutlet UILabel *hudLabel;
@property (nonatomic) NSInteger styleIndex;
@property (nonatomic) BOOL debugLoggingEnabled;
@property (nonatomic) BOOL customUserLocationAnnnotationEnabled;
@property (nonatomic) BOOL usingLocaleBasedCountryLabels;
@property (nonatomic) BOOL reuseQueueStatsEnabled;

@end

@interface MGLMapView (MBXViewController)

@property (nonatomic) BOOL usingLocaleBasedCountryLabels;
@property (nonatomic) NSDictionary *annotationViewReuseQueueByIdentifier;

@end

@implementation MBXViewController
{
    BOOL _isTouringWorld;
}

#pragma mark - Setup & Teardown

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
    self.hudLabel.hidden = YES;

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

- (void)dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver:self];

    [self saveState:nil];
}

#pragma mark - Debugging Interface

- (IBAction)showSettings:(__unused id)sender
{
    UITableViewController *settingsViewController = [[UITableViewController alloc] initWithStyle:UITableViewStyleGrouped];
    settingsViewController.tableView.delegate = self;
    settingsViewController.tableView.dataSource = self;
    settingsViewController.title = @"Debugging";
    settingsViewController.navigationItem.rightBarButtonItem = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemDone target:self action:@selector(dismissSettings:)];
    UINavigationController *wrapper = [[UINavigationController alloc] initWithRootViewController:settingsViewController];
    wrapper.navigationBar.tintColor = self.navigationController.navigationBar.tintColor;
    [self.navigationController presentViewController:wrapper animated:YES completion:nil];
}

- (void)dismissSettings:(__unused id)sender
{
    [self dismissViewControllerAnimated:YES completion:nil];
}

- (NSArray <NSString *> *)settingsSectionTitles
{
    return @[
        @"Core Rendering",
        @"Annotations",
        @"Runtime Styling",
        @"Miscellaneous"
    ];
}

- (NSArray <NSString *> *)settingsTitlesForSection:(NSInteger)section
{
    NSMutableArray *settingsTitles = [NSMutableArray array];

    MGLMapDebugMaskOptions debugMask = self.mapView.debugMask;

    switch (section)
    {
        case MBXSettingsCoreRendering:
            [settingsTitles addObjectsFromArray:@[
                @"Reset Position",
                [NSString stringWithFormat:@"%@ Tile Boundaries",
                    (debugMask & MGLMapDebugTileBoundariesMask ? @"Hide" :@"Show")],
                [NSString stringWithFormat:@"%@ Tile Info",
                    (debugMask & MGLMapDebugTileInfoMask ? @"Hide" :@"Show")],
                [NSString stringWithFormat:@"%@ Tile Timestamps",
                    (debugMask & MGLMapDebugTimestampsMask ? @"Hide" :@"Show")],
                [NSString stringWithFormat:@"%@ Collision Boxes",
                    (debugMask & MGLMapDebugCollisionBoxesMask ? @"Hide" :@"Show")],
                [NSString stringWithFormat:@"%@ Overdraw Visualization",
                    (debugMask & MGLMapDebugOverdrawVisualizationMask ? @"Hide" :@"Show")],
            ]];
            break;
        case MBXSettingsAnnotations:
            [settingsTitles addObjectsFromArray:@[
                @"Add 100 Views",
                @"Add 1,000 Views",
                @"Add 10,000 Views",
                @"Add 100 Sprites",
                @"Add 1,000 Sprites",
                @"Add 10,000 Sprites",
                @"Add Test Shapes",
                @"Add Point With Custom Callout",
                @"Query Annotations",
                @"Remove Annotations",
            ]];
            break;
        case MBXSettingsRuntimeStyling:
            [settingsTitles addObjectsFromArray:@[
                @"Style Water With Function",
                @"Style Roads With Function",
                @"Add Raster & Apply Function",
                @"Add GeoJSON & Apply Fill",
                @"Style Symbol Color",
                @"Style Building Fill Color",
                @"Style Ferry Line Color",
                @"Remove Parks",
                @"Style Fill With Filter",
                @"Style Lines With Filter",
                @"Style Fill With Numeric Filter",
                @"Style Query For GeoJSON",
                @"Style Feature",
                @"Style Dynamic Point Collection",
                @"Update GeoJSON Source: Data",
                @"Update GeoJSON Source: URL",
                @"Update GeoJSON Source: Features",
                @"Style Vector Source",
                @"Style Raster Source",
                [NSString stringWithFormat:@"Label Countries in %@", (_usingLocaleBasedCountryLabels ? @"Local Language" : [[NSLocale currentLocale] displayNameForKey:NSLocaleIdentifier value:[self bestLanguageForUser]])],
                @"Add Route Line",
            ]];
            break;
        case MBXSettingsMiscellaneous:
            [settingsTitles addObject:@"Show Reuse Queue Stats"];

            [settingsTitles addObjectsFromArray:@[
                @"Start World Tour",
                [NSString stringWithFormat:@"%@ Custom User Dot", (_customUserLocationAnnnotationEnabled ? @"Disable" : @"Enable")],
            ]];

            if (self.debugLoggingEnabled)
            {
                [settingsTitles addObjectsFromArray:@[
                    @"Print Telemetry Logfile",
                    @"Delete Telemetry Logfile",
                ]];
            };

            break;
        default:
            NSAssert(NO, @"All settings sections should be implemented");
            break;
    }

    return settingsTitles;
}

- (void)performActionForSettingAtIndexPath:(NSIndexPath *)indexPath
{
    switch (indexPath.section)
    {
        case MBXSettingsCoreRendering:
            switch (indexPath.row)
            {
                case MBXSettingsCoreRenderingResetPosition:
                    [self.mapView resetPosition];
                    break;
                case MBXSettingsCoreRenderingTileBoundaries:
                    self.mapView.debugMask ^= MGLMapDebugTileBoundariesMask;
                    break;
                case MBXSettingsCoreRenderingTileInfo:
                    self.mapView.debugMask ^= MGLMapDebugTileInfoMask;
                    break;
                case MBXSettingsCoreRenderingTimestamps:
                    self.mapView.debugMask ^= MGLMapDebugTimestampsMask;
                    break;
                case MBXSettingsCoreRenderingCollisionBoxes:
                    self.mapView.debugMask ^= MGLMapDebugCollisionBoxesMask;
                    break;
                case MBXSettingsCoreRenderingOverdrawVisualization:
                    self.mapView.debugMask ^= MGLMapDebugOverdrawVisualizationMask;
                    break;
                default:
                    NSAssert(NO, @"All core rendering setting rows should be implemented");
                    break;
            }
            break;
        case MBXSettingsAnnotations:
            switch (indexPath.row)
            {
                case MBXSettingsAnnotations100Views:
                    [self parseFeaturesAddingCount:100 usingViews:YES];
                    break;
                case MBXSettingsAnnotations1000Views:
                    [self parseFeaturesAddingCount:1000 usingViews:YES];
                    break;
                case MBXSettingsAnnotations10000Views:
                    [self parseFeaturesAddingCount:10000 usingViews:YES];
                    break;
                case MBXSettingsAnnotations100Sprites:
                    [self parseFeaturesAddingCount:100 usingViews:NO];
                    break;
                case MBXSettingsAnnotations1000Sprites:
                    [self parseFeaturesAddingCount:1000 usingViews:NO];
                    break;
                case MBXSettingsAnnotations10000Sprites:
                    [self parseFeaturesAddingCount:10000 usingViews:NO];
                    break;
                case MBXSettingsAnnotationsTestShapes:
                    [self addTestShapes];
                    break;
                case MBXSettingsAnnotationsCustomCallout:
                    [self addAnnotationWithCustomCallout];
                    break;
                case MBXSettingsAnnotationsQueryAnnotations:
                    [self testQueryPointAnnotations];
                    break;
                case MBXSettingsAnnotationsRemoveAnnotations:
                    [self.mapView removeAnnotations:self.mapView.annotations];
                    break;
                default:
                    NSAssert(NO, @"All annotations setting rows should be implemented");
                    break;
            }
            break;
        case MBXSettingsRuntimeStyling:
            switch (indexPath.row)
            {
                case MBXSettingsRuntimeStylingWater:
                    [self styleWaterLayer];
                    break;
                case MBXSettingsRuntimeStylingRoads:
                    [self styleRoadLayer];
                    break;
                case MBXSettingsRuntimeStylingRaster:
                    [self styleRasterLayer];
                    break;
                case MBXSettingsRuntimeStylingGeoJSON:
                    [self styleGeoJSONSource];
                    break;
                case MBXSettingsRuntimeStylingSymbols:
                    [self styleSymbolLayer];
                    break;
                case MBXSettingsRuntimeStylingBuildings:
                    [self styleBuildingLayer];
                    break;
                case MBXSettingsRuntimeStylingFerry:
                    [self styleFerryLayer];
                    break;
                case MBXSettingsRuntimeStylingParks:
                    [self removeParkLayer];
                    break;
                case MBXSettingsRuntimeStylingFilteredFill:
                    [self styleFilteredFill];
                    break;
                case MBXSettingsRuntimeStylingFilteredLines:
                    [self styleFilteredLines];
                    break;
                case MBXSettingsRuntimeStylingNumericFilteredFill:
                    [self styleNumericFilteredFills];
                    break;
                case MBXSettingsRuntimeStylingStyleQuery:
                    [self styleQuery];
                    break;
                case MBXSettingsRuntimeStylingFeatureSource:
                    [self styleFeature];
                    break;
                case MBXSettingsRuntimeStylingPointCollection:
                    [self styleDynamicPointCollection];
                    break;
                case MBXSettingsRuntimeStylingUpdateGeoJSONSourceURL:
                    [self updateGeoJSONSourceURL];
                    break;
                case MBXSettingsRuntimeStylingUpdateGeoJSONSourceData:
                    [self updateGeoJSONSourceData];
                    break;
                case MBXSettingsRuntimeStylingUpdateGeoJSONSourceFeatures:
                    [self updateGeoJSONSourceFeatures];
                    break;
                case MBXSettingsRuntimeStylingVectorSource:
                    [self styleVectorSource];
                    break;
                case MBXSettingsRuntimeStylingRasterSource:
                    [self styleRasterSource];
                    break;
                case MBXSettingsRuntimeStylingCountryLabels:
                    [self styleCountryLabelsLanguage];
                    break;
                case MBXSettingsRuntimeStylingRouteLine:
                    [self styleRouteLine];
                    break;
                default:
                    NSAssert(NO, @"All runtime styling setting rows should be implemented");
                    break;
            }
            break;
        case MBXSettingsMiscellaneous:
            switch (indexPath.row)
            {
                case MBXSettingsMiscellaneousWorldTour:
                    [self startWorldTour];
                    break;
                case MBXSettingsMiscellaneousCustomUserDot:
                    [self toggleCustomUserDot];
                    break;
                case MBXSettingsMiscellaneousPrintLogFile:
                    [self printTelemetryLogFile];
                    break;
                case MBXSettingsMiscellaneousDeleteLogFile:
                    [self deleteTelemetryLogFile];
                    break;
                case MBXSettingsMiscellaneousShowReuseQueueStats:
                {
                    self.reuseQueueStatsEnabled = YES;
                    self.hudLabel.hidden = NO;
                    break;
                }
                default:
                    NSAssert(NO, @"All miscellaneous setting rows should be implemented");
                    break;
            }
            break;
        default:
            NSAssert(NO, @"All settings sections should be implemented");
            break;
    }
}

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return [[self settingsSectionTitles] count];
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return [[self settingsTitlesForSection:section] count];
}

- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section;
{
    return [[self settingsSectionTitles] objectAtIndex:section];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    UITableViewCell *cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:nil];

    cell.textLabel.text = [[self settingsTitlesForSection:indexPath.section] objectAtIndex:indexPath.row];

    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    [tableView deselectRowAtIndexPath:indexPath animated:NO];

    [self dismissViewControllerAnimated:YES completion:^
    {
        [self performActionForSettingAtIndexPath:indexPath];
    }];
}

#pragma mark - Debugging Actions

- (void)parseFeaturesAddingCount:(NSUInteger)featuresCount usingViews:(BOOL)useViews
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

                MGLPointAnnotation *annotation = (useViews ? [MGLPointAnnotation new] : [MBXSpriteBackedAnnotation new]);

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

- (void)addTestShapes
{
    // Pacific Northwest triangle
    //
    CLLocationCoordinate2D triangleCoordinates[3] =
    {
        CLLocationCoordinate2DMake(44, -122),
        CLLocationCoordinate2DMake(46, -122),
        CLLocationCoordinate2DMake(46, -121)
    };

    MGLPolygon *triangle = [MGLPolygon polygonWithCoordinates:triangleCoordinates count:3];

    [self.mapView addAnnotation:triangle];

    // Orcas Island, WA hike polyline
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

    // PA/NJ/DE polygons
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

    // Null Island polygon with an interior hole
    //
    CLLocationCoordinate2D innerCoordinates[] = {
        CLLocationCoordinate2DMake(-5, -5),
        CLLocationCoordinate2DMake(-5, 5),
        CLLocationCoordinate2DMake(5, 5),
        CLLocationCoordinate2DMake(5, -5),
    };
    MGLPolygon *innerPolygon = [MGLPolygon polygonWithCoordinates:innerCoordinates count:sizeof(innerCoordinates) / sizeof(innerCoordinates[0])];
    CLLocationCoordinate2D outerCoordinates[] = {
        CLLocationCoordinate2DMake(-10, -10),
        CLLocationCoordinate2DMake(-10, 10),
        CLLocationCoordinate2DMake(10, 10),
        CLLocationCoordinate2DMake(10, -10),
    };
    MGLPolygon *outerPolygon = [MGLPolygon polygonWithCoordinates:outerCoordinates count:sizeof(outerCoordinates) / sizeof(outerCoordinates[0]) interiorPolygons:@[innerPolygon]];
    [self.mapView addAnnotation:outerPolygon];
}

- (void)addAnnotationWithCustomCallout
{
    [self.mapView removeAnnotations:self.mapView.annotations];

    MBXCustomCalloutAnnotation *annotation = [[MBXCustomCalloutAnnotation alloc] init];
    annotation.coordinate = CLLocationCoordinate2DMake(48.8533940, 2.3775439);
    annotation.title = @"Custom Callout";

    [self.mapView addAnnotation:annotation];
    [self.mapView showAnnotations:@[annotation] animated:YES];
}

- (void)styleWaterLayer
{
    MGLFillStyleLayer *waterLayer = (MGLFillStyleLayer *)[self.mapView.style layerWithIdentifier:@"water"];
    MGLStyleValue *waterColorFunction = [MGLStyleValue<UIColor *> valueWithStops:@{
        @6.0f: [MGLStyleValue<UIColor *> valueWithRawValue:[UIColor yellowColor]],
        @8.0f: [MGLStyleValue<UIColor *> valueWithRawValue:[UIColor blueColor]],
        @10.0f: [MGLStyleValue<UIColor *> valueWithRawValue:[UIColor redColor]],
        @12.0f: [MGLStyleValue<UIColor *> valueWithRawValue:[UIColor greenColor]],
        @14.0f: [MGLStyleValue<UIColor *> valueWithRawValue:[UIColor blueColor]],
    }];
    waterLayer.fillColor = waterColorFunction;

    MGLStyleValue *fillAntialias = [MGLStyleValue<NSNumber *> valueWithStops:@{
        @11: [MGLStyleValue<NSNumber *> valueWithRawValue:@YES],
        @12: [MGLStyleValue<NSNumber *> valueWithRawValue:@NO],
        @13: [MGLStyleValue<NSNumber *> valueWithRawValue:@YES],
        @14: [MGLStyleValue<NSNumber *> valueWithRawValue:@NO],
        @15: [MGLStyleValue<NSNumber *> valueWithRawValue:@YES],
    }];
    waterLayer.fillAntialias = fillAntialias;
}

- (void)styleRoadLayer
{
    MGLLineStyleLayer *roadLayer = (MGLLineStyleLayer *)[self.mapView.style layerWithIdentifier:@"road-primary"];
    roadLayer.lineColor = [MGLStyleValue<UIColor *> valueWithRawValue:[UIColor blackColor]];
    MGLStyleValue *lineWidthFunction = [MGLStyleValue<NSNumber *> valueWithStops:@{}];

    MGLStyleValue *roadLineColor = [MGLStyleValue<UIColor *> valueWithStops:@{
        @10: [MGLStyleValue<UIColor *> valueWithRawValue:[UIColor purpleColor]],
        @13: [MGLStyleValue<UIColor *> valueWithRawValue:[UIColor yellowColor]],
        @16: [MGLStyleValue<UIColor *> valueWithRawValue:[UIColor cyanColor]],
    }];
    roadLayer.lineColor = roadLineColor;
    roadLayer.lineWidth = lineWidthFunction;
    roadLayer.lineGapWidth = lineWidthFunction;

    roadLayer.visible = YES;
    roadLayer.maximumZoomLevel = 15;
    roadLayer.minimumZoomLevel = 13;
}

- (void)styleRasterLayer
{
    NSURL *rasterURL = [NSURL URLWithString:@"mapbox://mapbox.satellite"];
    MGLRasterSource *rasterSource = [[MGLRasterSource alloc] initWithIdentifier:@"my-raster-source" URL:rasterURL tileSize:512];
    [self.mapView.style addSource:rasterSource];

    MGLRasterStyleLayer *rasterLayer = [[MGLRasterStyleLayer alloc] initWithIdentifier:@"my-raster-layer" source:rasterSource];
    MGLStyleValue *opacityFunction = [MGLStyleValue<NSNumber *> valueWithStops:@{
        @20.0f: [MGLStyleValue<NSNumber *> valueWithRawValue:@1.0f],
        @5.0f: [MGLStyleValue<NSNumber *> valueWithRawValue:@0.0f],
    }];
    rasterLayer.rasterOpacity = opacityFunction;
    [self.mapView.style addLayer:rasterLayer];
}

- (void)styleGeoJSONSource
{
    NSString *filePath = [[NSBundle bundleForClass:self.class] pathForResource:@"amsterdam" ofType:@"geojson"];
    NSURL *geoJSONURL = [NSURL fileURLWithPath:filePath];
    MGLGeoJSONSource *source = [[MGLGeoJSONSource alloc] initWithIdentifier:@"ams" URL:geoJSONURL options:nil];
    [self.mapView.style addSource:source];

    MGLFillStyleLayer *fillLayer = [[MGLFillStyleLayer alloc] initWithIdentifier:@"test" source:source];
    fillLayer.fillColor = [MGLStyleValue<UIColor *> valueWithRawValue:[UIColor purpleColor]];
    [self.mapView.style addLayer:fillLayer];
    
}

- (void)styleSymbolLayer
{
    MGLSymbolStyleLayer *stateLayer = (MGLSymbolStyleLayer *)[self.mapView.style layerWithIdentifier:@"state-label-lg"];
    stateLayer.textColor = [MGLStyleValue<UIColor *> valueWithRawValue:[UIColor redColor]];
}

- (void)styleBuildingLayer
{
    MGLFillStyleLayer *buildingLayer = (MGLFillStyleLayer *)[self.mapView.style layerWithIdentifier:@"building"];
    buildingLayer.fillColor = [MGLStyleValue<UIColor *> valueWithRawValue:[UIColor blackColor]];
}

- (void)styleFerryLayer
{
    MGLLineStyleLayer *ferryLineLayer = (MGLLineStyleLayer *)[self.mapView.style layerWithIdentifier:@"ferry"];
    ferryLineLayer.lineColor = [MGLStyleValue<UIColor *> valueWithRawValue:[UIColor redColor]];
}

- (void)removeParkLayer
{
    MGLFillStyleLayer *parkLayer = (MGLFillStyleLayer *)[self.mapView.style layerWithIdentifier:@"park"];
    [self.mapView.style removeLayer:parkLayer];
}

- (void)styleFilteredFill
{
    // set style and focus on Texas
    [self.mapView setStyleURL:[NSURL fileURLWithPath:[[NSBundle mainBundle] pathForResource:@"fill_filter_style" ofType:@"json"]]];
    [self.mapView setCenterCoordinate:CLLocationCoordinate2DMake(31, -100) zoomLevel:3 animated:NO];

    // after slight delay, fill in Texas (atypical use; we want to clearly see the change for test purposes)
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(2 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^
    {
        MGLFillStyleLayer *statesLayer = (MGLFillStyleLayer *)[self.mapView.style layerWithIdentifier:@"states"];

        // filter
        statesLayer.predicate = [NSPredicate predicateWithFormat:@"name == 'Texas'"];

        // paint properties
        statesLayer.fillColor = [MGLStyleValue<UIColor *> valueWithRawValue:[UIColor redColor]];
        statesLayer.fillOpacity = [MGLStyleValue<NSNumber *> valueWithRawValue:@0.25];
    });
}

+ (MGLStyleConstantValue<NSValue *> *)testEnum:(NSUInteger)value type:(const char *)type
{
    return [MGLStyleConstantValue<NSValue *> valueWithRawValue:[NSValue value:&value withObjCType:type]];
}

+ (MGLStyleFunction<NSValue *> *)testEnumFunction:(NSUInteger)value type:(const char *)type
{
    return [MGLStyleFunction<NSValue *> valueWithStops:@{
                                                         @18: [self testEnum:value type:type],
                                                         }];
}

- (void)styleFilteredLines
{
    // set style and focus on lower 48
    [self.mapView setStyleURL:[NSURL fileURLWithPath:[[NSBundle mainBundle] pathForResource:@"line_filter_style" ofType:@"json"]]];
    [self.mapView setCenterCoordinate:CLLocationCoordinate2DMake(40, -97) zoomLevel:5 animated:NO];

    // after slight delay, change styling for all Washington-named counties  (atypical use; we want to clearly see the change for test purposes)
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(2 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^
    {
        MGLLineStyleLayer *countiesLayer = (MGLLineStyleLayer *)[self.mapView.style layerWithIdentifier:@"counties"];

        // filter
        countiesLayer.predicate = [NSPredicate predicateWithFormat:@"NAME10 == 'Washington'"];

        // paint properties
        countiesLayer.lineColor = [MGLStyleValue<UIColor *> valueWithRawValue:[UIColor redColor]];
        countiesLayer.lineOpacity = [MGLStyleValue<NSNumber *> valueWithRawValue:@0.75];
        countiesLayer.lineWidth = [MGLStyleValue<NSNumber *> valueWithRawValue:@5];
    });
}

- (void)styleNumericFilteredFills
{
    // set style and focus on lower 48
    [self.mapView setStyleURL:[NSURL fileURLWithPath:[[NSBundle mainBundle] pathForResource:@"numeric_filter_style" ofType:@"json"]]];
    [self.mapView setCenterCoordinate:CLLocationCoordinate2DMake(40, -97) zoomLevel:5 animated:NO];

    // after slight delay, change styling for regions 200-299 (atypical use; we want to clearly see the change for test purposes)
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(2 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^
    {
        MGLFillStyleLayer *regionsLayer = (MGLFillStyleLayer *)[self.mapView.style layerWithIdentifier:@"regions"];

        // filter (testing both inline and format strings)
        regionsLayer.predicate = [NSPredicate predicateWithFormat:@"HRRNUM >= %@ AND HRRNUM < 300", @(200)];

        // paint properties
        regionsLayer.fillColor = [MGLStyleValue<UIColor *> valueWithRawValue:[UIColor blueColor]];
        regionsLayer.fillOpacity = [MGLStyleValue<NSNumber *> valueWithRawValue:@0.5];
    });
}

- (void)styleQuery
{
    CGRect queryRect = CGRectInset(self.mapView.bounds, 100, 200);
    NSArray *features = [self.mapView visibleFeaturesInRect:queryRect];
    
    NSString *querySourceID = @"query-source-id";
    NSString *queryLayerID = @"query-layer-id";
    
    // RTE if you don't remove the layer first
    // RTE if you pass a nill layer to remove layer
    MGLStyleLayer *layer = [self.mapView.style layerWithIdentifier:queryLayerID];
    if (layer) {
        [self.mapView.style removeLayer:layer];
    }
    
    // RTE if you pass a nill source to remove source
    MGLSource *source = [self.mapView.style sourceWithIdentifier:querySourceID];
    if (source) {
        [self.mapView.style removeSource:source];
    }
    
    
    dispatch_async(dispatch_get_main_queue(), ^{
        MGLGeoJSONSource *source = [[MGLGeoJSONSource alloc] initWithIdentifier:querySourceID features:features options:nil];
        [self.mapView.style addSource:source];
        
        MGLFillStyleLayer *fillLayer = [[MGLFillStyleLayer alloc] initWithIdentifier:queryLayerID source:source];
        fillLayer.fillColor = [MGLStyleConstantValue<UIColor *> valueWithRawValue:[UIColor blueColor]];
        fillLayer.fillOpacity = [MGLStyleConstantValue<NSNumber *> valueWithRawValue:@0.5];
        [self.mapView.style addLayer:fillLayer];
    });
}

- (void)styleFeature
{
    self.mapView.zoomLevel = 10;
    self.mapView.centerCoordinate = CLLocationCoordinate2DMake(51.068585180672635, -114.06074523925781);
    
    CLLocationCoordinate2D leafCoords[] = {
        CLLocationCoordinate2DMake(50.9683733218221,-114.07035827636719),
        CLLocationCoordinate2DMake(51.02325750523972,-114.06967163085938),
        CLLocationCoordinate2DMake(51.009434536947786,-114.14245605468749),
        CLLocationCoordinate2DMake(51.030599281184124,-114.12597656249999),
        CLLocationCoordinate2DMake(51.060386316691016,-114.21043395996094),
        CLLocationCoordinate2DMake(51.063838646941576,-114.17816162109375),
        CLLocationCoordinate2DMake(51.08152779888779,-114.19876098632812),
        CLLocationCoordinate2DMake(51.08066507029602,-114.16854858398438),
        CLLocationCoordinate2DMake(51.09662294502995,-114.17472839355469),
        CLLocationCoordinate2DMake(51.07764539352731,-114.114990234375),
        CLLocationCoordinate2DMake(51.13670896949613,-114.12391662597656),
        CLLocationCoordinate2DMake(51.13369295212583,-114.09576416015624),
        CLLocationCoordinate2DMake(51.17546878815025,-114.07585144042969),
        CLLocationCoordinate2DMake(51.140155605265896,-114.04632568359375),
        CLLocationCoordinate2DMake(51.15049396880196,-114.01542663574219),
        CLLocationCoordinate2DMake(51.088860342359965,-114.00924682617186),
        CLLocationCoordinate2DMake(51.12205789681453,-113.94813537597656),
        CLLocationCoordinate2DMake(51.106539930027225,-113.94882202148438),
        CLLocationCoordinate2DMake(51.117747873223344,-113.92616271972656),
        CLLocationCoordinate2DMake(51.10093493903458,-113.92616271972656),
        CLLocationCoordinate2DMake(51.10697105503078,-113.90625),
        CLLocationCoordinate2DMake(51.09144802136697,-113.9117431640625),
        CLLocationCoordinate2DMake(51.04916446529361,-113.97010803222655),
        CLLocationCoordinate2DMake(51.045279344649146,-113.9398956298828),
        CLLocationCoordinate2DMake(51.022825599852496,-114.06211853027344),
        CLLocationCoordinate2DMake(51.045279344649146,-113.9398956298828),
        CLLocationCoordinate2DMake(51.022825599852496,-114.06211853027344),
        CLLocationCoordinate2DMake(51.022825599852496,-114.06280517578125),
        CLLocationCoordinate2DMake(50.968805734317804,-114.06280517578125),
        CLLocationCoordinate2DMake(50.9683733218221,-114.07035827636719),
    };
    NSUInteger coordsCount = sizeof(leafCoords) / sizeof(leafCoords[0]);
    
    MGLPolygonFeature *feature = [MGLPolygonFeature polygonWithCoordinates:leafCoords count:coordsCount];
    feature.identifier = @"leaf-feature";
    feature.attributes = @{@"color": @"red"};
    
    MGLGeoJSONSource *source = [[MGLGeoJSONSource alloc] initWithIdentifier:@"leaf-source" features:@[feature] options:nil];
    [self.mapView.style addSource:source];
    
    MGLFillStyleLayer *layer = [[MGLFillStyleLayer alloc] initWithIdentifier:@"leaf-fill-layer" source:source];
    layer.predicate = [NSPredicate predicateWithFormat:@"color = %@", @"red"];
    MGLStyleValue *fillColor = [MGLStyleValue<UIColor *> valueWithRawValue:[UIColor redColor]];
    layer.fillColor = fillColor;
    [self.mapView.style addLayer:layer];
}

- (void)updateGeoJSONSourceData
{
    [self.mapView setCenterCoordinate:CLLocationCoordinate2DMake(40.329795743702064, -107.75390625) zoomLevel:11 animated:NO];
    
    NSString *geoJSON = @"{\"type\": \"FeatureCollection\",\"features\": [{\"type\": \"Feature\",\"properties\": {},\"geometry\": {\"type\": \"LineString\",\"coordinates\": [[-107.75390625,40.329795743702064],[-104.34814453125,37.64903402157866]]}}]}";
    
    NSData *data = [geoJSON dataUsingEncoding:NSUTF8StringEncoding];
    MGLGeoJSONSource *source = [[MGLGeoJSONSource alloc] initWithIdentifier:@"mutable-data-source-id" geoJSONData:data options:nil];
    [self.mapView.style addSource:source];
    
    MGLLineStyleLayer *layer = [[MGLLineStyleLayer alloc] initWithIdentifier:@"mutable-data-layer-id" source:source];
    [self.mapView.style addLayer:layer];
    
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(1 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        NSString *geoJSON = @"{\"type\": \"FeatureCollection\",\"features\": [{\"type\": \"Feature\",\"properties\": {},\"geometry\": {\"type\": \"LineString\",\"coordinates\": [[-107.75390625,40.329795743702064],[-109.34814453125,37.64903402157866]]}}]}";
        NSData *data = [geoJSON dataUsingEncoding:NSUTF8StringEncoding];
        
        source.geoJSONData = data;
    });
}

- (void)updateGeoJSONSourceURL
{
    [self.mapView setCenterCoordinate:CLLocationCoordinate2DMake(48.668731, -122.857151) zoomLevel:11 animated:NO];
    
    NSString *filePath = [[NSBundle bundleForClass:self.class] pathForResource:@"polyline" ofType:@"geojson"];
    NSURL *geoJSONURL = [NSURL fileURLWithPath:filePath];
    MGLGeoJSONSource *source = [[MGLGeoJSONSource alloc] initWithIdentifier:@"mutable-data-source-url-id" URL:geoJSONURL options:nil];
    [self.mapView.style addSource:source];
    
    MGLLineStyleLayer *layer = [[MGLLineStyleLayer alloc] initWithIdentifier:@"mutable-data-layer-url-id" source:source];
    [self.mapView.style addLayer:layer];
    
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(1 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        [self.mapView setCenterCoordinate:CLLocationCoordinate2DMake(41.563986787078704, -75.04843935793578) zoomLevel:8 animated:NO];
        
        NSString *filePath = [[NSBundle bundleForClass:self.class] pathForResource:@"threestates" ofType:@"geojson"];
        NSURL *geoJSONURL = [NSURL fileURLWithPath:filePath];
        
        source.URL = geoJSONURL;
    });
}

- (void)updateGeoJSONSourceFeatures
{
    [self.mapView setCenterCoordinate:CLLocationCoordinate2DMake(-41.1520, 288.6592) zoomLevel:10 animated:NO];
    
    CLLocationCoordinate2D smallBox[] = {
        {-41.14763798539186, 288.68019104003906},
        {-41.140915920129665, 288.68019104003906},
        {-41.140915920129665, 288.6887741088867},
        {-41.14763798539186, 288.6887741088867},
        {-41.14763798539186, 288.68019104003906}
    };
    
    CLLocationCoordinate2D largeBox[] = {
        {-41.17710352162799, 288.67298126220703},
        {-41.13962313627545, 288.67298126220703},
        {-41.13962313627545, 288.7261962890625},
        {-41.17710352162799, 288.7261962890625},
        {-41.17710352162799, 288.67298126220703}
    };
    
    MGLPolygonFeature *smallBoxFeature = [MGLPolygonFeature polygonWithCoordinates:smallBox count:sizeof(smallBox)/sizeof(smallBox[0])];
    MGLPolygonFeature *largeBoxFeature = [MGLPolygonFeature polygonWithCoordinates:largeBox count:sizeof(largeBox)/sizeof(largeBox[0])];

    MGLGeoJSONSource *source = [[MGLGeoJSONSource alloc] initWithIdentifier:@"mutable-data-source-features-id" features:@[smallBoxFeature] options:nil];
    [self.mapView.style addSource:source];
    
    MGLFillStyleLayer *layer = [[MGLFillStyleLayer alloc] initWithIdentifier:@"mutable-data-layer-features-id" source:source];
    MGLStyleValue *fillColor = [MGLStyleValue<UIColor *> valueWithRawValue:[UIColor redColor]];
    layer.fillColor = fillColor;
    [self.mapView.style addLayer:layer];
    
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(2 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        source.features = @[largeBoxFeature];
    });
}

- (void)styleDynamicPointCollection
{
    [self.mapView setCenterCoordinate:CLLocationCoordinate2DMake(36.9979, -109.0441) zoomLevel:14 animated:NO];
    CLLocationCoordinate2D coordinates[] = {
        {37.00145594210082, -109.04960632324219},
        {37.00173012609867, -109.0404224395752},
        {36.99453246847359, -109.04960632324219},
        {36.99508088541243, -109.04007911682129},
    };
    MGLPointCollectionFeature *feature = [MGLPointCollectionFeature pointCollectionWithCoordinates:coordinates count:4];
    MGLGeoJSONSource *source = [[MGLGeoJSONSource alloc] initWithIdentifier:@"wiggle-source" features:@[feature] options:nil];
    [self.mapView.style addSource:source];
    
    MGLCircleStyleLayer *layer = [[MGLCircleStyleLayer alloc] initWithIdentifier:@"wiggle-layer" source:source];
    [self.mapView.style addLayer:layer];
}

- (void)styleVectorSource
{
    NSURL *url = [[NSURL alloc] initWithString:@"mapbox://mapbox.mapbox-terrain-v2"];
    MGLVectorSource *vectorSource = [[MGLVectorSource alloc] initWithIdentifier:@"style-vector-source-id" URL:url];
    [self.mapView.style addSource:vectorSource];
    
    MGLBackgroundStyleLayer *backgroundLayer = [[MGLBackgroundStyleLayer alloc] initWithIdentifier:@"style-vector-background-layer-id"];
    backgroundLayer.backgroundColor = [MGLStyleValue<UIColor *> valueWithRawValue:[UIColor blackColor]];
    [self.mapView.style addLayer:backgroundLayer];
    
    MGLLineStyleLayer *lineLayer = [[MGLLineStyleLayer alloc] initWithIdentifier:@"style-vector-line-layer-id" source:vectorSource];
    lineLayer.sourceLayerIdentifier = @"contour";
    NSUInteger lineJoinValue = MGLLineJoinRound;
    lineLayer.lineJoin = [MGLStyleValue<NSValue *> valueWithRawValue:[NSValue value:&lineJoinValue withObjCType:@encode(MGLLineJoin)]];
    NSUInteger lineCapValue = MGLLineCapRound;
    lineLayer.lineCap = [MGLStyleValue<NSValue *> valueWithRawValue:[NSValue value:&lineCapValue withObjCType:@encode(MGLLineCap)]];
    lineLayer.lineColor = [MGLStyleValue<UIColor *> valueWithRawValue:[UIColor greenColor]];

    [self.mapView.style addLayer:lineLayer];
}

- (void)styleRasterSource
{
    // 3rd party raster source requires NSAppTransportSecurity exception for stamen.com
    MGLTileSet *rasterTileSet = [[MGLTileSet alloc] initWithTileURLTemplates:@[@"http://a.tile.stamen.com/terrain-background/{z}/{x}/{y}.jpg"]];
    MGLRasterSource *rasterSource = [[MGLRasterSource alloc] initWithIdentifier:@"style-raster-source-id" tileSet:rasterTileSet tileSize:256];
    [self.mapView.style addSource:rasterSource];
    
    MGLRasterStyleLayer *rasterLayer = [[MGLRasterStyleLayer alloc] initWithIdentifier:@"style-raster-layer-id" source:rasterSource];
    [self.mapView.style addLayer:rasterLayer];
}

-(void)styleCountryLabelsLanguage
{
    NSArray<NSString *> *labelLayers = @[
        @"country-label-lg",
        @"country-label-md",
        @"country-label-sm",
    ];
    [self styleLabelLanguageForLayersNamed:labelLayers];
}

- (void)styleRouteLine
{
    CLLocationCoordinate2D coords[] = {
        { 43.84455590478528, 10.504238605499268 },
        { 43.84385562343126, 10.504125952720642 },
        { 43.84388657526694, 10.503299832344055 },
        { 43.84332557075269, 10.503235459327698 },
        { 43.843441641085036, 10.502264499664307 },
        { 43.84396395478592, 10.50242006778717 },
        { 43.84406067904351, 10.501744151115416 },
        { 43.84422317544319, 10.501792430877686 }
    };
    NSInteger count = sizeof(coords) / sizeof(coords[0]);

    [self.mapView setCenterCoordinate:coords[0] zoomLevel:16 animated:YES];

    MGLPolylineFeature *routeLine = [MGLPolylineFeature polylineWithCoordinates:coords count:count];

    MGLGeoJSONSource *routeSource = [[MGLGeoJSONSource alloc] initWithIdentifier:@"style-route-source" features:@[routeLine] options:nil];
    [self.mapView.style addSource:routeSource];

    MGLLineStyleLayer *baseRouteLayer = [[MGLLineStyleLayer alloc] initWithIdentifier:@"style-base-route-layer" source:routeSource];
    baseRouteLayer.lineColor = [MGLStyleConstantValue valueWithRawValue:[UIColor orangeColor]];
    baseRouteLayer.lineWidth = [MGLStyleConstantValue valueWithRawValue:@20];
    baseRouteLayer.lineOpacity = [MGLStyleConstantValue valueWithRawValue:@0.5];
    baseRouteLayer.lineCap = [MGLStyleConstantValue valueWithRawValue:[NSValue valueWithMGLLineCap:MGLLineCapRound]];
    baseRouteLayer.lineJoin = [MGLStyleConstantValue valueWithRawValue:[NSValue valueWithMGLLineJoin:MGLLineJoinRound]];
    [self.mapView.style addLayer:baseRouteLayer];

    MGLLineStyleLayer *routeLayer = [[MGLLineStyleLayer alloc] initWithIdentifier:@"style-base-route-layer" source:routeSource];
    routeLayer.lineColor = [MGLStyleConstantValue valueWithRawValue:[UIColor whiteColor]];
    routeLayer.lineWidth = [MGLStyleConstantValue valueWithRawValue:@15];
    routeLayer.lineOpacity = [MGLStyleConstantValue valueWithRawValue:@0.8];
    routeLayer.lineCap = [MGLStyleConstantValue valueWithRawValue:[NSValue valueWithMGLLineCap:MGLLineCapRound]];
    routeLayer.lineJoin = [MGLStyleConstantValue valueWithRawValue:[NSValue valueWithMGLLineJoin:MGLLineJoinRound]];
    [self.mapView.style addLayer:routeLayer];
}

- (void)styleLabelLanguageForLayersNamed:(NSArray<NSString *> *)layers
{
    _usingLocaleBasedCountryLabels = !_usingLocaleBasedCountryLabels;
    NSString *bestLanguageForUser = [NSString stringWithFormat:@"{name_%@}", [self bestLanguageForUser]];
    NSString *language = _usingLocaleBasedCountryLabels ? bestLanguageForUser : @"{name}";

    for (NSString *layerName in layers) {
        MGLSymbolStyleLayer *layer = (MGLSymbolStyleLayer *)[self.mapView.style layerWithIdentifier:layerName];

        if ([layer isKindOfClass:[MGLSymbolStyleLayer class]]) {
            if ([layer.textField isKindOfClass:[MGLStyleConstantValue class]]) {
                MGLStyleConstantValue *label = (MGLStyleConstantValue<NSString *> *)layer.textField;
                if ([label.rawValue hasPrefix:@"{name"]) {
                    layer.textField = [MGLStyleValue valueWithRawValue:language];
                }
            } else if ([layer.textField isKindOfClass:[MGLStyleFunction class]]) {
                MGLStyleFunction *function = (MGLStyleFunction<NSString *> *)layer.textField;
                [function.stops enumerateKeysAndObjectsUsingBlock:^(id zoomLevel, id stop, BOOL *done) {
                    if ([stop isKindOfClass:[MGLStyleConstantValue class]]) {
                        MGLStyleConstantValue *label = (MGLStyleConstantValue<NSString *> *)stop;
                        if ([label.rawValue hasPrefix:@"{name"]) {
                            [function.stops setValue:[MGLStyleValue valueWithRawValue:language] forKey:zoomLevel];
                        }
                    }
                }];
                layer.textField = function;
            }
        } else {
            NSLog(@"%@ is not a symbol style layer", layerName);
        }
    }
}

- (NSString *)bestLanguageForUser
{
    NSArray *supportedLanguages = @[ @"en", @"es", @"fr", @"de", @"ru", @"zh" ];
    NSArray<NSString *> *preferredLanguages = [NSLocale preferredLanguages];
    NSString *bestLanguage;

    for (NSString *language in preferredLanguages) {
        NSString *thisLanguage = [NSLocale localeWithLocaleIdentifier:language].languageCode;
        if ([supportedLanguages containsObject:thisLanguage]) {
            bestLanguage = thisLanguage;
            break;
        }
    }

    return bestLanguage ?: @"en";
}

- (IBAction)startWorldTour
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

- (void)toggleCustomUserDot
{
    _customUserLocationAnnnotationEnabled = !_customUserLocationAnnnotationEnabled;
    self.mapView.showsUserLocation = NO;
    self.mapView.userTrackingMode = MGLUserTrackingModeFollow;
}

- (void)testQueryPointAnnotations {
    NSNumber *visibleAnnotationCount = @(self.mapView.visibleAnnotations.count);
    NSString *message;
    if ([visibleAnnotationCount integerValue] == 1) {
        message = [NSString stringWithFormat:@"There is %@ visible annotation.", visibleAnnotationCount];
    } else {
        message = [NSString stringWithFormat:@"There are %@ visible annotations.", visibleAnnotationCount];
    }
    
    UIAlertController *alertController = [UIAlertController alertControllerWithTitle:@"Visible Annotations" message:message preferredStyle:UIAlertControllerStyleAlert];
    [alertController addAction:[UIAlertAction actionWithTitle:@"Ok" style:UIAlertActionStyleCancel handler:nil]];
    [self presentViewController:alertController animated:YES completion:nil];
}

- (void)printTelemetryLogFile
{
    NSString *fileContents = [NSString stringWithContentsOfFile:[self telemetryDebugLogFilePath] encoding:NSUTF8StringEncoding error:nil];
    NSLog(@"%@", fileContents);
}

- (void)deleteTelemetryLogFile
{
    NSString *filePath = [self telemetryDebugLogFilePath];
    if ([[NSFileManager defaultManager] isDeletableFileAtPath:filePath])
    {
        NSError *error;
        BOOL success = [[NSFileManager defaultManager] removeItemAtPath:filePath error:&error];
        if (success) {
            NSLog(@"Deleted telemetry log.");
        } else {
            NSLog(@"Error deleting telemetry log: %@", error.localizedDescription);
        }
    }
}

- (NSString *)telemetryDebugLogFilePath
{
    NSDateFormatter *dateFormatter = [[NSDateFormatter alloc] init];
    [dateFormatter setDateFormat:@"yyyy'-'MM'-'dd"];
    [dateFormatter setTimeZone:[NSTimeZone systemTimeZone]];
    NSString *filePath = [[NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) firstObject] stringByAppendingPathComponent:[NSString stringWithFormat:@"telemetry_log-%@.json", [dateFormatter stringFromDate:[NSDate date]]]];

    return filePath;
}

#pragma mark - User Actions

- (IBAction)handleLongPress:(UILongPressGestureRecognizer *)longPress
{
    if (longPress.state == UIGestureRecognizerStateBegan)
    {
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

#pragma mark - Map Delegate

- (MGLAnnotationView *)mapView:(MGLMapView *)mapView viewForAnnotation:(id<MGLAnnotation>)annotation
{
    if (annotation == mapView.userLocation)
    {
        if (_customUserLocationAnnnotationEnabled)
        {
            MBXUserLocationAnnotationView *annotationView = [[MBXUserLocationAnnotationView alloc] initWithFrame:CGRectZero];
            annotationView.frame = CGRectMake(0, 0, annotationView.intrinsicContentSize.width, annotationView.intrinsicContentSize.height);
            return annotationView;
        }

        return nil;
    }
    // Use GL backed pins for dropped pin annotations
    if ([annotation isKindOfClass:[MBXDroppedPinAnnotation class]] || [annotation isKindOfClass:[MBXSpriteBackedAnnotation class]])
    {
        return nil;
    }
    
    MBXAnnotationView *annotationView = (MBXAnnotationView *)[mapView dequeueReusableAnnotationViewWithIdentifier:MBXViewControllerAnnotationViewReuseIdentifer];
    if (!annotationView)
    {
        annotationView = [[MBXAnnotationView alloc] initWithReuseIdentifier:MBXViewControllerAnnotationViewReuseIdentifer];
        annotationView.frame = CGRectMake(0, 0, 10, 10);
        annotationView.backgroundColor = [UIColor whiteColor];

        // Note that having two long press gesture recognizers on overlapping
        // views (`self.view` & `annotationView`) will cause weird behaviour.
        // Comment out the pin dropping functionality in the handleLongPress:
        // method in this class to make draggable annotation views play nice.
        annotationView.draggable = YES;
        
        // Uncomment to force annotation view to maintain a constant size when
        // the map is tilted. By default, annotation views will shrink and grow
        // as they move towards and away from the horizon. Relatedly, annotations
        // backed by GL sprites currently ONLY scale with viewing distance.
        // annotationView.scalesWithViewingDistance = NO;
    } else {
        // orange indicates that the annotation view was reused
        annotationView.backgroundColor = [UIColor orangeColor];
    }
    return annotationView;
}

- (MGLAnnotationImage *)mapView:(MGLMapView * __nonnull)mapView imageForAnnotation:(id <MGLAnnotation> __nonnull)annotation
{
    if ([annotation isKindOfClass:[MBXDroppedPinAnnotation class]] || [annotation isKindOfClass:[MBXCustomCalloutAnnotation class]])
    {
        return nil; // use default marker
    }

    NSAssert([annotation isKindOfClass:[MBXSpriteBackedAnnotation class]], @"Annotations should be sprite-backed.");

    NSString *title = [(MGLPointAnnotation *)annotation title];
    if (!title.length) return nil;
    NSString *lastTwoCharacters = [title substringFromIndex:title.length - 2];

    MGLAnnotationImage *annotationImage = [mapView dequeueReusableAnnotationImageWithIdentifier:lastTwoCharacters];

    if ( ! annotationImage)
    {
        UIColor *color;

        // make every tenth annotation blue
        if ([lastTwoCharacters hasSuffix:@"0"]) {
            color = [UIColor blueColor];
        } else {
            color = [UIColor redColor];
        }

        UIImage *image = [self imageWithText:lastTwoCharacters backgroundColor:color];
        annotationImage = [MGLAnnotationImage annotationImageWithImage:image reuseIdentifier:lastTwoCharacters];

        // don't allow touches on blue annotations
        if ([color isEqual:[UIColor blueColor]]) annotationImage.enabled = NO;
    }

    return annotationImage;
}


- (UIImage *)imageWithText:(NSString *)text backgroundColor:(UIColor *)color
{
    CGRect rect = CGRectMake(0, 0, 20, 15);

    UIGraphicsBeginImageContextWithOptions(rect.size, NO, [[UIScreen mainScreen] scale]);

    CGContextRef ctx = UIGraphicsGetCurrentContext();

    CGContextSetFillColorWithColor(ctx, [[color colorWithAlphaComponent:0.75] CGColor]);
    CGContextFillRect(ctx, rect);

    CGContextSetStrokeColorWithColor(ctx, [[UIColor blackColor] CGColor]);
    CGContextStrokeRectWithWidth(ctx, rect, 2);

    NSAttributedString *drawString = [[NSAttributedString alloc] initWithString:text attributes:@{
        NSFontAttributeName: [UIFont fontWithName:@"Arial-BoldMT" size:12],
        NSForegroundColorAttributeName: [UIColor whiteColor],
    }];
    CGSize stringSize = drawString.size;
    CGRect stringRect = CGRectMake((rect.size.width - stringSize.width) / 2,
                                   (rect.size.height - stringSize.height) / 2,
                                   stringSize.width,
                                   stringSize.height);
    [drawString drawInRect:stringRect];

    UIImage *image = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    return image;
}

- (BOOL)mapView:(__unused MGLMapView *)mapView annotationCanShowCallout:(__unused id <MGLAnnotation>)annotation
{
    return YES;
}

- (UIColor *)mapView:(__unused MGLMapView *)mapView strokeColorForShapeAnnotation:(MGLShape *)annotation
{
    return ([annotation isKindOfClass:[MGLPolyline class]] ? [UIColor purpleColor] : [UIColor blackColor]);
}

- (UIColor *)mapView:(__unused MGLMapView *)mapView fillColorForPolygonAnnotation:(__unused MGLPolygon *)annotation
{
    UIColor *color = annotation.pointCount > 3 ? [UIColor greenColor] : [UIColor redColor];
    return [color colorWithAlphaComponent:0.5];
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

- (void)mapView:(MGLMapView *)mapView didFinishLoadingStyle:(MGLStyle *)style
{
    // Default Mapbox styles use {name_en} as their label language, which means
    // that a device with an English-language locale is already effectively
    // using locale-based country labels.
    _usingLocaleBasedCountryLabels = [[self bestLanguageForUser] isEqualToString:@"en"];
}

- (void)mapViewRegionIsChanging:(MGLMapView *)mapView
{
    if (self.reuseQueueStatsEnabled) {
        NSUInteger queuedAnnotations = 0;
        for (NSArray *queue in self.mapView.annotationViewReuseQueueByIdentifier.allValues)
        {
            queuedAnnotations += queue.count;
        }
        self.hudLabel.text = [NSString stringWithFormat:@"Visible: %ld  Queued: %ld", (unsigned long)mapView.visibleAnnotations.count, (unsigned long)queuedAnnotations];
    }
}

@end
