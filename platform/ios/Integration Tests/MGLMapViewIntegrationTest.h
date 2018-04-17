#import <XCTest/XCTest.h>
#import <Mapbox/Mapbox.h>

#define TestFailWithSelf(myself, ...) \
    _XCTPrimitiveFail(myself, __VA_ARGS__)

#define TestAssertWithSelf(expression, myself, ...) \
    _XCTPrimitiveAssertTrue(myself, expression, @#expression, __VA_ARGS__)

@interface MGLMapViewIntegrationTest : XCTestCase <MGLMapViewDelegate>
@property (nonatomic) MGLMapView *mapView;
@property (nonatomic) MGLStyle *style;
@property (nonatomic) XCTestExpectation *styleLoadingExpectation;
@property (nonatomic) XCTestExpectation *renderFinishedExpectation;
@property (nonatomic) void (^regionWillChange)(MGLMapView *mapView, BOOL animated);
@property (nonatomic) void (^regionIsChanging)(MGLMapView *mapView);
@property (nonatomic) void (^regionDidChange)(MGLMapView *mapView, MGLCameraChangeReason reason, BOOL animated);

// Utility methods
- (void)waitForMapViewToFinishLoadingStyleWithTimeout:(NSTimeInterval)timeout;
- (void)waitForMapViewToBeRenderedWithTimeout:(NSTimeInterval)timeout;
@end
