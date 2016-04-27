#import "MGLUITestCase.h"

@interface MGLAnnotationTests : MGLUITestCase

@end

@implementation MGLAnnotationTests

- (void)testDropPin {
    XCUIElement *mapElement = [[XCUIApplication alloc] init].otherElements[@"MGLMapView"];
    NSPredicate *exists = [NSPredicate predicateWithFormat:@"exists == YES"];
    NSPredicate *isAbsent = [NSPredicate predicateWithFormat:@"exists == NO"];
    
    // Drop a pin.
    XCUIElement *mapProxyElement = mapElement.buttons[@"MGLMapViewProxyAccessibilityElement"];
    XCTAssertFalse(mapElement.buttons[@"MGLMapViewProxyAccessibilityElement"].exists, @"Map proxy element should be absent until opening callout view.");
    [self expectationForPredicate:exists evaluatedWithObject:mapProxyElement handler:nil];
    [mapElement pressForDuration:1.1];
    [self waitForExpectationsWithTimeout:2 handler:nil];
    
    // Inspect the callout view.
    NSString *annotationTitle = @"Dropped Pin";
    XCUIElement *calloutTitleText = mapElement.staticTexts[annotationTitle];
    XCTAssertTrue(calloutTitleText.exists, @"Callout title should be present after opening callout.");
    NSString *annotationSubtitle = @"17°0′56″ south, 0° west";
    XCUIElement *calloutSubtitleText = mapElement.staticTexts[annotationSubtitle];
    XCTAssertTrue(calloutSubtitleText.exists, @"Callout subtitle should be present after opening callout.");
    XCUIElement *calloutLeftAccessoryButton = mapElement.buttons[@"Left"];
    XCTAssertTrue(calloutLeftAccessoryButton.exists, @"Callout left accessory button should be present after opening callout.");
    XCUIElement *calloutRightAccessoryButton = mapElement.buttons[@"Right"];
    XCTAssertTrue(calloutRightAccessoryButton.exists, @"Callout right accessory button should be present after opening callout.");
    
    // Close the callout view by tapping on the map proxy element. Note that the map proxy element has a gaping hole in the middle to accommodate the callout view.
    [self expectationForPredicate:isAbsent evaluatedWithObject:mapProxyElement handler:nil];
    XCUICoordinate *coordinate = [[mapProxyElement coordinateWithNormalizedOffset:CGVectorMake(0, 0)] coordinateWithOffset:CGVectorMake(100, 100)];
    [coordinate tap];
    [self waitForExpectationsWithTimeout:2 handler:nil];
    
    XCTAssertFalse(calloutTitleText.exists, @"Callout title should be absent after closing callout.");
    XCTAssertFalse(calloutSubtitleText.exists, @"Callout subtitle should be absent after closing callout.");
    XCTAssertFalse(calloutLeftAccessoryButton.exists, @"Callout left accessory button should be absent after closing callout.");
    XCTAssertFalse(calloutRightAccessoryButton.exists, @"Callout right accessory button should be absent after closing callout.");
    
    // Inspect the annotation itself.
    XCUIElement *annotation = mapElement.buttons[@"MGLMapViewAnnotation 0"];
    XCTAssertTrue(annotation.exists, @"Annotation accessibility element should be present after closing callout view.");
    XCTAssertEqualObjects(annotation.label, annotationTitle, @"Annotation label should be title.");
    XCTAssertEqualObjects(annotation.value, annotationSubtitle, @"Annotation value should be subtitle.");
}

- (void)testFrameWithGestures {
    XCUIElement *mapElement = [[XCUIApplication alloc] init].otherElements[@"MGLMapView"];
    NSPredicate *exists = [NSPredicate predicateWithFormat:@"exists == YES"];
    NSPredicate *isAbsent = [NSPredicate predicateWithFormat:@"exists == NO"];
    
    // Drop a pin.
    XCUIElement *mapProxyElement = mapElement.buttons[@"MGLMapViewProxyAccessibilityElement"];
    [self expectationForPredicate:exists evaluatedWithObject:mapProxyElement handler:nil];
    [mapElement pressForDuration:1.1];
    [self waitForExpectationsWithTimeout:1 handler:nil];
    
    // Close the callout view.
    [self expectationForPredicate:isAbsent evaluatedWithObject:mapProxyElement handler:nil];
    XCUICoordinate *coordinate = [[mapProxyElement coordinateWithNormalizedOffset:CGVectorMake(0, 0)] coordinateWithOffset:CGVectorMake(100, 100)];
    [coordinate tap];
    [self waitForExpectationsWithTimeout:2 handler:nil];
    
    XCUIElement *annotation = mapElement.buttons[@"MGLMapViewAnnotation 0"];
    CGRect frame = annotation.frame;
    
    // Make sure the annotation stays in place when zooming in.
    [mapElement doubleTap];
    CGRect frameAfterZoomingIn = annotation.frame;
    XCTAssertEqualWithAccuracy(frame.origin.x, frameAfterZoomingIn.origin.x, 0.1, @"Annotation moved after zooming in.");
    XCTAssertEqualWithAccuracy(frame.origin.y, frameAfterZoomingIn.origin.y, 0.1, @"Annotation moved after zooming in.");
    XCTAssertEqualWithAccuracy(frame.size.width, frameAfterZoomingIn.size.width, 0.1, @"Annotation resized after zooming in.");
    XCTAssertEqualWithAccuracy(frame.size.height, frameAfterZoomingIn.size.height, 0.1, @"Annotation resized after zooming in.");
    
    // Make sure the annotation moves after panning.
    CGVector offset = CGVectorMake(50, 0);
    XCUICoordinate *startCoordinate = [[mapElement coordinateWithNormalizedOffset:CGVectorMake(0, 0)]
                                       coordinateWithOffset:CGVectorMake(100, 100)];
    XCUICoordinate *endCoordinate = [startCoordinate coordinateWithOffset:offset];
    [startCoordinate pressForDuration:0 thenDragToCoordinate:endCoordinate];
    [mapElement tap];
    CGRect frameAfterPanning = annotation.frame;
    XCTAssertLessThanOrEqual(frame.origin.x + offset.dx, frameAfterPanning.origin.x, @"Annotation moved after panning right.");
    XCTAssertEqualWithAccuracy(frame.origin.y + offset.dy, frameAfterPanning.origin.y, 0.1, @"Annotation moved after panning right.");
    XCTAssertEqualWithAccuracy(frame.size.width, frameAfterPanning.size.width, 0.1, @"Annotation resized after panning right.");
    XCTAssertEqualWithAccuracy(frame.size.height, frameAfterPanning.size.height, 0.1, @"Annotation resized after panning right.");
}

- (void)testRemoveAnnotations {
    XCUIApplication *app = [[XCUIApplication alloc] init];
    NSPredicate *exists = [NSPredicate predicateWithFormat:@"exists == YES"];
    NSPredicate *isAbsent = [NSPredicate predicateWithFormat:@"exists == NO"];
    
    // Drop a pin.
    XCUIElement *mapElement = app.otherElements[@"MGLMapView"];
    XCUIElement *mapProxyElement = mapElement.buttons[@"MGLMapViewProxyAccessibilityElement"];
    [self expectationForPredicate:exists evaluatedWithObject:mapProxyElement handler:nil];
    [mapElement pressForDuration:1.1];
    [self waitForExpectationsWithTimeout:2 handler:nil];
    
    // Remove all annotations, closing the callout view.
    [app.navigationBars[@"MBXNavigationBar"].buttons[@"MBXSettingsButton"] tap];
    [app.sheets[@"Map Settings"] swipeUp];
    [self expectationForPredicate:isAbsent evaluatedWithObject:mapProxyElement handler:nil];
    [app.sheets[@"Map Settings"].collectionViews.buttons[@"Remove Annotations"] tap];
    [self waitForExpectationsWithTimeout:2 handler:nil];
    
    XCTAssertFalse(mapElement.buttons[@"MGLMapViewAnnotation 0"].exists, @"Annotation should be gone after removing all annotations.");
}

- (void)testAddHundredPoints {
    XCUIApplication *app = [[XCUIApplication alloc] init];
    NSPredicate *exists = [NSPredicate predicateWithFormat:@"exists == YES"];
    [app.navigationBars[@"MBXNavigationBar"].buttons[@"MBXSettingsButton"] tap];
    
    // Add 100 points.
    XCUIElement *mapElement = app.otherElements[@"MGLMapView"];
    XCUIElement *lastAnnotationElement = mapElement.buttons[@"MGLMapViewAnnotation 99"];
    [self expectationForPredicate:exists evaluatedWithObject:lastAnnotationElement handler:nil];
    [app.sheets[@"Map Settings"].collectionViews.buttons[@"Add 100 Points"] tap];
    [self waitForExpectationsWithTimeout:2 handler:nil];
    
    XCTAssertGreaterThanOrEqual(mapElement.buttons.count, 100, @"Adding 100 points should leave at least 100 buttons visible within the map.");
}

- (void)testTourWorld {
    XCUIApplication *app = [[XCUIApplication alloc] init];
    NSPredicate *exists = [NSPredicate predicateWithFormat:@"exists == YES"];
    
    XCUIElement *settingsSheet = app.sheets[@"Map Settings"];
    [self expectationForPredicate:exists evaluatedWithObject:settingsSheet handler:nil];
    [app.navigationBars[@"MBXNavigationBar"].buttons[@"MBXSettingsButton"] tap];
    [self waitForExpectationsWithTimeout:1 handler:nil];
    
    // Start the world tour.
    XCUIElement *mapElement = app.otherElements[@"MGLMapView"];
    [self expectationForPredicate:exists evaluatedWithObject:mapElement.buttons[@"Washington, D.C."] handler:nil];
    [settingsSheet.collectionViews.buttons[@"Start World Tour"] tap];
    [self waitForExpectationsWithTimeout:2 handler:nil];
    [self expectationForPredicate:exists evaluatedWithObject:mapElement.buttons[@"San Francisco"] handler:nil];
    [self waitForExpectationsWithTimeout:15 handler:nil];
    [self expectationForPredicate:exists evaluatedWithObject:mapElement.buttons[@"Bangalore"] handler:nil];
    [self waitForExpectationsWithTimeout:20 handler:nil];
    [self expectationForPredicate:exists evaluatedWithObject:mapElement.buttons[@"Ayacucho"] handler:nil];
    [self waitForExpectationsWithTimeout:20 handler:nil];
}

@end
