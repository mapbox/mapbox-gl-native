#import <Mapbox/Mapbox.h>
#import <XCTest/XCTest.h>

@interface MGLSourceQueryTests : XCTestCase <MGLMapViewDelegate>

@end

@implementation MGLSourceQueryTests

- (void) testQueryVectorSource {
    MGLVectorSource *source = [[MGLVectorSource alloc] initWithIdentifier:@"vector" tileURLTemplates:@[@"fake"] options:nil];
    NSSet *sourceLayers = [NSSet setWithObjects:@"buildings", @"water", nil];
    NSArray* features = [source featuresInSourceLayersWithIdentifiers:sourceLayers predicate:nil];
    // Source not added yet, so features is 0
    XCTAssertEqual([features count], 0);
}

- (void) testQueryShapeSource {
    MGLShapeSource *source = [[MGLShapeSource alloc] initWithIdentifier:@"shape" shape:[MGLShapeCollection shapeCollectionWithShapes:@[]] options:nil];
    NSArray* features = [source featuresMatchingPredicate:nil];
    // Source not added yet, so features is 0
    XCTAssertEqual([features count], 0);
}

@end
