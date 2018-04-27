#import <XCTest/XCTest.h>

#import "MGLTypes.h"
#import "NSExpression+MGLPrivateAdditions.h"

@interface MGLExpressionJSTests : XCTestCase

@end

@implementation MGLExpressionJSTests

NSString *testRootPath = @"expression-tests";

NSSet *testsToSkip = [NSSet setWithObjects:
    /* crashers */
    @"to-boolean", // EXC_BAD_ACCESS (code=1, address=0x1)
    @"concat/arity-1", // EXC_BAD_ACCESS (code=1, address=0x1)
    @"plus/arity-1", // EXC_BAD_ACCESS (code=EXC_I386_GPFLT)
    @"minus/arity-0", // EXC_BAD_ACCESS (code=EXC_I386_GPFLT)
    @"minus/arity-1", // EXC_BAD_ACCESS (code=EXC_I386_GPFLT)
    @"times/arity-1", // EXC_BAD_ACCESS (code=EXC_I386_GPFLT)

    /* not yet implemented */
    @"/log2",
    @"/geometry-type",
    @"/pow",
    @"/not_equal",
    @"/not",
    @"/less",
    @"/greater",
    @"/array",
    @"/match",
    @"/less_or_equal",
    @"/to-rgba",
    @"/rgba",
    @"/rgb",
    @"/sin",
    @"/acos",
    @"/cos",
    @"/step",
    @"/all",
    @"/to-color",
    @"/number",
    @"/to-number",
    @"/any",
    @"/object",
    @"/boolean",
    @"/coalesce",
    @"/tan",
    @"/atan",
    @"/typeof",
    @"/equal",
    @"/has",
    @"/string",
    @"/ln2",
    @"/interpolate", // Interpolation expressions lack underlying Objective-C implementations.
    @"/asin",
    @"/max",
    @"/properties",
    @"/id",
    @"/at",

    /* unmet dependencies */
    @"get/from-literal", // number
    @"get/basic", // number
    @"get/from-literal--missing", // number
    @"get/from-object-property", // number
    @"to-string/color", // rgba
    @"let/property-function", // number
    @"min/arity-0", // all
    @"at/basic", // number
    @"case/precedence", // boolean
    @"constant-folding/evaluation-error", // interpolation
    @"zoom/nested-coalesce", // coalesce
    @"zoom/invalid-nested-4", // coalesce
    @"case/infer-array-type", // boolean

    /* failures */
    @"concat/basic", // ((expressionValue) equal to (expectedValue)) failed: ("ab") is not equal to ("abc")
    @"concat/arity-0", // *** -[NSArray subarrayWithRange:]: range {1, 18446744073709551615} extends beyond bounds for empty array
    @"heatmap-density/basic", // Can't get value for 'heatmapDensity' in bindings { }.
    @"literal/nested-array", // -[__NSCFNumber isEqualToString:]: unrecognized selector sent to instance 0xb000000000000033
    @"literal/multiple-args", // ((expressionValue) equal to (expectedValue)) failed: ("{ }") is not equal to ("(null)")
    @"times/arity-0", // *** -[__NSPlaceholderArray initWithObjects:count:]: attempt to insert nil object from objects[0]
    @"parse/empty", // *** -[NSArray subarrayWithRange:]: range {1, 18446744073709551615} extends beyond bounds for empty array
    @"parse/non-string", // -[__NSCFNumber isEqualToString:]: unrecognized selector sent to instance 0xb000000000000013
    @"pi/basic", // ((expressionValue) equal to (expectedValue)) failed: ("3.141592653589793") is not equal to ("3.14159")
    @"e/basic", // ((expressionValue) equal to (expectedValue)) failed: ("2.718281828459045") is not equal to ("2.71828")
    @"to-string/basic", // ((expressionValue) equal to (expectedValue)) failed: ("0") is not equal to ("false")
    @"let/shadow", // Can't get value for 'a' in bindings { }
    @"let/basic", // Can't get value for 'a' in bindings { }
    @"let/zoom", // Can't get value for 'zoomLevel' in bindings { }
    @"let/nested", // Can't get value for 'a' in bindings { }
    @"case/basic", // Unrecognized expression conditional operator get.
    @"zoom/invalid-no-curve", // Can't get value for 'zoomLevel' in bindings { }
    @"zoom/nested-let", // Can't get value for 'zoomLevel' in bindings { }
    @"zoom/basic", // Can't get value for 'zoomLevel' in bindings { }
    @"zoom/invalid-nested-1", // Can't get value for 'zoomLevel' in bindings { }
    @"zoom/invalid-nested-2", // Can't get value for 'zoomLevel' in bindings { }
    @"zoom/invalid-nested-3", // Can't get value for 'zoomLevel' in bindings { }
    @"zoom/invalid-nested-5", // *** -[__NSPlaceholderArray initWithObjects:count:]: attempt to insert nil object from objects[1]
    @"plus/arity-0", // *** -[__NSPlaceholderArray initWithObjects:count:]: attempt to insert nil object from objects[0]
    @"plus/arity-1", // *** -[__NSPlaceholderArray initWithObjects:count:]: attempt to insert nil object from objects[0]

    /* intentional failures */
    @"length/invalid", // Invoked count with non-collection parameter.
    @"length/implicit", // Invoked count with non-collection parameter.
    @"parse/unknown-expression", // Expression operator FAKE-EXPRESSION not yet implemented.

    nil
];

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
                NSLog(@"Running %@", testName);
                [self runTestsNamed:testName withDictionary:jsonDict];
            } catch (NSException *e) {
                XCTFail(@"%@: %@", testName, e.reason);
                continue;
            }
        }
    }
}

- (BOOL)shouldSkipTest:(NSURL *)fileURL {
    NSPredicate *blacklistPredicate = [NSPredicate predicateWithBlock:^BOOL( NSString * _Nullable evaluatedString, NSDictionary<NSString *,id> * _Nullable bindings) {
        return [fileURL.absoluteString containsString:evaluatedString];
    }];

    BOOL shouldSkip = [testsToSkip filteredSetUsingPredicate:blacklistPredicate].count > 0;

    if (shouldSkip) {
        NSLog(@"Skipping blacklisted test: %@", [self testNameFromURL:fileURL]);
    }

    return shouldSkip;
}

- (void)runTestsNamed:(NSString *)testName withDictionary:(NSDictionary *)testcase {
    NSArray *inputs = testcase[@"inputs"];

    NSExpression *exp = [NSExpression expressionWithMGLJSONObject:testcase[@"expression"]];

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
