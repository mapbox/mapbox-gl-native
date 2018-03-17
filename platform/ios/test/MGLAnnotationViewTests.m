#import <Mapbox/Mapbox.h>
#import <XCTest/XCTest.h>

static NSString * const MGLTestAnnotationReuseIdentifer = @"MGLTestAnnotationReuseIdentifer";


@interface MGLMapView (Tests)
@property (nonatomic) MGLCameraChangeReason cameraChangeReasonBitmask;
@end



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

- (void)presentCalloutFromRect:(CGRect)rect inView:(nonnull UIView *)view constrainedToRect:(CGRect)constrainedRect animated:(BOOL)animated {}

@end

@interface MGLAnnotationViewTests : XCTestCase <MGLMapViewDelegate>
@property (nonatomic) XCTestExpectation *expectation;
@property (nonatomic) MGLMapView *mapView;
@property (nonatomic, weak) MGLAnnotationView *annotationView;
@property (nonatomic) NSInteger annotationSelectedCount;
@property (nonatomic) void (^prepareAnnotationView)(MGLAnnotationView*);
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

- (void)testSelectingOnscreenAnnotationThatHasNotBeenAdded {
    // See https://github.com/mapbox/mapbox-gl-native/issues/11476

    // This bug occurs under the following conditions:
    //
    // - There are content insets (e.g. navigation bar) for the compare against
    //      CGRectZero (now CGRectNull)
    // - annotationView.enabled == NO - Currently this can happen if you use
    //      `-initWithFrame:` rather than one of the provided initializers
    //

    self.prepareAnnotationView = ^(MGLAnnotationView *view) {
        view.enabled = NO;
    };

    self.mapView.contentInset = UIEdgeInsetsMake(10.0, 10.0, 10.0, 10.0);

    MGLCameraChangeReason reasonBefore = self.mapView.cameraChangeReasonBitmask;
    XCTAssert(reasonBefore == MGLCameraChangeReasonNone, @"Camera should not have moved at start of test");

    // Create annotation
    MGLPointFeature *point = [[MGLPointFeature alloc] init];
    point.title = NSStringFromSelector(_cmd);
    point.coordinate = CLLocationCoordinate2DMake(0.0, 0.0);

    MGLCoordinateBounds coordinateBounds = [self.mapView convertRect:self.mapView.bounds toCoordinateBoundsFromView:self.mapView];
    XCTAssert(MGLCoordinateInCoordinateBounds(point.coordinate, coordinateBounds), @"The test point should be within the visible map view");

    // Select on screen annotation (DO NOT ADD FIRST).
    [self.mapView selectAnnotation:point animated:YES];

    // Expect - the camera NOT to move.
    MGLCameraChangeReason reasonAfter = self.mapView.cameraChangeReasonBitmask;
    XCTAssert(reasonAfter == MGLCameraChangeReasonNone, @"Camera should not have moved");
}

- (void)checkDefaultPropertiesForAnnotationView:(MGLAnnotationView*)view {
    XCTAssertNil(view.annotation);
    XCTAssertNil(view.reuseIdentifier);
    XCTAssertEqual(view.centerOffset.dx, 0.0);
    XCTAssertEqual(view.centerOffset.dy, 0.0);
    XCTAssertFalse(view.scalesWithViewingDistance);
    XCTAssertFalse(view.rotatesToMatchCamera);
    XCTAssertFalse(view.isSelected);
    XCTAssert(view.isEnabled);
    XCTAssertFalse(view.isDraggable);
    XCTAssertEqual(view.dragState, MGLAnnotationViewDragStateNone);
}

- (void)testAnnotationViewInitWithFrame {
    CGRect frame = CGRectMake(10.0, 10.0, 100.0, 100.0);
    MGLAnnotationView *view = [[MGLAnnotationView alloc] initWithFrame:frame];
    [self checkDefaultPropertiesForAnnotationView:view];
}

- (void)testAnnotationViewInitWithReuseIdentifier {
    MGLAnnotationView *view = [[MGLAnnotationView alloc] initWithReuseIdentifier:nil];
    [self checkDefaultPropertiesForAnnotationView:view];
}

- (void)testSelectingADisabledAnnotationView {
    self.prepareAnnotationView = ^(MGLAnnotationView *view) {
        view.enabled = NO;
    };
    
    // Create annotation
    MGLPointFeature *point = [[MGLPointFeature alloc] init];
    point.title = NSStringFromSelector(_cmd);
    point.coordinate = CLLocationCoordinate2DMake(0.0, 0.0);
    
    XCTAssert(self.mapView.selectedAnnotations.count == 0, @"There should be 0 selected annotations");
    
    [self.mapView selectAnnotation:point animated:NO];
    
    XCTAssert(self.mapView.selectedAnnotations.count == 0, @"There should be 0 selected annotations");
}

- (void)testAddAnnotationWithBoundaryCoordinates
{
    typedef struct {
        CLLocationDegrees lat;
        CLLocationDegrees lon;
        BOOL expectation;
    } TestParam;

    TestParam params[] = {
        //          Lat     Lon     Valid
        {   -91.0,  0.0,    NO},

        // The follow coordinate fails, essentially because the following in projection.hpp
        //
        //      util::LONGITUDE_MAX - util::RAD2DEG * std::log(std::tan(M_PI / 4 + latLng.latitude() * M_PI / util::DEGREES_MAX))
        //
        // boils down to ln(0)
        //
        // It makes sense that -90° latitude (south pole) should be invalid from a projection point
        // of view, but in that case shouldn't +90° (north pole) also be invalid?
        //
        // In Obj-C code, perhaps we need to replace usage of CLLocationCoordinate2DIsValid for an
        // MGL one...

        {   -90.0,  0.0,    YES}, // South pole. Should this really be considered an invalid coordinate?

        // The rest for completeness
        {   -89.0,  0.0,    YES},
        {   90.0,   0.0,    YES}, // North pole. Similarly, should this one be considered invalid?
        {   91.0,   0.0,    NO},

        {   0.0,    -181.0, NO},
        {   0.0,    -180.0, YES},
        {   0.0,    180.0,  YES},
        {   0.0,    181.0,  NO},
    };

    for (int i = 0; i < sizeof(params)/sizeof(params[0]); i++) {
        TestParam param = params[i];

        // Essentially a deconstructed -[MGLMapView convertCoordinate:toPointToView]
        CLLocationCoordinate2D coordinate = CLLocationCoordinate2DMake(param.lat, param.lon);
        NSString *coordDesc = [NSString stringWithFormat:@"(%0.1f,%0.1f)", param.lat, param.lon];

        XCTAssert(CLLocationCoordinate2DIsValid(coordinate) == param.expectation, @"Unexpected valid result for coordinate %@", coordDesc);

        CGPoint point = [_mapView convertCoordinate:coordinate toPointToView:_mapView];
        (void)point;
        XCTAssert(isnan(point.x) != param.expectation, @"Unexpected point.x for coordinate %@", coordDesc);
        XCTAssert(isnan(point.y) != param.expectation, @"Unexpected point.y for coordinate %@", coordDesc);

        if (param.expectation) {
            // If we expect a valid coordinate, let's finally try to add an annotation

            // The above method is called by the following, which will trigger CALayer to raise an
            // exception
            MGLTestAnnotation *annotation = [[MGLTestAnnotation alloc] init];
            annotation.coordinate = coordinate;

            @try {
                [_mapView addAnnotation:annotation];
            }
            @catch (NSException *e) {
                XCTFail("addAnnotation triggered exception: %@ for coordinate %@", e, coordDesc);
            }
        }
    }
}

#pragma mark - MGLMapViewDelegate -


- (MGLAnnotationView *)mapView:(MGLMapView *)mapView viewForAnnotation:(id<MGLAnnotation>)annotation
{
    MGLAnnotationView *annotationView = [mapView dequeueReusableAnnotationViewWithIdentifier:MGLTestAnnotationReuseIdentifer];

    if (!annotationView)
    {
        annotationView = [[MGLAnnotationView alloc] initWithAnnotation:annotation reuseIdentifier:MGLTestAnnotationReuseIdentifer];
    }

    if (self.prepareAnnotationView) {
        self.prepareAnnotationView(annotationView);
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
