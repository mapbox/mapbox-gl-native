#import <XCTest/XCTest.h>

#import "../../darwin/src/NSData+MGLAdditions.h"

@interface MGLNSDataAdditionsTests : XCTestCase
@end

@implementation MGLNSDataAdditionsTests

- (void)testCompressDecompress
{
    NSArray *originalArray = [self mockDataWithCount:180];

    NSData *originalData = [NSJSONSerialization dataWithJSONObject:originalArray options:0 error:nil];

    NSData *compressedData = [originalData mgl_compressedData];
    NSData *decompressedData = [compressedData mgl_decompressedData];

    NSArray *decompressedArray = [NSJSONSerialization JSONObjectWithData:decompressedData options:0 error:nil];

    XCTAssertTrue([originalArray isEqualToArray:decompressedArray], @"originalArray and decompressedArray should be equal");
}

- (NSArray *)mockDataWithCount:(NSUInteger)count
{
    NSMutableArray *array = [NSMutableArray array];

    for (NSUInteger i=0;i<count;i++)
    {
        [array addObject:@{@"lat": @([self safeValueBetween:-90 and:90]),
                           @"lng": @([self safeValueBetween:-180 and:180]),
                           @"timestamp": @((floor([NSDate date].timeIntervalSince1970) * 100) / 100)}];
    }

    return array;
}

- (double)safeValueBetween:(double)lowerBound and:(double)upperBound
{
    return floor([self randomBetween:lowerBound and:upperBound] * 100 ) / 100;
}

- (double)randomBetween:(double)lowerBound and:(double)upperBound
{
    return lowerBound * drand48() + upperBound * drand48();
}

@end
