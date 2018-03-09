
#import <XCTest/XCTest.h>

#import "MGLTypes.h"
#import "NSExpression+MGLPrivateAdditions.h"

@interface MGLExpressionJSTests : XCTestCase

@end

@implementation MGLExpressionJSTests

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

- (void)testSqrt {
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
        NSLog(@"=====>e error %@", error);
    }

    NSArray *inputs = jsonDict[@"inputs"];

    NSExpression *exp = [NSExpression mgl_expressionWithJSONObject:jsonDict[@"expression"]];

    //    for (NSArray *inputArray in inputs) {
    for (int i = 0; i < inputs.count; i++) {
        NSArray *input = inputs[i];
        //TODO: context is the input[1]
        id expIn = [exp expressionValueWithObject:input[0] context:[NSMutableDictionary dictionary]];//[input[1] mutableCopy]];
        // TODO: get the correct output for this input
        id expOut = jsonDict[@"expected"][@"outputs"][i];
        XCTAssertEqualObjects(expIn, expOut);
    }
}

- (void)testUpcase {
    // TODO: load this from file
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

    NSArray *inputs = jsonDict[@"inputs"];

    NSExpression *exp = [NSExpression mgl_expressionWithJSONObject:jsonDict[@"expression"]];

    //    for (NSArray *inputArray in inputs) {
    for (int i = 0; i < inputs.count; i++) {
        NSArray *input = inputs[i];
        //TODO: context is the input[1]
        id expIn = [exp expressionValueWithObject:input[0] context:[input[1] mutableCopy]];
        // TODO: get the correct output for this input
        id expOut = jsonDict[@"expected"][@"outputs"][i];
        XCTAssertEqualObjects(expIn, expOut);
    }
}

- (void)testEqualsString {
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

- (void)testZoomInterpolate {

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
        for (NSDictionary *input in inputArray) {
            NSMutableDictionary *md = [input mutableCopy];
            [exp expressionValueWithObject:input context:md];
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
