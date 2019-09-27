#import <Mapbox/Mapbox.h>
#import <XCTest/XCTest.h>
#import "MGLNetworkConfiguration_Private.h"
#include <mbgl/storage/network_status.hpp>


@interface MGLNetworkConfigurationTests : XCTestCase
@end

@implementation MGLNetworkConfigurationTests

// Regression test for https://github.com/mapbox/mapbox-gl-native/issues/14982
- (void)testAccessingEventsFromMultipleThreads {
    MGLNetworkConfiguration *configuration = [[MGLNetworkConfiguration alloc] init];
    
    // Concurrent
    dispatch_queue_t queue = dispatch_queue_create("com.mapbox.testAccessingEventsFromMultipleThreads", DISPATCH_QUEUE_CONCURRENT);
    
    NSUInteger numberOfConcurrentBlocks = 20;
    
    XCTestExpectation *expectation = [self expectationWithDescription:@"wait-for-threads"];
    expectation.expectedFulfillmentCount = numberOfConcurrentBlocks;
    
    for (NSUInteger i = 0; i < numberOfConcurrentBlocks; i++) {
        
        NSString *event = [NSString stringWithFormat:@"test://event-%ld", i];
        NSString *resourceType = @"test";
        
        dispatch_async(queue, ^{
            [configuration startDownloadEvent:event type:resourceType];
            
            NSURL *url = [NSURL URLWithString:event];
            NSURLResponse *response = [[NSURLResponse alloc] initWithURL:url MIMEType:nil expectedContentLength:0 textEncodingName:nil];
            
            [configuration stopDownloadEventForResponse:response];
            
            dispatch_async(dispatch_get_main_queue(), ^{
                [expectation fulfill];
            });
        });
    }
    
    [self waitForExpectations:@[expectation] timeout:10.0];
}

- (void)testMBGLNetworkStatus
{
    auto currentStatus = mbgl::NetworkStatus::Get();

    mbgl::NetworkStatus::Set(mbgl::NetworkStatus::Status::Offline);
    auto networkStatus = mbgl::NetworkStatus::Get();
    
    // This will fail if libmbgl-core is linked twice.
    XCTAssertEqual(networkStatus, mbgl::NetworkStatus::Status::Offline);

    // Reset
    mbgl::NetworkStatus::Set(currentStatus);
}
@end
