#import "MGLMapViewTests.h"

@interface MGLSourceTests : MGLMapViewTests

@end

@implementation MGLSourceTests

- (void)testDuplicateSources {
    MGLVectorSource *source1 = [[MGLVectorSource alloc] initWithIdentifier:@"my-source" URL:[NSURL URLWithString:@"mapbox://mapbox.mapbox-terrain-v2"]];
    MGLVectorSource *source2 = [[MGLVectorSource alloc] initWithIdentifier:@"my-source" URL:[NSURL URLWithString:@"mapbox://mapbox.mapbox-terrain-v2"]];
    
    [self.mapView.style addSource: source1];
    
    @try {
        [self.mapView.style addSource: source2];
        XCTFail(@"Should not have reached this point");
    }
    @catch (NSException *e) {
        XCTAssertNotNil(e, @"Should have thrown an exception");
    }
}

@end
