#import "MGLMapViewIntegrationTest.h"
#import "MGLTestUtility.h"
#import "../../darwin/src/MGLGeometry_Private.h"

#include <mbgl/map/camera.hpp>

@interface MGLCameraTransitionFinishTests : MGLMapViewIntegrationTest
@end

@implementation MGLCameraTransitionFinishTests

- (void)testEaseToCompletionHandler {
    
    MGLCoordinateBounds bounds = MGLCoordinateBoundsMake(CLLocationCoordinate2DMake(0.0, 0.0),
                                                         CLLocationCoordinate2DMake(1.0, 1.0));
    MGLMapCamera *camera = [self.mapView cameraThatFitsCoordinateBounds:bounds];
    
    XCTestExpectation *expectation = [self expectationWithDescription:@"Completion block should be called"];
    
    [self.mapView setCamera:camera
               withDuration:0.0
    animationTimingFunction:nil
          completionHandler:^{
              [expectation fulfill];
          }];
    
    [self waitForExpectations:@[expectation] timeout:0.5];
}

- (void)testEaseToCompletionHandlerAnimated {
    
    MGLCoordinateBounds bounds = MGLCoordinateBoundsMake(CLLocationCoordinate2DMake(0.0, 0.0),
                                                         CLLocationCoordinate2DMake(1.0, 1.0));
    MGLMapCamera *camera = [self.mapView cameraThatFitsCoordinateBounds:bounds];
    
    XCTestExpectation *expectation = [self expectationWithDescription:@"Completion block should be called"];
    
    [self.mapView setCamera:camera
               withDuration:0.3
    animationTimingFunction:nil
          completionHandler:^{
              [expectation fulfill];
          }];
    
    [self waitForExpectations:@[expectation] timeout:0.5];
}

- (void)testFlyToCompletionHandler {
    
    MGLCoordinateBounds bounds = MGLCoordinateBoundsMake(CLLocationCoordinate2DMake(0.0, 0.0),
                                                         CLLocationCoordinate2DMake(1.0, 1.0));
    MGLMapCamera *camera = [self.mapView cameraThatFitsCoordinateBounds:bounds];
    
    XCTestExpectation *expectation = [self expectationWithDescription:@"Completion block should be called"];
    
    [self.mapView flyToCamera:camera
                 withDuration:0.0
            completionHandler:^{
                [expectation fulfill];
            }];
    
    [self waitForExpectations:@[expectation] timeout:0.5];
}

- (void)testFlyToCompletionHandlerAnimated {
    
    MGLCoordinateBounds bounds = MGLCoordinateBoundsMake(CLLocationCoordinate2DMake(0.0, 0.0),
                                                         CLLocationCoordinate2DMake(1.0, 1.0));
    MGLMapCamera *camera = [self.mapView cameraThatFitsCoordinateBounds:bounds];
    
    XCTestExpectation *expectation = [self expectationWithDescription:@"Completion block should be called"];
    
    [self.mapView flyToCamera:camera
                 withDuration:0.3
            completionHandler:^{
                [expectation fulfill];
            }];
    
    [self waitForExpectations:@[expectation] timeout:0.5];
}
@end

#pragma mark - camera transitions with NaN values

@interface MGLMapView (MGLCameraTransitionFinishNaNTests)
- (mbgl::CameraOptions)cameraOptionsObjectForAnimatingToCamera:(MGLMapCamera *)camera edgePadding:(UIEdgeInsets)insets;
@end

@interface MGLCameraTransitionNaNZoomMapView: MGLMapView
@end

@implementation MGLCameraTransitionNaNZoomMapView
- (mbgl::CameraOptions)cameraOptionsObjectForAnimatingToCamera:(MGLMapCamera *)camera edgePadding:(UIEdgeInsets)insets {
    mbgl::CameraOptions options = [super cameraOptionsObjectForAnimatingToCamera:camera edgePadding:insets];
    options.zoom = NAN;
    return options;
}
@end

// Subclass the entire test suite, but with a different MGLMapView subclass
@interface MGLCameraTransitionFinishNaNTests : MGLCameraTransitionFinishTests
@end

@implementation MGLCameraTransitionFinishNaNTests
- (MGLMapView *)mapViewForTestWithFrame:(CGRect)rect styleURL:(NSURL *)styleURL {
    return [[MGLCameraTransitionNaNZoomMapView alloc] initWithFrame:rect styleURL:styleURL];
}
@end

