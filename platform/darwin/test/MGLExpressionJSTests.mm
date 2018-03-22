#import <XCTest/XCTest.h>

#import "MGLTypes.h"
#import "NSExpression+MGLPrivateAdditions.h"

@interface MGLExpressionJSTests : XCTestCase

@end

@implementation MGLExpressionJSTests

NSString *testRootPath = @"expression-tests";

- (void)testAllJavascriptTests {
    NSBundle *bundle = [NSBundle bundleForClass:[self class]];
    NSURL *rootURL = [[bundle bundleURL] URLByAppendingPathComponent:testRootPath];

    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSDirectoryEnumerator *enumerator = [fileManager enumeratorAtURL:rootURL
                                          includingPropertiesForKeys:@[NSURLIsDirectoryKey]
                                                             options:0
                                                        errorHandler:nil];
    for (NSURL *fileURL in enumerator) {
        if ([[fileURL lastPathComponent] isEqual:@"test.json"] && ![self shouldSkipTest:fileURL] ) {
            NSData *data = [NSData dataWithContentsOfURL:fileURL];
            NSError *error = nil;
            NSDictionary *jsonDict = [NSJSONSerialization JSONObjectWithData:data options:0 error:&error];
            NSString *testName = [self testNameFromURL:fileURL];

            if (!jsonDict && error) {
                XCTFail(@"JSON parse failed with error: %@", error.localizedDescription);
                continue;
            }

            try {
                [self runTestsNamed:testName withDictionary:jsonDict];
            } catch (NSException *e) {
                XCTFail(@"%@: %@", testName, e.reason);
                continue;
            }
        }
    }
}

NSSet *testsToSkip = [NSSet setWithObjects:
                      @"to-boolean", // EXC_BAD_ACCESS (code=1, address=0x1)
                      @"concat/arity-1", // EXC_BAD_ACCESS (code=1, address=0x1)
                      @"plus/arity-1", // EXC_BAD_ACCESS (code=EXC_I386_GPFLT)
                      @"minus/arity-0", // EXC_BAD_ACCESS (code=EXC_I386_GPFLT)
                      @"minus/arity-1", // EXC_BAD_ACCESS (code=EXC_I386_GPFLT)
                      @"times/arity-1", // EXC_BAD_ACCESS (code=EXC_I386_GPFLT)
                      nil];

- (BOOL)shouldSkipTest:(NSURL *)fileURL {
    NSPredicate *blacklistPredicate = [NSPredicate predicateWithBlock:^BOOL( NSString * _Nullable evaluatedString, NSDictionary<NSString *,id> * _Nullable bindings) {
        return [fileURL.absoluteString containsString:evaluatedString];
    }];

    BOOL shouldSkip = [testsToSkip filteredSetUsingPredicate:blacklistPredicate].count > 0;

    if (shouldSkip) {
        NSLog(@"Skipping test due to blacklisted file URL: %@", [self testNameFromURL:fileURL]);
    }

    return shouldSkip;
}

- (void)runTestsNamed:(NSString *)testName withDictionary:(NSDictionary *)testcase {
    NSArray *inputs = testcase[@"inputs"];

    NSExpression *exp = [NSExpression mgl_expressionWithJSONObject:testcase[@"expression"]];

    for (int i = 0; i < inputs.count; i++) {
        NSArray *input = inputs[i];
        NSDictionary *actualInput = [input[1] objectForKey:@"properties"];

        id expressionValue = [exp expressionValueWithObject:actualInput context:[NSMutableDictionary dictionary]];
        id expectedValue = testcase[@"expected"][@"outputs"][i];
        XCTAssertEqualObjects(expressionValue, expectedValue, @"in %@", testName);
    }
}

- (NSString *)testNameFromURL:(NSURL *)testURL {
    return [testURL.absoluteString componentsSeparatedByString:testRootPath].lastObject;
}

@end
