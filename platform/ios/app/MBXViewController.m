#import "MBXViewController.h"

#import "MBXAppDelegate.h"
#import "MBXOfflinePacksTableViewController.h"
#import "MBXAnnotationView.h"
#import "LimeGreenStyleLayer.h"
#import "MBXEmbeddedMapViewController.h"
#import "MBXOrnamentsViewController.h"
#import "MBXStateManager.h"
#import "MBXState.h"

#import "MBXFrameTimeGraphView.h"

#import <Mapbox/Mapbox.h>
#import "../src/MGLMapView_Experimental.h"

#import <objc/runtime.h>

static NSString * const MBXViewControllerAnnotationViewReuseIdentifer = @"MBXViewControllerAnnotationViewReuseIdentifer";

typedef NS_ENUM(NSInteger, MBXSettingsSections) {
    MBXSettingsDebugTools = 0,
    MBXSettingsAnnotations,
    MBXSettingsRuntimeStyling,
    MBXSettingsMiscellaneous,
};

typedef NS_ENUM(NSInteger, MBXSettingsDebugToolsRows) {
    MBXSettingsDebugToolsResetPosition = 0,
    MBXSettingsDebugToolsTileBoundaries,
    MBXSettingsDebugToolsTileInfo,
    MBXSettingsDebugToolsTimestamps,
    MBXSettingsDebugToolsCollisionBoxes,
    MBXSettingsDebugToolsOverdrawVisualization,
    MBXSettingsDebugToolsShowZoomLevel,
    MBXSettingsDebugToolsShowFrameTimeGraph,
    MBXSettingsDebugToolsShowReuseQueueStats
};

typedef NS_ENUM(NSInteger, MBXSettingsAnnotationsRows) {
    MBXSettingsAddAnnotations = 0,
    MBXSettingsAddSymbols,
    MBXSettingsAnnotationAnimation, // TODO: Move to ios-sdk-examples
    MBXSettingsAnnotationsRemoveAnnotations,
    MBXSettingsAnnotationSelectRandomOffscreenPointAnnotation,
    MBXSettingsAnnotationCenterSelectedAnnotation,
    MBXSettingsAnnotationAddVisibleAreaPolyline
};

typedef NS_ENUM(NSInteger, MBXSettingsRuntimeStylingRows) {
    MBXSettingsRuntimeStylingAddLimeGreenTriangleLayer,
    MBXSettingsRuntimeStylingDDSPolygon,
    MBXSettingsRuntimeStylingCustomLatLonGrid, // TODO: Move to ios-sdk-examples
};

typedef NS_ENUM(NSInteger, MBXSettingsMiscellaneousRows) {
    MBXSettingsMiscellaneousRandomTour,
    MBXSettingsMiscellaneousScrollView,
    MBXSettingsMiscellaneousToggleTwoMaps,
    MBXSettingsMiscellaneousLocalizeLabels,
    MBXSettingsMiscellaneousMissingIcon,
    MBXSettingsMiscellaneousShouldLimitCameraChanges,
    MBXSettingsMiscellaneousShowCustomLocationManager,
    MBXSettingsMiscellaneousOrnamentsPlacement,
    MBXSettingsMiscellaneousPrintLogFile,
    MBXSettingsMiscellaneousDeleteLogFile
};

typedef NS_ENUM(NSInteger, MBXSettingsMarkerTypes) {
    MBXSettingsMarkerTypeAnnotation = 0,
    MBXSettingsMarkerTypeSymbol
};

// Utility methods
CLLocationCoordinate2D coordinateCentered(CLLocationCoordinate2D origin, CLLocationDegrees bearing, CLLocationDistance distance) {

    // Convert to radians
    double multiplier      = M_PI / 180.0;
    double sourceLatitude  = multiplier * origin.latitude;
    double sourceLongitude = multiplier * origin.longitude;
    bearing *= multiplier;
    distance /= 6378137.0;

    // Pulled from MGLRadianCoordinateAtDistanceFacingDirection:
    double latitude = asin((sin(sourceLatitude) * cos(distance)) +
                           (cos(sourceLatitude) * sin(distance) * cos(bearing)));

    double longitude = sourceLongitude + atan2((sin(bearing) * sin(distance) * cos(sourceLatitude)),
                                               cos(distance) - (sin(sourceLatitude) * sin(latitude)));

    CLLocationCoordinate2D result;
    result.latitude  = fmin(85.0, fmax(-85.0, (latitude / multiplier)));
    result.longitude = longitude / multiplier;
    return result;
}

CLLocationCoordinate2D randomWorldCoordinate() {

    static const struct {
        CLLocationCoordinate2D coordinate;
        CLLocationDistance radius;
    } landmasses[] = {
        // Rough land masses
        {{ 38.328531,   94.778736 },    4100000 },  // Asia
        {{ 1.477244,    18.138111 },    4100000 },  // Africa
        {{ 52.310059,   22.295425 },    2000000 },  // Europe
        {{ 42.344216,   -96.532700 },   3000000 },  // N America
        {{ -11.537273,  -57.035181 },   2220000 },  // S America
        {{ -20.997030,  134.660541 },   2220000 },  // Australia

        // A few cities
        {{ 51.504787,   -0.106977 },    33000 },    // London
        {{ 37.740186,   -122.437086 },  8500 },     // SF
        {{ 52.509978,   13.406510 },    12000 },    // Berlin
        {{ 12.966246,   77.586505 },    19000 }     // Bengaluru
    };

    NSInteger index                   = drand48() * (sizeof(landmasses)/sizeof(landmasses[0]));
    CLLocationCoordinate2D coordinate = landmasses[index].coordinate;
    CLLocationDistance radius         = landmasses[index].radius;

    // Now create a world coord
    CLLocationDegrees heading          = drand48()*360.0;
    CLLocationDistance distance        = drand48()*radius;
    CLLocationCoordinate2D newLocation = coordinateCentered(coordinate, heading, distance);
    return newLocation;
}

@interface MBXDroppedPinAnnotation : MGLPointAnnotation
@end

@implementation MBXDroppedPinAnnotation
@end

@interface MBXSpriteBackedAnnotation : MGLPointAnnotation
@end

@implementation MBXSpriteBackedAnnotation
@end

@interface MBXViewController () <UITableViewDelegate,
                                 UITableViewDataSource,
                                 MGLMapViewDelegate,
                                 MGLComputedShapeSourceDataSource>


@property (nonatomic) IBOutlet MGLMapView *mapView;
@property (nonatomic) MBXState *currentMapState;
@property (weak, nonatomic) IBOutlet UIButton *mapInfoLabel;
@property (weak, nonatomic) IBOutlet MBXFrameTimeGraphView *frameTimeGraphView;
@property (nonatomic) NSInteger styleIndex;
@property (nonatomic) BOOL debugLoggingEnabled;
@property (nonatomic) BOOL customUserLocationAnnnotationEnabled;
@property (nonatomic, getter=isLocalizingLabels) BOOL localizingLabels;
@property (nonatomic) BOOL reuseQueueStatsEnabled;
@property (nonatomic) BOOL randomWalk;
@property (nonatomic) NSMutableArray<UIWindow *> *helperWindows;

@end

@interface MGLMapView (MBXViewController)
@property (nonatomic) NSDictionary *annotationViewReuseQueueByIdentifier;
@end

@implementation MBXViewController
{
    BOOL _isTouringWorld;
}

#pragma mark - Setup & Teardown

- (void)viewDidLoad
{
    [super viewDidLoad];

    self.currentMapState = [MBXStateManager sharedManager].currentState;

    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(saveState:) name:UIApplicationDidEnterBackgroundNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(saveState:) name:UIApplicationWillTerminateNotification object:nil];

    // Restore state here

    self.debugLoggingEnabled = [[NSUserDefaults standardUserDefaults] boolForKey:@"MGLMapboxMetricsDebugLoggingEnabled"];

    self.mapView.showsScale = YES;
    self.mapView.showsUserHeadingIndicator = YES;
    self.mapView.experimental_enableFrameRateMeasurement = YES;
    self.mapInfoLabel.titleLabel.font = [UIFont monospacedDigitSystemFontOfSize:10 weight:UIFontWeightRegular];

    // Add fall-through single tap gesture recognizer. This will be called when
    // the map view's tap recognizers fail.
    UITapGestureRecognizer *singleTap = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(handleSingleTap:)];
    for (UIGestureRecognizer *gesture in self.mapView.gestureRecognizers) {
        if ([gesture isKindOfClass:[UITapGestureRecognizer class]]) {
            [singleTap requireGestureRecognizerToFail:gesture];
        }
    }
    [self.mapView addGestureRecognizer:singleTap];
    
    // Display a secondary map on any connected external display.
    // https://developer.apple.com/documentation/uikit/windows_and_screens/displaying_content_on_a_connected_screen?language=objc
    self.helperWindows = [NSMutableArray array];
    [[NSNotificationCenter defaultCenter] addObserverForName:UIScreenDidConnectNotification object:nil queue:nil usingBlock:^(NSNotification * _Nonnull note) {
        UIScreen *helperScreen = note.object;
        UIWindow *helperWindow = [[UIWindow alloc] initWithFrame:helperScreen.bounds];
        helperWindow.screen = helperScreen;
        UIViewController *helperViewController = [[UIViewController alloc] init];
        MGLMapView *helperMapView = [[MGLMapView alloc] initWithFrame:helperWindow.bounds styleURL:MGLStyle.satelliteStreetsStyleURL];
        helperMapView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
        helperMapView.camera = self.mapView.camera;
        helperMapView.compassView.hidden = YES;
        helperViewController.view = helperMapView;
        helperWindow.rootViewController = helperViewController;
        helperWindow.hidden = NO;
        [self.helperWindows addObject:helperWindow];
    }];
    [[NSNotificationCenter defaultCenter] addObserverForName:UIScreenDidDisconnectNotification object:nil queue:nil usingBlock:^(NSNotification * _Nonnull note) {
        UIScreen *helperScreen = note.object;
        for (UIWindow *window in self.helperWindows) {
            if (window.screen == helperScreen) {
                [self.helperWindows removeObject:window];
            }
        }
    }];
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
    [[MBXStateManager sharedManager] resetState];
}

#pragma mark - Debugging Interface

- (IBAction)showSettings:(__unused id)sender
{
    self.randomWalk = NO;

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
        @"Debug Tools",
        @"Annotations",
        @"Runtime Styling",
        @"Miscellaneous"
    ];
}

- (NSArray <NSString *> *)settingsTitlesForSection:(NSInteger)section
{
    NSMutableArray *settingsTitles = [NSMutableArray array];

    MGLMapDebugMaskOptions debugMask = self.currentMapState.debugMask;

    switch (section)
    {
        case MBXSettingsDebugTools:
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
                [NSString stringWithFormat:@"%@ Map Info Ornament", (self.currentMapState.showsZoomLevelOrnament ? @"Hide" :@"Show")],
                [NSString stringWithFormat:@"%@ Frame Time Graph", (self.currentMapState.showsTimeFrameGraph ? @"Hide" :@"Show")],
                [NSString stringWithFormat:@"%@ Reuse Queue Stats", (_reuseQueueStatsEnabled ? @"Hide" :@"Show")]
            ]];
            break;
        case MBXSettingsAnnotations:
            [settingsTitles addObjectsFromArray:@[
                @"Add view annotations",
                @"Add symbols",
                @"Animate an Annotation View", // TODO: Move to ios-sdk-examples
                @"Remove Annotations",
                @"Select an offscreen point annotation",
                @"Center selected annotation",
                @"Add visible area polyline"
            ]];
            break;
        case MBXSettingsRuntimeStyling:
            [settingsTitles addObjectsFromArray:@[
                @"Add Lime Green Triangle Layer",
                @"Dynamically Style Polygon",
                @"Add Custom Lat/Lon Grid",
            ]];
            break;
        case MBXSettingsMiscellaneous:
            [settingsTitles addObjectsFromArray:@[
                @"Random Tour",
                @"Embedded Map View",
                [NSString stringWithFormat:@"%@ Second Map", ([self.view viewWithTag:2] == nil ? @"Show" : @"Hide")],
                [NSString stringWithFormat:@"Show Labels in %@", (_localizingLabels ? @"Default Language" : [[NSLocale currentLocale] displayNameForKey:NSLocaleIdentifier value:[self bestLanguageForUser]])],
                @"Ornaments Placement",
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
        case MBXSettingsDebugTools:
            switch (indexPath.row)
            {
                case MBXSettingsDebugToolsResetPosition:
                    [self.mapView resetPosition];
                    break;
                case MBXSettingsDebugToolsTileBoundaries:
                    self.currentMapState.debugMask ^= MGLMapDebugTileBoundariesMask;
                    break;
                case MBXSettingsDebugToolsTileInfo:
                    self.currentMapState.debugMask ^= MGLMapDebugTileInfoMask;
                    break;
                case MBXSettingsDebugToolsTimestamps:
                    self.currentMapState.debugMask ^= MGLMapDebugTimestampsMask;
                    break;
                case MBXSettingsDebugToolsCollisionBoxes:
                    self.currentMapState.debugMask ^= MGLMapDebugCollisionBoxesMask;
                    break;
                case MBXSettingsDebugToolsOverdrawVisualization:
                    self.currentMapState.debugMask ^= MGLMapDebugOverdrawVisualizationMask;
                    break;
                case MBXSettingsDebugToolsShowZoomLevel:
                {
                    self.currentMapState.showsZoomLevelOrnament = !self.currentMapState.showsZoomLevelOrnament;
                    self.mapInfoLabel.hidden = !self.currentMapState.showsZoomLevelOrnament;

                    self.reuseQueueStatsEnabled = NO;
                    [self updateHUD];
                    break;
                }
                case MBXSettingsDebugToolsShowFrameTimeGraph:
                {
                    self.currentMapState.showsTimeFrameGraph = !self.currentMapState.showsTimeFrameGraph;
                    self.frameTimeGraphView.hidden = !self.currentMapState.showsTimeFrameGraph;
                    break;
                }
                case MBXSettingsDebugToolsShowReuseQueueStats:
                {
                    self.reuseQueueStatsEnabled = !self.reuseQueueStatsEnabled;
                    self.mapInfoLabel.hidden = !self.reuseQueueStatsEnabled;
                    self.currentMapState.showsZoomLevelOrnament = NO;
                    break;
                }
                default:
                    NSAssert(NO, @"All debug tools setting rows should be implemented");
                    break;
            }
            break;
        case MBXSettingsAnnotations:
            switch (indexPath.row)
            {
                case MBXSettingsAddAnnotations:
                    [self addMarkersWithType:MBXSettingsMarkerTypeAnnotation];
                    break;
                case MBXSettingsAddSymbols:
                    [self addMarkersWithType:MBXSettingsMarkerTypeSymbol];
                    break;
                case MBXSettingsAnnotationAnimation: // TODO: Move to ios-sdk-examples
                    [self animateAnnotationView];
                    break;
                case MBXSettingsAnnotationsRemoveAnnotations:
                    [self.mapView removeAnnotations:self.mapView.annotations];
                    break;
                case MBXSettingsAnnotationSelectRandomOffscreenPointAnnotation:
                    [self selectAnOffscreenPointAnnotation];
                    break;

                case MBXSettingsAnnotationCenterSelectedAnnotation:
                    [self centerSelectedAnnotation];
                    break;

                case MBXSettingsAnnotationAddVisibleAreaPolyline:
                    [self addVisibleAreaPolyline];
                    break;

                default:
                    NSAssert(NO, @"All annotations setting rows should be implemented");
                    break;
            }
            break;
        case MBXSettingsRuntimeStyling:
            switch (indexPath.row)
            {
                case MBXSettingsRuntimeStylingAddLimeGreenTriangleLayer:
                    [self styleAddLimeGreenTriangleLayer];
                    break;
                case MBXSettingsRuntimeStylingCustomLatLonGrid:
                    [self addLatLonGrid];
                    break;
                default:
                    NSAssert(NO, @"All runtime styling setting rows should be implemented");
                    break;
            }
            break;
        case MBXSettingsMiscellaneous:
            switch (indexPath.row)
            {
                case MBXSettingsMiscellaneousLocalizeLabels: // TODO: Move to ios-sdk-examples
                    [self toggleStyleLabelsLanguage];
                    break;
                case MBXSettingsMiscellaneousRandomTour:
                    [self randomWorldTour];
                    break;
                case MBXSettingsMiscellaneousPrintLogFile:
                    [self printTelemetryLogFile];
                    break;
                case MBXSettingsMiscellaneousDeleteLogFile:
                    [self deleteTelemetryLogFile];
                    break;
                case MBXSettingsMiscellaneousScrollView:
                {
                    UIStoryboard *storyboard = [UIStoryboard storyboardWithName:@"Main" bundle:nil];
                    MBXEmbeddedMapViewController *embeddedMapViewController = (MBXEmbeddedMapViewController *)[storyboard instantiateViewControllerWithIdentifier:@"MBXEmbeddedMapViewController"];
                    [self.navigationController pushViewController:embeddedMapViewController animated:YES];
                    break;
                }
                case MBXSettingsMiscellaneousToggleTwoMaps:
                    [self toggleSecondMapView];
                    break;
                case MBXSettingsMiscellaneousMissingIcon:
                {
                    [self loadMissingIcon];
                    break;
                }
                case MBXSettingsMiscellaneousShowCustomLocationManager:
                {
                    [self performSegueWithIdentifier:@"ShowCustomLocationManger" sender:nil];
                    break;
                }
                case MBXSettingsMiscellaneousOrnamentsPlacement:
                {
                    MBXOrnamentsViewController *ornamentsViewController = [[MBXOrnamentsViewController alloc] init];
                    [self.navigationController pushViewController:ornamentsViewController animated:YES];
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

- (void)addMarkersWithType:(NSInteger)markerType
{
    NSString *markerTitle = (markerType == MBXSettingsMarkerTypeAnnotation ? @"annotations" : @"symbols");

    UIAlertController *addMarkerAlert = [UIAlertController alertControllerWithTitle:[NSString stringWithFormat:@"Add %@", markerTitle]
                                                                            message:@"Enter a number"
                                                                     preferredStyle:UIAlertControllerStyleAlert];
    UIAlertAction *saveMarkerAction = [UIAlertAction actionWithTitle:@"Save" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {

        NSUInteger markerCount = addMarkerAlert.textFields[0].text.integerValue;

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

                                   MGLPointAnnotation *annotation = (markerType == MBXSettingsMarkerTypeAnnotation ? [MGLPointAnnotation new] : [MBXSpriteBackedAnnotation new]);

                                   annotation.coordinate = coordinate;
                                   annotation.title = title;

                                   [annotations addObject:annotation];

                                   if (annotations.count == markerCount) break;
                               }

                               dispatch_async(dispatch_get_main_queue(), ^
                                              {
                                                  [self.mapView addAnnotations:annotations];
                                                  [self.mapView showAnnotations:annotations animated:YES];
                                              });
                           }
                       });
    }];

    [addMarkerAlert addTextFieldWithConfigurationHandler:^(UITextField * _Nonnull textField) {
        textField.keyboardType = UIKeyboardTypeNumberPad;
    }];

    [addMarkerAlert addAction:saveMarkerAction];
    [self presentViewController:addMarkerAlert animated:YES completion:nil];

}

- (void)animateAnnotationView
    {
        MGLPointAnnotation *annot = [[MGLPointAnnotation alloc] init];
        annot.coordinate = self.mapView.centerCoordinate;
        [self.mapView addAnnotation:annot];
        
        // Move the annotation to a point that is offscreen.
        CGPoint point = CGPointMake(self.view.frame.origin.x - 200, CGRectGetMidY(self.view.frame));
        
        CLLocationCoordinate2D coord = [self.mapView convertPoint:point toCoordinateFromView:self.view];
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(2 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
            [UIView animateWithDuration:10 animations:^{
                annot.coordinate = coord;
            }];
        });
    };

-(void)toggleStyleLabelsLanguage // TODO: Move to ios-sdk-examples
{
    _localizingLabels = !_localizingLabels;
    [self.mapView.style localizeLabelsIntoLocale:_localizingLabels ? [NSLocale localeWithLocaleIdentifier:@"mul"] : nil];
}

- (void)styleAddLimeGreenTriangleLayer
{
    LimeGreenStyleLayer *layer = [[LimeGreenStyleLayer alloc] initWithIdentifier:@"mbx-custom"];
    [self.mapView.style addLayer:layer];
}

// TODO: Move to ios-sdk-examples
- (void)addLatLonGrid
{
    MGLComputedShapeSource *source = [[MGLComputedShapeSource alloc] initWithIdentifier:@"latlon"
                                                                              options:@{MGLShapeSourceOptionMaximumZoomLevel:@14}];
    source.dataSource = self;
    [self.mapView.style addSource:source];
    MGLLineStyleLayer *lineLayer = [[MGLLineStyleLayer alloc] initWithIdentifier:@"latlonlines"
                                                                          source:source];
    [self.mapView.style addLayer:lineLayer];
    MGLSymbolStyleLayer *labelLayer = [[MGLSymbolStyleLayer alloc] initWithIdentifier:@"latlonlabels"
                                                                               source:source];
    labelLayer.text = [NSExpression expressionForKeyPath:@"value"];
    [self.mapView.style addLayer:labelLayer];
}

- (NSString *)bestLanguageForUser
{
    // https://www.mapbox.com/vector-tiles/mapbox-streets-v8/#name-text--name_lang-code-text
    NSArray *supportedLanguages = @[ @"ar", @"de", @"en", @"es", @"fr", @"ja", @"ko", @"pt", @"ru", @"zh", @"zh-Hans", @"zh-Hant" ];
    NSArray<NSString *> *preferredLanguages = [NSBundle preferredLocalizationsFromArray:supportedLanguages forPreferences:[NSLocale preferredLanguages]];
    NSString *mostSpecificLanguage;

    for (NSString *language in preferredLanguages)
    {
        if (language.length > mostSpecificLanguage.length)
        {
            mostSpecificLanguage = language;
        }
    }

    return mostSpecificLanguage ?: @"en";
}

- (id<MGLAnnotation>)randomOffscreenPointAnnotation {

    NSPredicate *pointAnnotationPredicate = [NSPredicate predicateWithBlock:^BOOL(id  _Nullable evaluatedObject, NSDictionary<NSString *,id> * _Nullable bindings) {
        return [evaluatedObject isKindOfClass:[MGLPointAnnotation class]];
    }];

    NSArray *annotations = [self.mapView.annotations filteredArrayUsingPredicate:pointAnnotationPredicate];

    if (annotations.count == 0) {
        return nil;
    }

    NSArray *visibleAnnotations = [self.mapView.visibleAnnotations filteredArrayUsingPredicate:pointAnnotationPredicate];

    if (visibleAnnotations.count == annotations.count) {
        return nil;
    }

    NSMutableArray *invisibleAnnotations = [annotations mutableCopy];

    if (visibleAnnotations.count > 0) {
        [invisibleAnnotations removeObjectsInArray:visibleAnnotations];
    }

    // Now pick a random offscreen annotation.
    uint32_t index = arc4random_uniform((uint32_t)invisibleAnnotations.count);
    return invisibleAnnotations[index];
}

- (void)selectAnOffscreenPointAnnotation {
    id<MGLAnnotation> annotation = [self randomOffscreenPointAnnotation];
    if (annotation) {
        [self.mapView selectAnnotation:annotation animated:YES];

        NSAssert(self.mapView.selectedAnnotations.firstObject, @"The annotation was not selected");
    }
}

- (void)centerSelectedAnnotation {
    id<MGLAnnotation> annotation = self.mapView.selectedAnnotations.firstObject;

    if (!annotation)
        return;

    CGPoint point = [self.mapView convertCoordinate:annotation.coordinate toPointToView:self.mapView];

    // Animate, so that point becomes the the center
    CLLocationCoordinate2D center = [self.mapView convertPoint:point toCoordinateFromView:self.mapView];
    [self.mapView setCenterCoordinate:center animated:YES];
}

- (void)addVisibleAreaPolyline {
    CGRect constrainedRect = UIEdgeInsetsInsetRect(self.mapView.bounds, self.mapView.contentInset);

    CLLocationCoordinate2D lineCoords[5];

    lineCoords[0] = [self.mapView convertPoint: CGPointMake(CGRectGetMinX(constrainedRect), CGRectGetMinY(constrainedRect)) toCoordinateFromView:self.mapView];
    lineCoords[1] = [self.mapView convertPoint: CGPointMake(CGRectGetMaxX(constrainedRect), CGRectGetMinY(constrainedRect)) toCoordinateFromView:self.mapView];
    lineCoords[2] = [self.mapView convertPoint: CGPointMake(CGRectGetMaxX(constrainedRect), CGRectGetMaxY(constrainedRect)) toCoordinateFromView:self.mapView];
    lineCoords[3] = [self.mapView convertPoint: CGPointMake(CGRectGetMinX(constrainedRect), CGRectGetMaxY(constrainedRect)) toCoordinateFromView:self.mapView];
    lineCoords[4] = lineCoords[0];

    MGLPolyline *line = [MGLPolyline polylineWithCoordinates:lineCoords
                                                       count:sizeof(lineCoords)/sizeof(lineCoords[0])];
    [self.mapView addAnnotation:line];
}

- (void)loadMissingIcon
{
    self.mapView.centerCoordinate = CLLocationCoordinate2DMake(0, 0);
    self.mapView.zoomLevel = 1;
    NSURL *customStyleJSON = [[NSBundle mainBundle] URLForResource:@"missing_icon" withExtension:@"json"];
    [self.mapView setStyleURL:customStyleJSON];
}

- (UIImage *)mapView:(MGLMapView *)mapView didFailToLoadImage:(NSString *)imageName {
    UIImage *backupImage = [UIImage imageNamed:@"AppIcon"];
    return backupImage;
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

#pragma mark - Random World Tour

- (void)addAnnotations:(NSInteger)numAnnotations aroundCoordinate:(CLLocationCoordinate2D)coordinate radius:(CLLocationDistance)radius {
    NSMutableArray *annotations = [[NSMutableArray alloc] initWithCapacity:numAnnotations];
    for (NSInteger i = 0; i<numAnnotations; i++) {

        CLLocationDegrees heading          = drand48()*360.0;
        CLLocationDistance distance        = drand48()*radius;
        CLLocationCoordinate2D newLocation = coordinateCentered(coordinate, heading, distance);

        MBXDroppedPinAnnotation *annotation = [[MBXDroppedPinAnnotation alloc] init];
        annotation.coordinate = newLocation;
        [annotations addObject:annotation];
    }
    [self.mapView addAnnotations:annotations];
}

- (void)randomWorldTour {
    // Consistent initial conditions (consider setting these by test params)
    srand48(0);
    [self.mapView removeAnnotations:self.mapView.annotations];
    [self.mapView setCenterCoordinate:CLLocationCoordinate2DMake(31, -100) zoomLevel:3 animated:NO];

    [self randomWorldTourInternal];
}

- (void)randomWorldTourInternal {

    self.randomWalk = YES;

    // Remove all annotations
    NSTimeInterval duration = 16.0;
    __weak MBXViewController *weakSelf = self;

    // Remove old annotations, half-way through the flight.
    NSArray *annotationsToRemove = [self.mapView.annotations copy];
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(duration * 0.5 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        [weakSelf.mapView removeAnnotations:annotationsToRemove];
    });

    MBXDroppedPinAnnotation *annotation = [[MBXDroppedPinAnnotation alloc] init];
    annotation.coordinate = randomWorldCoordinate();
    [self.mapView addAnnotation:annotation];

    // Add annotations around that coord
    [self addAnnotations:50 aroundCoordinate:annotation.coordinate radius:100000.0]; // 100km

    MGLMapCamera *camera = [MGLMapCamera cameraLookingAtCenterCoordinate:annotation.coordinate
                                                                altitude:10000.0
                                                                   pitch:drand48()*60.0
                                                                 heading:drand48()*360];
    [self.mapView flyToCamera:camera
                 withDuration:duration
                 peakAltitude:2000000.0
            completionHandler:^{
                // This completion handler is currently called BEFORE the
                // region did change delegate method, and we don't have a "reason"
                // so we can't tell if the motion was cancelled. We use the delegate
                // for that, and set self.randomWalk. But since we want a delay
                // anyway, we can just check later. Not ideal though..
                dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(1 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
                    MBXViewController *strongSelf = weakSelf;
                    if (strongSelf.randomWalk) {
                        [strongSelf randomWorldTourInternal];
                    }
                });
            }];
}

- (void)toggleSecondMapView {
    if ([self.view viewWithTag:2] == nil) {
        MGLMapView *secondMapView = [[MGLMapView alloc] initWithFrame:
                                     CGRectMake(0, self.view.bounds.size.height / 2,
                                                self.view.bounds.size.width, self.view.bounds.size.height / 2)];
        secondMapView.showsScale = YES;
        secondMapView.translatesAutoresizingMaskIntoConstraints = NO;
        secondMapView.tag = 2;
        for (NSLayoutConstraint *constraint in self.view.constraints)
        {
            if ((constraint.firstItem  == self.mapView && constraint.firstAttribute  == NSLayoutAttributeBottom) ||
                (constraint.secondItem == self.mapView && constraint.secondAttribute == NSLayoutAttributeBottom))
            {
                [self.view removeConstraint:constraint];
                break;
            }
        }
        [self.view addSubview:secondMapView];
        [self.view addConstraints:@[
            [NSLayoutConstraint constraintWithItem:self.mapView
                                         attribute:NSLayoutAttributeBottom
                                         relatedBy:NSLayoutRelationEqual
                                            toItem:self.view
                                         attribute:NSLayoutAttributeCenterY
                                        multiplier:1
                                          constant:0],
            [NSLayoutConstraint constraintWithItem:secondMapView
                                         attribute:NSLayoutAttributeCenterX
                                         relatedBy:NSLayoutRelationEqual
                                            toItem:self.view
                                         attribute:NSLayoutAttributeCenterX
                                        multiplier:1
                                          constant:0],
            [NSLayoutConstraint constraintWithItem:secondMapView
                                         attribute:NSLayoutAttributeWidth
                                         relatedBy:NSLayoutRelationEqual
                                            toItem:self.view
                                         attribute:NSLayoutAttributeWidth
                                        multiplier:1
                                          constant:0],
            [NSLayoutConstraint constraintWithItem:secondMapView
                                         attribute:NSLayoutAttributeTop
                                         relatedBy:NSLayoutRelationEqual
                                            toItem:self.view
                                         attribute:NSLayoutAttributeCenterY
                                        multiplier:1
                                          constant:0],
            [NSLayoutConstraint constraintWithItem:secondMapView
                                         attribute:NSLayoutAttributeBottom
                                         relatedBy:NSLayoutRelationEqual
                                            toItem:self.bottomLayoutGuide
                                         attribute:NSLayoutAttributeBottom
                                        multiplier:1
                                          constant:0],
        ]];
    } else {
        NSMutableArray *constraintsToRemove = [NSMutableArray array];
        MGLMapView *secondMapView = (MGLMapView *)[self.view viewWithTag:2];
        for (NSLayoutConstraint *constraint in self.view.constraints)
        {
            if (constraint.firstItem == secondMapView || constraint.secondItem == secondMapView)
            {
                [constraintsToRemove addObject:constraint];
            }
        }
        [self.view removeConstraints:constraintsToRemove];
        [secondMapView removeFromSuperview];
        [self.view addConstraint:[NSLayoutConstraint constraintWithItem:self.mapView
                                                              attribute:NSLayoutAttributeBottom
                                                              relatedBy:NSLayoutRelationEqual
                                                                 toItem:self.bottomLayoutGuide
                                                              attribute:NSLayoutAttributeTop
                                                             multiplier:1
                                                               constant:0]];
    }
}

#pragma mark - User Actions

- (void)handleSingleTap:(UITapGestureRecognizer *)singleTap {
    [self.navigationController setNavigationBarHidden:!self.navigationController.navigationBarHidden animated:YES];

    // This is how you'd get the coordinate for the point where the user tapped:
    //    CGPoint tapPoint = [singleTap locationInView:self.mapView];
    //    CLLocationCoordinate2D tapCoordinate = [self.mapView convertPoint:tapPoint toCoordinateFromView:nil];
}

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


        // Calling `addAnnotation:` on mapView is required here (since `selectAnnotation:animated` has
        // the side effect of adding the annotation if required, but returning an incorrect callout
        // positioning rect)

        [self.mapView addAnnotation:pin];
        [self.mapView selectAnnotation:pin animated:YES];
    }
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

#pragma mark - MGLMapViewDelegate

- (MGLAnnotationView *)mapView:(MGLMapView *)mapView viewForAnnotation:(id<MGLAnnotation>)annotation
{
    if (annotation == mapView.userLocation)
    {
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
        // views (`self.view` & `annotationView`) will cause weird behavior.
        // Comment out the pin dropping functionality in the handleLongPress:
        // method in this class to make draggable annotation views play nice.
        annotationView.draggable = YES;
    } else {
        // orange indicates that the annotation view was reused
        annotationView.backgroundColor = [UIColor orangeColor];
    }
    return annotationView;
}

- (MGLAnnotationImage *)mapView:(MGLMapView * __nonnull)mapView imageForAnnotation:(id <MGLAnnotation> __nonnull)annotation
{
    if ([annotation isKindOfClass:[MBXDroppedPinAnnotation class]])
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

- (CGFloat)mapView:(__unused MGLMapView *)mapView alphaForShapeAnnotation:(MGLShape *)annotation
{
    return ([annotation isKindOfClass:[MGLPolygon class]] ? 0.5 : 1.0);
}

- (UIColor *)mapView:(__unused MGLMapView *)mapView strokeColorForShapeAnnotation:(MGLShape *)annotation
{
    UIColor *color = [annotation isKindOfClass:[MGLPolyline class]] ? [UIColor greenColor] : [UIColor blackColor];
    return [color colorWithAlphaComponent:0.9];
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

    MGLPointAnnotation *point = (MGLPointAnnotation *)annotation;
    point.coordinate = [self.mapView convertPoint:self.mapView.center toCoordinateFromView:self.mapView];
}

- (void)mapView:(MGLMapView *)mapView didFinishLoadingStyle:(MGLStyle *)style
{
    // Default Mapbox styles use {name_en} as their label language, which means
    // that a device with an English-language locale is already effectively
    // using locale-based country labels.
    _localizingLabels = [[self bestLanguageForUser] isEqualToString:@"en"];
}

- (void)mapViewRegionIsChanging:(MGLMapView *)mapView
{
    [self updateHUD];
    [self updateHelperMapViews];
}

- (void)mapView:(MGLMapView *)mapView regionDidChangeWithReason:(MGLCameraChangeReason)reason animated:(BOOL)animated
{
    if (reason != MGLCameraChangeReasonProgrammatic) {
        self.randomWalk = NO;
    }

    [self updateHUD];
    [self updateHelperMapViews];
}

- (void)mapView:(MGLMapView *)mapView didUpdateUserLocation:(MGLUserLocation *)userLocation {
    [self updateHUD];
}

- (void)updateHelperMapViews {
    for (UIWindow *window in self.helperWindows) {
        MGLMapView *mapView = (MGLMapView *)window.rootViewController.view;
        mapView.camera = self.mapView.camera;
    }
}

- (void)updateHUD {
    if (!self.reuseQueueStatsEnabled && !self.currentMapState.showsZoomLevelOrnament == YES) return;

    if (self.mapInfoLabel.hidden) self.mapInfoLabel.hidden = NO;

    NSString *mapInfoString;

    if (self.reuseQueueStatsEnabled) {
        NSUInteger queuedAnnotations = 0;
        for (NSArray *queue in self.mapView.annotationViewReuseQueueByIdentifier.allValues) {
            queuedAnnotations += queue.count;
        }
        mapInfoString = [NSString stringWithFormat:@"Visible: %ld  Queued: %ld", (unsigned long)self.mapView.visibleAnnotations.count, (unsigned long)queuedAnnotations];
    } else if (self.currentMapState.showsZoomLevelOrnament == YES) {
        mapInfoString = [NSString stringWithFormat:@"%.f FPS (%.1fms) ∕ %.2f ∕ ↕\U0000FE0E%.f° ∕ %.f°",
                     roundf(self.mapView.averageFrameRate), self.mapView.averageFrameTime,
                     self.mapView.zoomLevel, self.mapView.camera.pitch, self.mapView.direction];
    }

    [self.mapInfoLabel setTitle:mapInfoString forState:UIControlStateNormal];
}

#pragma mark - MGLComputedShapeSourceDataSource
// TODO: Move to ios-sdk-examples
- (NSArray<id <MGLFeature>>*)featuresInCoordinateBounds:(MGLCoordinateBounds)bounds zoomLevel:(NSUInteger)zoom {
    double gridSpacing;
    if(zoom >= 13) {
        gridSpacing = 0.01;
    } else if(zoom >= 11) {
        gridSpacing = 0.05;
    } else if(zoom == 10) {
        gridSpacing = .1;
    } else if(zoom == 9) {
        gridSpacing = 0.25;
    } else if(zoom == 8) {
        gridSpacing = 0.5;
    } else if (zoom >= 6) {
        gridSpacing = 1;
    } else if(zoom == 5) {
        gridSpacing = 2;
    } else if(zoom >= 4) {
        gridSpacing = 5;
    } else if(zoom == 2) {
        gridSpacing = 10;
    } else {
        gridSpacing = 20;
    }

    NSMutableArray <id <MGLFeature>> * features = [NSMutableArray array];
    CLLocationCoordinate2D coords[2];

    for (double y = ceil(bounds.ne.latitude / gridSpacing) * gridSpacing; y >= floor(bounds.sw.latitude / gridSpacing) * gridSpacing; y -= gridSpacing) {
        coords[0] = CLLocationCoordinate2DMake(y, bounds.sw.longitude);
        coords[1] = CLLocationCoordinate2DMake(y, bounds.ne.longitude);
        MGLPolylineFeature *feature = [MGLPolylineFeature polylineWithCoordinates:coords count:2];
        feature.attributes = @{@"value": @(y)};
        [features addObject:feature];
    }

    for (double x = floor(bounds.sw.longitude / gridSpacing) * gridSpacing; x <= ceil(bounds.ne.longitude / gridSpacing) * gridSpacing; x += gridSpacing) {
        coords[0] = CLLocationCoordinate2DMake(bounds.sw.latitude, x);
        coords[1] = CLLocationCoordinate2DMake(bounds.ne.latitude, x);
        MGLPolylineFeature *feature = [MGLPolylineFeature polylineWithCoordinates:coords count:2];
        feature.attributes = @{@"value": @(x)};
        [features addObject:feature];
    }

    return features;
}

- (void)mapViewDidFinishRenderingFrame:(MGLMapView *)mapView fullyRendered:(BOOL)fullyRendered {
    if (self.currentMapState.showsTimeFrameGraph) {
        [self.frameTimeGraphView updatePathWithFrameDuration:mapView.frameTime];
    }
}

@end
