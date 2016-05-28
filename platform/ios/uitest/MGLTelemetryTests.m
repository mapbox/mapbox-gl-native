#import "MGLUITestCase.h"

@interface MGLTelemetryTests : MGLUITestCase

@end

@implementation MGLTelemetryTests

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
