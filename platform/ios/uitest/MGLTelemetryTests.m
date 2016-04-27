#import "MGLUITests.h"

@interface MGLTelemetryTests : XCTestCase

@end

@implementation MGLTelemetryTests

- (void)setUp {
    [super setUp];
    
    self.continueAfterFailure = NO;
    [XCUIDevice sharedDevice].orientation = UIDeviceOrientationPortrait;
    XCUIApplication *app = [[XCUIApplication alloc] init];
    
    // Bypass the access token prompt.
    NSMutableDictionary <NSString *, NSString *> *environment = app.launchEnvironment.mutableCopy;
    environment[@"MAPBOX_ACCESS_TOKEN"] = MGLUITestsBogusAccessToken;
    app.launchEnvironment = environment;
    
    [app launch];
}

- (void)testOptOut {
    XCUIApplication *app = [[XCUIApplication alloc] init];
    XCUIElement *mapElement = app.otherElements[@"MGLMapView"];
    XCUIElement *infoButton = mapElement.buttons[@"MGLMapViewInfo"];
    [infoButton tap];
    
    XCUIElement *telemetryButton = app.sheets[@"Mapbox iOS SDK"].collectionViews.buttons[@"Mapbox Telemetry"];
    [telemetryButton tap];
    
    XCUIElementQuery *collectionViewsQuery = app.alerts[@"Make Mapbox Maps Better"].collectionViews;
    [collectionViewsQuery.buttons[@"Keep Participating"] tap];
    [infoButton tap];
    [telemetryButton tap];
    [collectionViewsQuery.buttons[@"Stop Participating"] tap];
    
    [infoButton tap];
    [telemetryButton tap];
    [collectionViewsQuery.buttons[@"Don’t Participate"] tap];
    [infoButton tap];
    [telemetryButton tap];
    [collectionViewsQuery.buttons[@"Participate"] tap];
}

@end
