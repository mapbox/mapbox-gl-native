//
//  MGLAnnotationViewIntegrationTests.m
//  integration
//
//  Created by Julian Rex on 6/29/18.
//  Copyright © 2018 Mapbox. All rights reserved.
//

#import "MGLMapViewIntegrationTest.h"
#import "MGLTestUtility.h"
#import "MGLMapAccessibilityElement.h"

@interface MGLMapView (Tests)
- (MGLAnnotationTag)annotationTagAtPoint:(CGPoint)point persistingResults:(BOOL)persist;
@end


@interface MGLAnnotationViewIntegrationTests : MGLMapViewIntegrationTest
@end

@implementation MGLAnnotationViewIntegrationTests

- (void)testSelectingAnnotationWithCenterOffsetPENDING {
    MGL_CHECK_IF_PENDING_TEST_SHOULD_RUN();

    NSString * const MGLTestAnnotationReuseIdentifer = @"MGLTestAnnotationReuseIdentifer";

    CGFloat epsilon = 0.0000001;
    CGSize size = self.mapView.bounds.size;

    __weak __typeof__(self) weakself = self;
    self.viewForAnnotation = ^MGLAnnotationView*(MGLMapView *view, id<MGLAnnotation> annotation) {

        if (![annotation isKindOfClass:[MGLPointAnnotation class]]) {
            return nil;
        }

        MGLAnnotationView *annotationView = [weakself.mapView dequeueReusableAnnotationViewWithIdentifier:MGLTestAnnotationReuseIdentifer];

        if (!annotationView)
        {
            annotationView = [[MGLAnnotationView alloc] initWithAnnotation:annotation reuseIdentifier:MGLTestAnnotationReuseIdentifer];
            annotationView.bounds = CGRectMake(0.0, 0.0, 40.0, 40.0);
            annotationView.backgroundColor = UIColor.redColor;
            annotationView.enabled = YES;
        }

        return annotationView;
        //        view.centerOffset = CGVectorMake(-size.width/4.0, -size.width/4.0);
    };

    MGLPointAnnotation *point = [[MGLPointAnnotation alloc] init];
    point.title = NSStringFromSelector(_cmd);
    point.coordinate = CLLocationCoordinate2DMake(0.0, 0.0);
    [self.mapView addAnnotation:point];

    [self waitForMapViewToBeRenderedWithTimeout:5.0];

    // Check that the annotation is in the center of the view
    CGPoint annotationPoint = [self.mapView convertCoordinate:point.coordinate toPointToView:self.mapView];
    XCTAssertEqualWithAccuracy(annotationPoint.x, size.width/2.0, epsilon);
    XCTAssertEqualWithAccuracy(annotationPoint.y, size.height/2.0, epsilon);

    MGLAnnotationTag tagAtPoint = [self.mapView annotationTagAtPoint:annotationPoint persistingResults:YES];
    XCTAssert(tagAtPoint != UINT32_MAX, @"Should have tapped on annotation");
}

@end
