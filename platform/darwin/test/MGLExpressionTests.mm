#import <XCTest/XCTest.h>

#import "MGLStyleLayerTests.h"

#import <string>

#import "MGLTypes.h"
#import "NSExpression+MGLPrivateAdditions.h"
#import "NSValue+MGLAdditions.h"
#if TARGET_OS_IPHONE
#import "UIColor+MGLAdditions.h"
#else
#import "NSColor+MGLAdditions.h"
#endif
#import "MGLAttributedExpression.h"

#define MGLAssertEqualValues(actual, expected, ...) \
    XCTAssertTrue(actual.is<__typeof__(expected)>()); \
    if (actual.is<__typeof__(expected)>()) { \
        XCTAssertEqual(actual.get<__typeof__(expected)>(), expected, __VA_ARGS__); \
    }

#define MGLAssertEqualValuesWithAccuracy(actual, expected, accuracy, ...) \
    XCTAssertTrue(actual.is<__typeof__(expected)>()); \
    if (actual.is<__typeof__(expected)>()) { \
        XCTAssertEqualWithAccuracy(actual.get<__typeof__(expected)>(), expected, accuracy, __VA_ARGS__); \
    }

#define MGLConstantExpression(constant) \
    [NSExpression expressionForConstantValue:constant]

#define MGLAssertConstantEqualsValue(constant, value, ...) \
    MGLAssertEqualValues(MGLConstantExpression(constant).mgl_constantMBGLValue, value, __VA_ARGS__);

#define MGLAssertConstantEqualsValueWithAccuracy(constant, value, accuracy, ...) \
    MGLAssertEqualValuesWithAccuracy(MGLConstantExpression(constant).mgl_constantMBGLValue, value, accuracy, __VA_ARGS__);

using namespace std::string_literals;

@interface MGLExpressionTests : XCTestCase

@end

@implementation MGLExpressionTests

#pragma mark - Utility

- (NSComparisonPredicate *)equalityComparisonPredicateWithRightConstantValue:(id)rightConstantValue
{
    NSComparisonPredicate *predicate = [NSComparisonPredicate
        predicateWithLeftExpression:[NSExpression expressionForKeyPath:@"foo"]
                    rightExpression:[NSExpression expressionForConstantValue:rightConstantValue]
                           modifier:NSDirectPredicateModifier
                               type:NSEqualToPredicateOperatorType
                            options:0];
    return predicate;
}

#pragma mark - Valuation tests

- (void)testStringValuation {
    MGLAssertConstantEqualsValue(@"bar", "bar"s, @"NSString should convert to std::string.");
    MGLAssertConstantEqualsValue(@"🆔🆗🇦🇶", "🆔🆗🇦🇶"s, @"NSString with non-ASCII characters should convert losslessly to std::string.");
}

- (void)testColorValuation {
    MGLAssertConstantEqualsValue([MGLColor redColor], "rgba(255,0,0,1)"s, @"MGLColor should convert to std::string containing CSS color string.");
}

- (void)testBooleanValuation {
    MGLAssertConstantEqualsValue(@NO, false, @"Boolean NSNumber should convert to bool.");
    MGLAssertConstantEqualsValue(@YES, true, @"Boolean NSNumber should convert to bool.");
}

- (void)testDoubleValuation
{
    MGLAssertConstantEqualsValue(@DBL_MIN, DBL_MIN, @"Double NSNumber should convert to double.");
    MGLAssertConstantEqualsValue(@DBL_MAX, DBL_MAX, @"Double NSNumber should convert to double.");
}

- (void)testFloatValuation {
    // Because we can't guarantee precision when using float, and because
    // we warn the user to this effect in -[NSExpression mgl_constantMBGLValue],
    // we just check that things are in the ballpark here with integer values
    // and some lower-precision checks.
    
    MGLAssertConstantEqualsValue(@-1.0f, -1.0, @"Float NSNumber should convert to double.");
    MGLAssertConstantEqualsValue(@1.0f, 1.0, @"Float NSNumber should convert to double.");
    MGLAssertConstantEqualsValueWithAccuracy(@-23.232342f, -23.232342, 0.000001, @"Float NSNumber should convert to double.");
    MGLAssertConstantEqualsValueWithAccuracy(@23.232342f, 23.232342, 0.000001, @"Float NSNumber should convert to double.");
    MGLAssertConstantEqualsValueWithAccuracy(@-FLT_MAX, static_cast<double>(-FLT_MAX), 0.000001, @"Float NSNumber should convert to double.");
    MGLAssertConstantEqualsValueWithAccuracy(@FLT_MAX, static_cast<double>(FLT_MAX), 0.000001, @"Float NSNumber should convert to double.");
}

- (void)testIntegerValuation {
    // Negative integers should always come back as int64_t per mbgl::Value definition.
    MGLAssertConstantEqualsValue(@SHRT_MIN, static_cast<int64_t>(SHRT_MIN), @"Negative short NSNumber should convert to int64_t.");
    MGLAssertConstantEqualsValue(@INT_MIN, static_cast<int64_t>(INT_MIN), @"Negative int NSNumber should convert to int64_t.");
    MGLAssertConstantEqualsValue(@LONG_MIN, static_cast<int64_t>(LONG_MIN), @"Negative long NSNumber should convert to int64_t.");
    MGLAssertConstantEqualsValue(@LLONG_MIN, static_cast<int64_t>(LLONG_MIN), @"Negative long long NSNumber should convert to int64_t.");
    MGLAssertConstantEqualsValue(@NSIntegerMin, static_cast<int64_t>(NSIntegerMin), @"Negative NSInteger NSNumber should convert to int64_t.");

    // Positive integers should always come back as uint64_t per mbgl::Value definition.
    MGLAssertConstantEqualsValue(@SHRT_MAX, static_cast<uint64_t>(SHRT_MAX), @"Positive short NSNumber should convert to uint64_t.");
    MGLAssertConstantEqualsValue(@INT_MAX, static_cast<uint64_t>(INT_MAX), @"Positive int NSNumber should convert to uint64_t.");
    MGLAssertConstantEqualsValue(@LONG_MAX, static_cast<uint64_t>(LONG_MAX), @"Positive long NSNumber should convert to uint64_t.");
    MGLAssertConstantEqualsValue(@LLONG_MAX, static_cast<uint64_t>(LLONG_MAX), @"Positive long long NSNumber should convert to uint64_t.");
    MGLAssertConstantEqualsValue(@NSIntegerMax, static_cast<uint64_t>(NSIntegerMax), @"Positive NSInteger NSNumber should convert to uint64_t.");
}

- (void)testUnsignedIntegerValuation {
    // Zero-value integers should always come back as uint64_t per mbgl::Value definition
    // (using the interpretation that zero is not negative). We use the unsigned long long
    // value just for parity with the positive integer test.
    MGLAssertConstantEqualsValue(@(static_cast<unsigned short>(0)), static_cast<uint64_t>(0), @"Unsigned short NSNumber should convert to uint64_t.");
    MGLAssertConstantEqualsValue(@0u, static_cast<uint64_t>(0), @"Unsigned int NSNumber should convert to uint64_t.");
    MGLAssertConstantEqualsValue(@0UL, static_cast<uint64_t>(0), @"Unsigned long NSNumber should convert to uint64_t.");
    MGLAssertConstantEqualsValue(@0ULL, static_cast<uint64_t>(0), @"Unsigned long long NSNumber should convert to uint64_t.");
    MGLAssertConstantEqualsValue(@(static_cast<NSUInteger>(0)), static_cast<uint64_t>(0), @"Unsigned NSUInteger NSNumber should convert to uint64_t.");

    // Positive integers should always come back as uint64_t per mbgl::Value definition.
    // We use the unsigned long long value because it can store the highest number on
    // both 32- and 64-bit and won't overflow.
    MGLAssertConstantEqualsValue(@USHRT_MAX, static_cast<uint64_t>(USHRT_MAX), @"Unsigned short NSNumber should convert to uint64_t.");
    MGLAssertConstantEqualsValue(@UINT_MAX, static_cast<uint64_t>(UINT_MAX), @"Unsigned int NSNumber should convert to uint64_t.");
    MGLAssertConstantEqualsValue(@ULONG_MAX, static_cast<uint64_t>(ULONG_MAX), @"Unsigned long NSNumber should convert to uint64_t.");
    MGLAssertConstantEqualsValue(@ULLONG_MAX, static_cast<uint64_t>(ULLONG_MAX), @"Unsigned long long NSNumber should convert to uint64_t.");
    MGLAssertConstantEqualsValue(@NSUIntegerMax, static_cast<uint64_t>(NSUIntegerMax), @"Unsigned NSUInteger NSNumber should convert to uint64_t.");
}

- (void)testNullValuation {
    mbgl::NullValue nullValue;
    MGLAssertConstantEqualsValue([NSNull null], nullValue, @"NSNull should convert to mbgl::NullValue.");
}

#pragma mark - Feature type tests

- (void)testFeatureType {
    XCTAssertEqual([NSExpression expressionForConstantValue:@"Point"].mgl_featureType, mbgl::FeatureType::Point);
    XCTAssertEqual([NSExpression expressionForConstantValue:@"LineString"].mgl_featureType, mbgl::FeatureType::LineString);
    XCTAssertEqual([NSExpression expressionForConstantValue:@"Polygon"].mgl_featureType, mbgl::FeatureType::Polygon);
    XCTAssertEqual([NSExpression expressionForConstantValue:@"Unknown"].mgl_featureType, mbgl::FeatureType::Unknown);
    XCTAssertEqual([NSExpression expressionForConstantValue:@""].mgl_featureType, mbgl::FeatureType::Unknown);
    
    XCTAssertEqual([NSExpression expressionForConstantValue:@1].mgl_featureType, mbgl::FeatureType::Point);
    XCTAssertEqual([NSExpression expressionForConstantValue:@2].mgl_featureType, mbgl::FeatureType::LineString);
    XCTAssertEqual([NSExpression expressionForConstantValue:@3].mgl_featureType, mbgl::FeatureType::Polygon);
    XCTAssertEqual([NSExpression expressionForConstantValue:@0].mgl_featureType, mbgl::FeatureType::Unknown);
    XCTAssertEqual([NSExpression expressionForConstantValue:@-1].mgl_featureType, mbgl::FeatureType::Unknown);
    XCTAssertEqual([NSExpression expressionForConstantValue:@4].mgl_featureType, mbgl::FeatureType::Unknown);
    
    XCTAssertEqual([NSExpression expressionForConstantValue:nil].mgl_featureType, mbgl::FeatureType::Unknown);
}

#pragma mark - JSON expression object tests

- (void)testVariableExpressionObject {
    {
        NSExpression *expression = [NSExpression expressionForVariable:@"zoomLevel"];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, @[@"zoom"]);
        XCTAssertEqualObjects([NSExpression expressionWithFormat:@"$zoomLevel"].mgl_jsonExpressionObject, @[@"zoom"]);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:@[@"zoom"]], expression);
        NSMutableDictionary *context = [@{@"zoomLevel": @16} mutableCopy];
        XCTAssertEqualObjects([expression expressionValueWithObject:nil context:context], @16);
    }
    {
        NSExpression *expression = [NSExpression expressionForVariable:@"heatmapDensity"];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, @[@"heatmap-density"]);
        XCTAssertEqualObjects([NSExpression expressionWithFormat:@"$heatmapDensity"].mgl_jsonExpressionObject, @[@"heatmap-density"]);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:@[@"heatmap-density"]], expression);
        NSMutableDictionary *context = [@{@"heatmapDensity": @1} mutableCopy];
        XCTAssertEqualObjects([expression expressionValueWithObject:nil context:context], @1);
    }
    {
        NSExpression *expression = [NSExpression expressionForVariable:@"lineProgress"];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, @[@"line-progress"]);
        XCTAssertEqualObjects([NSExpression expressionWithFormat:@"$lineProgress"].mgl_jsonExpressionObject, @[@"line-progress"]);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:@[@"line-progress"]], expression);
        NSMutableDictionary *context = [@{@"lineProgress": @1} mutableCopy];
        XCTAssertEqualObjects([expression expressionValueWithObject:nil context:context], @1);
    }
    {
        NSExpression *expression = [NSExpression expressionForVariable:@"featureAccumulated"];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, @[@"accumulated"]);
        XCTAssertEqualObjects([NSExpression expressionWithFormat:@"$featureAccumulated"].mgl_jsonExpressionObject, @[@"accumulated"]);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:@[@"accumulated"]], expression);
    }

    {
        NSExpression *expression = [NSExpression expressionForVariable:@"geometryType"];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, @[@"geometry-type"]);
        XCTAssertEqualObjects([NSExpression expressionWithFormat:@"$geometryType"].mgl_jsonExpressionObject, @[@"geometry-type"]);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:@[@"geometry-type"]], expression);
    }
    {
        NSExpression *expression = [NSExpression expressionForVariable:@"featureIdentifier"];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, @[@"id"]);
        XCTAssertEqualObjects([NSExpression expressionWithFormat:@"$featureIdentifier"].mgl_jsonExpressionObject, @[@"id"]);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:@[@"id"]], expression);
    }
    {
        NSExpression *expression = [NSExpression expressionForVariable:@"featureAttributes"];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, @[@"properties"]);
        XCTAssertEqualObjects([NSExpression expressionWithFormat:@"$featureAttributes"].mgl_jsonExpressionObject, @[@"properties"]);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:@[@"properties"]], expression);
    }
    {
        NSExpression *expression = [NSExpression expressionForVariable:@"loremIpsum"];
        NSArray *jsonExpression = @[@"var", @"loremIpsum"];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithFormat:@"$loremIpsum"].mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
        NSMutableDictionary *context = [@{@"loremIpsum": @"Lorem ipsum dolor sit amet"} mutableCopy];
        XCTAssertEqualObjects([expression expressionValueWithObject:nil context:context], @"Lorem ipsum dolor sit amet");
    }
    {
        NSDictionary *context = @{@"loremIpsum": MGLConstantExpression(@"Lorem ipsum dolor sit amet")};
        NSExpression *expression = [NSExpression expressionWithFormat:@"MGL_LET('loremIpsum', 'Lorem ipsum dolor sit amet', uppercase($loremIpsum))", context];
        NSExpression *compatibilityExpression = [NSExpression expressionWithFormat:@"FUNCTION(uppercase($loremIpsum), 'mgl_expressionWithContext:', %@)", context];
        NSArray *jsonExpression = @[@"let", @"loremIpsum", @"Lorem ipsum dolor sit amet", @[@"upcase", @[@"var", @"loremIpsum"]]];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects(compatibilityExpression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
}

- (void)testConstantValueExpressionObject {
    {
        NSExpression *expression = [NSExpression expressionForConstantValue:nil];
        XCTAssert(expression.mgl_jsonExpressionObject == [NSNull null]);
        XCTAssert([NSExpression expressionWithFormat:@"nil"].mgl_jsonExpressionObject == [NSNull null]);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:[NSNull null]], expression);
        XCTAssertNil([expression expressionValueWithObject:nil context:nil]);
    }
    {
        NSExpression *expression = [NSExpression expressionForConstantValue:@1];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, @1);
        XCTAssertEqualObjects([NSExpression expressionWithFormat:@"1"].mgl_jsonExpressionObject, @1);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:@1], expression);
        XCTAssertEqualObjects([expression expressionValueWithObject:nil context:nil], @1);
    }
    {
        NSExpression *expression = [NSExpression expressionForConstantValue:@YES];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, @YES);
        XCTAssertEqualObjects([NSExpression expressionWithFormat:@"TRUE"].mgl_jsonExpressionObject, @YES);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:@YES], expression);
        XCTAssertEqualObjects([expression expressionValueWithObject:nil context:nil], @YES);
    }
    {
        NSExpression *expression = [NSExpression expressionForConstantValue:nil];
        XCTAssert(expression.mgl_jsonExpressionObject == [NSNull null]);
        XCTAssert([NSExpression expressionWithFormat:@"nil"].mgl_jsonExpressionObject == [NSNull null]);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:[NSNull null]], expression);
        XCTAssertNil([expression expressionValueWithObject:nil context:nil]);
    }
    {
        CGVector vector = CGVectorMake(1, 2);
        NSExpression *expression = [NSExpression expressionForConstantValue:@(vector)];
#if !TARGET_OS_IPHONE
        NSArray *jsonExpression = @[@"literal", @[@1, @-2]];
#else
        NSArray *jsonExpression = @[@"literal", @[@1, @2]];
#endif
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        // No way to distinguish offsets from ordinary arrays in expressions.
        XCTAssertEqualObjects([[NSExpression expressionWithMGLJSONObject:jsonExpression].collection valueForKeyPath:@"constantValue"], jsonExpression.lastObject);
        XCTAssertEqualObjects([expression expressionValueWithObject:nil context:nil], @(vector));
    }
    {
#if !TARGET_OS_IPHONE
        NSEdgeInsets padding = {1, 2, 3, 4};
        NSValue *value = [NSValue valueWithEdgeInsets:padding];
#else
        UIEdgeInsets padding = {1, 2, 3, 4};
        NSValue *value = [NSValue valueWithUIEdgeInsets:padding];
#endif
        NSExpression *expression = [NSExpression expressionForConstantValue:value];
        NSArray *jsonExpression = @[@"literal", @[@1, @4, @3, @2]];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        // No way to distinguish offsets from ordinary arrays in expressions.
        XCTAssertEqualObjects([[NSExpression expressionWithMGLJSONObject:jsonExpression].collection valueForKeyPath:@"constantValue"], jsonExpression.lastObject);
        XCTAssertEqualObjects([expression expressionValueWithObject:nil context:nil], value);
    }
    {
        MGLColor *color = [MGLColor mgl_colorWithColor:{ 255.0/255, 239.0/255, 213.0/255, 1 }]; // papayawhip
        NSExpression *expression = [NSExpression expressionForConstantValue:color];
        NSArray *jsonExpression = @[@"rgb", @255, @239, @213];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([expression expressionValueWithObject:nil context:nil], color);
    }
    {
        // Transform color components to non-premultiplied values
        float alpha = 0.5;
        float red = (255.0 * alpha) / 255;
        float green = (239.0 * alpha) / 255;
        float blue = (213.0 * alpha) / 255;
        MGLColor *color = [MGLColor mgl_colorWithColor:{ red, green, blue, alpha }]; // papayawhip
        NSExpression *expression = [NSExpression expressionForConstantValue:color];
        NSArray *jsonExpression = @[@"rgba", @127.5, @119.5, @106.5, @0.5];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([expression expressionValueWithObject:nil context:nil], color);
    }
    {
        NSExpression *expression = [NSExpression expressionWithFormat:@"noindex(513)"];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, @513);
        XCTAssertEqualObjects([expression expressionValueWithObject:nil context:nil], @513);
    }
}

- (void)testKeyPathExpressionObject {
    {
        NSExpression *expression = [NSExpression expressionForKeyPath:@"highway"];
        NSArray *jsonExpression = @[@"get", @"highway"];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithFormat:@"highway"].mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
    {
        NSExpression *expression = [NSExpression expressionWithFormat:@"%@.population", @{@"population": MGLConstantExpression(@12000)}];
        NSArray *jsonExpression = @[@"get", @"population", @[@"literal", @{@"population": @12000}]];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
    {
        NSExpression *expression = [NSExpression expressionWithFormat:@"%@.uppercase('population')", @{@"POPULATION": MGLConstantExpression(@12000)}];
        NSArray *jsonExpression = @[@"get", @[@"upcase", @"population"], @[@"literal", @{@"POPULATION": @12000}]];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
    {
        NSExpression *expression = [NSExpression expressionForKeyPath:@"lineStyle.color"];
        NSArray *jsonExpression = @[@"get", @"color", @[@"get", @"lineStyle"]];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
    {
        NSExpression *expression = [NSExpression expressionForKeyPath:@"map.box.gl"];
        NSArray *jsonExpression = @[@"get", @"gl", @[@"get", @"box", @[@"get", @"map"]]];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
}

- (void)testStatisticalExpressionObject {
    {
        NSExpression *expression = [NSExpression expressionWithFormat:@"average({1, 2, 2, 3, 4, 7, 9})"];
        NSArray *jsonExpression = @[@"/", @[@"+", @1, @2, @2, @3, @4, @7, @9], @[@"length", @[@"literal", @[@1, @2, @2, @3, @4, @7, @9]]]];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([expression expressionValueWithObject:nil context:nil], @4);
    }
    {
        NSExpression *expression = [NSExpression expressionWithFormat:@"sum({1, 2, 2, 3, 4, 7, 9})"];
        NSArray *jsonExpression = @[@"+", @1, @2, @2, @3, @4, @7, @9];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([expression expressionValueWithObject:nil context:nil], @28);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
    {
        NSExpression *expression = [NSExpression expressionWithFormat:@"count({1, 2, 2, 3, 4, 7, 9})"];
        NSArray *jsonExpression = @[@"length", @[@"literal", @[@1, @2, @2, @3, @4, @7, @9]]];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([expression expressionValueWithObject:nil context:nil], @7);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
    {
        NSExpression *expression = [NSExpression expressionWithFormat:@"min({1, 2, 2, 3, 4, 7, 9})"];
        NSArray *jsonExpression = @[@"min", @1, @2, @2, @3, @4, @7, @9];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([expression expressionValueWithObject:nil context:nil], @1);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
    {
        NSExpression *expression = [NSExpression expressionWithFormat:@"max({1, 2, 2, 3, 4, 7, 9})"];
        NSArray *jsonExpression = @[@"max", @1, @2, @2, @3, @4, @7, @9];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([expression expressionValueWithObject:nil context:nil], @9);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
}

- (void)testArithmeticExpressionObject {
    NSArray *arguments = @[MGLConstantExpression(@1), MGLConstantExpression(@1)];
    {
        NSExpression *expression = [NSExpression expressionForFunction:@"add:to:" arguments:arguments];
        NSArray *jsonExpression = @[@"+", @1, @1];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithFormat:@"1 + 1"].mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
    {
        NSExpression *testExpression = [NSExpression expressionWithFormat:@"sum:({1, 1, 2})"];
        NSExpression *expression = [NSExpression expressionForFunction:@"sum:" arguments:@[[NSExpression expressionForAggregate:@[MGLConstantExpression(@1), MGLConstantExpression(@1), MGLConstantExpression(@2)]]]];

        NSArray *jsonExpression = @[@"+", @1, @1, @2];

        XCTAssertEqualObjects(testExpression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects(expression, testExpression);
    }
    {
        NSExpression *expression = [NSExpression expressionForFunction:@"sum:" arguments:@[MGLConstantExpression(@1), MGLConstantExpression(@1), MGLConstantExpression(@2)]];
        NSArray *jsonExpression = @[@"+", @1, @1, @2];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);

        // - [NSExpression expressionWithMGLJSONObject:] creates an expression with an aggregate expression as an argument. This is not equal to an expression with an array of expressions as an argument. For testing purposes, we will compare their operands and arrays of expressions.
        NSExpression *aggregateExpression = [NSExpression expressionWithMGLJSONObject:jsonExpression];
        XCTAssertEqualObjects(aggregateExpression.operand, expression.operand);
        XCTAssertEqualObjects(aggregateExpression.arguments.firstObject.collection, expression.arguments);
    }
    {
        NSArray *threeArguments = @[MGLConstantExpression(@1), MGLConstantExpression(@1), MGLConstantExpression(@1)];
        NSExpression *expression = [NSExpression expressionForFunction:@"add:to:" arguments:threeArguments];
        NSArray *jsonExpression = @[@"+", @1, @1, @1];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        jsonExpression = @[@"+", @[@"+", @1, @1], @1];
        XCTAssertEqualObjects([NSExpression expressionWithFormat:@"1 + 1 + 1"].mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], [NSExpression expressionWithFormat:@"1 + 1 + 1"]);
    }
    {
        NSExpression *expression = [NSExpression expressionForFunction:@"from:subtract:" arguments:arguments];
        NSArray *jsonExpression = @[@"-", @1, @1];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithFormat:@"1 - 1"].mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
    {
        NSExpression *expression = [NSExpression expressionForFunction:@"multiply:by:" arguments:arguments];
        NSArray *jsonExpression = @[@"*", @1, @1];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithFormat:@"1 * 1"].mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
    {
        NSExpression *expression = [NSExpression expressionForFunction:@"divide:by:" arguments:arguments];
        NSArray *jsonExpression = @[@"/", @1, @1];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithFormat:@"1 / 1"].mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
    {
        NSExpression *expression = [NSExpression expressionForFunction:@"modulus:by:" arguments:arguments];
        NSArray *jsonExpression = @[@"%", @1, @1];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        // NSExpression lacks a shorthand operator for modulus.
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
    {
        NSExpression *expression = [NSExpression expressionForFunction:@"max:" arguments:arguments];
        NSArray *jsonExpression = @[@"max", @1, @1];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);

        NSExpression *aggregateExpression = [NSExpression expressionWithMGLJSONObject:jsonExpression];
        XCTAssertEqualObjects(aggregateExpression.operand, expression.operand);
        XCTAssertEqualObjects(aggregateExpression.arguments.firstObject.collection, expression.arguments);
    }
    {
        NSExpression *expression = [NSExpression expressionForFunction:@"min:" arguments:arguments];
        NSArray *jsonExpression = @[@"min", @1, @1];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);

        NSExpression *aggregateExpression = [NSExpression expressionWithMGLJSONObject:jsonExpression];
        XCTAssertEqualObjects(aggregateExpression.operand, expression.operand);
        XCTAssertEqualObjects(aggregateExpression.arguments.firstObject.collection, expression.arguments);
    }
    {
        NSExpression *expression = [NSExpression expressionForFunction:@"ceiling:" arguments:@[MGLConstantExpression(@1.5)]];
        NSArray *jsonExpression = @[@"ceil", @1.5];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([expression expressionValueWithObject:nil context:nil], @2);
    }
    {
        NSExpression *expression = [NSExpression expressionForFunction:@"ceiling:" arguments:@[MGLConstantExpression(@-1.5)]];
        NSArray *jsonExpression = @[@"ceil", @-1.5];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([expression expressionValueWithObject:nil context:nil], @-1);
    }
    {
        NSExpression *expression = [NSExpression expressionForFunction:@"ceiling:" arguments:@[MGLConstantExpression(@2)]];
        NSArray *jsonExpression = @[@"ceil", @2];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([expression expressionValueWithObject:nil context:nil], @2);
    }
    {
        NSExpression *expression = [NSExpression expressionForFunction:@"ceiling:" arguments:@[MGLConstantExpression(@-2)]];
        NSArray *jsonExpression = @[@"ceil", @-2];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([expression expressionValueWithObject:nil context:nil], @-2);
    }
    {
        NSExpression *expression = [NSExpression expressionForFunction:@"trunc:" arguments:@[MGLConstantExpression(@1.5)]];
        NSArray *jsonExpression = @[@"-", @1.5, @[@"%", @1.5, @1]];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([expression expressionValueWithObject:nil context:nil], @1);
    }
    {
        NSExpression *expression = [NSExpression expressionForFunction:@"trunc:" arguments:@[MGLConstantExpression(@-1.5)]];
        NSArray *jsonExpression = @[@"-", @-1.5, @[@"%", @-1.5, @1]];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([expression expressionValueWithObject:nil context:nil], @-1);
    }
    {
        NSExpression *expression = [NSExpression expressionForFunction:@"abs:" arguments:@[MGLConstantExpression(@2)]];
        NSArray *jsonExpression = @[@"abs", @2];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([expression expressionValueWithObject:nil context:nil], @2);
    }
    {
        NSExpression *expression = [NSExpression expressionForFunction:@"abs:" arguments:@[MGLConstantExpression(@-2)]];
        NSArray *jsonExpression = @[@"abs", @-2];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([expression expressionValueWithObject:nil context:nil], @2);
    }
    {
        NSExpression *expression = [NSExpression expressionForFunction:@"floor:" arguments:@[MGLConstantExpression(@1.5)]];
        NSArray *jsonExpression = @[@"floor", @1.5];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([expression expressionValueWithObject:nil context:nil], @1);
    }
    {
        NSExpression *expression = [NSExpression expressionForFunction:@"floor:" arguments:@[MGLConstantExpression(@-1.5)]];
        NSArray *jsonExpression = @[@"floor", @-1.5];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([expression expressionValueWithObject:nil context:nil], @-2);
    }
    {
        NSExpression *expression = [NSExpression expressionForFunction:@"floor:" arguments:@[MGLConstantExpression(@2)]];
        NSArray *jsonExpression = @[@"floor", @2];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([expression expressionValueWithObject:nil context:nil], @2);
    }
    {
        NSExpression *expression = [NSExpression expressionForFunction:@"floor:" arguments:@[MGLConstantExpression(@-2)]];
        NSArray *jsonExpression = @[@"floor", @-2];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([expression expressionValueWithObject:nil context:nil], @-2);
    }
    {
        NSExpression *expression = [NSExpression expressionForFunction:@"mgl_round:" arguments:@[MGLConstantExpression(@1.5)]];
        NSArray *jsonExpression = @[@"round", @1.5];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([expression expressionValueWithObject:nil context:nil], @2);
    }
    {
        NSExpression *expression = [NSExpression expressionForFunction:@"mgl_round:" arguments:@[MGLConstantExpression(@-1.5)]];
        NSArray *jsonExpression = @[@"round", @-1.5];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([expression expressionValueWithObject:nil context:nil], @-2);
    }
    {
        NSExpression *expression = [NSExpression expressionForFunction:@"mgl_round:" arguments:@[MGLConstantExpression(@2.5)]];
        NSArray *jsonExpression = @[@"round", @2.5];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([expression expressionValueWithObject:nil context:nil], @3);
    }
    {
        NSExpression *expression = [NSExpression expressionForFunction:@"mgl_round:" arguments:@[MGLConstantExpression(@-2.5)]];
        NSArray *jsonExpression = @[@"round", @-2.5];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([expression expressionValueWithObject:nil context:nil], @-3);
    }
}

- (void)testTrigonometricExpressionObject {
    NSArray *arguments = @[MGLConstantExpression(@1), MGLConstantExpression(@1)];
    {
        NSExpression *expression = [NSExpression expressionForFunction:@"sqrt:" arguments:arguments];
        NSArray *jsonExpression = @[@"sqrt", @1, @1];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
    {
        NSExpression *expression = [NSExpression expressionForFunction:@"ln:" arguments:arguments];
        NSArray *jsonExpression = @[@"ln", @1, @1];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
    {
        NSExpression *expression = [NSExpression expressionForFunction:@"mgl_log2:" arguments:@[MGLConstantExpression(@1024)]];
        NSArray *jsonExpression = @[@"log2", @1024];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([expression expressionValueWithObject:nil context:nil], @10);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
    {
        NSExpression *expression = [NSExpression expressionForFunction:@"raise:toPower:" arguments:arguments];
        NSArray *jsonExpression = @[@"^", @1, @1];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithFormat:@"1 ** 1"].mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
    {
        NSExpression *expression = [NSExpression expressionForFunction:@"exp:" arguments:@[MGLConstantExpression(@0)]];
        NSArray *jsonExpression = @[@"^", @[@"e"], @0];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([expression expressionValueWithObject:nil context:nil], @1);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
    {
        NSExpression *expression = [NSExpression expressionForConstantValue:@(M_E)];
        NSArray *jsonExpression = @[@"e"];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([expression expressionValueWithObject:nil context:nil], @(M_E));
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
    {
        NSExpression *expression = [NSExpression expressionForConstantValue:@(M_PI)];
        NSArray *jsonExpression = @[@"pi"];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([expression expressionValueWithObject:nil context:nil], @(M_PI));
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
    {
        NSExpression *expression = [NSExpression expressionForFunction:@"mgl_acos:" arguments:@[MGLConstantExpression(@1)]];
        NSArray *jsonExpression = @[@"acos", @1];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([expression expressionValueWithObject:nil context:nil], @0);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
    {
        NSExpression *expression = [NSExpression expressionForFunction:@"mgl_cos:" arguments:@[MGLConstantExpression(@0)]];
        NSArray *jsonExpression = @[@"cos", @0];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([expression expressionValueWithObject:nil context:nil], @1);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
    {
        NSExpression *expression = [NSExpression expressionForFunction:@"mgl_asin:" arguments:@[MGLConstantExpression(@0)]];
        NSArray *jsonExpression = @[@"asin", @0];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([expression expressionValueWithObject:nil context:nil], @0);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
    {
        NSExpression *expression = [NSExpression expressionForFunction:@"mgl_sin:" arguments:@[MGLConstantExpression(@0)]];
        NSArray *jsonExpression = @[@"sin", @0];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([expression expressionValueWithObject:nil context:nil], @0);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
    {
        NSExpression *expression = [NSExpression expressionForFunction:@"mgl_atan:" arguments:@[MGLConstantExpression(@20)]];
        NSArray *jsonExpression = @[@"atan", @20];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        NSNumber *value = [expression expressionValueWithObject:nil context:nil];
        XCTAssertEqualWithAccuracy(value.doubleValue, 1.52, 0.001);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
    {
        NSExpression *expression = [NSExpression expressionForFunction:@"mgl_tan:" arguments:@[MGLConstantExpression(@0)]];
        NSArray *jsonExpression = @[@"tan", @0];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([expression expressionValueWithObject:nil context:nil], @0);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
}

- (void)testStringFormattingExpressionObject {
    NSArray *arguments = @[MGLConstantExpression(@"MacDonald")];
    {
        NSExpression *expression = [NSExpression expressionWithFormat:@"FUNCTION('Old', 'stringByAppendingString:', 'MacDonald')"];
        NSExpression *aftermarketExpression = [NSExpression expressionWithFormat:@"mgl_join({'Old', 'MacDonald'})"];
        NSArray *jsonExpression = @[@"concat", @"Old", @"MacDonald"];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects(aftermarketExpression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([expression expressionValueWithObject:nil context:nil], @"OldMacDonald");
        XCTAssertEqualObjects([aftermarketExpression expressionValueWithObject:nil context:nil], @"OldMacDonald");
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], aftermarketExpression);
    }
    {
        NSExpression *expression = [NSExpression expressionForFunction:@"mgl_join:" arguments:@[@"Old", @"MacDonald"]];
        NSExpression *aftermarketExpression = [NSExpression expressionWithFormat:@"mgl_join({'Old', 'MacDonald'})"];
        NSArray *jsonExpression = @[@"concat", @"Old", @"MacDonald"];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);

        XCTAssertEqualObjects(aftermarketExpression.mgl_jsonExpressionObject, expression.mgl_jsonExpressionObject);
        NSExpression *aggregateExpression = [NSExpression expressionWithMGLJSONObject:jsonExpression];
        XCTAssertEqualObjects(aggregateExpression.operand, expression.operand);
    }
    {
        NSExpression *expression = [NSExpression expressionForFunction:@"uppercase:" arguments:arguments];
        NSArray *jsonExpression = @[@"upcase", @"MacDonald"];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
    {
        NSExpression *expression = [NSExpression expressionForFunction:@"lowercase:" arguments:arguments];
        NSArray *jsonExpression = @[@"downcase", @"MacDonald"];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
    {
        NSExpression *expression = [NSExpression expressionForFunction:@"length:" arguments:arguments];
        NSArray *jsonExpression = @[@"length", @"MacDonald"];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
}

- (void)testTypeConversionExpressionObject {
    {
        NSExpression *expression = [NSExpression expressionWithFormat:@"FUNCTION(number, 'boolValue')"];
        NSArray *jsonExpression = @[@"to-boolean", @[@"get", @"number"]];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        // NSExpression is unable to evaluate -[NSNumber boolValue] by itself
        // because it returns a primitive instead of an object.
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
    {
        NSExpression *expression = [NSExpression expressionWithFormat:@"FUNCTION(postalCode, 'mgl_number')"];
        NSArray *jsonExpression = @[@"to-number", @[@"get", @"postalCode"]];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithFormat:@"FUNCTION(postalCode, 'doubleValue')"].mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithFormat:@"FUNCTION(postalCode, 'floatValue')"].mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithFormat:@"FUNCTION(postalCode, 'decimalValue')"].mgl_jsonExpressionObject, jsonExpression);
        // NSExpression is unable to evaluate NSNumber’s -floatValue,
        // -doubleValue, or -decimalValue by themselves because they each return
        // a primitive instead of an object.
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression],
                              [NSExpression expressionWithFormat:@"CAST(postalCode, 'NSNumber')"]);
    }
    {
        NSExpression *expression = [NSExpression expressionWithFormat:@"FUNCTION(postalCode, 'mgl_numberWithFallbackValues:', zipCode)"];
        NSArray *jsonExpression = @[@"to-number", @[@"get", @"postalCode"], @[@"get", @"zipCode"]];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithFormat:@"FUNCTION(postalCode, 'doubleValue', zipCode)"].mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithFormat:@"FUNCTION(postalCode, 'floatValue', zipCode)"].mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithFormat:@"FUNCTION(postalCode, 'decimalValue', zipCode)"].mgl_jsonExpressionObject, jsonExpression);
        // NSExpression is unable to evaluate NSNumber’s -floatValue,
        // -doubleValue, or -decimalValue by themselves because they each return
        // a primitive instead of an object.
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
    {
        NSExpression *expression = [NSExpression expressionWithFormat:@"CAST(postalCode, 'NSNumber')"];
        NSExpression *compatibilityExpression = [NSExpression expressionWithFormat:@"FUNCTION(postalCode, 'mgl_numberWithFallbackValues:')"];
        NSArray *jsonExpression = @[@"to-number", @[@"get", @"postalCode"]];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects(compatibilityExpression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([expression expressionValueWithObject:@{@"postalCode": @"02134"} context:nil], @02134);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
    {
        NSExpression *expression = [NSExpression expressionWithFormat:@"CAST(number, 'NSString')"];
        NSExpression *compatibilityExpression = [NSExpression expressionWithFormat:@"FUNCTION(number, 'stringValue')"];
        NSArray *jsonExpression = @[@"to-string", @[@"get", @"number"]];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects(compatibilityExpression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([expression expressionValueWithObject:@{@"number": @1.5} context:nil], @"1.5");
        XCTAssertEqualObjects([compatibilityExpression expressionValueWithObject:@{@"number": @1.5} context:nil], @"1.5");
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
    {
#if TARGET_OS_IPHONE
        NSExpression *expression = [NSExpression expressionWithFormat:@"CAST(x, 'UIColor')"];
#else
        NSExpression *expression = [NSExpression expressionWithFormat:@"CAST(x, 'NSColor')"];
#endif
        
        NSArray *jsonExpression = @[@"to-color", @[@"get", @"x"]];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
    {
        NSExpression *expression = [NSExpression expressionWithFormat:@"MGL_FUNCTION('to-color', x, y, z)"];
        NSArray *jsonExpression = @[@"to-color", @[@"get", @"x"], @[@"get", @"y"], @[@"get", @"z"]];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
    }
    {
        NSExpression *expression = [NSExpression expressionWithFormat:@"CAST(noindex(x), 'NSArray')"];
        NSArray *jsonExpression = @[@"to-rgba", @[@"get", @"x"]];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
    {
        NSExpression *expression = [NSExpression expressionWithFormat:@"CAST(noindex(%@), 'NSArray')", MGLConstantExpression(MGLColor.blueColor)];
        NSArray *jsonExpression = @[@"to-rgba", @[@"rgb", @0, @0, @255]];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
    {
        NSExpression *expression = [NSExpression expressionWithFormat:@"CAST(noindex('x'), 'NSArray')"];
        XCTAssertThrowsSpecificNamed(expression.mgl_jsonExpressionObject, NSException, NSInvalidArgumentException);
    }
}

- (void)testInterpolationExpressionObject {
    {
        NSDictionary *stops = @{@0: MGLConstantExpression(@100), @10: MGLConstantExpression(@200)};
        NSExpression *expression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:(x, 'linear', nil, %@)", stops];
        NSExpression *compatibilityExpression = [NSExpression expressionWithFormat:@"FUNCTION(x, 'mgl_interpolateWithCurveType:parameters:stops:', 'linear', nil, %@)", stops];
        NSArray *jsonExpression = @[@"interpolate", @[@"linear"], @[@"get", @"x"], @0, @100, @10, @200];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects(compatibilityExpression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
    {
        NSDictionary *stops = @{@1: MGLConstantExpression(@2), @3: MGLConstantExpression(@6)};
        NSExpression *expression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:(x, 'exponential', 2, %@)", stops];
        NSArray *jsonExpression = @[@"interpolate", @[@"exponential", @2], @[@"get", @"x"], @1, @2, @3, @6];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
    {
        NSDictionary *stops = @{@0: MGLConstantExpression(@0), @100: MGLConstantExpression(@100)};
        NSExpression *expression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:(x, 'cubic-bezier', { 0.42, 0, 0.58, 1 }, %@)", stops];
        NSArray *jsonExpression = @[@"interpolate", @[@"cubic-bezier", @0.42, @0, @0.58, @1], @[@"get", @"x"], @0, @0, @100, @100];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
    {
        NSDictionary *stops = @{@0: MGLConstantExpression(@111), @1: MGLConstantExpression(@1111)};
        NSExpression *expression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:(x, 11, %@)", stops];
        NSExpression *compatibilityExpression = [NSExpression expressionWithFormat:@"FUNCTION(x, 'mgl_stepWithMinimum:stops:', 11, %@)", stops];
        NSArray *jsonExpression = @[@"step", @[@"get", @"x"], @11, @0, @111, @1, @1111];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects(compatibilityExpression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
    {
        NSDictionary *stops = @{@0: MGLConstantExpression(@111), @1: MGLConstantExpression(@1111)};
        NSExpression *expression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, 11, %@)", stops];
        NSArray *jsonExpression = @[@"step", @[@"zoom"], @11, @0, @111, @1, @1111];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
    {
        NSDictionary *stops = @{};
        NSExpression *expression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:(x, 'cubic-bezier', { 0.42, 0, 0.58, 1 }, %@)", stops];
        XCTAssertThrowsSpecificNamed(expression.mgl_jsonExpressionObject, NSException, NSInvalidArgumentException);
    }
    {
        NSDictionary *stops = @{};
        NSExpression *expression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, 11, %@)", stops];
        XCTAssertThrowsSpecificNamed(expression.mgl_jsonExpressionObject, NSException, NSInvalidArgumentException);
    }
}

- (void)testMatchExpressionObject {
    {
        NSExpression *expression = [NSExpression expressionWithFormat:@"MGL_MATCH(2 - 1,  %@, %@, %@, %@, 'default')", MGLConstantExpression(@1),
                                    MGLConstantExpression(@"one"),
                                    MGLConstantExpression(@0),
                                    MGLConstantExpression(@"zero")];
        NSExpression *predicate = [NSExpression expressionWithFormat:@"2 - 1"];
        NSExpression *compatibilityExpression = [NSExpression expressionWithFormat:@"FUNCTION(%@, 'mgl_match:', %@)", predicate, @[MGLConstantExpression(@1),
                                                 MGLConstantExpression(@"one"),
                                                 MGLConstantExpression(@0),
                                                 MGLConstantExpression(@"zero"),
                                                 MGLConstantExpression(@"default")]];
        NSArray *jsonExpression =  @[@"match", @[@"-", @2, @1], @1, @"one", @0, @"zero", @"default"];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects(compatibilityExpression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
    {
        NSExpression *expression = [NSExpression expressionWithFormat:@"MGL_MATCH(2 * 1, %@, %@, 'default')", MGLConstantExpression(@1), MGLConstantExpression(@"one")];
        NSArray *jsonExpression =  @[@"match", @[@"*", @2, @1], @1, @"one", @"default"];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
    {
        NSExpression *expression = [NSExpression expressionWithFormat:@"MGL_MATCH(x, {'a', 'A'}, 'Apple', {'b', 'B'}, 'Banana', 'Kumquat')"];
        NSArray *jsonExpression =  @[@"match", @[@"get", @"x"], @[@"a", @"A"], @"Apple", @[@"b", @"B"], @"Banana", @"Kumquat"];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
    {
        NSExpression *expression = [NSExpression expressionWithFormat:@"MGL_MATCH(x, %@, 'Apple', %@, 'Banana', 'Kumquat')",
                                    @[@"a", @"A"], @"Bb"];
        NSArray *jsonExpression =  @[@"match", @[@"get", @"x"], @[@"a", @"A"], @"Apple", @"Bb", @"Banana", @"Kumquat"];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression].description, expression.description);
    }
}

- (void)testCoalesceExpressionObject {
    {
        NSExpression *expression = [NSExpression expressionWithFormat:@"mgl_coalesce(%@)",
                                    @[[NSExpression expressionForKeyPath:@"x"],
                                      [NSExpression expressionForKeyPath:@"y"],
                                      [NSExpression expressionForKeyPath:@"z"],
                                      [NSExpression expressionForConstantValue:@0]]];
        NSExpression *compatibilityExpression = [NSExpression expressionWithFormat:@"FUNCTION(%@, 'mgl_coalesce')", @[[NSExpression expressionForKeyPath:@"x"],
                                                                                                                      [NSExpression expressionForKeyPath:@"y"],
                                                                                                                      [NSExpression expressionForKeyPath:@"z"],
                                                                                                                      [NSExpression expressionForConstantValue:@0]]];
        NSArray *jsonExpression = @[@"coalesce", @[@"get", @"x"], @[@"get", @"y"], @[@"get", @"z"], @0];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects(compatibilityExpression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
    
}

- (void)testConditionalExpressionObject {
    {
        NSPredicate *conditional = [NSPredicate predicateWithFormat:@"1 = 2"];
        NSExpression *trueExpression = [NSExpression expressionForConstantValue:@YES];
        NSExpression *falseExpression = [NSExpression expressionForConstantValue:@NO];
        NSExpression *expression = [NSExpression expressionForConditional:conditional trueExpression:trueExpression falseExpression:falseExpression];
        NSArray *jsonExpression = @[@"case", @[@"==", @1, @2], @YES, @NO];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithFormat:@"TERNARY(1 = 2, TRUE, FALSE)"].mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([expression expressionValueWithObject:nil context:nil], @NO);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
    {
        NSExpression *expression = [NSExpression expressionWithFormat:@"TERNARY(0 = 1, TRUE, TERNARY(1 = 2, TRUE, FALSE))"];
        NSArray *jsonExpression = @[@"case", @[@"==", @0, @1], @YES, @[@"case", @[@"==", @1, @2], @YES, @NO]];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([expression expressionValueWithObject:nil context:nil], @NO);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
    {
        NSExpression *expression = [NSExpression expressionWithFormat:@"MGL_IF(%@, %@, %@)",
                                    [NSExpression expressionWithFormat:@"%@", [NSPredicate predicateWithFormat:@"1 = 2"]],
                                    MGLConstantExpression(@YES),
                                    MGLConstantExpression(@NO)];
        NSExpression *compatibilityExpression = [NSExpression expressionWithFormat:@"FUNCTION(%@, 'mgl_if:', %@)", [NSPredicate predicateWithFormat:@"1 = 2"], @[MGLConstantExpression(@YES), MGLConstantExpression(@NO)]];
        NSArray *jsonExpression = @[@"case", @[@"==", @1, @2], @YES, @NO];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects(compatibilityExpression.mgl_jsonExpressionObject, jsonExpression);
        expression = [NSExpression expressionWithFormat:@"TERNARY(1 = 2, YES, NO)"];
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
        XCTAssertEqualObjects([expression expressionValueWithObject:nil context:nil], @NO);
    }
    {
        NSExpression *expression = [NSExpression expressionWithFormat:@"MGL_IF(%@, %@, %@, %@, %@)",
                                    [NSExpression expressionWithFormat:@"%@", [NSPredicate predicateWithFormat:@"1 = 2"]],
                                    MGLConstantExpression(@YES),
                                    [NSExpression expressionWithFormat:@"%@", [NSPredicate predicateWithFormat:@"1 = 1"]],
                                    MGLConstantExpression(@YES),
                                    MGLConstantExpression(@NO)];
        NSArray *jsonExpression = @[@"case", @[@"==", @1, @2], @YES, @[@"==", @1, @1], @YES, @NO];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
        XCTAssertEqualObjects([expression expressionValueWithObject:nil context:nil], @YES);
    }
    {
        NSArray *jsonExpression = @[
            @"case",
            @[
                @"<",
                @[@"get", @"area"],
                @80000
            ],
            @[@"get", @"abbr"],
            @[@"get", @"name_en"]
        ];
        NSExpression *expression = [NSExpression expressionWithFormat:@"TERNARY(area < 80000, abbr, name_en)"];
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
    }
}

- (void)testLookupExpressionObject {
    {
        NSExpression *array = [NSExpression expressionForAggregate:@[MGLConstantExpression(@9),
                                                                     MGLConstantExpression(@8),
                                                                     MGLConstantExpression(@7)]];
        NSExpression *expression = [NSExpression expressionForFunction:@"objectFrom:withIndex:"
                                                             arguments:@[array, MGLConstantExpression(@"FIRST")]];
        NSArray *jsonExpression = @[@"at", @0, @[ @"literal", @[@9, @8, @7]]];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
    }
    {
        NSExpression *array = [NSExpression expressionForAggregate:@[MGLConstantExpression(@9),
                                                                     MGLConstantExpression(@8),
                                                                     MGLConstantExpression(@7)]];
        NSExpression *expression = [NSExpression expressionForFunction:@"objectFrom:withIndex:"
                                                             arguments:@[array, MGLConstantExpression(@"LAST")]];
        NSArray *jsonExpression = @[@"at", @[@"-",  @[@"length", @[ @"literal", @[@9, @8, @7]]], @1], @[ @"literal", @[@9, @8, @7]]];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
    }
    {
        NSExpression *array = [NSExpression expressionForAggregate:@[MGLConstantExpression(@9),
                                                                     MGLConstantExpression(@8),
                                                                     MGLConstantExpression(@7)]];
        NSExpression *expression = [NSExpression expressionForFunction:@"objectFrom:withIndex:"
                                                             arguments:@[array, MGLConstantExpression(@"SIZE")]];
        NSArray *jsonExpression = @[@"length", @[ @"literal", @[@9, @8, @7]]];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
    }
    {
        NSExpression *array = [NSExpression expressionForAggregate:@[MGLConstantExpression(@9),
                                                                     MGLConstantExpression(@8),
                                                                     MGLConstantExpression(@7)]];
        NSExpression *expression = [NSExpression expressionForFunction:@"objectFrom:withIndex:"
                                                             arguments:@[array, MGLConstantExpression(@1)]];
        NSArray *jsonExpression = @[@"at", @1, @[ @"literal", @[@9, @8, @7]]];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
    {
        NSExpression *array = [NSExpression expressionForAggregate:@[MGLConstantExpression(@9),
                                                                     MGLConstantExpression(@8),
                                                                     MGLConstantExpression(@7)]];
        NSExpression *expression = [NSExpression expressionForFunction:@"objectFrom:withIndex:"
                                                             arguments:@[array, [NSExpression expressionForKeyPath:@"x"]]];
        NSArray *jsonExpression = @[@"at", @[@"get", @"x"], @[ @"literal", @[@9, @8, @7]]];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
    {
        NSExpression *expression = [NSExpression expressionForFunction:@"mgl_does:have:"
                                                             arguments:@[[NSExpression expressionForEvaluatedObject],
                                                                         [NSExpression expressionForConstantValue:@"x"]]];
        NSExpression *compatibilityExpression = [NSExpression expressionWithFormat:@"FUNCTION(self, 'mgl_has:', 'x')"];
        NSArray *jsonExpression = @[@"has", @"x"];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects(compatibilityExpression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
    {
        NSExpression *expression = [NSExpression expressionForFunction:@"mgl_does:have:"
                                                             arguments:@[MGLConstantExpression(@{@"x": MGLConstantExpression(@0)}),
                                                                         MGLConstantExpression(@"x")]];
        NSArray *jsonExpression = @[@"has", @"x",  @[@"literal", @{@"x": @0}]];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
    {
        NSExpression *expression = [NSExpression expressionForFunction:@"mgl_does:have:"
                                                             arguments:@[[NSExpression expressionForVariable:@"featureAttributes"],
                                                                         [NSExpression expressionForConstantValue:@"x"]]];
        NSArray *jsonExpression = @[@"has", @"x", @[@"properties"]];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
    {
        NSExpression *expression;
        expression = [NSExpression expressionWithFormat:@"TERNARY(key != nil, 1, 0)"];
        NSArray *jsonExpression = @[@"case", @[@"!=", @[@"get", @"key"], [NSNull null]], @1, @0];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
        XCTAssertEqualObjects([expression expressionValueWithObject:@{} context:nil], @NO);
        XCTAssertEqualObjects([expression expressionValueWithObject:@{@"key": @"🗝"} context:nil], @YES);
    }
    {
        NSDictionary *dictionary = @{@"key": @"🔑"};
        NSExpression *expression;
        expression = [NSExpression expressionWithFormat:@"TERNARY(%@.key != nil, 1, 0)", dictionary];
        NSArray *jsonExpression = @[@"case", @[@"!=", @[@"get", @"key", @[@"literal", dictionary]], [NSNull null]], @1, @0];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        // The dictionary isn’t equal enough.
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression].description, expression.description);
        XCTAssertEqualObjects([expression expressionValueWithObject:nil context:nil], @YES);
    }
}

- (void)testFormatExpressionObject {
    {
        MGLAttributedExpression *attribute1 = [MGLAttributedExpression attributedExpression:[NSExpression expressionForConstantValue:@"foo"]
                                                                                  fontNames:nil
                                                                                  fontScale:@(1.2)];
        MGLAttributedExpression *attribute2 = [MGLAttributedExpression attributedExpression:[NSExpression expressionForConstantValue:@"biz"]
                                                                                  fontNames:nil
                                                                                  fontScale:@(1.0)];
        MGLAttributedExpression *attribute3 = [MGLAttributedExpression attributedExpression:[NSExpression expressionForConstantValue:@"bar"]
                                                                                  fontNames:nil
                                                                                  fontScale:@(0.8)];
        MGLAttributedExpression *attribute4 = [MGLAttributedExpression attributedExpression:[NSExpression expressionForConstantValue:@"\r"]
                                                                                  fontNames:@[]
                                                                                  fontScale:nil];
        NSExpression *expression = [NSExpression expressionWithFormat:@"mgl_attributed:(%@, %@, %@, %@)",
                                    MGLConstantExpression(attribute1),
                                    MGLConstantExpression(attribute4),
                                    MGLConstantExpression(attribute2),
                                    MGLConstantExpression(attribute3)];
        NSArray *jsonExpression = @[@"format", @"foo", @{@"font-scale": @1.2}, @"\r", @{}, @"biz", @{@"font-scale": @1.0}, @"bar", @{@"font-scale": @0.8}];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
    {
        MGLAttributedExpression *attribute1 = [MGLAttributedExpression attributedExpression:[NSExpression expressionForConstantValue:@"foo"]
                                                                                  fontNames:nil
                                                                                  fontScale:@(1.2)];
        MGLAttributedExpression *attribute2 = [MGLAttributedExpression attributedExpression:[NSExpression expressionForConstantValue:@"biz"]
                                                                                  fontNames:nil
                                                                                  fontScale:@(1.0)];
        MGLAttributedExpression *attribute3 = [MGLAttributedExpression attributedExpression:[NSExpression expressionForConstantValue:@"bar"]
                                                                                  fontNames:nil
                                                                                  fontScale:@(0.8)];
        MGLAttributedExpression *attribute4 = [MGLAttributedExpression attributedExpression:[NSExpression expressionForConstantValue:@"\n"]
                                                                                  fontNames:@[]
                                                                                  fontScale:nil];
        NSExpression *expression = [NSExpression expressionWithFormat:@"mgl_attributed:(%@, %@, %@, %@)",
                                    MGLConstantExpression(attribute1),
                                    MGLConstantExpression(attribute4),
                                    MGLConstantExpression(attribute2),
                                    MGLConstantExpression(attribute3)];
        NSArray *jsonExpression = @[@"format", @"foo", @{@"font-scale": @1.2}, @"\n", @{}, @"biz", @{@"font-scale": @1.0}, @"bar", @{@"font-scale": @0.8}];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
    {
        MGLAttributedExpression *attribute1 = [MGLAttributedExpression attributedExpression:[NSExpression expressionForConstantValue:@"foo"]
                                                                                  fontNames:nil
                                                                                   fontScale:@(1.2)];
        NSExpression *expression = [NSExpression expressionWithFormat:@"mgl_attributed:(%@)", MGLConstantExpression(attribute1)];
        
        NSExpression *compatibilityExpression = [NSExpression expressionForFunction:@"mgl_attributed:" arguments:@[MGLConstantExpression(attribute1)]];
        NSArray *jsonExpression = @[@"format", @"foo", @{@"font-scale": @1.2}];
        XCTAssertEqualObjects(compatibilityExpression.mgl_jsonExpressionObject, expression.mgl_jsonExpressionObject);
        XCTAssertEqualObjects(compatibilityExpression, expression);
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
    {
        MGLAttributedExpression *attribute1 = [[MGLAttributedExpression alloc] initWithExpression:[NSExpression expressionForConstantValue:@"foo"]
                                                                                       attributes:@{ MGLFontScaleAttribute: MGLConstantExpression(@(1.2)),
                                                                                                     MGLFontColorAttribute: MGLConstantExpression(@"yellow") }] ;
        NSExpression *expression = [NSExpression expressionWithFormat:@"mgl_attributed:(%@)", MGLConstantExpression(attribute1)];
        
        NSExpression *compatibilityExpression = [NSExpression expressionForFunction:@"mgl_attributed:" arguments:@[MGLConstantExpression(attribute1)]];
        NSArray *jsonExpression = @[ @"format", @"foo", @{ @"font-scale": @1.2, @"text-color": @"yellow" } ];
        XCTAssertEqualObjects(compatibilityExpression.mgl_jsonExpressionObject, expression.mgl_jsonExpressionObject);
        XCTAssertEqualObjects(compatibilityExpression, expression);
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
    {
        MGLAttributedExpression *attribute1 = [[MGLAttributedExpression alloc] initWithExpression:[NSExpression expressionForConstantValue:@"foo"]] ;
        NSExpression *expression = [NSExpression expressionWithFormat:@"mgl_attributed:(%@)", MGLConstantExpression(attribute1)];
        
        NSArray *jsonExpression = @[ @"format", @"foo", @{ } ];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
    {
        NSExpression *fontNames = [NSExpression expressionForAggregate:@[ MGLConstantExpression(@"DIN Offc Pro Bold"), MGLConstantExpression(@"Arial Unicode MS Bold") ]];
        MGLAttributedExpression *attribute1 = [[MGLAttributedExpression alloc] initWithExpression:[NSExpression expressionForConstantValue:@"foo"]
                                                                                       attributes:@{ MGLFontScaleAttribute: MGLConstantExpression(@(1.2)),
                                                                                                     MGLFontColorAttribute: MGLConstantExpression(@"yellow"),
                                                                                                     MGLFontNamesAttribute: fontNames
                                                                                                     }] ;
        NSExpression *expression = [NSExpression expressionWithFormat:@"mgl_attributed:(%@)", MGLConstantExpression(attribute1)];
        
        NSArray *jsonExpression = @[ @"format", @"foo", @{ @"font-scale": @1.2, @"text-color": @"yellow" , @"text-font" : @[ @"literal", @[ @"DIN Offc Pro Bold", @"Arial Unicode MS Bold" ]]} ];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
    {
        NSExpression *fontNames = [NSExpression expressionForAggregate:@[ MGLConstantExpression(@"DIN Offc Pro Bold"), MGLConstantExpression(@"Arial Unicode MS Bold") ]];
        MGLAttributedExpression *attribute1 = [[MGLAttributedExpression alloc] initWithExpression:[NSExpression expressionForConstantValue:@"foo"]
                                                                                       attributes:@{ MGLFontScaleAttribute: MGLConstantExpression(@(1.2)),
                                                                                                     MGLFontColorAttribute: MGLConstantExpression([MGLColor redColor]),
                                                                                                     MGLFontNamesAttribute: fontNames
                                                                                                     }] ;
        NSExpression *expression = [NSExpression expressionWithFormat:@"mgl_attributed:(%@)", MGLConstantExpression(attribute1)];
        
        NSArray *jsonExpression = @[ @"format", @"foo", @{ @"font-scale": @1.2, @"text-color": @[@"rgb", @255, @0, @0] , @"text-font" : @[ @"literal", @[ @"DIN Offc Pro Bold", @"Arial Unicode MS Bold" ]]} ];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
    {
        NSExpression *fontNames = [NSExpression expressionForAggregate:@[ MGLConstantExpression(@"DIN Offc Pro Bold"), MGLConstantExpression(@"Arial Unicode MS Bold") ]];
        MGLAttributedExpression *attribute1 = [[MGLAttributedExpression alloc] initWithExpression:[NSExpression expressionWithFormat:@"CAST(x, 'NSString')"]
                                                                                       attributes:@{ MGLFontScaleAttribute: MGLConstantExpression(@(1.2)),
                                                                                                     MGLFontColorAttribute: MGLConstantExpression([MGLColor redColor]),
                                                                                                     MGLFontNamesAttribute: fontNames
                                                                                                     }] ;
        NSExpression *expression = [NSExpression expressionWithFormat:@"mgl_attributed:(%@)", MGLConstantExpression(attribute1)];
        
        NSArray *jsonExpression = @[ @"format", @[@"to-string", @[@"get", @"x"]], @{ @"font-scale": @1.2, @"text-color": @[@"rgb", @255, @0, @0] , @"text-font" : @[ @"literal", @[ @"DIN Offc Pro Bold", @"Arial Unicode MS Bold" ]]} ];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
    {
        MGLAttributedExpression *attribute1 = [MGLAttributedExpression attributedExpression:[NSExpression expressionForConstantValue:@"foo"]
                                                                                  fontNames:nil
                                                                                  fontScale:@(1.2)];
        MGLAttributedExpression *attribute2 = [MGLAttributedExpression attributedExpression:[NSExpression expressionForConstantValue:@"biz"]
                                                                                  fontNames:nil
                                                                                  fontScale:@(1.0)];
        MGLAttributedExpression *attribute3 = [MGLAttributedExpression attributedExpression:[NSExpression expressionForConstantValue:@"bar"]
                                                                                  fontNames:nil
                                                                                  fontScale:@(0.8)];
        MGLAttributedExpression *attribute4 = [MGLAttributedExpression attributedExpression:[NSExpression expressionForConstantValue:@"\n"]
                                                                                  fontNames:@[]
                                                                                  fontScale:nil];
        NSExpression *expression = [NSExpression mgl_expressionForAttributedExpressions:@[MGLConstantExpression(attribute1),
                                                                                          MGLConstantExpression(attribute4),
                                                                                          MGLConstantExpression(attribute2),
                                                                                          MGLConstantExpression(attribute3)]];
        NSArray *jsonExpression = @[@"format", @"foo", @{@"font-scale": @1.2}, @"\n", @{}, @"biz", @{@"font-scale": @1.0}, @"bar", @{@"font-scale": @0.8}];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
}

- (void)testGenericExpressionObject {
    {
        NSExpression *expression = [NSExpression expressionWithFormat:@"MGL_FUNCTION('random', 1, 2, 3, 4, 5)"];
        NSArray *jsonExpression = @[@"random", @1, @2, @3, @4, @5];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
    {
        NSExpression *expression = [NSExpression expressionWithFormat:@"MGL_FUNCTION('random', 1, 2, 3, 4)"];
        XCTAssertThrowsSpecificNamed([expression expressionValueWithObject:nil context:nil], NSException, NSInvalidArgumentException);
    }
    {
        NSArray *arguments = @[
            MGLConstantExpression(@"one"), MGLConstantExpression(@1),
            [NSExpression expressionForVariable:@"one"],
        ];
        NSExpression *nullaryExpression = [NSExpression expressionForFunction:@"MGL_LET" arguments:arguments];
        NSExpression *unaryExpression = [NSExpression expressionForFunction:@"MGL_LET:" arguments:arguments];
        XCTAssertEqualObjects(nullaryExpression.mgl_jsonExpressionObject, unaryExpression.mgl_jsonExpressionObject);
    }
    {
        NSArray *arguments = @[
            [NSExpression expressionForVariable:@"x"],
            MGLConstantExpression(@YES), MGLConstantExpression(@"yes"),
            MGLConstantExpression(@NO), MGLConstantExpression(@"no"),
        ];
        NSExpression *nullaryExpression = [NSExpression expressionForFunction:@"MGL_MATCH" arguments:arguments];
        NSExpression *unaryExpression = [NSExpression expressionForFunction:@"MGL_MATCH:" arguments:arguments];
        XCTAssertEqualObjects(nullaryExpression.mgl_jsonExpressionObject, unaryExpression.mgl_jsonExpressionObject);
    }
    {
        NSArray *arguments = @[
            [NSPredicate predicateWithValue:YES],
            MGLConstantExpression(@"yes"), MGLConstantExpression(@"no"),
        ];
        NSExpression *nullaryExpression = [NSExpression expressionForFunction:@"MGL_IF" arguments:arguments];
        NSExpression *unaryExpression = [NSExpression expressionForFunction:@"MGL_IF:" arguments:arguments];
        XCTAssertEqualObjects(nullaryExpression.mgl_jsonExpressionObject, unaryExpression.mgl_jsonExpressionObject);
    }
    {
        NSArray *arguments = @[MGLConstantExpression(@"zoom")];
        NSExpression *nullaryExpression = [NSExpression expressionForFunction:@"MGL_FUNCTION" arguments:arguments];
        NSExpression *unaryExpression = [NSExpression expressionForFunction:@"MGL_FUNCTION:" arguments:arguments];
        XCTAssertEqualObjects(nullaryExpression.mgl_jsonExpressionObject, unaryExpression.mgl_jsonExpressionObject);
    }
}

#pragma mark - Localization tests

- (void)testLocalization {
    {
        NSExpression *original = MGLConstantExpression(@"");
        NSExpression *expected = original;
        XCTAssertEqualObjects([original mgl_expressionLocalizedIntoLocale:nil], expected);
    }
    {
        NSExpression *original = MGLConstantExpression(@"Old MacDonald");
        NSExpression *expected = original;
        XCTAssertEqualObjects([original mgl_expressionLocalizedIntoLocale:nil], expected);
    }
    {
        NSExpression *original = MGLConstantExpression(@"{name_en}");
        NSExpression *expected = original;
        XCTAssertEqualObjects([original mgl_expressionLocalizedIntoLocale:nil], expected);
    }
    {
        NSExpression *keyExpression = [NSExpression expressionForKeyPath:@"name_en"];
        MGLAttributedExpression *attributedExpression = [MGLAttributedExpression attributedExpression:keyExpression attributes:@{}];
        NSExpression *original = [NSExpression expressionForConstantValue:attributedExpression];
        
        NSExpression *coalesceExpression = [NSExpression expressionWithFormat:@"mgl_coalesce({%K, %K})", @"name_en", @"name"];
        MGLAttributedExpression *expectedAttributedExpression = [MGLAttributedExpression attributedExpression:coalesceExpression attributes:@{}];
        NSExpression *expected = [NSExpression expressionForConstantValue:expectedAttributedExpression];
        XCTAssertEqualObjects([original mgl_expressionLocalizedIntoLocale:nil], expected);
    }
    {
        NSExpression *original = [NSExpression expressionForKeyPath:@"name_en"];
        NSExpression *expected = [NSExpression expressionWithFormat:@"mgl_coalesce({%K, %K})", @"name_en", @"name"];
        XCTAssertEqualObjects([original mgl_expressionLocalizedIntoLocale:nil], expected);
    }
    {
        NSExpression *original = [NSExpression expressionForKeyPath:@"name_en"];
        NSExpression *expected = [NSExpression expressionForKeyPath:@"name"];
        XCTAssertEqualObjects([original mgl_expressionLocalizedIntoLocale:[NSLocale localeWithLocaleIdentifier:@"mul"]], expected);
    }
    {
        NSExpression *original = [NSExpression expressionForKeyPath:@"name_en"];
        NSExpression *expected = [NSExpression expressionWithFormat:@"mgl_coalesce({%K, %K})", @"name_fr", @"name"];
        XCTAssertEqualObjects([original mgl_expressionLocalizedIntoLocale:[NSLocale localeWithLocaleIdentifier:@"fr-CA"]], expected);
    }
    {
        NSExpression *original = [NSExpression expressionForKeyPath:@"name_en"];
        NSExpression *expected = [NSExpression expressionWithFormat:@"mgl_coalesce({%K, %K, %K, %K})", @"name_zh-Hans", @"name_zh-CN", @"name_zh", @"name"];
        XCTAssertEqualObjects([original mgl_expressionLocalizedIntoLocale:[NSLocale localeWithLocaleIdentifier:@"zh-Hans"]], expected);
    }
    {
        NSExpression *original = [NSExpression expressionWithFormat:@"mgl_coalesce({%K, %K})", @"name_en", @"name"];
        NSExpression *expected = [NSExpression expressionWithFormat:@"mgl_coalesce:({mgl_coalesce:({name_en, name}), mgl_coalesce:({name_en, name})})"];
        XCTAssertEqualObjects([original mgl_expressionLocalizedIntoLocale:nil], expected);
    }
    {
        NSExpression *original = [NSExpression expressionWithFormat:@"mgl_coalesce({%K, %K})", @"name_en", @"name"];
        NSExpression *expected = [NSExpression expressionWithFormat:@"mgl_coalesce:({mgl_coalesce:({name_ja, name}), mgl_coalesce:({name_ja, name})})"];
        XCTAssertEqualObjects([original mgl_expressionLocalizedIntoLocale:[NSLocale localeWithLocaleIdentifier:@"ja-JP"]], expected);
    }
    {
        NSExpression *original = [NSExpression expressionForKeyPath:@"name_en"];
        NSExpression *expected = [NSExpression expressionForKeyPath:@"name"];
        XCTAssertEqualObjects([original mgl_expressionLocalizedIntoLocale:[NSLocale localeWithLocaleIdentifier:@"tlh"]], expected);
    }
    {
        NSExpression *original = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, short, %@)", @{
            @1: [NSExpression expressionForKeyPath:@"abbr"],
            @2: @"…",
            @3: [NSExpression expressionForKeyPath:@"name_fr"],
        }];
        NSExpression *expected = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, short, %@)", @{
            @1: [NSExpression expressionForKeyPath:@"abbr"],
            @2: @"…",
            @3: [NSExpression expressionWithFormat:@"mgl_coalesce({%K, %K})", @"name_es", @"name"]
        }];
        XCTAssertEqualObjects([original mgl_expressionLocalizedIntoLocale:[NSLocale localeWithLocaleIdentifier:@"es-PR"]], expected);
    }
    {
        NSArray *jsonExpression = @[
            @"step",
            @[@"zoom"],
            @[
                @"case",
                @[
                    @"<",
                    @[
                        @"to-number",
                        @[@"get", @"area"]
                    ],
                    @80000
                ],
                @[@"get", @"abbr"],
                @[@"get", @"name_en"]
            ],
            @5, @[@"get", @"name_en"]
        ];
        NSArray *localizedJSONExpression = @[
            @"step",
            @[@"zoom"],
            @[
                @"case",
                @[
                    @"<",
                    @[
                        @"to-number",
                        @[@"get", @"area"]
                    ],
                    @80000
                ],
                @[@"get", @"abbr"],
                @[@"get", @"name"]
            ],
            @5, @[@"get", @"name"]
        ];
        NSExpression *expression = [NSExpression expressionWithMGLJSONObject:jsonExpression];
        NSExpression *localizedExpression = [expression mgl_expressionLocalizedIntoLocale:[NSLocale localeWithLocaleIdentifier:@"mul"]];
        XCTAssertEqualObjects(localizedExpression.mgl_jsonExpressionObject, localizedJSONExpression);
    }
}

- (void)testConvenienceInitializers {
    {
        NSExpression *expression = [NSExpression mgl_expressionForConditional:[NSPredicate predicateWithFormat:@"1 = 2"]
                                                               trueExpression:MGLConstantExpression(@YES)
                                                             falseExpresssion:MGLConstantExpression(@NO)];
        
        NSArray *jsonExpression = @[@"case", @[@"==", @1, @2], @YES, @NO];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
        XCTAssertEqualObjects([expression expressionValueWithObject:nil context:nil], @NO);
    }
    {
        NSDictionary *stops = @{@0: MGLConstantExpression(@111), @1: MGLConstantExpression(@1111)};
        NSExpression *expression = [NSExpression mgl_expressionForSteppingExpression:[NSExpression expressionForKeyPath:@"x"]
                                                                      fromExpression:[NSExpression expressionForConstantValue:@11]
                                                                               stops:[NSExpression expressionForConstantValue:stops]];
        NSArray *jsonExpression = @[@"step", @[@"get", @"x"], @11, @0, @111, @1, @1111];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
    {
        NSDictionary *stops = @{@0: MGLConstantExpression(@100), @10: MGLConstantExpression(@200)};
        NSExpression *expression = [NSExpression mgl_expressionForInterpolatingExpression:[NSExpression expressionForKeyPath:@"x"]
                                                                            withCurveType:MGLExpressionInterpolationModeLinear
                                                                               parameters:nil
                                                                                    stops:[NSExpression expressionForConstantValue:stops]];
        NSArray *jsonExpression = @[@"interpolate", @[@"linear"], @[@"get", @"x"], @0, @100, @10, @200];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
    {
        NSExpression *expression = [[NSExpression expressionForConstantValue:@"Old"] mgl_expressionByAppendingExpression:[NSExpression expressionForConstantValue:@"MacDonald"]];

        NSArray *jsonExpression = @[@"concat", @"Old", @"MacDonald"];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([expression expressionValueWithObject:nil context:nil], @"OldMacDonald");
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
    {
        NSDictionary *values = @{ MGLConstantExpression(@1): MGLConstantExpression(@"one") };
        NSExpression *expression = [NSExpression mgl_expressionForMatchingExpression:[NSExpression expressionWithFormat:@"2 * 1"]
                                                                        inDictionary:values
                                                                   defaultExpression:[NSExpression expressionForConstantValue:@"default"]];
        NSArray *jsonExpression =  @[@"match", @[@"*", @2, @1], @1, @"one", @"default"];
        XCTAssertEqualObjects(expression.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSExpression expressionWithMGLJSONObject:jsonExpression], expression);
    }
}


@end
