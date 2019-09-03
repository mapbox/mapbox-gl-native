#import "MGLMapViewIntegrationTest.h"
#import "MGLTestUtility.h"
#import "MGLMapAccessibilityElement.h"
#import "MGLTestLocationManager.h"
#import "MGLCompactCalloutView.h"

#import "MGLGeometry_Private.h"
#import "MGLMapView_Private.h"

#include <mbgl/util/geo.hpp>
#include <mbgl/map/camera.hpp>
#include <mbgl/map/map.hpp>

@interface MGLTestCalloutView : MGLCompactCalloutView
@property (nonatomic) BOOL implementsMarginHints;
@end

@implementation MGLTestCalloutView
- (BOOL)respondsToSelector:(SEL)aSelector {
    if (!self.implementsMarginHints &&
        (aSelector == @selector(marginInsetsHintForPresentationFromRect:))) {
        return NO;
    }
    return [super respondsToSelector:aSelector];
}
@end


@interface MGLMapView (Tests)
- (MGLAnnotationTag)annotationTagAtPoint:(CGPoint)point persistingResults:(BOOL)persist;
- (id <MGLAnnotation>)annotationWithTag:(MGLAnnotationTag)tag;
- (MGLMapCamera *)cameraByRotatingToDirection:(CLLocationDirection)degrees aroundAnchorPoint:(CGPoint)anchorPoint;
- (MGLMapCamera *)cameraByZoomingToZoomLevel:(double)zoom aroundAnchorPoint:(CGPoint)anchorPoint;
- (MGLMapCamera *)cameraForCameraOptions:(const mbgl::CameraOptions &)cameraOptions;
@property (nonatomic) UIView<MGLCalloutView> *calloutViewForSelectedAnnotation;
@end

@interface MGLAnnotationViewIntegrationTests : MGLMapViewIntegrationTest
@end

@implementation MGLAnnotationViewIntegrationTests

#pragma mark - Offscreen/panning selection tests

typedef struct PanTestData {
    CGPoint relativeCoord;
    BOOL showsCallout;
    BOOL implementsMargins;
    BOOL moveIntoView;
    BOOL expectMapToHavePanned;
    BOOL calloutOnScreen;
} PanTestData;

#define PAN_TEST_TERMINATOR {{FLT_MAX, FLT_MAX}, NO, NO, NO, NO, NO}
static const CGPoint kAnnotationRelativeScale = { 0.05f, 0.125f };

- (void)internalTestOffscreenSelectionTitle:(NSString*)title withTestData:(PanTestData)test animateSelection:(BOOL)animateSelection {
    
    CGPoint relativeCoordinate          = test.relativeCoord;
    BOOL showsCallout                   = test.showsCallout;
    BOOL calloutImplementsMarginHints   = test.implementsMargins;
    BOOL moveIntoView                   = test.moveIntoView;
    BOOL expectMapToHavePanned          = test.expectMapToHavePanned;
    BOOL expectCalloutToBeFullyOnscreen = test.calloutOnScreen;
    
    // Reset the map to a consistent state - want the map to be zoomed in, so that
    // it's free to be panned without hitting boundaries.
    [self.mapView setCenterCoordinate:CLLocationCoordinate2DMake(0, 0) zoomLevel:14 animated:NO];
    [self waitForMapViewToBeRenderedWithTimeout:1.0];
    
    XCTAssert(self.mapView.annotations.count == 0);
    
    NSString * const MGLTestAnnotationReuseIdentifer = @"MGLTestAnnotationReuseIdentifer";
    CGSize size = self.mapView.bounds.size;
    CGSize annotationSize = CGSizeMake(floor(size.width*kAnnotationRelativeScale.x), floor(size.height*kAnnotationRelativeScale.y));
    
    self.viewForAnnotation = ^MGLAnnotationView*(MGLMapView *view, id<MGLAnnotation> annotation) {
        
        if (![annotation isKindOfClass:[MGLPointAnnotation class]]) {
            return nil;
        }
        
        // No dequeue
        MGLAnnotationView *annotationView = [[MGLAnnotationView alloc] initWithAnnotation:annotation reuseIdentifier:MGLTestAnnotationReuseIdentifer];
        annotationView.bounds             = (CGRect){ .origin = CGPointZero, .size = annotationSize };
        annotationView.backgroundColor    = UIColor.redColor;
        annotationView.enabled            = YES;
        
        return annotationView;
    };
    
    // Coordinate for annotation screen coordinate
    CGPoint annotationPoint = CGPointMake(floor(relativeCoordinate.x * size.width), floor(relativeCoordinate.y * size.height)   );
    CLLocationCoordinate2D coordinate = [self.mapView convertPoint:annotationPoint toCoordinateFromView:self.mapView];
    
    MGLPointAnnotation *point = [[MGLPointAnnotation alloc] init];
    point.title = title;
    point.coordinate = coordinate;
    
    self.mapViewAnnotationCanShowCalloutForAnnotation = ^BOOL(MGLMapView *mapView, id<MGLAnnotation> annotation) {
        return showsCallout;
    };
    
    self.mapViewCalloutViewForAnnotation = ^id<MGLCalloutView>(MGLMapView *mapView, id<MGLAnnotation> annotation) {
        if (!showsCallout)
            return nil;
        
        MGLTestCalloutView *calloutView = [[MGLTestCalloutView alloc] init];
        calloutView.representedObject = annotation;
        calloutView.implementsMarginHints = calloutImplementsMarginHints;
        return calloutView;
    };
    
    [self.mapView addAnnotation:point];

    // Check assumptions before selection
    UIView *annotationViewBeforeSelection =  [self.mapView viewForAnnotation:point];
    XCTAssertNotNil(annotationViewBeforeSelection);

    CLLocationCoordinate2D mapCenterBeforeSelection = self.mapView.centerCoordinate;
    XCTAssert(CLLocationCoordinate2DIsValid(mapCenterBeforeSelection));

    // Also note, that at this point, the internal mechanism that determines if
    // an annotation view is offscreen and should be put back in the reuse queue
    // will have run, and `viewForAnnotation` may return nil

    XCTestExpectation *selectionCompleted = [self expectationWithDescription:@"Selection completed"];
    [self.mapView selectAnnotation:point moveIntoView:moveIntoView animateSelection:animateSelection completionHandler:^{
        [selectionCompleted fulfill];
    }];

    // Animated selection takes MGLAnimationDuration (0.3 seconds), so wait a little
    // longer. We don't need to wait as long if we're not animated (but we do
    // want the runloop to tick over)
    [self waitForExpectations:@[selectionCompleted] timeout:animateSelection ? 0.4: 0.05];

    UIView *annotationViewAfterSelection =  [self.mapView viewForAnnotation:point];
    CLLocationCoordinate2D mapCenterAfterSelection = self.mapView.centerCoordinate;
    XCTAssert(CLLocationCoordinate2DIsValid(mapCenterAfterSelection));

    // If the annotation is still "offscreen" at this point, then the above annotation view
    // may be nil, which is expected.
    BOOL (^CGRectContainsRectWithAccuracy)(CGRect, CGRect, CGFloat) = ^(CGRect rect1, CGRect rect2, CGFloat accuracy) {
        CGRect expandedRect1 = CGRectInset(rect1, -accuracy, -accuracy);
        return (BOOL)CGRectContainsRect(expandedRect1, rect2);
    };
    
    CGFloat epsilon = 0.00001;
    if (expectMapToHavePanned) {
        CLLocationDegrees latitudeDelta = fabs(mapCenterAfterSelection.latitude - mapCenterBeforeSelection.latitude);
        CLLocationDegrees longitudeDelta = fabs(mapCenterAfterSelection.longitude - mapCenterBeforeSelection.longitude);
        
        XCTAssert( (latitudeDelta > epsilon) || (longitudeDelta > epsilon), @"Deltas: lat=%f, long=%f", latitudeDelta, longitudeDelta); // One of them should have moved

        // If the map panned - the intention is that the annotation is on-screen,
        // and so should have an annotation view and that it is fully on screen
        CGRect annotationFrameAfterSelection = annotationViewAfterSelection.frame;

        XCTAssertNotNil(annotationViewAfterSelection);
        
        XCTAssert(CGRectContainsRectWithAccuracy(self.mapView.bounds, annotationFrameAfterSelection, 0.25), @"Mapview:%@ frame:%@", NSStringFromCGRect(self.mapView.bounds), NSStringFromCGRect(annotationFrameAfterSelection));
        
        // Check the callout
        if (showsCallout) {
            UIView *calloutView = self.mapView.calloutViewForSelectedAnnotation;
            XCTAssertNotNil(calloutView);
            
            // This can fail if the callout view's width is < the annotations. This is really a warning, so
            // if you need this NOT to fail the tests, consider replacing with MGLTestWarning
            XCTAssert(expectCalloutToBeFullyOnscreen == CGRectContainsRectWithAccuracy(self.mapView.bounds, calloutView.frame, 0.25),
                      @"Expect contains:%d, Mapview:%@ annotation:%@ callout:%@",
                      expectCalloutToBeFullyOnscreen,
                      NSStringFromCGRect(self.mapView.bounds),
                      NSStringFromCGRect(annotationFrameAfterSelection),
                      NSStringFromCGRect(calloutView.frame));
        }
    }
    else {
        // The map shouldn't have moved, so use equality (rather than an error check)
        XCTAssertEqual(mapCenterBeforeSelection.latitude, mapCenterAfterSelection.latitude);
        XCTAssertEqual(mapCenterBeforeSelection.longitude, mapCenterAfterSelection.longitude);
        
        // Annotation shouldn't have moved
        CGPoint annotationPoint2 = [self.mapView convertCoordinate:point.coordinate toPointToView:self.mapView];
        CGFloat xDelta = fabs(annotationPoint2.x - annotationPoint.x);
        CGFloat yDelta = fabs(annotationPoint2.y - annotationPoint.y);
        
        XCTAssert((xDelta < epsilon) && (yDelta < epsilon));
        
        if (showsCallout) {
            UIView *calloutView = self.mapView.calloutViewForSelectedAnnotation;
            
            if (annotationViewAfterSelection) {
                XCTAssertNotNil(calloutView);

                // If kAnnotationScale == 0.25, then the following assert can fail.
                // This is really a warning (see https://github.com/mapbox/mapbox-gl-native/issues/13744 )
                // If you need this NOT to fail the tests, consider replacing with MGLTestWarning
                XCTAssert((expectCalloutToBeFullyOnscreen == CGRectContainsRectWithAccuracy(self.mapView.bounds, calloutView.frame, 0.25)),
                          @"Mapview:%@ annotation:%@ callout:%@",
                          NSStringFromCGRect(self.mapView.bounds),
                          NSStringFromCGRect(annotationViewAfterSelection.frame),
                          NSStringFromCGRect(calloutView.frame));
            }
            else {
                // If there's no annotation view, should we expect a callout?
                XCTAssertNil(calloutView);
                XCTAssertFalse(expectCalloutToBeFullyOnscreen);
            }
        }
    }
   
    // Remove the annotation
    [self.mapView removeAnnotation:point];
    
    XCTAssert(self.mapView.annotations.count == 0);
}

// See https://github.com/mapbox/mapbox-gl-native/pull/13727#issuecomment-454028698
// What follows are tests based on this table.
// This is not a full-set of possible combinations, just the most important/likely
// ones
- (void)internalRunTests:(PanTestData*)testData
{
    // Test both animated and not-animated.
    for (int i = 0; i<2; i++) {
        int row = 0;
        PanTestData *test = testData;
        while (test->relativeCoord.x != FLT_MAX) {
            NSString *activityTitle = [NSString stringWithFormat:@"Row %d/%d", row, i];
            [XCTContext runActivityNamed:activityTitle
                                   block:^(id<XCTActivity>  _Nonnull activity) {
                                       [self internalTestOffscreenSelectionTitle:activityTitle
                                                                    withTestData:*test
                                                                animateSelection:!i];
                                   }];
            ++test;
            ++row;
        }
    }
}

- (void)testBasicSelection  {
    // Tests moveIntoView:NO
    // WITHOUT a callout
    
    PanTestData tests[] = {
        //  Coord           showsCallout    impl margins?   moveIntoView    expectMapToPan  calloutOnScreen
        //  Offscreen
        {   {-1.0f, 0.5f},  NO,             NO,             NO,             NO,             NO },
        {   { 2.0f, 0.5f},  NO,             NO,             NO,             NO,             NO },
        {   { 0.5f,-1.0f},  NO,             NO,             NO,             NO,             NO },
        {   { 0.5f, 2.0f},  NO,             NO,             NO,             NO,             NO },
        
        //  Partial
        {   { 0.0f, 0.5f},  NO,             NO,             NO,             NO,             NO },
        {   { 1.0f, 0.5f},  NO,             NO,             NO,             NO,             NO },
        {   { 0.5f, 0.0f},  NO,             NO,             NO,             NO,             NO },
        {   { 0.5f, 1.0f},  NO,             NO,             NO,             NO,             NO },
        
        //  Onscreen
        {   { 0.5f, 0.5f},  NO,             NO,             NO,             NO,             NO },
        
        PAN_TEST_TERMINATOR
    };
    
    [self internalRunTests:tests];
}

- (void)testBasicSelectionWithCallout  {
    // Tests moveIntoView:NO
    // WITH the default callout (implements marginshint)
    
    PanTestData tests[] = {
        //  Coord           showsCallout    impl margins?   moveIntoView    expectMapToPan  calloutOnScreen
        {   {-1.0f, 0.5f},  YES,            YES,            NO,             NO,             NO },
        {   { 0.0f, 0.5f},  YES,            YES,            NO,             NO,             NO },
        {   { 0.5f, 1.0f},  YES,            YES,            NO,             NO,             YES }, // Because annotation was off the bottom of screen, and callout is above annotation
        {   { 0.5f, 0.5f},  YES,            YES,            NO,             NO,             YES },
        
        PAN_TEST_TERMINATOR
    };
    
    [self internalRunTests:tests];
}

- (void)testSelectionMoveIntoView  {
    // Tests moveIntoView:YES
    // without a callout
    
    // From https://github.com/mapbox/mapbox-gl-native/pull/13727#issuecomment-454028698
    //
    // | Annotation position | Has callout? | Callout implements `marginInsets...`? | Map pans when selected with moveIntoView=YES? |
    // |---------------------|--------------|---------------------------------------|-----------------------------------------------|
    // | Offscreen           | No           | n/a                                   | Yes (no margins)                              |
    // | Partially           | No           | n/a                                   | No                                            |
    // | Onscreen            | No           | n/a                                   | No                                            |
    //
    
    PanTestData tests[] = {
        //  Coord           showsCallout    impl margins?   moveIntoView     expectMapToPan  calloutOnScreen
        //  Offscreen
        {   {-1.0f, 0.5f},  NO,             NO,             YES,            YES,            NO },
        {   { 2.0f, 0.5f},  NO,             NO,             YES,            YES,            NO },
        {   { 0.5f,-1.0f},  NO,             NO,             YES,            YES,            NO },
        {   { 0.5f, 2.0f},  NO,             NO,             YES,            YES,            NO },
        
        //  Partial
        {   { 0.0f, 0.5f},  NO,             NO,             YES,            NO,             NO },
        {   { 1.0f, 0.5f},  NO,             NO,             YES,            NO,             NO },
        {   { 0.5f, 0.0f},  NO,             NO,             YES,            NO,             NO },
        {   { 0.5f, 1.0f},  NO,             NO,             YES,            NO,             NO },
        
        //  Onscreen
        {   { 0.5f, 0.5f},  NO,             NO,             YES,            NO,             NO },
        
        PAN_TEST_TERMINATOR
    };
    
    [self internalRunTests:tests];
}

- (void)testSelectionMoveIntoViewWithCallout  {
    // Tests moveIntoView:YES
    // WITH the default callout (implements marginshint)
    
    // From https://github.com/mapbox/mapbox-gl-native/pull/13727#issuecomment-454028698
    //
    // | Annotation position | Has callout? | Callout implements `marginInsets...`? | Map pans when selected with moveIntoView=YES? |
    // |---------------------|--------------|---------------------------------------|-----------------------------------------------|
    // | Offscreen           | Yes          | Yes                                   | Yes to ensure callout is fully visible        |
    // | Partially           | Yes          | Yes                                   | Yes to ensure callout is fully visible        |
    // | Onscreen            | Yes          | Yes                                   | Yes, but *only* to ensure callout is fully visible |
    //

    CGFloat offset = kAnnotationRelativeScale.x * 0.5f;
    
    PanTestData tests[] = {
        //  Coord           showsCallout    impl margins?   moveIntoView    expectMapToPan  calloutOnScreen
        //  Offscreen
        {   {-1.0f, 0.5f},  YES,            YES,            YES,            YES,            YES },
        {   { 2.0f, 0.5f},  YES,            YES,            YES,            YES,            YES },
        {   { 0.5f,-1.0f},  YES,            YES,            YES,            YES,            YES },
        {   { 0.5f, 2.0f},  YES,            YES,            YES,            YES,            YES },
        
        //  Partial
        {   { 0.0f, 0.5f},  YES,            YES,            YES,            YES,            YES },
        {   { 1.0f, 0.5f},  YES,            YES,            YES,            YES,            YES },
        {   { 0.5f, 0.0f},  YES,            YES,            YES,            YES,            YES },
        {   { 0.5f, 1.0f},  YES,            YES,            YES,            YES,            YES },
        
        //  Onscreen
        {   { 0.5f, 0.5f},  YES,            YES,            YES,            NO,             YES },
        
        //  Just at the edge of the screen.
        //  Expects to move, because although onscreen, callout would not be.
        //  However, if the scale is 0.25, then expectToPan should be NO, because
        //  of the width of the annotation

        //  Coord                   showsCallout  impl margins?   moveIntoView    expectMapToPan    calloutOnScreen
        {   {offset, 0.5f},         YES,          YES,            YES,            YES,              YES },
        {   {1.0 - offset, 0.5f},   YES,          YES,            YES,            YES,              YES },

        PAN_TEST_TERMINATOR
    };
    
    [self internalRunTests:tests];
}

- (void)testSelectionMoveIntoViewWithBasicCallout  {
    // Tests moveIntoView:YES
    // WITH a callout that DOES NOT implement marginshint
    
    // From https://github.com/mapbox/mapbox-gl-native/pull/13727#issuecomment-454028698
    //
    // | Annotation position | Has callout? | Callout implements `marginInsets...`? | Map pans when selected with moveIntoView=YES? |
    // |---------------------|--------------|---------------------------------------|-----------------------------------------------|
    // | Offscreen           | Yes          | No                                    | Yes, but only to show annotation (not callout) with no margins |
    // | Partially           | Yes          | No                                    | No                                            |
    // | Onscreen            | Yes          | No                                    | No                                            |
    //

    PanTestData tests[] = {
        //  Coord           showsCallout    impl margins?   moveIntoView    expectMapToPan  calloutOnScreen
        //  Offscreen
        {   {-1.0f, 0.5f},  YES,            NO,             YES,            YES,            NO },
        {   { 2.0f, 0.5f},  YES,            NO,             YES,            YES,            NO },
        {   { 0.5f,-1.0f},  YES,            NO,             YES,            YES,            NO },
        {   { 0.5f, 2.0f},  YES,            NO,             YES,            YES,            YES },   // Because annotation was off the bottom of screen, and callout is above annotation
        {   { 2.0f, 2.0f},  YES,            NO,             YES,            YES,            NO },

        //  Partial
        {   { 0.0f, 0.5f},  YES,            NO,             YES,            NO,             NO },
        {   { 1.0f, 0.5f},  YES,            NO,             YES,            NO,             NO },
        {   { 0.5f, 0.0f},  YES,            NO,             YES,            NO,             NO },
        {   { 0.5f, 1.0f},  YES,            NO,             YES,            NO,             YES }, // Because annotation was off the bottom of screen, and callout is above annotation
        {   { 1.0f, 1.0f},  YES,            NO,             YES,            NO,             NO },

        //  Onscreen
        {   { 0.5f, 0.5f},  YES,            NO,             YES,            NO,             YES },

        PAN_TEST_TERMINATOR
    };
    
    [self internalRunTests:tests];
}

#pragma mark - Selection with an offset

- (void)testSelectingAnnotationWithCenterOffset {

    for (CGFloat dx = -100.0; dx <= 100.0; dx += 100.0 ) {
        for (CGFloat dy = -100.0; dy <= 100.0; dy += 100.0 ) {
            CGVector offset = CGVectorMake(dx, dy);
            UIEdgeInsets edgeInsets = UIEdgeInsetsMake(fmax(-dy, 0.0), fmax(-dy, 0.0), fmax(dy, 0.0), fmax(dx, 0.0));
            [self internalTestSelectingAnnotationWithCenterOffsetWithOffset:offset edgeInsets:edgeInsets];
        }
    }
}

- (void)internalTestSelectingAnnotationWithCenterOffsetWithOffset:(CGVector)offset edgeInsets:(UIEdgeInsets)edgeInsets {

    NSString * const MGLTestAnnotationReuseIdentifer = @"MGLTestAnnotationReuseIdentifer";

    self.mapView.contentInset = edgeInsets;
    CGSize size = self.mapView.bounds.size;

    CGSize annotationSize = CGSizeMake(40.0, 40.0);

    self.viewForAnnotation = ^MGLAnnotationView*(MGLMapView *view, id<MGLAnnotation> annotation) {

        if (![annotation isKindOfClass:[MGLPointAnnotation class]]) {
            return nil;
        }

        // No dequeue
        MGLAnnotationView *annotationView = [[MGLAnnotationView alloc] initWithAnnotation:annotation reuseIdentifier:MGLTestAnnotationReuseIdentifer];
        annotationView.bounds             = (CGRect){ .origin = CGPointZero, .size = annotationSize };
        annotationView.backgroundColor    = UIColor.redColor;
        annotationView.enabled            = YES;
        annotationView.centerOffset       = offset;

        return annotationView;
    };

    MGLPointAnnotation *point = [[MGLPointAnnotation alloc] init];
    point.title = NSStringFromSelector(_cmd);
    point.coordinate = CLLocationCoordinate2DMake(0.0, 0.0);
    [self.mapView addAnnotation:point];

    // From https://github.com/mapbox/mapbox-gl-native/issues/12259#issuecomment-401414168
    //
    //      queryRenderedFeatures depends on collision detection having been run
    //      before it shows results [...]. Collision detection runs asynchronously
    //      (at least every 300ms, sometimes more often), and therefore the results
    //      of queryRenderedFeatures are similarly asynchronous.
    //
    // So, we need to wait before `annotationTagAtPoint:persistingResults:` will
    // return out newly added annotation

    [self waitForCollisionDetectionToRun];

    // Check that the annotation is in the center of the view
    CGPoint annotationPoint = [self.mapView convertCoordinate:point.coordinate toPointToView:self.mapView];
    XCTAssertEqualWithAccuracy(annotationPoint.x, (size.width - edgeInsets.right + edgeInsets.left)/2.0, 0.25);
    XCTAssertEqualWithAccuracy(annotationPoint.y, (size.height - edgeInsets.bottom + edgeInsets.top)/2.0, 0.25);

    // Now test taps around the annotation
    CGPoint tapPoint = CGPointMake(annotationPoint.x + offset.dx, annotationPoint.y + offset.dy);

    MGLAnnotationTag tagAtPoint = [self.mapView annotationTagAtPoint:tapPoint persistingResults:YES];
    XCTAssert(tagAtPoint != UINT32_MAX, @"Should have tapped on annotation");

    CGPoint testPoints[] = {
        { tapPoint.x - annotationSize.width, tapPoint.y },
        { tapPoint.x + annotationSize.width, tapPoint.y },
        { tapPoint.x, tapPoint.y - annotationSize.height },
        { tapPoint.x, tapPoint.y + annotationSize.height },
        CGPointZero
    };

    CGPoint *testPoint = testPoints;

    while (!CGPointEqualToPoint(*testPoint, CGPointZero)) {
        tagAtPoint = [self.mapView annotationTagAtPoint:*testPoints persistingResults:YES];
        XCTAssert(tagAtPoint == UINT32_MAX, @"Tap should to the side of the annotation");
        testPoint++;
    }
}

- (void)testUserLocationWithOffsetAnchorPoint {
    [self.mapView setCenterCoordinate:CLLocationCoordinate2DMake(37.787357, -122.39899)];
    MGLTestLocationManager *locationManager = [[MGLTestLocationManager alloc] init];
    self.mapView.locationManager = locationManager;

    [self.mapView setUserTrackingMode:MGLUserTrackingModeFollow animated:NO completionHandler:nil];
    CGRect originalFrame = [self.mapView viewForAnnotation:self.mapView.userLocation].frame;
    
    // Temporarily disable location tracking so we can save the value of
    // the originalFrame in memory
    [self.mapView setUserTrackingMode:MGLUserTrackingModeNone animated:NO completionHandler:nil];
    
    CGPoint offset = CGPointMake(20, 20);
    
    self.mapViewUserLocationAnchorPoint = ^CGPoint (MGLMapView *mapView) {
        return offset;;
    };
    
    [self.mapView setUserTrackingMode:MGLUserTrackingModeFollow animated:NO completionHandler:nil];
    CGRect offsetFrame = [self.mapView viewForAnnotation:self.mapView.userLocation].frame;
    
    XCTAssertEqual(originalFrame.origin.x + offset.x, offsetFrame.origin.x);
    XCTAssertEqual(originalFrame.origin.y + offset.y, offsetFrame.origin.y);
}

#pragma mark - Rotating/zooming

- (void)testSelectingAnnotationWhenMapIsRotated {
    
    CLLocationCoordinate2D coordinates[] = {
        { 40.0, 40.0 },
        { NAN, NAN }
    };
    
    NSArray *annotations = [self internalAddAnnotationsAtCoordinates:coordinates];
    MGLPointAnnotation *annotation = annotations.firstObject;
    
    // Rotate
    CLLocationDirection lastAngle = 0.0;

    srand48(0);
    for (NSInteger iter = 0; iter < 10; iter++ ) {
                
        CLLocationDirection angle = (CLLocationDirection)((drand48()*1080.0) - 540.0);
        
        CGPoint anchor = CGPointMake(drand48()*CGRectGetWidth(self.mapView.bounds), drand48()*CGRectGetHeight(self.mapView.bounds));
        
        NSString *activityTitle = [NSString stringWithFormat:@"Rotate to: %0.1f from: %0.1f", angle, lastAngle];
        [XCTContext runActivityNamed:activityTitle
                               block:^(id<XCTActivity>  _Nonnull activity) {

                                   MGLMapCamera *toCamera = [self.mapView cameraByRotatingToDirection:angle aroundAnchorPoint:anchor];
                                   [self internalTestSelecting:annotation withCamera:toCamera];
                               }];
        
        lastAngle = angle;
    }
}

- (void)testSelectingAnnotationWhenMapIsScaled {

    CLLocationCoordinate2D coordinates[] = {
        { 0.005, 0.005 },
        { NAN, NAN }
    };
    
    NSArray *annotations = [self internalAddAnnotationsAtCoordinates:coordinates];
    MGLPointAnnotation *annotation = annotations.firstObject;
    
    CGPoint anchor = CGPointMake(CGRectGetMidX(self.mapView.bounds), CGRectGetMidY(self.mapView.bounds));
    
    srand48(0);
    for (NSInteger iter = 0; iter < 10; iter++ ) {
        
        double zoom = (double)(drand48()*14.0);
        
        NSString *activityTitle = [NSString stringWithFormat:@"Zoom to %0.1f", zoom];
        [XCTContext runActivityNamed:activityTitle
                               block:^(id<XCTActivity>  _Nonnull activity) {
                                   MGLMapCamera *toCamera = [self.mapView cameraByZoomingToZoomLevel:zoom aroundAnchorPoint:anchor];
                                   [self internalTestSelecting:annotation withCamera:toCamera];
                               }];
    }
}

- (void)testSelectingAnnotationWhenMapIsScaledAndRotated {
    
    CLLocationCoordinate2D coordinates[] = {
        { 0.005, 0.005 },
        { NAN, NAN }
    };
    
    NSArray *annotations = [self internalAddAnnotationsAtCoordinates:coordinates];
    MGLPointAnnotation *annotation = annotations.firstObject;
    
    srand48(0);
    for (NSInteger iter = 0; iter < 10; iter++ ) {
        
        double zoom = (double)(7.0 + drand48()*7.0);
        CLLocationDirection angle = (CLLocationDirection)((drand48()*1080.0) - 540.0);
        
        CGPoint anchor = CGPointMake(drand48()*CGRectGetWidth(self.mapView.bounds), drand48()*CGRectGetHeight(self.mapView.bounds));

        NSString *activityTitle = [NSString stringWithFormat:@"Zoom to %0.1f", zoom];
        [XCTContext runActivityNamed:activityTitle
                               block:^(id<XCTActivity>  _Nonnull activity)
         {
             mbgl::CameraOptions currentCameraOptions;
             
             currentCameraOptions.bearing = angle;
             currentCameraOptions.anchor = mbgl::ScreenCoordinate { anchor.x, anchor.y };
             currentCameraOptions.zoom = zoom;
             MGLMapCamera *toCamera = [self.mapView cameraForCameraOptions:currentCameraOptions];
             
             [self internalTestSelecting:annotation withCamera:toCamera];
         }];
    }
}


- (void)testShowingAnnotationsThenSelectingAnimated {
    [self internalTestShowingAnnotationsThenSelectingAnimated:YES];
}

- (void)testShowingAnnotationsThenSelecting {
    [self internalTestShowingAnnotationsThenSelectingAnimated:NO];
}

- (void)internalTestShowingAnnotationsThenSelectingAnimated:(BOOL)animated {
    srand48(0);

    CGFloat maxXPadding = std::max(CGRectGetWidth(self.mapView.bounds)/5.0, 100.0);
    CGFloat maxYPadding = std::max(CGRectGetHeight(self.mapView.bounds)/5.0, 100.0);

    for (int i = 0; i < 10; i++) {
        UIEdgeInsets edgePadding;
        edgePadding.top    = floor(drand48()*maxYPadding);
        edgePadding.bottom = floor(drand48()*maxYPadding);
        edgePadding.left   = floor(drand48()*maxXPadding);
        edgePadding.right  = floor(drand48()*maxXPadding);

        UIEdgeInsets contentInsets;
        contentInsets.top    = floor(drand48()*maxYPadding);
        contentInsets.bottom = floor(drand48()*maxYPadding);
        contentInsets.left   = floor(drand48()*maxXPadding);
        contentInsets.right  = floor(drand48()*maxXPadding);

        [self internalTestShowingAnnotationsThenSelectingAnimated:animated edgePadding:edgePadding contentInsets:contentInsets];
    }
}

- (void)internalTestShowingAnnotationsThenSelectingAnimated:(BOOL)animated edgePadding:(UIEdgeInsets)edgeInsets contentInsets:(UIEdgeInsets)contentInsets {
    CLLocationCoordinate2D coordinates[21];
    
    for (int i = 0; i < (int)(sizeof(coordinates)/sizeof(coordinates[0])); i++)
    {
        coordinates[i].latitude = drand48();
        coordinates[i].longitude = drand48();
    }
    coordinates[20] = CLLocationCoordinate2DMake(NAN, NAN);

    NSArray *annotations = [self internalAddAnnotationsAtCoordinates:coordinates];

    XCTestExpectation *showCompleted = [self expectationWithDescription:@"showCompleted"];

    self.mapView.contentInset = contentInsets;
    [self.mapView showAnnotations:annotations
                      edgePadding:edgeInsets
                         animated:animated
                completionHandler:^{
                    [showCompleted fulfill];
                }];
    
    [self waitForExpectations:@[showCompleted] timeout:3.5];
  
    // These tests will fail if this isn't here. But this isn't quite what we're
    // seeing in https://github.com/mapbox/mapbox-gl-native/issues/15106
    [self waitForCollisionDetectionToRun];
    
    for (MGLPointAnnotation *point in annotations) {
        [self internalSelectDeselectAnnotation:point];
    }
    
    [self.mapView removeAnnotations:annotations];
    self.mapView.contentInset = UIEdgeInsetsZero;
    [self waitForCollisionDetectionToRun];
}

- (NSArray*)internalAddAnnotationsAtCoordinates:(CLLocationCoordinate2D*)coordinates
{
    __block NSMutableArray *annotations = [NSMutableArray array];
    
    [XCTContext runActivityNamed:@"Map setup"
                           block:^(id<XCTActivity>  _Nonnull activity)
     {
         
         NSString * const MGLTestAnnotationReuseIdentifer = @"MGLTestAnnotationReuseIdentifer";
         
         CGSize annotationSize = CGSizeMake(40.0, 40.0);
         
         self.viewForAnnotation = ^MGLAnnotationView*(MGLMapView *view, id<MGLAnnotation> annotation2) {
             
             if (![annotation2 isKindOfClass:[MGLPointAnnotation class]]) {
                 return nil;
             }
             
             // No dequeue
             MGLAnnotationView *annotationView = [[MGLAnnotationView alloc] initWithAnnotation:annotation2 reuseIdentifier:MGLTestAnnotationReuseIdentifer];
             annotationView.bounds             = (CGRect){ .origin = CGPointZero, .size = annotationSize };
             annotationView.backgroundColor    = UIColor.redColor;
             annotationView.enabled            = YES;
             
             return annotationView;
         };
         
         CLLocationCoordinate2D *coordinatePtr = coordinates;
         while (!isnan(coordinatePtr->latitude)) {
             CLLocationCoordinate2D coordinate = *coordinatePtr++;
         
             MGLPointAnnotation *annotation = [[MGLPointAnnotation alloc] init];
             annotation.title = NSStringFromSelector(_cmd);
             annotation.coordinate = coordinate;
             [annotations addObject:annotation];
         }
         
         [self.mapView addAnnotations:annotations];

     }];

    NSArray *copiedAnnotations = [annotations copy];
    annotations = nil;
    
    return copiedAnnotations;
}

- (void)internalTestSelecting:(MGLPointAnnotation*)point withCamera:(MGLMapCamera*)camera {
    
    // Rotate
    XCTestExpectation *rotationCompleted = [self expectationWithDescription:@"rotationCompleted"];
    [self.mapView setCamera:camera withDuration:0.1 animationTimingFunction:nil completionHandler:^{
        [rotationCompleted fulfill];
    }];
    
    [self waitForExpectations:@[rotationCompleted] timeout:1.5];
    
    // Collision detection may not have completed, if not we may not get our annotation.
    [self waitForCollisionDetectionToRun];
    
    // Look up annotation at point
    [self internalSelectDeselectAnnotation:point];
}

- (void)internalSelectDeselectAnnotation:(MGLPointAnnotation*)point {
    [XCTContext runActivityNamed:[NSString stringWithFormat:@"Select annotation: %@", point]
                           block:^(id<XCTActivity>  _Nonnull activity)
     {
         CGPoint annotationPoint = [self.mapView convertCoordinate:point.coordinate toPointToView:self.mapView];
         
         MGLAnnotationTag tagAtPoint = [self.mapView annotationTagAtPoint:annotationPoint persistingResults:YES];
         if (tagAtPoint != UINT32_MAX)
         {
             id <MGLAnnotation> annotation = [self.mapView annotationWithTag:tagAtPoint];
             XCTAssertNotNil(annotation);
             
             // Select
             XCTestExpectation *selectionCompleted = [self expectationWithDescription:@"Selection completed"];
             [self.mapView selectAnnotation:annotation moveIntoView:NO animateSelection:NO completionHandler:^{
                 [selectionCompleted fulfill];
             }];
             
             [self waitForExpectations:@[selectionCompleted] timeout:0.05];
             
             XCTAssert(self.mapView.selectedAnnotations.count == 1, @"There should only be 1 selected annotation");
             XCTAssertEqualObjects(self.mapView.selectedAnnotations.firstObject, annotation, @"The annotation should be selected");
             
             // Deselect
             [self.mapView deselectAnnotation:annotation animated:NO];
         }
         else
         {
             XCTFail(@"Should be an annotation at this point: %@", NSStringFromCGPoint(annotationPoint));
         }
     }];
    
}

#pragma mark - Utilities

- (void)runRunLoop {
    [[NSRunLoop mainRunLoop] runMode:NSDefaultRunLoopMode beforeDate:[NSDate distantFuture]];
}

- (void)waitFor:(NSTimeInterval)seconds {
    XCTestExpectation *timerExpired = [self expectationWithDescription:@"Timer expires"];
    
    NSTimer *timer = [NSTimer scheduledTimerWithTimeInterval:0.1
                                                      target:self
                                                    selector:@selector(runRunLoop)
                                                    userInfo:nil
                                                     repeats:YES];

    double duration = seconds * (double)NSEC_PER_SEC;
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)duration), dispatch_get_main_queue(), ^{
        [timerExpired fulfill];
    });
    
    [self waitForExpectations:@[timerExpired] timeout:seconds + 1.0];
    [timer invalidate];
}

- (void)waitForCollisionDetectionToRun {
    XCTAssertNil(self.renderFinishedExpectation, @"Incorrect test setup");

    [self.mapView setNeedsRerender];
    self.renderFinishedExpectation = [self expectationWithDescription:@"Map view should be rendered"];
    XCTestExpectation *timerExpired = [self expectationWithDescription:@"Timer expires"];

    // Wait 1/2 second
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(NSEC_PER_SEC >> 1)), dispatch_get_main_queue(), ^{
        [timerExpired fulfill];
    });

    [self waitForExpectations:@[timerExpired, self.renderFinishedExpectation] timeout:5];
    
    self.renderFinishedExpectation = nil;
}

@end
