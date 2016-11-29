#import <Mapbox/Mapbox.h>
#import "MGLRuntimeStylingHelper.h"
#import <XCTest/XCTest.h>

@interface MGLMapViewTests : XCTestCase <MGLMapViewDelegate>

@property (nonatomic) IBOutlet MGLMapView *mapView;
@property (nonatomic) XCTestExpectation *expectation;

@end
