#import "MGLStyle.h"

#import "NSBundle+MGLAdditions.h"

#import <mbgl/util/default_styles.hpp>

#import <XCTest/XCTest.h>
#import <objc/runtime.h>

@interface MGLStyleTests : XCTestCase
@end

@implementation MGLStyleTests

- (void)testUnversionedStyleURLs {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
    XCTAssertEqualObjects([MGLStyle streetsStyleURL].absoluteString, @"mapbox://styles/mapbox/streets-v8");
    XCTAssertEqualObjects([MGLStyle emeraldStyleURL].absoluteString, @"mapbox://styles/mapbox/emerald-v8");
    XCTAssertEqualObjects([MGLStyle lightStyleURL].absoluteString, @"mapbox://styles/mapbox/light-v8");
    XCTAssertEqualObjects([MGLStyle darkStyleURL].absoluteString, @"mapbox://styles/mapbox/dark-v8");
    XCTAssertEqualObjects([MGLStyle satelliteStyleURL].absoluteString, @"mapbox://styles/mapbox/satellite-v8");
    XCTAssertEqualObjects([MGLStyle hybridStyleURL].absoluteString, @"mapbox://styles/mapbox/satellite-hybrid-v8");
#pragma clang diagnostic pop
}

- (void)testVersionedStyleURLs {
    // Test that all the default styles have publicly-declared MGLStyle class
    // methods and that the URLs all have the right values.
    XCTAssertEqualObjects([MGLStyle streetsStyleURLWithVersion:MGLStyleDefaultVersion].absoluteString, @(mbgl::util::default_styles::streets.url));
    XCTAssertEqualObjects([MGLStyle outdoorsStyleURLWithVersion:MGLStyleDefaultVersion].absoluteString, @(mbgl::util::default_styles::outdoors.url));
    XCTAssertEqualObjects([MGLStyle lightStyleURLWithVersion:MGLStyleDefaultVersion].absoluteString, @(mbgl::util::default_styles::light.url));
    XCTAssertEqualObjects([MGLStyle darkStyleURLWithVersion:MGLStyleDefaultVersion].absoluteString, @(mbgl::util::default_styles::dark.url));
    XCTAssertEqualObjects([MGLStyle satelliteStyleURLWithVersion:MGLStyleDefaultVersion].absoluteString, @(mbgl::util::default_styles::satellite.url));
    XCTAssertEqualObjects([MGLStyle satelliteStreetsStyleURLWithVersion:MGLStyleDefaultVersion].absoluteString, @(mbgl::util::default_styles::satelliteStreets.url));
    
    static_assert(6 == mbgl::util::default_styles::numOrderedStyles,
                  "MGLStyleTests isn’t testing all the styles in mbgl::util::default_styles.");
}

- (void)testStyleURLDeclarations {
    // Make sure this test is comprehensive.
    const unsigned numImplicitArgs = 2 /* _cmd, self */;
    unsigned numMethods = 0;
    Method *methods = class_copyMethodList(object_getClass([MGLStyle class]), &numMethods);
    unsigned numVersionedMethods = 0;
    for (NSUInteger i = 0; i < numMethods; i++) {
        Method method = methods[i];
        SEL selector = method_getName(method);
        NSString *name = @(sel_getName(selector));
        unsigned numArgs = method_getNumberOfArguments(method);
        if ([name hasSuffix:@"StyleURL"]) {
            XCTAssertEqual(numArgs, numImplicitArgs, @"Unversioned style URL method should have no parameters, but it has %u.", numArgs - numImplicitArgs);
        } else if ([name hasSuffix:@"StyleURLWithVersion:"]) {
            XCTAssertEqual(numArgs, numImplicitArgs + 1, @"Versioned style URL method should have one parameter, but it has %u.", numArgs - numImplicitArgs);
            numVersionedMethods++;
        } else {
            XCTAssertEqual([name rangeOfString:@"URL"].location, NSNotFound, @"MGLStyle style URL method %@ is malformed.", name);
        }
    }
    XCTAssertEqual(mbgl::util::default_styles::numOrderedStyles, numVersionedMethods,
                   @"There are %lu default styles but MGLStyleTests only provides versioned style URL methods for %u of them.",
                   mbgl::util::default_styles::numOrderedStyles, numVersionedMethods);
    
    // Test that all the versioned style methods are in the public header.
    NSString *styleHeader = self.stringWithContentsOfStyleHeader;
    
    NSError *versionedMethodError;
    NSString *versionedMethodExpressionString = @(R"RE(^\+\s*\(NSURL\s*\*\s*\)\s*\w+StyleURLWithVersion\s*:\s*\(\s*NSInteger\s*\)\s*version\s*;)RE");
    NSRegularExpression *versionedMethodExpression = [NSRegularExpression regularExpressionWithPattern:versionedMethodExpressionString options:NSRegularExpressionAnchorsMatchLines error:&versionedMethodError];
    XCTAssertNil(versionedMethodError, @"Error compiling regular expression to search for versioned methods.");
    NSUInteger numVersionedMethodDeclarations = [versionedMethodExpression numberOfMatchesInString:styleHeader options:0 range:NSMakeRange(0, styleHeader.length)];
    XCTAssertEqual(numVersionedMethodDeclarations, numVersionedMethods);
    
    // Test that “current version is” statements are present and current for all versioned style methods.
    NSError *versionError;
    NSString *versionExpressionString = @(R"RE(the current version is `(\d+)`)RE");
    NSRegularExpression *versionExpression = [NSRegularExpression regularExpressionWithPattern:versionExpressionString options:0 error:&versionError];
    XCTAssertNil(versionError, @"Error compiling regular expression to search for current version statements.");
    NSUInteger numVersionDeclarations = [versionExpression numberOfMatchesInString:styleHeader options:0 range:NSMakeRange(0, styleHeader.length)];
    XCTAssertEqual(numVersionDeclarations, numVersionedMethods);
    [versionExpression enumerateMatchesInString:styleHeader options:0 range:NSMakeRange(0, styleHeader.length) usingBlock:^(NSTextCheckingResult * _Nullable result, NSMatchingFlags flags, BOOL * _Nonnull stop) {
        XCTAssertEqual(result.numberOfRanges, 2, @"Regular expression should have one capture group.");
        NSString *version = [styleHeader substringWithRange:[result rangeAtIndex:1]];
        XCTAssertEqual([version integerValue], MGLStyleDefaultVersion, @"Versioned style URL method should document current version as %ld, not %ld.", MGLStyleDefaultVersion, version.integerValue);
    }];
}

- (NSString *)stringWithContentsOfStyleHeader {
    NSURL *styleHeaderURL = [[[NSBundle mgl_frameworkBundle].bundleURL
                              URLByAppendingPathComponent:@"Headers" isDirectory:YES]
                             URLByAppendingPathComponent:@"MGLStyle.h"];
    NSError *styleHeaderError;
    NSString *styleHeader = [NSString stringWithContentsOfURL:styleHeaderURL usedEncoding:nil error:&styleHeaderError];
    XCTAssertNil(styleHeaderError, @"Error getting contents of MGLStyle.h.");
    return styleHeader;
}

@end
