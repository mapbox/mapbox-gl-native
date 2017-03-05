#import <Mapbox/Mapbox.h>

#import "NSBundle+MGLAdditions.h"

#import <XCTest/XCTest.h>
#if TARGET_OS_IPHONE
    #import <UIKit/UIKit.h>
#else
    #import <Cocoa/Cocoa.h>
#endif

@interface MGLSourceQueryTests : XCTestCase <MGLMapViewDelegate>

@property (nonatomic) MGLMapView *mapView;
@property (nonatomic) MGLStyle *style;

@end

@implementation MGLSourceQueryTests {
    XCTestExpectation *_styleLoadingExpectation;
}

- (void)setUp {
    [super setUp];
    
    [MGLAccountManager setAccessToken:@"pk.feedcafedeadbeefbadebede"];
    NSURL *styleURL = [[NSBundle bundleForClass:[self class]] URLForResource:@"query-style" withExtension:@"json"];
    self.mapView = [[MGLMapView alloc] initWithFrame:CGRectMake(0, 0, 100, 100) styleURL:styleURL];
    self.mapView.delegate = self;
    if (!self.mapView.style) {
        _styleLoadingExpectation = [self expectationWithDescription:@"Map view should finish loading style."];
        [self waitForExpectationsWithTimeout:1 handler:nil];
    }
}

- (void)mapView:(MGLMapView *)mapView didFinishLoadingStyle:(MGLStyle *)style {
    XCTAssertNotNil(mapView.style);
    XCTAssertEqual(mapView.style, style);
    
    [_styleLoadingExpectation fulfill];
}

- (void)tearDown {
    _styleLoadingExpectation = nil;
    self.mapView = nil;
    
    [super tearDown];
}

- (MGLStyle *)style {
    return self.mapView.style;
}

- (void) testQueryVectorSource {
    MGLVectorSource *source = (MGLVectorSource *)[self.style sourceWithIdentifier:@"source5"];
    
    NSSet *sourceLayers = [NSSet setWithObjects:@"buildings", @"water", nil];
    NSArray* features = [source featuresInSourceLayersWithIdentifiers:sourceLayers predicate:nil];
    // Source won't be loaded yet, so features is 0
    XCTAssertEqual([features count], 0);
}

- (void) testQueryShapeSource {
    MGLShapeSource *source = (MGLShapeSource *)[self.style sourceWithIdentifier:@"source4"];
    
    NSPredicate *eqPredicate = [NSPredicate predicateWithFormat:@"key1 == 'value1'"];
    NSArray* features = [source featuresMatchingPredicate:eqPredicate];
    // Source won't be loaded yet, so features is 0
    XCTAssertEqual([features count], 0);
    
}

@end
