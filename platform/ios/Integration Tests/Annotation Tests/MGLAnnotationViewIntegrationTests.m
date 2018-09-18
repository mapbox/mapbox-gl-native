#import "MGLMapViewIntegrationTest.h"
#import "MGLTestUtility.h"
#import "MGLMapAccessibilityElement.h"

@interface MGLMapView (Tests)
- (MGLAnnotationTag)annotationTagAtPoint:(CGPoint)point persistingResults:(BOOL)persist;
@end

@interface MGLAnnotationViewIntegrationTests : MGLMapViewIntegrationTest
@end

@implementation MGLAnnotationViewIntegrationTests

- (void)testSelectingAnnotationWithCenterOffset {

    for (CGFloat dx = -100.0; dx <= 100.0; dx += 100.0 ) {
        for (CGFloat dy = -100.0; dy <= 100.0; dy += 100.0 ) {
            CGVector offset = CGVectorMake(dx, dy);
            [self internalTestSelectingAnnotationWithCenterOffsetWithOffset:offset];
        }
    }
}

- (void)internalTestSelectingAnnotationWithCenterOffsetWithOffset:(CGVector)offset {

    NSString * const MGLTestAnnotationReuseIdentifer = @"MGLTestAnnotationReuseIdentifer";

    CGFloat epsilon = 0.0000001;
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
    XCTAssertEqualWithAccuracy(annotationPoint.x, size.width/2.0, epsilon);
    XCTAssertEqualWithAccuracy(annotationPoint.y, size.height/2.0, epsilon);

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

- (void)waitForCollisionDetectionToRun {
    XCTAssertNil(self.renderFinishedExpectation, @"Incorrect test setup");

    self.renderFinishedExpectation = [self expectationWithDescription:@"Map view should be rendered"];
    XCTestExpectation *timerExpired = [self expectationWithDescription:@"Timer expires"];

    // Wait 1/2 second
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(NSEC_PER_SEC >> 1)), dispatch_get_main_queue(), ^{
        [timerExpired fulfill];
    });

    [self waitForExpectations:@[timerExpired, self.renderFinishedExpectation] timeout:5];
}

@end
