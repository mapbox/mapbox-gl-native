#import <Mapbox/Mapbox.h>
#import "MGLRuntimeStylingHelper.h"
#import <XCTest/XCTest.h>

@interface MGLStyleLayerTests : XCTestCase <MGLMapViewDelegate>

@property (nonatomic) IBOutlet MGLMapView *mapView;
@property (nonatomic) XCTestExpectation *expectation;
@property (nonatomic, copy, readonly, class) NSString *layerType;

- (void)testPropertyName:(NSString *)name isBoolean:(BOOL)isBoolean;

@end

@interface NSString (MGLStyleLayerTestAdditions)

@property (nonatomic, readonly, copy) NS_ARRAY_OF(NSString *) *lexicalClasses;
@property (nonatomic, readonly, copy) NSString *lemma;

@end
