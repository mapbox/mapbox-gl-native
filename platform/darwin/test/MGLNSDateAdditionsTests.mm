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
    mbgl::Duration expectedDurationMiliSeconds = 5000ms;
    mbgl::Duration expectedDurationMicroSeconds = 5000000us;
    mbgl::Duration expectedDurationNanoSeconds = 5000000000ns;
    
    XCTAssertEqual(timeInterval, durationTimeInterval);
    XCTAssertEqual(timeInterval, MGLTimeIntervalFromDuration(expectedDuration));
    XCTAssertEqual(timeInterval, MGLTimeIntervalFromDuration(expectedDurationMiliSeconds));
    XCTAssertEqual(timeInterval, MGLTimeIntervalFromDuration(expectedDurationMicroSeconds));
    XCTAssertEqual(timeInterval, MGLTimeIntervalFromDuration(expectedDurationNanoSeconds));
    
    mbgl::Duration durationMiliSeconds = 2500ms;
    mbgl::Duration durationMicroSeconds = 2500000us;
    mbgl::Duration durationNanoSeconds = 2500000000ns;
    
    XCTAssertEqual(NSTimeInterval(2.5), MGLTimeIntervalFromDuration(durationMiliSeconds));
    XCTAssertEqual(NSTimeInterval(2.5), MGLTimeIntervalFromDuration(durationMicroSeconds));
    XCTAssertEqual(NSTimeInterval(2.5), MGLTimeIntervalFromDuration(durationNanoSeconds));
    
}

@end
