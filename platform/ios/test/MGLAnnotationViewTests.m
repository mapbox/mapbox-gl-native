#import <Mapbox/Mapbox.h>
#import <XCTest/XCTest.h>

static NSString * const MGLTestAnnotationReuseIdentifer = @"MGLTestAnnotationReuseIdentifer";

@interface MGLCustomAnnotationView : MGLAnnotationView

@end

@implementation MGLCustomAnnotationView

- (instancetype)initWithReuseIdentifier:(NSString *)reuseIdentifier {
    return [super initWithReuseIdentifier:@"reuse-id"];
}

@end

@interface MGLAnnotationView (Test)

@property (nonatomic) MGLMapView *mapView;
@property (nonatomic, readwrite) MGLAnnotationViewDragState dragState;
- (void)setDragState:(MGLAnnotationViewDragState)dragState;

@end

@interface MGLMapView (Test)
@property (nonatomic) UIView<MGLCalloutView> *calloutViewForSelectedAnnotation;
@end

@interface MGLTestAnnotation : NSObject <MGLAnnotation>
@property (nonatomic, assign) CLLocationCoordinate2D coordinate;
@end

@implementation MGLTestAnnotation
@end

@interface MGLTestCalloutView: UIView<MGLCalloutView>
@property (nonatomic) BOOL didCallDismissCalloutAnimated;
@property (nonatomic, strong) id <MGLAnnotation> representedObject;
@property (nonatomic, strong) UIView *leftAccessoryView;
@property (nonatomic, strong) UIView *rightAccessoryView;
@property (nonatomic, weak) id<MGLCalloutViewDelegate> delegate;
@end

@implementation MGLTestCalloutView

- (void)dismissCalloutAnimated:(BOOL)animated
{
    _didCallDismissCalloutAnimated = YES;
}

- (void)presentCalloutFromRect:(CGRect)rect inView:(UIView *)view constrainedToView:(UIView *)constrainedView animated:(BOOL)animated { }

- (void)presentCalloutFromRect:(CGRect)rect inView:(nonnull UIView *)view constrainedToRect:(CGRect)constrainedRect animated:(BOOL)animated {}

@end

@interface MGLAnnotationViewTests : XCTestCase <MGLMapViewDelegate>
@property (nonatomic) XCTestExpectation *expectation;
@property (nonatomic) MGLMapView *mapView;
@property (nonatomic, weak) MGLAnnotationView *annotationView;
@property (nonatomic) NSInteger annotationSelectedCount;
@end

@implementation MGLAnnotationViewTests

- (void)setUp
{
    [super setUp];
    _mapView = [[MGLMapView alloc] initWithFrame:CGRectMake(0, 0, 64, 64)];
    _mapView.delegate = self;
}

- (void)testAnnotationView
{
    _expectation = [self expectationWithDescription:@"annotation property"];

    MGLTestAnnotation *annotation = [[MGLTestAnnotation alloc] init];
    [_mapView addAnnotation:annotation];

    [self waitForExpectationsWithTimeout:1 handler:nil];

    XCTAssert(_mapView.annotations.count == 1, @"number of annotations should be 1");
    XCTAssertNotNil(_annotationView.annotation, @"annotation property should not be nil");
    XCTAssertNotNil(_annotationView.mapView, @"mapView property should not be nil");

    MGLTestCalloutView *testCalloutView = [[MGLTestCalloutView  alloc] init];
    _mapView.calloutViewForSelectedAnnotation = testCalloutView;
    _annotationView.dragState = MGLAnnotationViewDragStateStarting;
    XCTAssertTrue(testCalloutView.didCallDismissCalloutAnimated, @"callout view was not dismissed");

    [_mapView removeAnnotation:_annotationView.annotation];

    XCTAssert(_mapView.annotations.count == 0, @"number of annotations should be 0");
    XCTAssertNil(_annotationView.annotation, @"annotation property should be nil");
}

- (void)testCustomAnnotationView
{
    MGLCustomAnnotationView *customAnnotationView = [[MGLCustomAnnotationView alloc] initWithReuseIdentifier:@"reuse-id"];
    XCTAssertNotNil(customAnnotationView);
}

- (void)testSelectingOffscreenAnnotation
{
    // Partial test for https://github.com/mapbox/mapbox-gl-native/issues/9790

    // This isn't quite the same as in updateAnnotationViews, but should be sufficient for this test.
    MGLCoordinateBounds coordinateBounds = [_mapView convertRect:_mapView.bounds toCoordinateBoundsFromView:_mapView];

    // -90 latitude is invalid. TBD.
    BOOL anyOffscreen = NO;
    NSInteger selectionCount = 0;

    for (NSInteger latitude = -89; latitude <= 90; latitude += 10)
    {
        for (NSInteger longitude = -180; longitude <= 180; longitude += 10)
        {
            MGLTestAnnotation *annotation = [[MGLTestAnnotation alloc] init];

            annotation.coordinate = CLLocationCoordinate2DMake(latitude, longitude);
            [_mapView addAnnotation:annotation];

            if (!(MGLCoordinateInCoordinateBounds(annotation.coordinate, coordinateBounds)))
                anyOffscreen = YES;

            XCTAssertNil(_mapView.selectedAnnotations.firstObject, @"There should be no selected annotation");

            // First selection
            [_mapView selectAnnotation:annotation animated:NO];
            selectionCount++;

            XCTAssert(_mapView.selectedAnnotations.count == 1, @"There should only be 1 selected annotation");
            XCTAssertEqualObjects(_mapView.selectedAnnotations.firstObject, annotation, @"The annotation should be selected");

            // Deselect
            [_mapView deselectAnnotation:annotation animated:NO];
            XCTAssert(_mapView.selectedAnnotations.count == 0, @"There should be no selected annotations");

            // Second selection
            _mapView.selectedAnnotations = @[annotation];
            selectionCount++;

            XCTAssert(_mapView.selectedAnnotations.count == 1, @"There should be 1 selected annotation");
            XCTAssertEqualObjects(_mapView.selectedAnnotations.firstObject, annotation, @"The annotation should be selected");

            // Deselect
            [_mapView deselectAnnotation:annotation animated:NO];
            XCTAssert(_mapView.selectedAnnotations.count == 0, @"There should be no selected annotations");
        }
    }

    XCTAssert(anyOffscreen, @"At least one of these annotations should be offscreen");
    XCTAssertEqual(selectionCount, self.annotationSelectedCount, @"-mapView:didSelectAnnotation: should be called for each selection");
}

#pragma mark - MGLMapViewDelegate -

- (MGLAnnotationView *)mapView:(MGLMapView *)mapView viewForAnnotation:(id<MGLAnnotation>)annotation
{
    MGLAnnotationView *annotationView = [mapView dequeueReusableAnnotationViewWithIdentifier:MGLTestAnnotationReuseIdentifer];

    if (!annotationView)
    {
        annotationView = [[MGLAnnotationView alloc] initWithAnnotation:annotation reuseIdentifier:MGLTestAnnotationReuseIdentifer];
    }

    _annotationView = annotationView;

    return annotationView;
}

- (void)mapView:(MGLMapView *)mapView didAddAnnotationViews:(NSArray<MGLAnnotationView *> *)annotationViews
{
    [_expectation fulfill];
}

- (void)mapView:(MGLMapView *)mapView didSelectAnnotation:(id<MGLAnnotation>)annotation
{
    self.annotationSelectedCount++;
}

@end
