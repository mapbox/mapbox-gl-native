#import "NSPredicate+MGLPrivateAdditions.h"

#import "MGLValueEvaluator.h"
#import "MGLStyleValue_Private.h"
#import "MGLLoggingConfiguration_Private.h"

#include <mbgl/style/conversion/filter.hpp>

@implementation NSPredicate (MGLPrivateAdditions)

- (mbgl::style::Filter)mgl_filter
{
    mbgl::style::conversion::Error valueError;
    NSArray *jsonObject = self.mgl_jsonExpressionObject;
    auto value = mbgl::style::conversion::convert<mbgl::style::Filter>(mbgl::style::conversion::makeConvertible(jsonObject), valueError);
    
    if (!value) {
        [NSException raise:NSInvalidArgumentException
                    format:@"Invalid filter value: %@", @(valueError.message.c_str())];
        return {};
    }
    mbgl::style::Filter filter = std::move(*value);
    
    return filter;
}

+ (instancetype)mgl_predicateWithFilter:(mbgl::style::Filter)filter
{
    if (filter.expression) {
        id jsonObject = MGLJSONObjectFromMBGLExpression(**filter.expression);
        return [NSPredicate predicateWithMGLJSONObject:jsonObject];
    } else {
        return nil;
    }
}

@end

@implementation NSPredicate (MGLAdditions)

NSArray *MGLSubpredicatesWithJSONObjects(NSArray *objects) {
    NSMutableArray *subpredicates = [NSMutableArray arrayWithCapacity:objects.count];
    for (id object in objects) {
        NSPredicate *predicate = [NSPredicate predicateWithMGLJSONObject:object];
        [subpredicates addObject:predicate];
    }
    return subpredicates;
}

static NSDictionary * const MGLPredicateOperatorTypesByJSONOperator = @{
    @"==": @(NSEqualToPredicateOperatorType),
    @"!=": @(NSNotEqualToPredicateOperatorType),
    @"<": @(NSLessThanPredicateOperatorType),
    @"<=": @(NSLessThanOrEqualToPredicateOperatorType),
    @">": @(NSGreaterThanPredicateOperatorType),
    @">=": @(NSGreaterThanOrEqualToPredicateOperatorType),
};

+ (instancetype)predicateWithMGLJSONObject:(id)object {
    if ([object isEqual:@YES]) {
        return [NSPredicate predicateWithValue:YES];
    }
    if ([object isEqual:@NO]) {
        return [NSPredicate predicateWithValue:NO];
    }
    
    MGLAssert([object isKindOfClass:[NSArray class]], @"Condition for case expression should be an expression.");
    NSArray *objects = (NSArray *)object;
    NSString *op = objects.firstObject;
    
    NSNumber *operatorTypeNumber = MGLPredicateOperatorTypesByJSONOperator[op];
    if (operatorTypeNumber) {
        NSPredicateOperatorType operatorType = (NSPredicateOperatorType)[operatorTypeNumber unsignedIntegerValue];
        
        NSComparisonPredicateOptions options = 0;
        if (objects.count > 3) {
            NSArray *collatorExpression = objects[3];
            MGLCAssert([collatorExpression isKindOfClass:[NSArray class]], @"Collators must be dictionaries.");
            MGLCAssert(collatorExpression.count == 2, @"Malformed collator expression");
            NSDictionary *collator = collatorExpression[1];
            MGLCAssert([collator isKindOfClass:[NSDictionary class]], @"Malformed collator in collator expression");
            
            // Predicate options can’t express specific locales as collators can.
            if (!collator[@"locale"]) {
                if ([(collator[@"case-sensitive"] ?: @YES) isEqual:@NO]) {
                    options |= NSCaseInsensitivePredicateOption;
                }
                if ([(collator[@"diacritic-sensitive"] ?: @YES) isEqual:@NO]) {
                    options |= NSDiacriticInsensitivePredicateOption;
                }
            }
        }
        
        NSArray *subexpressions = MGLSubexpressionsWithJSONObjects([objects subarrayWithRange:NSMakeRange(1, objects.count - 1)]);
        return [NSComparisonPredicate predicateWithLeftExpression:subexpressions[0]
                                                  rightExpression:subexpressions[1]
                                                         modifier:NSDirectPredicateModifier
                                                             type:operatorType
                                                          options:options];
    }
    
    if ([op isEqualToString:@"!"]) {
        NSArray *subpredicates = MGLSubpredicatesWithJSONObjects([objects subarrayWithRange:NSMakeRange(1, objects.count - 1)]);
        if (subpredicates.count > 1) {
            NSCompoundPredicate *predicate = [NSCompoundPredicate orPredicateWithSubpredicates:subpredicates];
            return [NSCompoundPredicate notPredicateWithSubpredicate:predicate];
        }
        if (subpredicates.count) {
            return [NSCompoundPredicate notPredicateWithSubpredicate:subpredicates.firstObject];
        }
        return [NSPredicate predicateWithValue:YES];
    }
    if ([op isEqualToString:@"all"]) {
        NSArray<NSPredicate *> *subpredicates = MGLSubpredicatesWithJSONObjects([objects subarrayWithRange:NSMakeRange(1, objects.count - 1)]);
        if (subpredicates.count == 2) {
            // Determine if the expression is of BETWEEN type
            if ([subpredicates[0] isKindOfClass:[NSComparisonPredicate class]] &&
                [subpredicates[1] isKindOfClass:[NSComparisonPredicate class]]) {
                NSComparisonPredicate *leftCondition = (NSComparisonPredicate *)subpredicates[0];
                NSComparisonPredicate *rightCondition = (NSComparisonPredicate *)subpredicates[1];
                
                NSArray *limits;
                NSExpression *leftConditionExpression;
                
                if(leftCondition.predicateOperatorType == NSGreaterThanOrEqualToPredicateOperatorType &&
                   rightCondition.predicateOperatorType == NSLessThanOrEqualToPredicateOperatorType) {
                    limits = @[leftCondition.rightExpression, rightCondition.rightExpression];
                    leftConditionExpression = leftCondition.leftExpression;
                    
                } else if (leftCondition.predicateOperatorType == NSLessThanOrEqualToPredicateOperatorType &&
                           rightCondition.predicateOperatorType == NSLessThanOrEqualToPredicateOperatorType) {
                    limits = @[leftCondition.leftExpression, rightCondition.rightExpression];
                    leftConditionExpression = leftCondition.rightExpression;
                
                } else if(leftCondition.predicateOperatorType == NSLessThanOrEqualToPredicateOperatorType &&
                          rightCondition.predicateOperatorType == NSGreaterThanOrEqualToPredicateOperatorType) {
                    limits = @[leftCondition.leftExpression, rightCondition.leftExpression];
                    leftConditionExpression = leftCondition.rightExpression;
                
                } else if(leftCondition.predicateOperatorType == NSGreaterThanOrEqualToPredicateOperatorType &&
                          rightCondition.predicateOperatorType == NSGreaterThanOrEqualToPredicateOperatorType) {
                    limits = @[leftCondition.rightExpression, rightCondition.leftExpression];
                    leftConditionExpression = leftCondition.leftExpression;
                }
                
                if (limits && leftConditionExpression) {
                     return [NSPredicate predicateWithFormat:@"%@ BETWEEN %@", leftConditionExpression, [NSExpression expressionForAggregate:limits]];
                }
            }
        }
        return [NSCompoundPredicate andPredicateWithSubpredicates:subpredicates];
    }
    if ([op isEqualToString:@"any"]) {
        NSArray *subpredicates = MGLSubpredicatesWithJSONObjects([objects subarrayWithRange:NSMakeRange(1, objects.count - 1)]);
        return [NSCompoundPredicate orPredicateWithSubpredicates:subpredicates];
    }
    
    NSExpression *expression = [NSExpression expressionWithMGLJSONObject:object];
    return [NSComparisonPredicate predicateWithLeftExpression:expression
                                              rightExpression:[NSExpression expressionForConstantValue:@YES]
                                                     modifier:NSDirectPredicateModifier
                                                         type:NSEqualToPredicateOperatorType
                                                      options:0];

}

- (id)mgl_jsonExpressionObject {
    if ([self isEqual:[NSPredicate predicateWithValue:YES]]) {
        return @YES;
    }
    if ([self isEqual:[NSPredicate predicateWithValue:NO]]) {
        return @NO;
    }
    
    if ([self.predicateFormat hasPrefix:@"BLOCKPREDICATE("]) {
        [NSException raise:NSInvalidArgumentException
                    format:@"Block-based predicates are not supported."];
    }
    
    [NSException raise:NSInvalidArgumentException
                format:@"Unrecognized predicate type."];
    return nil;
}

@end

@implementation NSPredicate (MGLExpressionAdditions)

- (id)mgl_if:(id)firstValue, ... {

    if ([self evaluateWithObject:nil]) {
            return firstValue;
        }

    id eachExpression;
    va_list argumentList;
    va_start(argumentList, firstValue);

    while ((eachExpression = va_arg(argumentList, id))) {
            if ([eachExpression isKindOfClass:[NSComparisonPredicate class]]) {
                    id valueExpression = va_arg(argumentList, id);
                    if ([eachExpression evaluateWithObject:nil]) {
                            return valueExpression;
                        }
                } else {
                        return eachExpression;
                    }
        }
    va_end(argumentList);

    return nil;
}

- (id)mgl_match:(NSExpression *)firstCase, ... {
    [NSException raise:NSInvalidArgumentException
                      format:@"Match expressions lack underlying Objective-C implementations."];
    return nil;
}

@end
