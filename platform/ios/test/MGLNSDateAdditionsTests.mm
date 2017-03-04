#import <XCTest/XCTest.h>

#include <mbgl/util/chrono.hpp>
#import "../../darwin/src/NSDate+MGLAdditions.h"

using namespace std::chrono_literals;

@interface MGLNSDateAdditionsTests : XCTestCase
@end

@implementation MGLNSDateAdditionsTests

- (void)testDurationToNSTimeInterval {
    
    NSTimeInterval timeInterval = 5;
    mbgl::Duration duration = MGLDurationFromTimeInterval(timeInterval);
    NSTimeInterval durationTimeInterval = MGLTimeIntervalFromDuration(duration);
    
    mbgl::Duration expectedDuration = 5s;
    
    XCTAssertEqual(timeInterval, durationTimeInterval);
    XCTAssertEqual(timeInterval, MGLTimeIntervalFromDuration(expectedDuration));
    
}

@end
