#import <XCTest/XCTest.h>
#import <Mapbox/Mapbox.h>

#import "NSPredicate+MGLAdditions.h"
#import "MGLValueEvaluator.h"

namespace mbgl {
    namespace style {
        bool operator!=(const Filter &a, const Filter &b) {
            return !(a == b);
        }
    }
}

#define MGLAssertEqualFilters(actual, expected, ...) \
    XCTAssertTrue(actual.is<__typeof__(expected)>()); \
    if (actual.is<__typeof__(expected)>()) { \
        XCTAssertEqual(actual.get<__typeof__(expected)>(), expected, __VA_ARGS__); \
    }

@interface MGLPredicateTests : XCTestCase
@end

@implementation MGLPredicateTests

- (void)testPredicate {
    {
        NSValue *expected = @YES;
        XCTAssertEqualObjects([NSPredicate predicateWithValue:YES].mgl_jsonExpressionObject, expected);
        [self testSymmetryWithPredicate:[NSPredicate predicateWithValue:YES]
                          mustRoundTrip:NO];
    }
    {
        NSValue *expected = @NO;
        XCTAssertEqualObjects([NSPredicate predicateWithValue:NO].mgl_jsonExpressionObject, expected);
        [self testSymmetryWithPredicate:[NSPredicate predicateWithValue:YES]
                          mustRoundTrip:NO];
    }
}

- (void)testUnsupportedPredicates {
    XCTAssertThrowsSpecificNamed([NSPredicate predicateWithFormat:@"a BEGINSWITH 'L'"].mgl_filter, NSException, NSInvalidArgumentException);
    XCTAssertThrowsSpecificNamed([NSPredicate predicateWithFormat:@"a ENDSWITH 'itude'"].mgl_filter, NSException, NSInvalidArgumentException);
    XCTAssertThrowsSpecificNamed([NSPredicate predicateWithFormat:@"a LIKE 'glob?trotter'"].mgl_filter, NSException, NSInvalidArgumentException);
    XCTAssertThrowsSpecificNamed([NSPredicate predicateWithFormat:@"a MATCHES 'i\\w{18}n'"].mgl_filter, NSException, NSInvalidArgumentException);
    NSPredicate *selectorPredicate = [NSPredicate predicateWithFormat:@"(SELF isKindOfClass: %@)", [MGLPolyline class]];
    XCTAssertThrowsSpecificNamed(selectorPredicate.mgl_filter, NSException, NSInvalidArgumentException);
    
    XCTAssertThrowsSpecificNamed([NSPredicate predicateWithBlock:^BOOL(id _Nullable evaluatedObject, NSDictionary<NSString *, id> * _Nullable bindings) {
        XCTAssertTrue(NO, @"Predicate block should not be evaluated.");
        return NO;
    }].mgl_filter, NSException, NSInvalidArgumentException);
}

- (void)testComparisonPredicates {
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"1 == 2"];
        NSArray *jsonExpression = @[@"==", @1, @2];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        auto predicateFilter = predicate.mgl_filter;
        NSPredicate *predicateAfter = [NSPredicate mgl_predicateWithFilter:predicateFilter];
        XCTAssertEqualObjects([NSPredicate predicateWithValue:NO], predicateAfter);
            
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"1 == 1"];
        NSArray *expected = @[@"==", @1, @1];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, expected);
        auto predicateFilter = predicate.mgl_filter;
        NSPredicate *predicateAfter = [NSPredicate mgl_predicateWithFilter:predicateFilter];
        XCTAssertEqualObjects([NSPredicate predicateWithValue:YES], predicateAfter);
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"x == YES"];
        NSArray *jsonExpression = @[@"==", @[@"get", @"x"],  @YES];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate mgl_predicateWithJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"x < 5"];
        NSArray *jsonExpression = @[@"<", @[@"number", @[@"get", @"x"]], @[@"number", @5]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate mgl_predicateWithJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"x > 5"];
        NSArray *jsonExpression = @[@">", @[@"number", @[@"get", @"x"]], @[@"number", @5]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate mgl_predicateWithJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"x <= 5"];
        NSArray *jsonExpression = @[@"<=", @[@"number", @[@"get", @"x"]], @[@"number", @5]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate mgl_predicateWithJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"x >= 5"];
        NSArray *jsonExpression = @[@">=", @[@"number", @[@"get", @"x"]], @[@"number", @5]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate mgl_predicateWithJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"x > 'value'"];
        NSArray *jsonExpression = @[@">",  @[@"string", @[@"get", @"x"]], @[@"string", @"value"]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate mgl_predicateWithJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"a = 'b'"];
        NSArray *jsonExpression = @[@"==", @[@"get", @"a"], @"b"];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        [self testSymmetryWithPredicate:predicate
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"$mgl_geometryType = 'Point'"];
        NSArray *jsonExpression = @[@"==", @[@"geometry-type"], @"Point"];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        [self testSymmetryWithPredicate:predicate
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"$mgl_featureIdentifier = 67086180"];
        NSArray *jsonExpression = @[@"==", @[@"id"], @67086180];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        [self testSymmetryWithPredicate:predicate
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"$mgl_featureIdentifier = nil"];
        NSArray *jsonExpression = @[@"==", @[@"id"], [NSNull null]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        [self testSymmetryWithPredicate:predicate
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"a = nil"];
        NSArray *jsonExpression = @[@"==", @[@"get", @"a"], [NSNull null]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        [self testSymmetryWithPredicate:predicate
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"$mgl_geometryType != 'Point'"];
        NSArray *jsonExpression = @[@"!=", @[@"geometry-type"], @"Point"];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        [self testSymmetryWithPredicate:predicate
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"$mgl_featureIdentifier != 67086180"];
        NSArray *jsonExpression = @[@"!=", @[@"id"], @67086180];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        [self testSymmetryWithPredicate:predicate
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"$mgl_featureIdentifier != nil"];
        NSArray *jsonExpression = @[@"!=", @[@"id"],  [NSNull null]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        [self testSymmetryWithPredicate:predicate
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"a != 'b'"];
        NSArray *jsonExpression = @[@"!=", @[@"get", @"a"], @"b"];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        [self testSymmetryWithPredicate:predicate
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"a != nil"];
        NSArray *jsonExpression = @[@"!=", @[@"get", @"a"], [NSNull null]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        [self testSymmetryWithPredicate:predicate
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"a < 'b'"];
        NSArray *jsonExpression = @[@"<", @[@"string", @[@"get", @"a"]], @[@"string", @"b"]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate mgl_predicateWithJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"a <= 'b'"];
        NSArray *jsonExpression = @[@"<=", @[@"string", @[@"get", @"a"]], @[@"string", @"b"]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate mgl_predicateWithJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"a > 'b'"];
        NSArray *jsonExpression = @[@">", @[@"string", @[@"get", @"a"]], @[@"string", @"b"]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate mgl_predicateWithJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"a >= 'b'"];
        NSArray *jsonExpression = @[@">=", @[@"string", @[@"get", @"a"]], @[@"string", @"b"]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate mgl_predicateWithJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"a BETWEEN {'b', 'z'}"];
        NSArray *jsonExpression =@[@"all", @[@"<=", @[@"string", @"b"], @[@"string", @[@"get", @"x"]]], @[@"<=", @[@"string", @[@"get", @"x"]], @[@"string", @"z"]]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate mgl_predicateWithJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"a BETWEEN %@", @[@"b", @"z"]];
        NSArray *jsonExpression = @[@"all", @[@"<=", @[@"string", @"b"], @[@"string", @[@"get", @"x"]]], @[@"<=", @[@"string", @[@"get", @"x"]], @[@"string", @"z"]]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate mgl_predicateWithJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"a IN {'b', 'c'}"];
        NSArray *jsonExpression = @[@"match", @[@"get", @"a"], @[@"literal", @[@"b", @"c"]]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate mgl_predicateWithJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"a IN %@", @[@"b", @"c"]];
        NSArray *jsonExpression = @[@"match", @[@"get", @"a"], @[@"literal", @[@"b", @"c"]]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate mgl_predicateWithJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
    {
        auto actual = [NSPredicate predicateWithFormat:@"a IN %@", @[@"b", @"c"]].mgl_filter;
        mbgl::style::InFilter expected = { .key = "a", .values = { std::string("b"), std::string("c") } };
        MGLAssertEqualFilters(actual, expected);
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"%K IN {'LineString', 'Polygon'}", @"$mgl_geometryType"];
        NSArray *jsonExpression = @[@"match", @[@"geometry-type"], @[@"literal", @[@"LineString", @"Polygon"]]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate mgl_predicateWithJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"%K IN %@", @"$mgl_geometryType", @[@"LineString", @"Polygon"]];
        NSArray *jsonExpression = @[@"match", @[@"geometry-type"], @[@"literal", @[@"LineString", @"Polygon"]]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate mgl_predicateWithJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"%K IN {67086180, 3709678893, 3352016856, 4189833989}", @"$mgl_featureIdentifier"];
        NSArray *jsonExpression = @[@"match", @[@"id"], @[@"literal", @[@67086180, @3709678893, @3352016856, @4189833989]]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate mgl_predicateWithJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"%K IN %@", @"$mgl_featureIdentifier", @[@67086180, @3709678893, @3352016856, @4189833989]];
        NSArray *jsonExpression = @[@"match", @[@"id"], @[@"literal", @[@67086180, @3709678893, @3352016856, @4189833989]]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate mgl_predicateWithJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"%K IN {67086180, 3709678893, 3352016856, 4189833989}", @"$mgl_featureIdentifier"];
        NSArray *jsonExpression = @[@"match", @[@"id"], @[@"literal", @[@67086180, @3709678893, @3352016856, @4189833989]]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate mgl_predicateWithJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"%@ CONTAINS a", @[@"b", @"c"]];
        NSArray *jsonExpression = @[@"has", @[@"literal", @[@"b", @"c"]], @[@"get", @"a"]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate mgl_predicateWithJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"%@ CONTAINS %K", @[@"LineString", @"Polygon"], @"$mgl_geometryType"];
        NSArray *jsonExpression = @[@"has", @[@"literal", @[@"b", @"c"]], @[@"geometry-type"]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate mgl_predicateWithJSONObject:jsonExpression]
                          mustRoundTrip:NO];
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:expected], predicate);
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"{67086180, 3709678893, 3352016856, 4189833989} CONTAINS %K", @"$mgl_featureIdentifier"];
        NSArray *jsonExpression = @[@"has", @[@"literal", @[@67086180, @3709678893, @3352016856, @4189833989]], @[@"id"]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:jsonExpression], predicate);
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"x BETWEEN %@", limits];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, expected);
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:expected], predicate);
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"%@ CONTAINS %K", @[@67086180, @3709678893, @3352016856, @4189833989], @"$mgl_featureIdentifier"];
        NSArray *jsonExpression = @[@"has", @[@"literal", @[@67086180, @3709678893, @3352016856, @4189833989]], @[@"id"]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate mgl_predicateWithJSONObject:jsonExpression]
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"x BETWEEN %@", limits];
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:expected], predicate);
    }
}

- (void)testCompoundPredicates {
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"a == 'b' AND c == 'd'"];
        NSArray *jsonExpression = @[@"all", @[@"==", @[@"get", @"a"], @"b"], @[@"==",  @[@"get", @"c"], @"d"]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate mgl_predicateWithJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"a == 'b' OR c == 'd'"];
        NSArray *jsonExpression = @[@"any", @[@"==", @[@"get", @"a"], @"b"], @[@"==",  @[@"get", @"c"], @"d"]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate mgl_predicateWithJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"NOT(a == 'b' AND c == 'd')"];
        NSArray *jsonExpression = @[@"!", @[@"all", @[@"==", @[@"get", @"a"], @"b"], @[@"==",  @[@"get", @"c"], @"d"]]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate mgl_predicateWithJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"NOT(a == 'b' OR c == 'd')"];
        NSArray *jsonExpression = @[@"!", @[@"any", @[@"==", @[@"get", @"a"], @"b"], @[@"==",  @[@"get", @"c"], @"d"]]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate mgl_predicateWithJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"NOT a == nil"];
        NSArray *jsonExpression = @[@"!", @[@"==", @[@"get", @"a"], [NSNull null]]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        [self testSymmetryWithPredicate:predicate
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"NOT a != nil"];
        NSArray *jsonExpression = @[@"!", @[@"!=", @[@"get", @"a"], [NSNull null]]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        [self testSymmetryWithPredicate:predicate
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"NOT a IN {'b', 'c'}"];
        NSArray *jsonExpression = @[@"!", @[@"match", @[@"get", @"a"], @[@"literal", @[@"b", @"c"]]]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate mgl_predicateWithJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"NOT a IN %@", @[@"b", @"c"]];
        NSArray *jsonExpression = @[@"!", @[@"match", @[@"get", @"a"], @[@"literal", @[@"b", @"c"]]]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate mgl_predicateWithJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"NOT {'b', 'c'} CONTAINS a"];
        NSArray *jsonExpression = @[@"!", @[@"has", @[@"literal", @[@"b", @"c"]], @[@"get", @"a"]]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate mgl_predicateWithJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"NOT %@ CONTAINS a", @[@"b", @"c"]];
        NSArray *jsonExpression = @[@"!", @[@"has", @[@"literal", @[@"b", @"c"]], @[@"get", @"a"]]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate mgl_predicateWithJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
}

- (void)testSymmetryWithPredicate:(NSPredicate *)forwardPredicate mustRoundTrip:(BOOL)mustRoundTrip {
    auto forwardFilter = forwardPredicate.mgl_filter;
    NSPredicate *forwardPredicateAfter = [NSPredicate mgl_predicateWithFilter:forwardFilter];
    if (mustRoundTrip) {
        // A collection of ints may turn into an aggregate of longs, for
        // example, so compare formats instead of the predicates themselves.
        XCTAssertEqualObjects(forwardPredicate.predicateFormat, forwardPredicateAfter.predicateFormat);
    } else {
        XCTAssertEqualObjects(forwardPredicate, forwardPredicateAfter);
    }
}

- (void)testComparisonExpressionArray {
    {
        NSArray *expected = @[@"==",  @[@"number", @1],  @[@"number", @2]];
        XCTAssertEqualObjects([NSPredicate predicateWithFormat:@"1 == 2"].mgl_jsonExpressionObject, expected);
        [self testSymmetryWithPredicate:[NSPredicate predicateWithFormat:@"1 == 2"]
                          mustRoundTrip:NO];
    }
    {
        NSArray *expected = @[@"all", @[@"<=", @10, @[@"get", @"x"]], @[@"<=", @[@"get", @"x"], @100]];
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"x BETWEEN {10, 100}"];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, expected);
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:expected], predicate);
    }
    {
        NSArray *expected = @[@"all", @[@">=",  @[@"get", @"x"], @10], @[@"<=", @[@"get", @"x"], @100]];
        NSExpression *limits = [NSExpression expressionForAggregate:@[[NSExpression expressionForConstantValue:@10], [NSExpression expressionForConstantValue:@100]]];
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"x BETWEEN %@", limits];
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:expected], predicate);
    }
    {
        NSArray *expected = @[@"all", @[@"<=", @10, @[@"get", @"x"]], @[@"<=", @[@"get", @"x"], @100]];
        NSExpression *limits = [NSExpression expressionForAggregate:@[[NSExpression expressionForConstantValue:@10], [NSExpression expressionForConstantValue:@100]]];
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"x BETWEEN %@", limits];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, expected);
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:expected], predicate);
    }
    {
        NSArray *expected = @[@"all", @[@"<=", @10, @[@"get", @"x"]], @[@">=", @100, @[@"get", @"x"]]];
        NSExpression *limits = [NSExpression expressionForAggregate:@[[NSExpression expressionForConstantValue:@10], [NSExpression expressionForConstantValue:@100]]];
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"x BETWEEN %@", limits];
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:expected], predicate);
    }
    {
        NSArray *expected = @[@"all", @[@">=",  @[@"get", @"x"], @10], @[@">=", @100, @[@"get", @"x"]]];
        NSExpression *limits = [NSExpression expressionForAggregate:@[[NSExpression expressionForConstantValue:@10], [NSExpression expressionForConstantValue:@100]]];
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"x BETWEEN %@", limits];
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:expected], predicate);
    }
    {
        NSArray *expected = @[@"all", @[@"==", @10, @[@"get", @"x"]], @[@"<=", @[@"get", @"x"], @100]];
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"%@ == x && x <= %@", [NSExpression expressionForConstantValue:@10], [NSExpression expressionForConstantValue:@100]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, expected);
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:expected], predicate);
    }
    {
        NSArray *expected = @[@"match", @[@"id"], @6, @YES, @5, @YES, @4, @YES, @3, @YES, @NO];
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"$featureIdentifier IN { 6, 5, 4, 3}"];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, expected);
        NSPredicate *predicateAfter = [NSPredicate predicateWithFormat:@"MGL_MATCH($featureIdentifier, 6, YES, 5, YES, 4, YES, 3, YES, NO) == YES"];
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:expected], predicateAfter);
    }
    {
        NSArray *expected = @[@"!", @[@"match", @[@"get", @"x"], @6, @YES, @5, @YES, @4, @YES, @3, @YES, @NO]];
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"NOT x IN { 6, 5, 4, 3}"];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, expected);
        NSPredicate *predicateAfter = [NSPredicate predicateWithFormat:@"NOT MGL_MATCH(x, 6, YES, 5, YES, 4, YES, 3, YES, NO) == YES"];
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:expected], predicateAfter);
    }
    {
        NSArray *expected = @[@"match", @[@"get", @"x"], @6, @YES, @5, @YES, @4, @YES, @3, @YES, @NO];
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"{ 6, 5, 4, 3} CONTAINS x"];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, expected);
        NSPredicate *predicateAfter = [NSPredicate predicateWithFormat:@"MGL_MATCH(x, 6, YES, 5, YES, 4, YES, 3, YES, NO) == YES"];
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:expected], predicateAfter);
    }
    {
        NSArray *expected = @[@"match", @[@"id"], @6, @YES, @5, @YES, @4, @YES, @3, @YES, @NO];
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"{ 6, 5, 4, 3} CONTAINS $featureIdentifier"];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, expected);
        NSPredicate *predicateAfter = [NSPredicate predicateWithFormat:@"MGL_MATCH($featureIdentifier, 6, YES, 5, YES, 4, YES, 3, YES, NO) == YES"];
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:expected], predicateAfter);
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"x == YES"];
        NSArray *jsonExpression = @[@"==", @[@"get", @"x"],  @[@"boolean", @YES]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate mgl_predicateWithJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"x < 5"];
        NSArray *jsonExpression = @[@"<", @[@"number", @[@"get", @"x"]], @[@"number", @5]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate mgl_predicateWithJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"x > 5"];
        NSArray *jsonExpression = @[@">", @[@"number", @[@"get", @"x"]], @[@"number", @5]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate mgl_predicateWithJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"x <= 5"];
        NSArray *jsonExpression = @[@"<=", @[@"number", @[@"get", @"x"]], @[@"number", @5]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate mgl_predicateWithJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"x >= 5"];
        NSArray *jsonExpression = @[@">=", @[@"number", @[@"get", @"x"]], @[@"number", @5]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate mgl_predicateWithJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"x < y"];
        NSArray *jsonExpression = @[@"<", @[@"number", @[@"get", @"x"]], @[@"number", @[@"get", @"y"]]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate mgl_predicateWithJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"x > y"];
        NSArray *jsonExpression = @[@">", @[@"number", @[@"get", @"x"]], @[@"number", @[@"get", @"y"]]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate mgl_predicateWithJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"x <= y"];
        NSArray *jsonExpression = @[@"<=", @[@"number", @[@"get", @"x"]], @[@"number", @[@"get", @"y"]]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate mgl_predicateWithJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"x >= y"];
        NSArray *jsonExpression = @[@">=", @[@"number", @[@"get", @"x"]], @[@"number", @[@"get", @"y"]]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate mgl_predicateWithJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"x > 'value'"];
        NSArray *jsonExpression = @[@">",  @[@"string", @[@"get", @"x"]], @[@"string", @"value"]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate mgl_predicateWithJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
}

@end
