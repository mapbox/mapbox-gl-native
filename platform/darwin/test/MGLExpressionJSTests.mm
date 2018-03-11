#import <XCTest/XCTest.h>

#import "MGLTypes.h"
#import "NSExpression+MGLPrivateAdditions.h"

@interface MGLExpressionJSTests : XCTestCase

@end

@implementation MGLExpressionJSTests

- (void)testAllJavascriptTests {

    NSSet *crashes = [NSSet setWithObjects:
                      @"to-boolean",
                      @"concat/arity-1",
                      @"plus/arity-1",
                      @"minus/arity-0",
                      @"minus/arity-1",
                      @"times/arity-1",
                      nil];

    NSString *testRootPath = @"expression-tests";

    NSBundle *bundle = [NSBundle bundleForClass:[self class]];
    NSURL *rootURL = [[bundle bundleURL] URLByAppendingPathComponent:testRootPath];

    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSDirectoryEnumerator *enumerator = [fileManager enumeratorAtURL:rootURL
                                          includingPropertiesForKeys:@[NSURLIsDirectoryKey]
                                                             options:0
                                                        errorHandler:nil];

    for (NSURL *fileURL in enumerator) {
        NSPredicate *blacklistPredicate = [NSPredicate predicateWithBlock:^BOOL( NSString * _Nullable evaluatedString, NSDictionary<NSString *,id> * _Nullable bindings) {
            return [fileURL.absoluteString containsString:evaluatedString];
        }];

        NSSet *possibleCrash = [crashes filteredSetUsingPredicate:blacklistPredicate];

        if (possibleCrash.count > 0) {
            NSLog(@"================> Skipping test due to blacklist: %@", fileURL);
            continue;
        }

        if ([[fileURL lastPathComponent] isEqual:@"test.json"]) {
            NSData *data = [NSData dataWithContentsOfURL:fileURL];
            NSError *error = nil;
            NSDictionary *jsonDict = [NSJSONSerialization JSONObjectWithData:data options:0 error:&error];

            if (!jsonDict && error) {
                NSLog(@"================> JSON parse failed with error: %@", error.localizedDescription);
                continue;
            }

            try {
                [self runTestsWithDictionary:jsonDict];
            } catch (NSException *e) {
                NSLog(@"================> File URL: %@", fileURL);
                NSLog(@"================> Exception: %@", e.reason);
                continue;
            }
        }
    }
}

- (void)runTestsWithDictionary:(NSDictionary *)testcase {
    NSArray *inputs = testcase[@"inputs"];

    NSExpression *exp = [NSExpression mgl_expressionWithJSONObject:testcase[@"expression"]];

    for (int i = 0; i < inputs.count; i++) {
        NSArray *input = inputs[i];
        NSDictionary *actualInput = [input[1] objectForKey:@"properties"];
        id expIn = [exp expressionValueWithObject:actualInput context:[NSMutableDictionary dictionary]];
        id expOut = testcase[@"expected"][@"outputs"][i];
        XCTAssertEqualObjects(expIn, expOut);
    }
}

- (void)xtestSqrt {
    NSString *json = @"{\
        \"expression\": [\"sqrt\", [\"get\", \"x\"]],\
        \"inputs\": [[{}, {\"properties\": {\"x\": 4}}], [{}, {\"properties\": {\"x\": 0.25}}]],\
        \"expected\": {\
            \"compiled\": {\
                \"result\": \"success\",\
                \"isFeatureConstant\": false,\
                \"isZoomConstant\": true,\
                \"type\": \"number\"\
            },\
            \"outputs\": [2, 0.5],\
            \"serialized\": [\"sqrt\", [\"number\", [\"get\", \"x\"]]]\
        }\
    }";

    NSData *data = [json dataUsingEncoding:NSASCIIStringEncoding];
    NSError *error;
    NSDictionary *jsonDict = [NSJSONSerialization JSONObjectWithData:data options:0 error:&error];

    if (!jsonDict && error != nil) {
        XCTFail(@"Failed to parse: %@", error.localizedDescription);
    }

    [self runTestsWithDictionary:jsonDict];
}

- (void)xtestUpcase {
    NSString *json = @"{\
        \"expression\": [\"upcase\", \"string\"],\
        \"inputs\": [[{}, {}]],\
        \"expected\": {\
            \"compiled\": {\
                \"result\": \"success\",\
                \"isFeatureConstant\": true,\
                \"isZoomConstant\": true,\
                \"type\": \"string\"\
            },\
            \"outputs\": [\"STRING\"],\
            \"serialized\": \"STRING\"\
        }\
    }";

    NSData *data = [json dataUsingEncoding:NSASCIIStringEncoding];
    NSError *error;
    NSDictionary *jsonDict = [NSJSONSerialization JSONObjectWithData:data options:0 error:&error];

    if (!jsonDict && error != nil) {
        NSLog(@"=====>e error %@", error);
    }

    [self runTestsWithDictionary:jsonDict];
}

- (void)xtestEqualsString {
    NSString *json = @"{\
        \"expression\": [\"==\", [\"string\", [\"get\", \"x\"]], [\"get\", \"y\"]],\
        \"inputs\": [\
                   [{}, {\"properties\": {\"x\": \"1\", \"y\": \"1\"}}],\
                   [{}, {\"properties\": {\"x\": \"1\", \"y\": 2}}],\
                   [{}, {\"properties\": {\"x\": \"1\", \"y\": 1}}]\
                   ],\
        \"expected\": {\
            \"compiled\": {\
                \"result\": \"success\",\
                \"isFeatureConstant\": false,\
                \"isZoomConstant\": true,\
                \"type\": \"boolean\"\
            },\
            \"outputs\": [true, false, false],\
            \"serialized\": [\"==\", [\"string\", [\"get\", \"x\"]], [\"get\", \"y\"]]\
        }\
    }";

    NSData *data = [json dataUsingEncoding:NSASCIIStringEncoding];
    NSError *error;
    NSDictionary *jsonDict = [NSJSONSerialization JSONObjectWithData:data options:0 error:&error];

    if (!jsonDict && error != nil) {
        NSLog(@"=====>e error %@", error);
    }

    XCTAssertThrows([NSExpression mgl_expressionWithJSONObject:jsonDict[@"expression"]]);
}

- (void)xtestZoomInterpolate {

    //TODO: this test case has a different format from the others we've looked at so far.
    NSString *json = @"{\
        \"expression\": [\"interpolate\", [\"linear\"], [\"zoom\"], 0, 0, 30, 30],\
        \"inputs\": [[{\"zoomLevel\": 5}, {}]],\
        \"expected\": {\
            \"compiled\": {\
                \"result\": \"success\",\
                \"isFeatureConstant\": true,\
                \"isZoomConstant\": false,\
                \"type\": \"number\"\
            },\
            \"outputs\": [5],\
            \"serialized\": [\"interpolate\", [\"exponential\", 1], [\"zoom\"], 0, 0, 30, 30]\
        }\
    }";

    NSData *data = [json dataUsingEncoding:NSASCIIStringEncoding];
    NSError *error;
    NSDictionary *jsonDict = [NSJSONSerialization JSONObjectWithData:data options:0 error:&error];

    if (!jsonDict && error != nil) {
        NSLog(@"=====>e error %@", error);
    }

    NSArray *inputs = jsonDict[@"inputs"];
    
    NSExpression *exp = [NSExpression mgl_expressionWithJSONObject:jsonDict[@"expression"]];

    for (NSArray *inputArray in inputs) {
        for (NSArray *input in inputArray) {
            NSDictionary *actualInput = input[0];
            [exp expressionValueWithObject:actualInput context:[NSMutableDictionary dictionary]];
        }
    }

    //NSArray *jsonObject = @[@"==", @{@"get": @"x"}];
    //NSExpression *exp = [NSExpression mgl_expressionWithJSONObject:jsonObject];
    // failed: caught "NSInvalidArgumentException", "Expression operator == not yet implemented."

    //NSArray *atan = @[@"atan", @1];
    //NSExpression *exp = [NSExpression mgl_expressionWithJSONObject:jsonObject];
    // failed: caught "NSInvalidArgumentException", "Expression operator atan not yet implemented."

//    NSExpression *expression = [NSExpression expressionForVariable:@"zoomLevel"];
//    XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, @[@"zoom"]);
//    XCTAssertEqualObjects([NSExpression expressionWithFormat:@"$zoomLevel"].mgl_jsonExpressionObject, @[@"zoom"]);
//    XCTAssertEqualObjects([NSExpression mgl_expressionWithJSONObject:@[@"zoom"]], expression);
//    NSMutableDictionary *context = [@{@"zoomLevel": @16} mutableCopy];
//    XCTAssertEqualObjects([expression expressionValueWithObject:nil context:context], @16);
}

@end
