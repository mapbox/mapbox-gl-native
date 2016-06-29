#import <XCTest/XCTest.h>

#import "../../darwin/src/NSData+MGLAdditions.h"

@interface MGLNSDataAdditionsTests : XCTestCase
@end

@implementation MGLNSDataAdditionsTests

- (void)setUp {
    [super setUp];
}

- (void)testCompressDecompress
{
    NSArray *originalArray = [self mockDataWithCount:180];
    
    NSData *originalData = [NSJSONSerialization dataWithJSONObject:originalArray options:0 error:nil];
    
    NSData *compressedData = [originalData mgl_compressedData];
    NSData *decompressedData = [compressedData mgl_decompressedData];
    
    NSArray *decompressedArray = [NSJSONSerialization JSONObjectWithData:decompressedData options:0 error:nil];
    
    XCTAssertTrue([originalArray isEqualToArray:decompressedArray], @"originalDict and decompressedDict should be equal");
}

- (NSArray *)mockDataWithCount:(NSUInteger)count
{
    NSMutableArray *array = [NSMutableArray array];
    for (NSUInteger i=0;i<count;i++) {
        [array addObject:@{@"lat": @(@([self randomFloatBetween:-90 and:90]).integerValue),
                           @"lng": @(@([self randomFloatBetween:-180 and:180]).integerValue),
                           @"timestamp": @(@([[NSDate date] timeIntervalSince1970]).integerValue)}];
    }
    return array;
}

- (float)randomFloatBetween:(float)lowerBound and:(float)upperBound {
    float diff = upperBound - lowerBound;
    return (((float) (arc4random() % ((unsigned)RAND_MAX + 1)) / RAND_MAX) * diff) + lowerBound;
}

@end
