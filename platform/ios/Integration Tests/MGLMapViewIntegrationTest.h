#import <XCTest/XCTest.h>
#import <Mapbox/Mapbox.h>
#import "MGLTestUtility.h"

#define MGLTestFail(myself, ...) \
    _XCTPrimitiveFail(myself, __VA_ARGS__)

#define MGLTestAssert(myself, expression, ...) \
    _XCTPrimitiveAssertTrue(myself, expression, @#expression, __VA_ARGS__)

#define MGLTestAssertEqualWithAccuracy(myself, expression1, expression2, accuracy, ...) \
    _XCTPrimitiveAssertEqualWithAccuracy(myself, expression1, @#expression1, expression2, @#expression2, accuracy, @#accuracy, __VA_ARGS__)
#define MGLTestAssertNil(myself, expression, ...) \
    _XCTPrimitiveAssertNil(myself, expression, @#expression, __VA_ARGS__)

#define MGLTestAssertNotNil(myself, expression, ...) \
    _XCTPrimitiveAssertNotNil(myself, expression, @#expression, __VA_ARGS__)

#define MGLTestWarning(expression, format, ...) \
({ \
    if (!(expression))  { \
        NSString *message = [NSString stringWithFormat:format, ##__VA_ARGS__]; \
        printf("warning: Test Case '%s' at line %d: '%s' %s\n", __PRETTY_FUNCTION__, __LINE__, #expression, message.UTF8String); \
    } \
})

@interface MGLMapViewIntegrationTest : XCTestCase <MGLMapViewDelegate>
@property (nonatomic) MGLMapView *mapView;
@property (nonatomic) UIWindow *window;
@property (nonatomic) MGLStyle *style;
@property (nonatomic) XCTestExpectation *styleLoadingExpectation;
@property (nonatomic) XCTestExpectation *renderFinishedExpectation;
@property (nonatomic) MGLAnnotationView * (^viewForAnnotation)(MGLMapView *mapView, id<MGLAnnotation> annotation);
@property (nonatomic) void (^regionWillChange)(MGLMapView *mapView, BOOL animated);
@property (nonatomic) void (^regionIsChanging)(MGLMapView *mapView);
@property (nonatomic) void (^regionDidChange)(MGLMapView *mapView, MGLCameraChangeReason reason, BOOL animated);
@property (nonatomic) CGPoint (^mapViewUserLocationAnchorPoint)(MGLMapView *mapView);
@property (nonatomic) BOOL (^mapViewAnnotationCanShowCalloutForAnnotation)(MGLMapView *mapView, id<MGLAnnotation> annotation);
@property (nonatomic) id<MGLCalloutView> (^mapViewCalloutViewForAnnotation)(MGLMapView *mapView, id<MGLAnnotation> annotation);

// Utility methods
- (NSString*)validAccessToken;
- (void)waitForMapViewToFinishLoadingStyleWithTimeout:(NSTimeInterval)timeout;
- (void)waitForMapViewToBeRenderedWithTimeout:(NSTimeInterval)timeout;
@end
