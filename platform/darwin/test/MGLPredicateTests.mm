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

- (void)testFilterization {
    {
        auto actual = [NSPredicate predicateWithValue:YES].mgl_filter;
        mbgl::style::AllFilter expected;
        MGLAssertEqualFilters(actual, expected);
    }

    {
        auto actual = [NSPredicate predicateWithValue:NO].mgl_filter;
        mbgl::style::AnyFilter expected;
        MGLAssertEqualFilters(actual, expected);
    }

    {
        auto actual = [NSPredicate predicateWithFormat:@"a = 'b'"].mgl_filter;
        mbgl::style::EqualsFilter expected = { .key = "a", .value = std::string("b") };
        MGLAssertEqualFilters(actual, expected);
    }
    
    {
        auto actual = [NSPredicate predicateWithFormat:@"%K = 'Point'", @"$type"].mgl_filter;
        mbgl::style::TypeEqualsFilter expected = { .value = mbgl::FeatureType::Point };
        MGLAssertEqualFilters(actual, expected);
    }
    
    {
        auto actual = [NSPredicate predicateWithFormat:@"%K = 67086180", @"$id"].mgl_filter;
        mbgl::style::IdentifierEqualsFilter expected = { .value = UINT64_C(67086180) };
        MGLAssertEqualFilters(actual, expected);
    }
    
    {
        auto actual = [NSPredicate predicateWithFormat:@"%K = nil", @"$id"].mgl_filter;
        mbgl::style::NotHasIdentifierFilter expected;
        MGLAssertEqualFilters(actual, expected);
    }

    {
        auto actual = [NSPredicate predicateWithFormat:@"a = nil"].mgl_filter;
        mbgl::style::NotHasFilter expected = { .key = "a" };
        MGLAssertEqualFilters(actual, expected);
    }
    
    {
        auto actual = [NSPredicate predicateWithFormat:@"%K != 'Point'", @"$type"].mgl_filter;
        mbgl::style::TypeNotEqualsFilter expected = { .value = mbgl::FeatureType::Point };
        MGLAssertEqualFilters(actual, expected);
    }
    
    {
        auto actual = [NSPredicate predicateWithFormat:@"%K != 67086180", @"$id"].mgl_filter;
        mbgl::style::IdentifierNotEqualsFilter expected = { .value = UINT64_C(67086180) };
        MGLAssertEqualFilters(actual, expected);
    }
    
    {
        auto actual = [NSPredicate predicateWithFormat:@"%K != nil", @"$id"].mgl_filter;
        mbgl::style::HasIdentifierFilter expected;
        MGLAssertEqualFilters(actual, expected);
    }

    {
        auto actual = [NSPredicate predicateWithFormat:@"a != 'b'"].mgl_filter;
        mbgl::style::NotEqualsFilter expected = { .key = "a", .value = std::string("b") };
        MGLAssertEqualFilters(actual, expected);
    }

    {
        auto actual = [NSPredicate predicateWithFormat:@"a != nil"].mgl_filter;
        mbgl::style::HasFilter expected = { .key = "a" };
        MGLAssertEqualFilters(actual, expected);
    }

    {
        auto actual = [NSPredicate predicateWithFormat:@"a < 'b'"].mgl_filter;
        mbgl::style::LessThanFilter expected = { .key = "a", .value = std::string("b") };
        MGLAssertEqualFilters(actual, expected);
    }

    {
        auto actual = [NSPredicate predicateWithFormat:@"a <= 'b'"].mgl_filter;
        mbgl::style::LessThanEqualsFilter expected = { .key = "a", .value = std::string("b") };
        MGLAssertEqualFilters(actual, expected);
    }

    {
        auto actual = [NSPredicate predicateWithFormat:@"a > 'b'"].mgl_filter;
        mbgl::style::GreaterThanFilter expected = { .key = "a", .value = std::string("b") };
        MGLAssertEqualFilters(actual, expected);
    }

    {
        auto actual = [NSPredicate predicateWithFormat:@"a >= 'b'"].mgl_filter;
        mbgl::style::GreaterThanEqualsFilter expected = { .key = "a", .value = std::string("b") };
        MGLAssertEqualFilters(actual, expected);
    }

    {
        auto actual = [NSPredicate predicateWithFormat:@"a BETWEEN {'b', 'z'}"].mgl_filter;
        mbgl::style::AllFilter expected = {
            .filters = {
                mbgl::style::GreaterThanEqualsFilter { .key = "a", .value = std::string("b") },
                mbgl::style::LessThanEqualsFilter { .key = "a", .value = std::string("z") },
            },
        };
        MGLAssertEqualFilters(actual, expected);
    }

    {
        auto actual = [NSPredicate predicateWithFormat:@"a BETWEEN %@", @[@"b", @"z"]].mgl_filter;
        mbgl::style::AllFilter expected = {
            .filters = {
                mbgl::style::GreaterThanEqualsFilter { .key = "a", .value = std::string("b") },
                mbgl::style::LessThanEqualsFilter { .key = "a", .value = std::string("z") },
            },
        };
        MGLAssertEqualFilters(actual, expected);
    }

    {
        auto actual = [NSPredicate predicateWithFormat:@"a IN {'b', 'c'}"].mgl_filter;
        mbgl::style::InFilter expected = { .key = "a", .values = { std::string("b"), std::string("c") } };
        MGLAssertEqualFilters(actual, expected);
    }

    {
        auto actual = [NSPredicate predicateWithFormat:@"a IN %@", @[@"b", @"c"]].mgl_filter;
        mbgl::style::InFilter expected = { .key = "a", .values = { std::string("b"), std::string("c") } };
        MGLAssertEqualFilters(actual, expected);
    }
    
    {
        auto actual = [NSPredicate predicateWithFormat:@"%K IN {'LineString', 'Polygon'}", @"$type"].mgl_filter;
        mbgl::style::TypeInFilter expected = { .values = { mbgl::FeatureType::LineString, mbgl::FeatureType::Polygon } };
        MGLAssertEqualFilters(actual, expected);
    }
    
    {
        auto actual = [NSPredicate predicateWithFormat:@"%K IN %@", @"$type", @[@"LineString", @"Polygon"]].mgl_filter;
        mbgl::style::TypeInFilter expected = { .values = { mbgl::FeatureType::LineString, mbgl::FeatureType::Polygon } };
        MGLAssertEqualFilters(actual, expected);
    }
    
    {
        auto actual = [NSPredicate predicateWithFormat:@"%K IN {67086180, 3709678893, 3352016856, 4189833989}", @"$id"].mgl_filter;
        mbgl::style::IdentifierInFilter expected = { .values = { UINT64_C(67086180), UINT64_C(3709678893), UINT64_C(3352016856), UINT64_C(4189833989) } };
        MGLAssertEqualFilters(actual, expected);
    }
    
    {
        auto actual = [NSPredicate predicateWithFormat:@"%K IN %@", @"$id", @[@67086180, @3709678893, @3352016856, @4189833989]].mgl_filter;
        mbgl::style::IdentifierInFilter expected = { .values = { UINT64_C(67086180), UINT64_C(3709678893), UINT64_C(3352016856), UINT64_C(4189833989) } };
        MGLAssertEqualFilters(actual, expected);
    }

    XCTAssertThrowsSpecificNamed([NSPredicate predicateWithFormat:@"'Mapbox' IN a"].mgl_filter, NSException, NSInvalidArgumentException);

    {
        auto actual = [NSPredicate predicateWithFormat:@"{'b', 'c'} CONTAINS a"].mgl_filter;
        mbgl::style::InFilter expected = { .key = "a", .values = { std::string("b"), std::string("c") } };
        MGLAssertEqualFilters(actual, expected);
    }

    {
        auto actual = [NSPredicate predicateWithFormat:@"%@ CONTAINS a", @[@"b", @"c"]].mgl_filter;
        mbgl::style::InFilter expected = { .key = "a", .values = { std::string("b"), std::string("c") } };
        MGLAssertEqualFilters(actual, expected);
    }
    
    {
        auto actual = [NSPredicate predicateWithFormat:@"%@ CONTAINS %K", @[@"LineString", @"Polygon"], @"$type"].mgl_filter;
        mbgl::style::TypeInFilter expected = { .values = { mbgl::FeatureType::LineString, mbgl::FeatureType::Polygon } };
        MGLAssertEqualFilters(actual, expected);
    }
    
    {
        auto actual = [NSPredicate predicateWithFormat:@"{67086180, 3709678893, 3352016856, 4189833989} CONTAINS %K", @"$id"].mgl_filter;
        mbgl::style::IdentifierInFilter expected = { .values = { UINT64_C(67086180), UINT64_C(3709678893), UINT64_C(3352016856), UINT64_C(4189833989) } };
        MGLAssertEqualFilters(actual, expected);
    }
    
    {
        auto actual = [NSPredicate predicateWithFormat:@"%@ CONTAINS %K", @[@67086180, @3709678893, @3352016856, @4189833989], @"$id"].mgl_filter;
        mbgl::style::IdentifierInFilter expected = { .values = { UINT64_C(67086180), UINT64_C(3709678893), UINT64_C(3352016856), UINT64_C(4189833989) } };
        MGLAssertEqualFilters(actual, expected);
    }

    XCTAssertThrowsSpecificNamed([NSPredicate predicateWithFormat:@"a CONTAINS 'Mapbox'"].mgl_filter, NSException, NSInvalidArgumentException);

    {
        auto actual = [NSPredicate predicateWithFormat:@"a == 'b' AND c == 'd'"].mgl_filter;
        mbgl::style::AllFilter expected = {
            .filters = {
                mbgl::style::EqualsFilter { .key = "a", .value = std::string("b") },
                mbgl::style::EqualsFilter { .key = "c", .value = std::string("d") },
            },
        };
        MGLAssertEqualFilters(actual, expected);
    }

    {
        auto actual = [NSPredicate predicateWithFormat:@"a == 'b' OR c == 'd'"].mgl_filter;
        mbgl::style::AnyFilter expected = {
            .filters = {
                mbgl::style::EqualsFilter { .key = "a", .value = std::string("b") },
                mbgl::style::EqualsFilter { .key = "c", .value = std::string("d") },
            },
        };
        MGLAssertEqualFilters(actual, expected);
    }

    {
        auto actual = [NSPredicate predicateWithFormat:@"NOT(a == 'b' AND c == 'd')"].mgl_filter;
        mbgl::style::NoneFilter expected = {
            .filters = {
                mbgl::style::AllFilter {
                    .filters = {
                        mbgl::style::EqualsFilter { .key = "a", .value = std::string("b") },
                        mbgl::style::EqualsFilter { .key = "c", .value = std::string("d") },
                    },
                },
            },
        };
        MGLAssertEqualFilters(actual, expected);
    }

    {
        auto actual = [NSPredicate predicateWithFormat:@"NOT(a == 'b' OR c == 'd')"].mgl_filter;
        mbgl::style::NoneFilter expected = {
            .filters = {
                mbgl::style::EqualsFilter { .key = "a", .value = std::string("b") },
                mbgl::style::EqualsFilter { .key = "c", .value = std::string("d") },
            },
        };
        MGLAssertEqualFilters(actual, expected);
    }

    {
        auto actual = [NSPredicate predicateWithFormat:@"NOT a == nil"].mgl_filter;
        mbgl::style::HasFilter expected = { .key = "a" };
        MGLAssertEqualFilters(actual, expected);
    }

    {
        auto actual = [NSPredicate predicateWithFormat:@"NOT a != nil"].mgl_filter;
        mbgl::style::NotHasFilter expected = { .key = "a" };
        MGLAssertEqualFilters(actual, expected);
    }

    {
        auto actual = [NSPredicate predicateWithFormat:@"NOT a IN {'b', 'c'}"].mgl_filter;
        mbgl::style::NotInFilter expected = { .key = "a", .values = { std::string("b"), std::string("c") } };
        MGLAssertEqualFilters(actual, expected);
    }

    {
        auto actual = [NSPredicate predicateWithFormat:@"NOT a IN %@", @[@"b", @"c"]].mgl_filter;
        mbgl::style::NotInFilter expected = { .key = "a", .values = { std::string("b"), std::string("c") } };
        MGLAssertEqualFilters(actual, expected);
    }

    {
        auto actual = [NSPredicate predicateWithFormat:@"NOT {'b', 'c'} CONTAINS a"].mgl_filter;
        mbgl::style::NotInFilter expected = { .key = "a", .values = { std::string("b"), std::string("c") } };
        MGLAssertEqualFilters(actual, expected);
    }

    {
        auto actual = [NSPredicate predicateWithFormat:@"NOT %@ CONTAINS a", @[@"b", @"c"]].mgl_filter;
        mbgl::style::NotInFilter expected = { .key = "a", .values = { std::string("b"), std::string("c") } };
        MGLAssertEqualFilters(actual, expected);
    }

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

- (void)testSymmetry {
    [self testSymmetryWithFormat:@"a = 1" reverseFormat:@"1 = a" mustRoundTrip:YES];
    [self testSymmetryWithFormat:@"a != 1" reverseFormat:@"1 != a" mustRoundTrip:YES];
    XCTAssertThrowsSpecificNamed([NSPredicate predicateWithFormat:@"a = b"].mgl_filter, NSException, NSInvalidArgumentException);
    XCTAssertThrowsSpecificNamed([NSPredicate predicateWithFormat:@"1 = 1"].mgl_filter, NSException, NSInvalidArgumentException);

    // In the predicate format language, $ is a special character denoting a
    // variable. Use %K to escape the special feature attribute $id.
    XCTAssertThrowsSpecificNamed([NSPredicate predicateWithFormat:@"$id == 670861802"].mgl_filter, NSException, NSInvalidArgumentException);
    XCTAssertThrowsSpecificNamed([NSPredicate predicateWithFormat:@"a = $id"].mgl_filter, NSException, NSInvalidArgumentException);

    [self testSymmetryWithFormat:@"a = nil" reverseFormat:@"nil = a" mustRoundTrip:YES];
    [self testSymmetryWithFormat:@"a != nil" reverseFormat:@"nil != a" mustRoundTrip:YES];

    [self testSymmetryWithFormat:@"a < 1" reverseFormat:@"1 > a" mustRoundTrip:YES];
    [self testSymmetryWithFormat:@"a <= 1" reverseFormat:@"1 >= a" mustRoundTrip:YES];
    [self testSymmetryWithFormat:@"a > 1" reverseFormat:@"1 < a" mustRoundTrip:YES];
    [self testSymmetryWithFormat:@"a >= 1" reverseFormat:@"1 <= a" mustRoundTrip:YES];

    [self testSymmetryWithFormat:@"a BETWEEN {1, 2}" reverseFormat:@"1 <= a && 2 >= a" mustRoundTrip:YES];
    [self testSymmetryWithPredicate:[NSPredicate predicateWithFormat:@"a BETWEEN %@", @[@1, @2]]
                   reversePredicate:[NSPredicate predicateWithFormat:@"1 <= a && 2 >= a"]
                      mustRoundTrip:YES];
    XCTAssertThrowsSpecificNamed([NSPredicate predicateWithFormat:@"{1, 2} BETWEEN a"].mgl_filter, NSException, NSInvalidArgumentException);
    NSPredicate *betweenSetPredicate = [NSPredicate predicateWithFormat:@"a BETWEEN %@", [NSSet setWithObjects:@1, @2, nil]];
    XCTAssertThrowsSpecificNamed(betweenSetPredicate.mgl_filter, NSException, NSInvalidArgumentException);
    XCTAssertThrowsSpecificNamed([NSPredicate predicateWithFormat:@"a BETWEEN {1}"].mgl_filter, NSException, NSInvalidArgumentException);
    XCTAssertThrowsSpecificNamed([NSPredicate predicateWithFormat:@"a BETWEEN {1, 2, 3}"].mgl_filter, NSException, NSInvalidArgumentException);

    [self testSymmetryWithFormat:@"a IN {1, 2}" reverseFormat:@"{1, 2} CONTAINS a" mustRoundTrip:NO];
    [self testSymmetryWithPredicate:[NSPredicate predicateWithFormat:@"a IN %@", @[@1, @2]]
                   reversePredicate:[NSPredicate predicateWithFormat:@"%@ CONTAINS a", @[@1, @2]]
                      mustRoundTrip:YES];

    // The reverse formats here are a bit backwards because we canonicalize
    // a reverse CONTAINS to a forward IN.
    [self testSymmetryWithFormat:@"{1, 2} CONTAINS a" reverseFormat:@"{1, 2} CONTAINS a" mustRoundTrip:NO];
    [self testSymmetryWithPredicate:[NSPredicate predicateWithFormat:@"%@ CONTAINS a", @[@1, @2]]
                   reversePredicate:[NSPredicate predicateWithFormat:@"%@ CONTAINS a", @[@1, @2]]
                      mustRoundTrip:NO];
}

- (void)testSymmetryWithFormat:(NSString *)forwardFormat reverseFormat:(NSString *)reverseFormat mustRoundTrip:(BOOL)mustRoundTrip {
    NSPredicate *forwardPredicate = [NSPredicate predicateWithFormat:forwardFormat];
    NSPredicate *reversePredicate = reverseFormat ? [NSPredicate predicateWithFormat:reverseFormat] : nil;
    [self testSymmetryWithPredicate:forwardPredicate reversePredicate:reversePredicate mustRoundTrip:mustRoundTrip];
}

- (void)testSymmetryWithPredicate:(NSPredicate *)forwardPredicate reversePredicate:(NSPredicate *)reversePredicate mustRoundTrip:(BOOL)mustRoundTrip {
    auto forwardFilter = forwardPredicate.mgl_filter;
    NSPredicate *forwardPredicateAfter = [NSPredicate mgl_predicateWithFilter:forwardFilter];
    if (mustRoundTrip) {
        // A collection of ints may turn into an aggregate of longs, for
        // example, so compare formats instead of the predicates themselves.
        XCTAssertEqualObjects(forwardPredicate.predicateFormat, forwardPredicateAfter.predicateFormat);
    }

    if (reversePredicate) {
        auto reverseFilter = reversePredicate.mgl_filter;
        NSPredicate *reversePredicateAfter = [NSPredicate mgl_predicateWithFilter:reverseFilter];
        XCTAssertNotEqualObjects(reversePredicate, reversePredicateAfter);

        XCTAssertEqualObjects(forwardPredicateAfter, reversePredicateAfter);
    }
}

- (void)testComparisonExpressionArray {
    {
        NSArray *expected = @[@"==", @1, @2];
        XCTAssertEqualObjects([NSPredicate predicateWithFormat:@"1 = 2"].mgl_jsonExpressionObject, expected);
    }
}

@end
