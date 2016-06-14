#import <Mapbox/Mapbox.h>
#import <XCTest/XCTest.h>

static NSString * const MGLTestAnnotationReuseIdentifer = @"MGLTestAnnotationReuseIdentifer";

@interface MGLTestAnnotation : NSObject <MGLAnnotation>
@property (nonatomic, assign) CLLocationCoordinate2D coordinate;
@end

@implementation MGLTestAnnotation
@end

@interface MGLAnnotationViewTests : XCTestCase <MGLMapViewDelegate>
@property (nonatomic) XCTestExpectation *expectation;
@property (nonatomic) MGLMapView *mapView;
@property (nonatomic, weak) MGLAnnotationView *annotationView;
@end

@implementation MGLAnnotationViewTests

- (void)setUp
{
    [super setUp];
    _mapView = [[MGLMapView alloc] initWithFrame:CGRectZero];
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
    
    [_mapView removeAnnotation:_annotationView.annotation];
    
    XCTAssert(_mapView.annotations.count == 0, @"number of annotations should be 0");
    XCTAssertNil(_annotationView.annotation, @"annotation property should be nil");
}

- (MGLAnnotationView *)mapView:(MGLMapView *)mapView viewForAnnotation:(id<MGLAnnotation>)annotation
{
    MGLAnnotationView *annotationView = [mapView dequeueReusableAnnotationViewWithIdentifier:MGLTestAnnotationReuseIdentifer];
    
    if (!annotationView)
    {
        annotationView = [[MGLAnnotationView alloc] initWithReuseIdentifier:MGLTestAnnotationReuseIdentifer];
    }
    
    _annotationView = annotationView;
    
    [_expectation fulfill];
    
    return annotationView;
}

@end
