#import <XCTest/XCTest.h>

#import <string>

#import "MGLTypes.h"
#import "NSExpression+MGLAdditions.h"

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

#define MGLAssertConstantEqualsValue(constant, value, ...) \
    MGLAssertEqualValues([NSExpression expressionForConstantValue:constant].mgl_constantMBGLValue, value, __VA_ARGS__);

#define MGLAssertConstantEqualsValueWithAccuracy(constant, value, accuracy, ...) \
    MGLAssertEqualValuesWithAccuracy([NSExpression expressionForConstantValue:constant].mgl_constantMBGLValue, value, accuracy, __VA_ARGS__);

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

@end
