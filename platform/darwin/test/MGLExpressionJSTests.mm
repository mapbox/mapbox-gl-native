#import <XCTest/XCTest.h>

#import "MGLTypes.h"
#import "NSExpression+MGLPrivateAdditions.h"

@interface MGLExpressionJSTests : XCTestCase

@end

@implementation MGLExpressionJSTests

- (void)testAllJavascriptTests {
    NSString *testRootPath = @"expression-tests";

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

            if (!jsonDict && error) {
                XCTFail(@"================> JSON parse failed with error: %@", error.localizedDescription);
                continue;
            }

            try {
                [self runTestsWithDictionary:jsonDict];
            } catch (NSException *e) {
                XCTFail(@"================> File at URL: %@ failed with reason: %@", fileURL, e.reason);
                continue;
            }
        }
    }
}

NSSet *testsToSkip = [NSSet setWithObjects:
                      @"to-boolean",
                      @"concat/arity-1",
                      @"plus/arity-1",
                      @"minus/arity-0",
                      @"minus/arity-1",
                      @"times/arity-1",
                      nil];

- (BOOL)shouldSkipTest:(NSURL *)fileURL {
    NSPredicate *blacklistPredicate = [NSPredicate predicateWithBlock:^BOOL( NSString * _Nullable evaluatedString, NSDictionary<NSString *,id> * _Nullable bindings) {
        return [fileURL.absoluteString containsString:evaluatedString];
    }];

    BOOL shouldSkip = [testsToSkip filteredSetUsingPredicate:blacklistPredicate].count > 0;

    if (shouldSkip) {
        XCTFail(@"================> Skipping test due to blacklisted file URL: %@", fileURL);
    }

    return shouldSkip;
}

- (void)runTestsWithDictionary:(NSDictionary *)testcase {
    NSArray *inputs = testcase[@"inputs"];

    NSExpression *exp = [NSExpression mgl_expressionWithJSONObject:testcase[@"expression"]];

    for (int i = 0; i < inputs.count; i++) {
        NSArray *input = inputs[i];
        NSDictionary *actualInput = [input[1] objectForKey:@"properties"];

        id expressionValue = [exp expressionValueWithObject:actualInput context:[NSMutableDictionary dictionary]];
        id expectedValue = testcase[@"expected"][@"outputs"][i];
        XCTAssertEqualObjects(expressionValue, expectedValue);
    }
}

@end
