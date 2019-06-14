
#import "MGLMapViewIntegrationTest.h"

@interface MGLStyleLoadingIntegrationTests : MGLMapViewIntegrationTest
@property (nonatomic) XCTestExpectation *styleLoadingExpectation;
@end

@implementation MGLStyleLoadingIntegrationTests

- (void)setUp {
    [super setUp];
    if (!self.mapView.style) {
        [self waitForMapViewToFinishLoadingStyleWithTimeout:10];
    }
}

- (void)tearDown {
    self.styleLoadingExpectation = nil;
}

- (void)mapView:(MGLMapView *)mapView didFinishLoadingStyle:(MGLStyle *)style {
    XCTAssertNotNil(mapView.style);
    XCTAssertEqual(mapView.style, style);
    
    [self.styleLoadingExpectation fulfill];
}

- (void)waitForMapViewToFinishLoadingStyleWithTimeout:(NSTimeInterval)timeout {
    XCTAssertNil(self.styleLoadingExpectation);
    self.styleLoadingExpectation = [self expectationWithDescription:@"Map view should finish loading style."];
    [self waitForExpectations:@[self.styleLoadingExpectation] timeout:timeout];
}

- (MGLStyle *)style {
    return self.mapView.style;
}
@end
