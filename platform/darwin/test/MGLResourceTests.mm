#import <Mapbox/Mapbox.h>
#import <XCTest/XCTest.h>
#import <mbgl/storage/resource.hpp>

namespace mbgl {
    extern NSURL *resourceURLWithAccountType(const Resource& resource, NSInteger accountType);
}
    
@interface MGLResourceTests : XCTestCase
@end

@implementation MGLResourceTests

- (void)testOfflineQueryParameterIsAddedForOfflineResource {
    
    using namespace mbgl;
    
    std::string testURL = "test://mapbox.com/testing_offline_query?a=one&b=two";
    
    // Is our test URL "correct" for subsequent checks?
    {
        NSURL *url = [NSURL URLWithString:@(testURL.c_str())];
        NSURLComponents *components = [NSURLComponents componentsWithURL:url resolvingAgainstBaseURL:NO];
        NSArray<NSURLQueryItem *> *items = components.queryItems;
        XCTAssert(items.count == 2 );
    }

    Resource resource(Resource::Kind::Unknown, testURL);

    // By default, resource are NOT offline
    {
        NSURL *url = resourceURLWithAccountType(resource, 0);
        NSURLComponents *components = [NSURLComponents componentsWithURL:url resolvingAgainstBaseURL:NO];
        for (NSURLQueryItem *item in components.queryItems) {
            XCTAssertFalse([item.name isEqualToString:@"offline"]);
        }
    }
    
    // Now check offline
    resource.setUsage(Resource::Usage::Offline);
    
    {
        NSURL *url = resourceURLWithAccountType(resource, 0);
        NSURLComponents *components = [NSURLComponents componentsWithURL:url resolvingAgainstBaseURL:NO];
        
        // For offline, we expect a single offline param and a sku param
        NSInteger foundCount = 0;
        
#if TARGET_OS_IOS
        for (NSURLQueryItem *item in components.queryItems) {
            if (([item.name isEqualToString:@"offline"] && [item.value isEqualToString:@"true"]) ||
                ([item.name isEqualToString:@"a"] && [item.value isEqualToString:@"one"]) ||
                ([item.name isEqualToString:@"b"] && [item.value isEqualToString:@"two"]) ||
                ([item.name isEqualToString:@"sku"])) {
                foundCount++;
            }
        }

        XCTAssert(foundCount == 4);
#else
        // NOTE: Currently the macOS and tvOS SDKs do not supply the sku or offline query parameters
        for (NSURLQueryItem *item in components.queryItems) {
            if (([item.name isEqualToString:@"a"] && [item.value isEqualToString:@"one"]) ||
                ([item.name isEqualToString:@"b"] && [item.value isEqualToString:@"two"])) {
                foundCount++;
            }
        }
        
        XCTAssert(foundCount == 2);
#endif
    }
}

@end
