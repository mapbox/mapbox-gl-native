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

- (void)testPredication {
    XCTAssertNil([NSPredicate mgl_predicateWithFilter:mbgl::style::NullFilter()]);
    
    {
        mbgl::style::EqualsFilter filter = { .key = "a", .value = std::string("b") };
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithFilter:filter], [NSPredicate predicateWithFormat:@"a = 'b'"]);
    }
    
    {
        mbgl::style::TypeEqualsFilter filter = { .value = mbgl::FeatureType::Point };
        NSPredicate *expected = [NSPredicate predicateWithFormat:@"%K = 'Point'", @"$type"];
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithFilter:filter], expected);
    }
    
    {
        mbgl::style::TypeEqualsFilter filter = { .value = mbgl::FeatureType::LineString };
        NSPredicate *expected = [NSPredicate predicateWithFormat:@"%K = 'LineString'", @"$type"];
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithFilter:filter], expected);
    }
    
    {
        mbgl::style::TypeEqualsFilter filter = { .value = mbgl::FeatureType::Polygon };
        NSPredicate *expected = [NSPredicate predicateWithFormat:@"%K = 'Polygon'", @"$type"];
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithFilter:filter], expected);
    }
    
    {
        mbgl::style::IdentifierEqualsFilter filter = { .value = UINT64_C(67086180) };
        NSPredicate *expected = [NSPredicate predicateWithFormat:@"%K = 67086180", @"$id"];
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithFilter:filter], expected);
    }
    
    {
        mbgl::style::NotHasIdentifierFilter filter;
        NSPredicate *expected = [NSPredicate predicateWithFormat:@"%K = nil", @"$id"];
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithFilter:filter], expected);
    }
    
    {
        mbgl::style::TypeEqualsFilter filter = { .value = mbgl::FeatureType::Unknown };
        XCTAssertThrowsSpecificNamed([NSPredicate mgl_predicateWithFilter:filter], NSException, NSInternalInconsistencyException);
    }
    
    {
        mbgl::style::NotHasFilter filter = { .key = "a" };
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithFilter:filter], [NSPredicate predicateWithFormat:@"a = nil"]);
    }
    
    {
        mbgl::style::NotEqualsFilter filter = { .key = "a", .value = std::string("b") };
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithFilter:filter], [NSPredicate predicateWithFormat:@"a != 'b'"]);
    }
    
    {
        mbgl::style::TypeNotEqualsFilter filter = { .value = mbgl::FeatureType::Point };
        NSPredicate *expected = [NSPredicate predicateWithFormat:@"%K != 'Point'", @"$type"];
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithFilter:filter], expected);
    }
    
    {
        mbgl::style::IdentifierNotEqualsFilter filter = { .value = UINT64_C(67086180) };
        NSPredicate *expected = [NSPredicate predicateWithFormat:@"%K != 67086180", @"$id"];
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithFilter:filter], expected);
    }
    
    {
        mbgl::style::HasIdentifierFilter filter;
        NSPredicate *expected = [NSPredicate predicateWithFormat:@"%K != nil", @"$id"];
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithFilter:filter], expected);
    }
    
    {
        mbgl::style::HasFilter filter = { .key = "a" };
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithFilter:filter], [NSPredicate predicateWithFormat:@"a != nil"]);
    }
    
    {
        mbgl::style::LessThanFilter filter = { .key = "a", .value = std::string("b") };
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithFilter:filter], [NSPredicate predicateWithFormat:@"a < 'b'"]);
    }
    
    {
        mbgl::style::LessThanEqualsFilter filter = { .key = "a", .value = std::string("b") };
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithFilter:filter], [NSPredicate predicateWithFormat:@"a <= 'b'"]);
    }
    
    {
        mbgl::style::GreaterThanFilter filter = { .key = "a", .value = std::string("b") };
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithFilter:filter], [NSPredicate predicateWithFormat:@"a > 'b'"]);
    }
    
    {
        mbgl::style::GreaterThanEqualsFilter filter = { .key = "a", .value = std::string("b") };
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithFilter:filter], [NSPredicate predicateWithFormat:@"a >= 'b'"]);
    }
    
    {
        mbgl::style::AllFilter filter = {
            .filters = {
                mbgl::style::GreaterThanEqualsFilter { .key = "a", .value = std::string("b") },
                mbgl::style::LessThanEqualsFilter { .key = "a", .value = std::string("z") },
            },
        };
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithFilter:filter], [NSPredicate predicateWithFormat:@"a BETWEEN {'b', 'z'}"]);
    }
    
    {
        mbgl::style::AllFilter filter = {
            .filters = {
                mbgl::style::LessThanEqualsFilter { .key = "a", .value = std::string("z") },
                mbgl::style::GreaterThanEqualsFilter { .key = "a", .value = std::string("b") },
            },
        };
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithFilter:filter], [NSPredicate predicateWithFormat:@"a BETWEEN {'b', 'z'}"]);
    }
    
    {
        mbgl::style::InFilter filter = { .key = "a", .values = { std::string("b"), std::string("c") } };
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithFilter:filter].predicateFormat, [NSPredicate predicateWithFormat:@"a IN {'b', 'c'}"].predicateFormat);
    }
    
    {
        mbgl::style::TypeInFilter filter = { .values = { mbgl::FeatureType::LineString, mbgl::FeatureType::Polygon } };
        NSPredicate *expected = [NSPredicate predicateWithFormat:@"%K IN {'LineString', 'Polygon'}", @"$type"];
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithFilter:filter].predicateFormat, expected.predicateFormat);
    }
    
    {
        mbgl::style::IdentifierInFilter filter = { .values = { UINT64_C(67086180), UINT64_C(3709678893), UINT64_C(3352016856), UINT64_C(4189833989) } };
        NSPredicate *expected = [NSPredicate predicateWithFormat:@"%K IN {67086180, 3709678893, 3352016856, 4189833989}", @"$id"];
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithFilter:filter], expected);
    }
    
    {
        mbgl::style::NotInFilter filter = { .key = "a", .values = { std::string("b"), std::string("c") } };
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithFilter:filter].predicateFormat, [NSPredicate predicateWithFormat:@"NOT a IN {'b', 'c'}"].predicateFormat);
    }
    
    {
        mbgl::style::TypeNotInFilter filter = { .values = { mbgl::FeatureType::LineString, mbgl::FeatureType::Polygon } };
        NSPredicate *expected = [NSPredicate predicateWithFormat:@"NOT %K IN {'LineString', 'Polygon'}", @"$type"];
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithFilter:filter].predicateFormat, expected.predicateFormat);
    }
    
    {
        mbgl::style::IdentifierNotInFilter filter = { .values = { UINT64_C(67086180), UINT64_C(3709678893), UINT64_C(3352016856), UINT64_C(4189833989) } };
        NSPredicate *expected = [NSPredicate predicateWithFormat:@"NOT %K IN {67086180, 3709678893, 3352016856, 4189833989}", @"$id"];
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithFilter:filter], expected);
    }
    
    {
        mbgl::style::AllFilter filter;
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithFilter:filter], [NSPredicate predicateWithValue:YES]);
    }
    
    {
        mbgl::style::AllFilter filter = {
            .filters = {
                mbgl::style::EqualsFilter { .key = "a", .value = std::string("b") },
                mbgl::style::EqualsFilter { .key = "c", .value = std::string("d") },
            },
        };
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithFilter:filter], [NSPredicate predicateWithFormat:@"a == 'b' AND c == 'd'"]);
    }
    
    {
        mbgl::style::AnyFilter filter;
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithFilter:filter], [NSPredicate predicateWithValue:NO]);
    }
    
    {
        mbgl::style::AnyFilter filter = {
            .filters = {
                mbgl::style::EqualsFilter { .key = "a", .value = std::string("b") },
                mbgl::style::EqualsFilter { .key = "c", .value = std::string("d") },
            },
        };
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithFilter:filter], [NSPredicate predicateWithFormat:@"a == 'b' OR c == 'd'"]);
    }
    
    {
        mbgl::style::NoneFilter filter;
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithFilter:filter], [NSPredicate predicateWithValue:YES]);
    }
    
    {
        mbgl::style::NoneFilter filter = {
            .filters = {
                mbgl::style::EqualsFilter { .key = "a", .value = std::string("b") },
                mbgl::style::EqualsFilter { .key = "c", .value = std::string("d") },
            },
        };
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithFilter:filter], [NSPredicate predicateWithFormat:@"NOT(a == 'b' OR c == 'd')"]);
    }
}

- (void)testUnsupportedFilterPredicates {
    XCTAssertThrowsSpecificNamed([NSPredicate predicateWithFormat:@"1 == 2"].mgl_filter, NSException, NSInvalidArgumentException);
    XCTAssertThrowsSpecificNamed([NSPredicate predicateWithFormat:@"1 == 1"].mgl_filter, NSException, NSInvalidArgumentException);
    XCTAssertThrowsSpecificNamed([NSPredicate predicateWithValue:YES].mgl_filter, NSException, NSInvalidArgumentException);
    XCTAssertThrowsSpecificNamed([NSPredicate predicateWithValue:NO].mgl_filter, NSException, NSInvalidArgumentException);
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
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"x == YES"];
        NSArray *jsonExpression = @[@"==", @[@"get", @"x"],  @YES];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate mgl_predicateWithJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"CAST(x, 'NSNumber') < 5"];
        NSArray *jsonExpression = @[@"<", @[@"to-number", @[@"get", @"x"]], @5];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate mgl_predicateWithJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"CAST(x, 'NSNumber') > 5"];
        NSArray *jsonExpression = @[@">", @[@"to-number", @[@"get", @"x"]], @5];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate mgl_predicateWithJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"CAST(x, 'NSNumber') <= 5"];
        NSArray *jsonExpression = @[@"<=", @[@"to-number", @[@"get", @"x"]], @5];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate mgl_predicateWithJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"CAST(x, 'NSNumber') >= 5"];
        NSArray *jsonExpression = @[@">=", @[@"to-number", @[@"get", @"x"]], @5];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate mgl_predicateWithJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"CAST(x, 'NSString') > 'value'"];
        NSArray *jsonExpression = @[@">",  @[@"to-string", @[@"get", @"x"]], @"value"];
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
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"CAST(a, 'NSString') < 'b'"];
        NSArray *jsonExpression = @[@"<", @[@"to-string", @[@"get", @"a"]], @"b"];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate mgl_predicateWithJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"CAST(a, 'NSString') <= 'b'"];
        NSArray *jsonExpression = @[@"<=", @[@"to-string", @[@"get", @"a"]], @"b"];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate mgl_predicateWithJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"CAST(a, 'NSString') > 'b'"];
        NSArray *jsonExpression = @[@">", @[@"to-string", @[@"get", @"a"]], @"b"];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate mgl_predicateWithJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"CAST(a, 'NSString') >= 'b'"];
        NSArray *jsonExpression = @[@">=", @[@"to-string", @[@"get", @"a"]], @"b"];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate mgl_predicateWithJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"CAST(a, 'NSString') BETWEEN {'b', 'z'}"];
        NSArray *jsonExpression =@[@"all", @[@"<=", @"b", @[@"to-string", @[@"get", @"a"]]], @[@"<=", @[@"to-string", @[@"get", @"a"]], @"z"]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate mgl_predicateWithJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
    {
        NSExpression *limits = [NSExpression expressionForAggregate:@[[NSExpression expressionForConstantValue:@10], [NSExpression expressionForConstantValue:@100]]];
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"CAST(x, 'NSNumber') BETWEEN %@", limits];
        NSArray *jsonExpression = @[@"all", @[@">=", @[@"to-number", @[@"get", @"x"]], @10], @[@"<=", @[@"to-number", @[@"get", @"x"]], @100]];
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate mgl_predicateWithJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
    {
        NSArray *expected = @[@"all", @[@"<=", @10, @[@"to-number", @[@"get", @"x"]]], @[@"<=", @[@"to-number", @[@"get", @"x"]], @100]];
        NSExpression *limits = [NSExpression expressionForAggregate:@[[NSExpression expressionForConstantValue:@10], [NSExpression expressionForConstantValue:@100]]];
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"CAST(x, 'NSNumber') BETWEEN %@", limits];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, expected);
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:expected], predicate);
        [self testSymmetryWithPredicate:[NSPredicate mgl_predicateWithJSONObject:expected]
                          mustRoundTrip:NO];
    }
    {
        NSArray *expected = @[@"all", @[@"<=", @10, @[@"to-number", @[@"get", @"x"]]], @[@">=", @100, @[@"to-number", @[@"get", @"x"]]]];
        NSExpression *limits = [NSExpression expressionForAggregate:@[[NSExpression expressionForConstantValue:@10], [NSExpression expressionForConstantValue:@100]]];
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"CAST(x, 'NSNumber') BETWEEN %@", limits];
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:expected], predicate);
        [self testSymmetryWithPredicate:[NSPredicate mgl_predicateWithJSONObject:expected]
                          mustRoundTrip:NO];
    }
    {
        NSArray *expected = @[@"all", @[@">=", @[@"to-number", @[@"get", @"x"]], @10], @[@">=", @100, @[@"to-number", @[@"get", @"x"]]]];
        NSExpression *limits = [NSExpression expressionForAggregate:@[[NSExpression expressionForConstantValue:@10], [NSExpression expressionForConstantValue:@100]]];
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"CAST(x, 'NSNumber') BETWEEN %@", limits];
        XCTAssertEqualObjects([NSPredicate mgl_predicateWithJSONObject:expected], predicate);
        [self testSymmetryWithPredicate:[NSPredicate mgl_predicateWithJSONObject:expected]
                          mustRoundTrip:NO];
    }
    {
        NSArray *expected = @[@"match", @[@"id"], @6, @YES, @5, @YES, @4, @YES, @3, @YES, @NO];
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"$mgl_featureIdentifier IN { 6, 5, 4, 3}"];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, expected);
        NSPredicate *predicateAfter = [NSPredicate predicateWithFormat:@"MGL_MATCH(CAST($mgl_featureIdentifier, 'NSNumber'), 3, YES, 4, YES, 5, YES, 6, YES, NO) == YES"];
        auto forwardFilter = [NSPredicate mgl_predicateWithJSONObject:expected].mgl_filter;
        NSPredicate *forwardPredicateAfter = [NSPredicate mgl_predicateWithFilter:forwardFilter];
        XCTAssertEqualObjects(predicateAfter, forwardPredicateAfter);
    }
    {
        NSArray *expected = @[@"!", @[@"match", @[@"get", @"x"], @6, @YES, @5, @YES, @4, @YES, @3, @YES, @NO]];
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"NOT x IN { 6, 5, 4, 3}"];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, expected);
        NSPredicate *predicateAfter = [NSPredicate predicateWithFormat:@"NOT MGL_MATCH(CAST(x, 'NSNumber'), 3, YES, 4, YES, 5, YES, 6, YES, NO) == YES"];
        auto forwardFilter = [NSPredicate mgl_predicateWithJSONObject:expected].mgl_filter;
        NSPredicate *forwardPredicateAfter = [NSPredicate mgl_predicateWithFilter:forwardFilter];
        XCTAssertEqualObjects(predicateAfter, forwardPredicateAfter);
    }
    {
        NSArray *expected = @[@"match", @[@"get", @"a"], @"b", @YES, @"c", @YES, @NO];
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"a IN { 'b', 'c' }"];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, expected);
        NSPredicate *predicateAfter = [NSPredicate predicateWithFormat:@"MGL_MATCH(CAST(a, 'NSString'), 'b', YES, 'c', YES, NO) == YES"];
        auto forwardFilter = [NSPredicate mgl_predicateWithJSONObject:expected].mgl_filter;
        NSPredicate *forwardPredicateAfter = [NSPredicate mgl_predicateWithFilter:forwardFilter];
        XCTAssertEqualObjects(predicateAfter, forwardPredicateAfter);
    }
    {
        NSArray *expected = @[@"match", @[@"geometry-type"], @"LineString", @YES, @"Polygon", @YES, @NO];
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"%@ IN %@", [NSExpression expressionForVariable:@"mgl_geometryType"], @[@"LineString", @"Polygon"]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, expected);
        NSPredicate *predicateAfter = [NSPredicate predicateWithFormat:@"MGL_MATCH($mgl_geometryType, 'LineString', YES, 'Polygon', YES, NO) == YES"];
        auto forwardFilter = [NSPredicate mgl_predicateWithJSONObject:expected].mgl_filter;
        NSPredicate *forwardPredicateAfter = [NSPredicate mgl_predicateWithFilter:forwardFilter];
        XCTAssertEqualObjects(predicateAfter, forwardPredicateAfter);
    }
    {
        NSArray *expected = @[@"match", @[@"get", @"x"], @6, @YES, @5, @YES, @4, @YES, @3, @YES, @NO];
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"{ 6, 5, 4, 3} CONTAINS x"];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, expected);
        NSPredicate *predicateAfter = [NSPredicate predicateWithFormat:@"MGL_MATCH(CAST(x, 'NSNumber'), 3, YES, 4, YES, 5, YES, 6, YES, NO) == YES"];
        auto forwardFilter = [NSPredicate mgl_predicateWithJSONObject:expected].mgl_filter;
        NSPredicate *forwardPredicateAfter = [NSPredicate mgl_predicateWithFilter:forwardFilter];
        XCTAssertEqualObjects(predicateAfter, forwardPredicateAfter);
    }
    {
        NSArray *expected = @[@"match", @[@"geometry-type"], @"LineString", @YES, @"Polygon", @YES, @NO];
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"%@ CONTAINS %@", @[@"LineString", @"Polygon"], [NSExpression expressionForVariable:@"mgl_geometryType"]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, expected);
        NSPredicate *predicateAfter = [NSPredicate predicateWithFormat:@"MGL_MATCH($mgl_geometryType, 'LineString', YES, 'Polygon', YES, NO) == YES"];
        auto forwardFilter = [NSPredicate mgl_predicateWithJSONObject:expected].mgl_filter;
        NSPredicate *forwardPredicateAfter = [NSPredicate mgl_predicateWithFilter:forwardFilter];
        XCTAssertEqualObjects(predicateAfter, forwardPredicateAfter);
    }
    {
        NSArray *expected = @[@"match", @[@"id"], @6, @YES, @5, @YES, @4, @YES, @3, @YES, @NO];
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"{ 6, 5, 4, 3} CONTAINS $mgl_featureIdentifier"];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, expected);
        NSPredicate *predicateAfter = [NSPredicate predicateWithFormat:@"MGL_MATCH(CAST($mgl_featureIdentifier, 'NSNumber'), 3, YES, 4, YES, 5, YES, 6, YES, NO) == YES"];
        auto forwardFilter = [NSPredicate mgl_predicateWithJSONObject:expected].mgl_filter;
        NSPredicate *forwardPredicateAfter = [NSPredicate mgl_predicateWithFilter:forwardFilter];
        XCTAssertEqualObjects(predicateAfter, forwardPredicateAfter);
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

@end
