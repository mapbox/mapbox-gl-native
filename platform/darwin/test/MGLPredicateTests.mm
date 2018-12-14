#import <XCTest/XCTest.h>
#import <Mapbox/Mapbox.h>

#import "NSPredicate+MGLPrivateAdditions.h"
#import "MGLValueEvaluator.h"

@interface MGLPredicateTests : XCTestCase
@end

@implementation MGLPredicateTests

- (void)testUnsupportedFilterPredicates {
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
        XCTAssertEqualObjects([NSPredicate predicateWithMGLJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate predicateWithMGLJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"CAST(x, 'NSNumber') < 5"];
        NSArray *jsonExpression = @[@"<", @[@"to-number", @[@"get", @"x"]], @5];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate predicateWithMGLJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate predicateWithMGLJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"CAST(x, 'NSNumber') > 5"];
        NSArray *jsonExpression = @[@">", @[@"to-number", @[@"get", @"x"]], @5];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate predicateWithMGLJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate predicateWithMGLJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"CAST(x, 'NSNumber') <= 5"];
        NSArray *jsonExpression = @[@"<=", @[@"to-number", @[@"get", @"x"]], @5];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate predicateWithMGLJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate predicateWithMGLJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"CAST(x, 'NSNumber') >= 5"];
        NSArray *jsonExpression = @[@">=", @[@"to-number", @[@"get", @"x"]], @5];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate predicateWithMGLJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate predicateWithMGLJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"CAST(x, 'NSString') > 'value'"];
        NSArray *jsonExpression = @[@">",  @[@"to-string", @[@"get", @"x"]], @"value"];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate predicateWithMGLJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate predicateWithMGLJSONObject:jsonExpression]
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
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"$geometryType = 'Point'"];
        NSArray *jsonExpression = @[@"==", @[@"geometry-type"], @"Point"];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        [self testSymmetryWithPredicate:predicate
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"$featureIdentifier = 67086180"];
        NSArray *jsonExpression = @[@"==", @[@"id"], @67086180];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        [self testSymmetryWithPredicate:predicate
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"$featureIdentifier = nil"];
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
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"$geometryType != 'Point'"];
        NSArray *jsonExpression = @[@"!=", @[@"geometry-type"], @"Point"];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        [self testSymmetryWithPredicate:predicate
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"$featureIdentifier != 67086180"];
        NSArray *jsonExpression = @[@"!=", @[@"id"], @67086180];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        [self testSymmetryWithPredicate:predicate
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"$featureIdentifier != nil"];
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
        XCTAssertEqualObjects([NSPredicate predicateWithMGLJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate predicateWithMGLJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"CAST(a, 'NSString') <= 'b'"];
        NSArray *jsonExpression = @[@"<=", @[@"to-string", @[@"get", @"a"]], @"b"];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate predicateWithMGLJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate predicateWithMGLJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"CAST(a, 'NSString') > 'b'"];
        NSArray *jsonExpression = @[@">", @[@"to-string", @[@"get", @"a"]], @"b"];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate predicateWithMGLJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate predicateWithMGLJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"CAST(a, 'NSString') >= 'b'"];
        NSArray *jsonExpression = @[@">=", @[@"to-string", @[@"get", @"a"]], @"b"];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate predicateWithMGLJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate predicateWithMGLJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"CAST(a, 'NSString') BETWEEN {'b', 'z'}"];
        NSArray *jsonExpression =@[@"all", @[@"<=", @"b", @[@"to-string", @[@"get", @"a"]]], @[@"<=", @[@"to-string", @[@"get", @"a"]], @"z"]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate predicateWithMGLJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate predicateWithMGLJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
    {
        NSExpression *limits = [NSExpression expressionForAggregate:@[[NSExpression expressionForConstantValue:@10], [NSExpression expressionForConstantValue:@100]]];
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"CAST(x, 'NSNumber') BETWEEN %@", limits];
        NSArray *jsonExpression = @[@"all", @[@">=", @[@"to-number", @[@"get", @"x"]], @10], @[@"<=", @[@"to-number", @[@"get", @"x"]], @100]];
        XCTAssertEqualObjects([NSPredicate predicateWithMGLJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate predicateWithMGLJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
    {
        NSArray *expected = @[@"all", @[@"<=", @10, @[@"to-number", @[@"get", @"x"]]], @[@"<=", @[@"to-number", @[@"get", @"x"]], @100]];
        NSExpression *limits = [NSExpression expressionForAggregate:@[[NSExpression expressionForConstantValue:@10], [NSExpression expressionForConstantValue:@100]]];
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"CAST(x, 'NSNumber') BETWEEN %@", limits];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, expected);
        XCTAssertEqualObjects([NSPredicate predicateWithMGLJSONObject:expected], predicate);
        [self testSymmetryWithPredicate:[NSPredicate predicateWithMGLJSONObject:expected]
                          mustRoundTrip:NO];
    }
    {
        NSArray *expected = @[@"all", @[@"<=", @10, @[@"to-number", @[@"get", @"x"]]], @[@">=", @100, @[@"to-number", @[@"get", @"x"]]]];
        NSExpression *limits = [NSExpression expressionForAggregate:@[[NSExpression expressionForConstantValue:@10], [NSExpression expressionForConstantValue:@100]]];
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"CAST(x, 'NSNumber') BETWEEN %@", limits];
        XCTAssertEqualObjects([NSPredicate predicateWithMGLJSONObject:expected], predicate);
        [self testSymmetryWithPredicate:[NSPredicate predicateWithMGLJSONObject:expected]
                          mustRoundTrip:NO];
    }
    {
        NSArray *expected = @[@"all", @[@">=", @[@"to-number", @[@"get", @"x"]], @10], @[@">=", @100, @[@"to-number", @[@"get", @"x"]]]];
        NSExpression *limits = [NSExpression expressionForAggregate:@[[NSExpression expressionForConstantValue:@10], [NSExpression expressionForConstantValue:@100]]];
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"CAST(x, 'NSNumber') BETWEEN %@", limits];
        XCTAssertEqualObjects([NSPredicate predicateWithMGLJSONObject:expected], predicate);
        [self testSymmetryWithPredicate:[NSPredicate predicateWithMGLJSONObject:expected]
                          mustRoundTrip:NO];
    }
    {
        NSArray *expected = @[@"match", @[@"id"], @[@6, @5, @4, @3], @YES, @NO];
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"$featureIdentifier IN { 6, 5, 4, 3}"];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, expected);
        NSPredicate *predicateAfter = [NSPredicate predicateWithFormat:@"MGL_MATCH($featureIdentifier, { 3, 4, 5, 6 }, YES, NO) == YES"];
        auto forwardFilter = [NSPredicate predicateWithMGLJSONObject:expected].mgl_filter;
        NSPredicate *forwardPredicateAfter = [NSPredicate mgl_predicateWithFilter:forwardFilter];
        XCTAssertEqualObjects(predicateAfter, forwardPredicateAfter);
    }
    {
        NSArray *expected = @[@"match", @[@"to-number", @[@"id"]], @[@3002970001, @3004140052, @3002950027, @3002970033], @YES, @NO];
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"CAST($featureIdentifier, 'NSNumber') IN { 3002970001, 3004140052, 3002950027, 3002970033 }"];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, expected);
        NSPredicate *predicateAfter = [NSPredicate predicateWithFormat:@"MGL_MATCH(CAST($featureIdentifier, 'NSNumber'), { 3002950027, 3002970001, 3002970033, 3004140052 }, YES, NO) == YES"];
        auto forwardFilter = [NSPredicate predicateWithMGLJSONObject:expected].mgl_filter;
        NSPredicate *forwardPredicateAfter = [NSPredicate mgl_predicateWithFilter:forwardFilter];
        XCTAssertEqualObjects(predicateAfter, forwardPredicateAfter);
    }
    {
        NSArray *expected = @[@"!", @[@"match", @[@"get", @"x"], @[@6, @5, @4, @3], @YES, @NO]];
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"NOT x IN { 6, 5, 4, 3}"];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, expected);
        NSPredicate *predicateAfter = [NSPredicate predicateWithFormat:@"NOT MGL_MATCH(x, { 3, 4, 5, 6 }, YES, NO) == YES"];
        auto forwardFilter = [NSPredicate predicateWithMGLJSONObject:expected].mgl_filter;
        NSPredicate *forwardPredicateAfter = [NSPredicate mgl_predicateWithFilter:forwardFilter];
        XCTAssertEqualObjects(predicateAfter, forwardPredicateAfter);
    }
    {
        NSArray *expected = @[@"match", @[@"get", @"a"], @[@"b", @"c"], @YES, @NO];
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"a IN { 'b', 'c' }"];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, expected);
        NSPredicate *predicateAfter = [NSPredicate predicateWithFormat:@"MGL_MATCH(a, { 'b', 'c' }, YES, NO) == YES"];
        auto forwardFilter = [NSPredicate predicateWithMGLJSONObject:expected].mgl_filter;
        NSPredicate *forwardPredicateAfter = [NSPredicate mgl_predicateWithFilter:forwardFilter];
        XCTAssertEqualObjects(predicateAfter, forwardPredicateAfter);
    }
    {
        NSArray *expected = @[@"match", @[@"geometry-type"], @[@"LineString", @"Polygon"], @YES, @NO];
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"%@ IN %@", [NSExpression expressionForVariable:@"geometryType"], @[@"LineString", @"Polygon"]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, expected);
        NSPredicate *predicateAfter = [NSPredicate predicateWithFormat:@"MGL_MATCH($geometryType, { 'LineString', 'Polygon' }, YES, NO) == YES"];
        auto forwardFilter = [NSPredicate predicateWithMGLJSONObject:expected].mgl_filter;
        NSPredicate *forwardPredicateAfter = [NSPredicate mgl_predicateWithFilter:forwardFilter];
        XCTAssertEqualObjects(predicateAfter, forwardPredicateAfter);
    }
    {
        NSArray *expected = @[@"match", @[@"get", @"x"], @[@6, @5, @4, @3], @YES, @NO];
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"{ 6, 5, 4, 3 } CONTAINS x"];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, expected);
        NSPredicate *predicateAfter = [NSPredicate predicateWithFormat:@"MGL_MATCH(x, { 3, 4, 5, 6 }, YES, NO) == YES"];
        auto forwardFilter = [NSPredicate predicateWithMGLJSONObject:expected].mgl_filter;
        NSPredicate *forwardPredicateAfter = [NSPredicate mgl_predicateWithFilter:forwardFilter];
        XCTAssertEqualObjects(predicateAfter, forwardPredicateAfter);
    }
    {
        NSArray *expected = @[@"match", @[@"geometry-type"], @[@"LineString", @"Polygon"], @YES, @NO];
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"%@ CONTAINS %@", @[@"LineString", @"Polygon"], [NSExpression expressionForVariable:@"geometryType"]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, expected);
        NSPredicate *predicateAfter = [NSPredicate predicateWithFormat:@"MGL_MATCH($geometryType, { 'LineString', 'Polygon' }, YES, NO) == YES"];
        auto forwardFilter = [NSPredicate predicateWithMGLJSONObject:expected].mgl_filter;
        NSPredicate *forwardPredicateAfter = [NSPredicate mgl_predicateWithFilter:forwardFilter];
        XCTAssertEqualObjects(predicateAfter, forwardPredicateAfter);
    }
    {
        NSArray *expected = @[@"match", @[@"id"], @[@6, @5, @4, @3], @YES, @NO];
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"{ 6, 5, 4, 3} CONTAINS $featureIdentifier"];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, expected);
        NSPredicate *predicateAfter = [NSPredicate predicateWithFormat:@"MGL_MATCH($featureIdentifier, { 3, 4, 5, 6 }, YES, NO) == YES"];
        auto forwardFilter = [NSPredicate predicateWithMGLJSONObject:expected].mgl_filter;
        NSPredicate *forwardPredicateAfter = [NSPredicate mgl_predicateWithFilter:forwardFilter];
        XCTAssertEqualObjects(predicateAfter, forwardPredicateAfter);
    }
}

- (void)testComparisonPredicatesWithOptions {
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"a =[c] 'b'"];
        NSArray *jsonExpression = @[@"==", @[@"get", @"a"], @"b", @[@"collator", @{@"case-sensitive": @NO, @"diacritic-sensitive": @YES}]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        [self testSymmetryWithPredicate:predicate
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"a =[d] 'b'"];
        NSArray *jsonExpression = @[@"==", @[@"get", @"a"], @"b", @[@"collator", @{@"case-sensitive": @YES, @"diacritic-sensitive": @NO}]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        [self testSymmetryWithPredicate:predicate
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"a =[cd] 'b'"];
        NSArray *jsonExpression = @[@"==", @[@"get", @"a"], @"b", @[@"collator", @{@"case-sensitive": @NO, @"diacritic-sensitive": @NO}]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        [self testSymmetryWithPredicate:predicate
                          mustRoundTrip:NO];
    }
    
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"a !=[cd] 'b'"];
        NSArray *jsonExpression = @[@"!=", @[@"get", @"a"], @"b", @[@"collator", @{@"case-sensitive": @NO, @"diacritic-sensitive": @NO}]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        [self testSymmetryWithPredicate:predicate
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"CAST(a, 'NSString') <[cd] 'b'"];
        NSArray *jsonExpression = @[@"<", @[@"to-string", @[@"get", @"a"]], @"b", @[@"collator", @{@"case-sensitive": @NO, @"diacritic-sensitive": @NO}]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        [self testSymmetryWithPredicate:predicate
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"CAST(a, 'NSString') <=[cd] 'b'"];
        NSArray *jsonExpression = @[@"<=", @[@"to-string", @[@"get", @"a"]], @"b", @[@"collator", @{@"case-sensitive": @NO, @"diacritic-sensitive": @NO}]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        [self testSymmetryWithPredicate:predicate
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"CAST(a, 'NSString') >[cd] 'b'"];
        NSArray *jsonExpression = @[@">", @[@"to-string", @[@"get", @"a"]], @"b", @[@"collator", @{@"case-sensitive": @NO, @"diacritic-sensitive": @NO}]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        [self testSymmetryWithPredicate:predicate
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"CAST(a, 'NSString') >=[cd] 'b'"];
        NSArray *jsonExpression = @[@">=", @[@"to-string", @[@"get", @"a"]], @"b", @[@"collator", @{@"case-sensitive": @NO, @"diacritic-sensitive": @NO}]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        [self testSymmetryWithPredicate:predicate
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"TRUE = MGL_FUNCTION('==', a, 'b', MGL_FUNCTION('collator', %@))", @{
            @"case-sensitive": @NO,
            @"diacritic-sensitive": @NO,
            @"locale": @"tlh",
        }];
        NSArray *jsonExpression = @[@"==", @[@"get", @"a"], @"b",
                                    @[@"collator",
                                      @{@"case-sensitive": @NO,
                                        @"diacritic-sensitive": @NO,
                                        @"locale": @"tlh"}]];
        XCTAssertEqualObjects([predicate.mgl_jsonExpressionObject lastObject], jsonExpression);
    }
}

- (void)testCompoundPredicates {
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"a == 'b' AND c == 'd'"];
        NSArray *jsonExpression = @[@"all", @[@"==", @[@"get", @"a"], @"b"], @[@"==",  @[@"get", @"c"], @"d"]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate predicateWithMGLJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate predicateWithMGLJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"a == 'b' OR c == 'd'"];
        NSArray *jsonExpression = @[@"any", @[@"==", @[@"get", @"a"], @"b"], @[@"==",  @[@"get", @"c"], @"d"]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate predicateWithMGLJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate predicateWithMGLJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"NOT(a == 'b' AND c == 'd')"];
        NSArray *jsonExpression = @[@"!", @[@"all", @[@"==", @[@"get", @"a"], @"b"], @[@"==",  @[@"get", @"c"], @"d"]]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate predicateWithMGLJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate predicateWithMGLJSONObject:jsonExpression]
                          mustRoundTrip:NO];
    }
    {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"NOT(a == 'b' OR c == 'd')"];
        NSArray *jsonExpression = @[@"!", @[@"any", @[@"==", @[@"get", @"a"], @"b"], @[@"==",  @[@"get", @"c"], @"d"]]];
        XCTAssertEqualObjects(predicate.mgl_jsonExpressionObject, jsonExpression);
        XCTAssertEqualObjects([NSPredicate predicateWithMGLJSONObject:jsonExpression], predicate);
        [self testSymmetryWithPredicate:[NSPredicate predicateWithMGLJSONObject:jsonExpression]
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
