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
        XCTAssert(items.count == 2);
    }

    Resource resource(Resource::Kind::Unknown, testURL);

    // By default, resources are NOT offline
    {
        bool skuTokenQueryItemFound;
        NSURL *url = resourceURLWithAccountType(resource, 0);
        NSURLComponents *components = [NSURLComponents componentsWithURL:url resolvingAgainstBaseURL:NO];
        for (NSURLQueryItem *item in components.queryItems) {
            XCTAssertFalse([item.name isEqualToString:@"offline"]);
            if ([item.name isEqualToString:@"sku"]) {
                skuTokenQueryItemFound = YES;
            }
        }

#if TARGET_OS_IPHONE || TARGET_OS_SIMULATOR
        XCTAssertTrue(skuTokenQueryItemFound, "Default resource URL should have SKU token query item");
#else
        XCTAssertFalse(skuTokenQueryItemFound, "Non-iOS platforms should not have a SKU token query item");
#endif
    }
    
    // Now check offline
    resource.setUsage(Resource::Usage::Offline);
    
    {
        NSURL *url = resourceURLWithAccountType(resource, 0);
        NSURLComponents *components = [NSURLComponents componentsWithURL:url resolvingAgainstBaseURL:NO];
        
        // For offline, we expect a single offline query item
        NSInteger foundCount = 0;
        
#if TARGET_OS_IPHONE || TARGET_OS_SIMULATOR
        for (NSURLQueryItem *item in components.queryItems) {
            if (([item.name isEqualToString:@"offline"] && [item.value isEqualToString:@"true"]) ||
                ([item.name isEqualToString:@"a"] && [item.value isEqualToString:@"one"]) ||
                ([item.name isEqualToString:@"b"] && [item.value isEqualToString:@"two"])) {
                foundCount++;
            }
            XCTAssertFalse([item.name isEqualToString:@"sku"]);
        }

        XCTAssert(foundCount == 3);
#else
        // NOTE: Currently the macOS SDK does not supply the sku or offline query parameters
        for (NSURLQueryItem *item in components.queryItems) {
            if (([item.name isEqualToString:@"a"] && [item.value isEqualToString:@"one"]) ||
                ([item.name isEqualToString:@"b"] && [item.value isEqualToString:@"two"])) {
                foundCount++;
            }
            XCTAssertFalse([item.name isEqualToString:@"sku"]);
        }
        
        XCTAssert(foundCount == 2);
#endif
    }
}

@end
