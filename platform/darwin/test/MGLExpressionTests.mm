#import <XCTest/XCTest.h>

#import <string>

#import "NSExpression+MGLAdditions.h"

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

#pragma mark - String Tests

- (void)testExpressionConversionString
{
    NSComparisonPredicate *predicate = [self equalityComparisonPredicateWithRightConstantValue:@"bar"];
    mbgl::Value convertedValue = predicate.rightExpression.mgl_constantMBGLValue;
    XCTAssertTrue(convertedValue.is<std::string>());
    XCTAssertEqualObjects(@(convertedValue.get<std::string>().c_str()), @"bar");
}

- (void)testExpressionConversionStringWithUnicode
{
    NSComparisonPredicate *predicate = [self equalityComparisonPredicateWithRightConstantValue:@"🆔🆗🇦🇶"];
    mbgl::Value convertedValue = predicate.rightExpression.mgl_constantMBGLValue;
    XCTAssertTrue(convertedValue.is<std::string>());
    XCTAssertEqual(convertedValue.get<std::string>(), "🆔🆗🇦🇶");
}

#pragma mark - Boolean Tests

- (void)testExpressionConversionBooleanTrue
{
    NSComparisonPredicate *predicate = [self equalityComparisonPredicateWithRightConstantValue:@YES];
    mbgl::Value convertedValue = predicate.rightExpression.mgl_constantMBGLValue;
    XCTAssertTrue(convertedValue.is<bool>());
    XCTAssertEqual(convertedValue.get<bool>(), true);
}

- (void)testExpressionConversionBooleanFalse
{
    NSComparisonPredicate *predicate = [self equalityComparisonPredicateWithRightConstantValue:@NO];
    mbgl::Value convertedValue = predicate.rightExpression.mgl_constantMBGLValue;
    XCTAssertTrue(convertedValue.is<bool>());
    XCTAssertEqual(convertedValue.get<bool>(), false);
}

#pragma mark - Floating Point Tests

- (void)testExpressionConversionDouble
{
    NSComparisonPredicate *predicate;
    mbgl::Value convertedValue;

    predicate = [self equalityComparisonPredicateWithRightConstantValue:[NSNumber numberWithDouble:DBL_MIN]];
    convertedValue = predicate.rightExpression.mgl_constantMBGLValue;
    XCTAssertTrue(convertedValue.is<double>());
    XCTAssertEqual(convertedValue.get<double>(), DBL_MIN);

    predicate = [self equalityComparisonPredicateWithRightConstantValue:[NSNumber numberWithDouble:DBL_MAX]];
    convertedValue = predicate.rightExpression.mgl_constantMBGLValue;
    XCTAssertTrue(convertedValue.is<double>());
    XCTAssertEqual(convertedValue.get<double>(), DBL_MAX);
}

- (void)testExpressionConversionFloat
{
    // Because we can't guarantee precision when using float, and because
    // we warn the user to this effect in -[NSExpression mgl_constantMBGLValue],
    // we just check that things are in the ballpark here with integer values
    // and some lower-precision checks.

    NSComparisonPredicate *predicate;
    mbgl::Value convertedValue;

    predicate = [self equalityComparisonPredicateWithRightConstantValue:[NSNumber numberWithFloat:-1]];
    convertedValue = predicate.rightExpression.mgl_constantMBGLValue;
    XCTAssertTrue(convertedValue.is<double>());
    XCTAssertEqual(convertedValue.get<double>(), -1);

    predicate = [self equalityComparisonPredicateWithRightConstantValue:[NSNumber numberWithFloat:1]];
    convertedValue = predicate.rightExpression.mgl_constantMBGLValue;
    XCTAssertTrue(convertedValue.is<double>());
    XCTAssertEqual(convertedValue.get<double>(), 1);

    predicate = [self equalityComparisonPredicateWithRightConstantValue:[NSNumber numberWithFloat:-23.232342]];
    convertedValue = predicate.rightExpression.mgl_constantMBGLValue;
    XCTAssertTrue(convertedValue.is<double>());
    XCTAssertEqualWithAccuracy(convertedValue.get<double>(), -23.232342, 0.000001);

    predicate = [self equalityComparisonPredicateWithRightConstantValue:[NSNumber numberWithFloat:23.232342]];
    convertedValue = predicate.rightExpression.mgl_constantMBGLValue;
    XCTAssertTrue(convertedValue.is<double>());
    XCTAssertEqualWithAccuracy(convertedValue.get<double>(), 23.232342, 0.000001);
}

#pragma mark - Integer Tests

- (void)testExpressionNegativeIntegers
{
    NSComparisonPredicate *predicate;
    mbgl::Value convertedValue;

    NSArray<NSNumber *> *minValues = @[
        [NSNumber numberWithShort:    SHRT_MIN],
        [NSNumber numberWithInt:      INT_MIN],
        [NSNumber numberWithLong:     LONG_MIN],
        [NSNumber numberWithLongLong: LLONG_MIN],
        [NSNumber numberWithInteger:  NSIntegerMin]
    ];

    NSArray<NSNumber *> *maxValues = @[
        [NSNumber numberWithShort:    SHRT_MAX],
        [NSNumber numberWithInt:      INT_MAX],
        [NSNumber numberWithLong:     LONG_MAX],
        [NSNumber numberWithLongLong: LLONG_MAX],
        [NSNumber numberWithInteger:  NSIntegerMax]
    ];

    // Negative integers should always come back as int64_t per mbgl::Value definition.
    // We use the long long value because it can store the highest number on both 32-
    // and 64-bit and won't overflow.

    for (NSNumber *min in minValues)
    {
        predicate = [self equalityComparisonPredicateWithRightConstantValue:min];
        convertedValue = predicate.rightExpression.mgl_constantMBGLValue;
        XCTAssertTrue(convertedValue.is<int64_t>());
        XCTAssertEqual(convertedValue.get<int64_t>(), min.longLongValue);
    }

    // Positive integers should always come back as uint64_t per mbgl::Value definition.
    // We use the unsigned long long value because it can store the highest number on
    // both 32- and 64-bit and won't overflow.

    for (NSNumber *max in maxValues)
    {
        predicate = [self equalityComparisonPredicateWithRightConstantValue:max];
        convertedValue = predicate.rightExpression.mgl_constantMBGLValue;
        XCTAssertTrue(convertedValue.is<uint64_t>());
        XCTAssertEqual(convertedValue.get<uint64_t>(), max.unsignedLongLongValue);
    }

}

- (void)testExpressionPositiveAndZeroIntegers
{
    NSComparisonPredicate *predicate;
    mbgl::Value convertedValue;

    NSArray<NSNumber *> *minValues = @[
        [NSNumber numberWithUnsignedShort:    0],
        [NSNumber numberWithUnsignedInt:      0],
        [NSNumber numberWithUnsignedLong:     0],
        [NSNumber numberWithUnsignedLongLong: 0],
        [NSNumber numberWithUnsignedInteger:  0]
    ];

    NSArray<NSNumber *> *maxValues = @[
        [NSNumber numberWithUnsignedShort:    USHRT_MAX],
        [NSNumber numberWithUnsignedInt:      UINT_MAX],
        [NSNumber numberWithUnsignedLong:     ULONG_MAX],
        [NSNumber numberWithUnsignedLongLong: ULLONG_MAX],
        [NSNumber numberWithUnsignedInteger:  NSUIntegerMax]
    ];

    // Zero-value integers should always come back as uint64_t per mbgl::Value definition
    // (using the interpretation that zero is not negative). We use the unsigned long long
    // value just for parity with the positive integer test.

    for (NSNumber *min in minValues)
    {
        predicate = [self equalityComparisonPredicateWithRightConstantValue:min];
        convertedValue = predicate.rightExpression.mgl_constantMBGLValue;
        XCTAssertTrue(convertedValue.is<uint64_t>());
        XCTAssertEqual(convertedValue.get<uint64_t>(), min.unsignedLongLongValue);
    }

    // Positive integers should always come back as uint64_t per mbgl::Value definition.
    // We use the unsigned long long value because it can store the highest number on
    // both 32- and 64-bit and won't overflow.

    for (NSNumber *max in maxValues)
    {
        predicate = [self equalityComparisonPredicateWithRightConstantValue:max];
        convertedValue = predicate.rightExpression.mgl_constantMBGLValue;
        XCTAssertTrue(convertedValue.is<uint64_t>());
        XCTAssertEqual(convertedValue.get<uint64_t>(), max.unsignedLongLongValue);
    }
}

#pragma mark - Null Tests

- (void)testExpressionConversionNull
{
    NSComparisonPredicate *predicate = [self equalityComparisonPredicateWithRightConstantValue:[NSNull null]];
    mbgl::Value convertedValue = predicate.rightExpression.mgl_constantMBGLValue;
    XCTAssertTrue(convertedValue.is<mbgl::NullValue>());
}

@end
