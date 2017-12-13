#import "NSPredicate+MGLAdditions.h"

#import "MGLValueEvaluator.h"

class FilterEvaluator {
public:

    NSArray *getPredicates(std::vector<mbgl::style::Filter> filters) {
        NSMutableArray *predicates = [NSMutableArray arrayWithCapacity:filters.size()];
        for (auto filter : filters) {
            [predicates addObject:mbgl::style::Filter::visit(filter, FilterEvaluator())];
        }
        return predicates;
    }

    template <typename MBGLType>
    NSExpression *getValues(std::vector<MBGLType> values) {
        NSMutableArray *array = [NSMutableArray arrayWithCapacity:values.size()];
        for (auto value : values) {
            id constantValue = MBGLType::visit(value, ValueEvaluator());
            [array addObject:[NSExpression expressionForConstantValue:constantValue]];
        }
        return [NSExpression expressionForAggregate:array];
    }
    
    NSString *getFeatureTypeString(mbgl::FeatureType type) {
        switch (type) {
            case mbgl::FeatureType::Point:
                return @"Point";
                
            case mbgl::FeatureType::LineString:
                return @"LineString";
                
            case mbgl::FeatureType::Polygon:
                return @"Polygon";
                
            default:
                NSCAssert(NO, @"Unrecognized feature type %hhu", type);
                return nil;
        }
    }
    
    NSExpression *getFeatureTypeStrings(std::vector<mbgl::FeatureType> values) {
        NSMutableArray *array = [NSMutableArray arrayWithCapacity:values.size()];
        for (auto value : values) {
            id typeString = getFeatureTypeString(value);
            [array addObject:[NSExpression expressionForConstantValue:typeString]];
        }
        return [NSExpression expressionForAggregate:array];
    }

    NSPredicate *operator()(mbgl::style::NullFilter filter) {
        return nil;
    }

    NSPredicate *operator()(mbgl::style::EqualsFilter filter) {
        return [NSPredicate predicateWithFormat:@"%K == %@", @(filter.key.c_str()), mbgl::Value::visit(filter.value, ValueEvaluator())];
    }

    NSPredicate *operator()(mbgl::style::NotEqualsFilter filter) {
        return [NSPredicate predicateWithFormat:@"%K != %@", @(filter.key.c_str()), mbgl::Value::visit(filter.value, ValueEvaluator())];
    }

    NSPredicate *operator()(mbgl::style::GreaterThanFilter filter) {
        return [NSPredicate predicateWithFormat:@"%K > %@", @(filter.key.c_str()), mbgl::Value::visit(filter.value, ValueEvaluator())];
    }

    NSPredicate *operator()(mbgl::style::GreaterThanEqualsFilter filter) {
        return [NSPredicate predicateWithFormat:@"%K >= %@", @(filter.key.c_str()), mbgl::Value::visit(filter.value, ValueEvaluator())];
    }

    NSPredicate *operator()(mbgl::style::LessThanFilter filter) {
        return [NSPredicate predicateWithFormat:@"%K < %@", @(filter.key.c_str()), mbgl::Value::visit(filter.value, ValueEvaluator())];
    }

    NSPredicate *operator()(mbgl::style::LessThanEqualsFilter filter) {
        return [NSPredicate predicateWithFormat:@"%K <= %@", @(filter.key.c_str()), mbgl::Value::visit(filter.value, ValueEvaluator())];
    }

    NSPredicate *operator()(mbgl::style::InFilter filter) {
        return [NSPredicate predicateWithFormat:@"%K IN %@", @(filter.key.c_str()), getValues(filter.values)];
    }

    NSPredicate *operator()(mbgl::style::NotInFilter filter) {
        return [NSPredicate predicateWithFormat:@"NOT %K IN %@", @(filter.key.c_str()), getValues(filter.values)];
    }
    
    NSPredicate *operator()(mbgl::style::TypeEqualsFilter filter) {
        return [NSPredicate predicateWithFormat:@"%K == %@", @"$type", getFeatureTypeString(filter.value)];
    }
    
    NSPredicate *operator()(mbgl::style::TypeNotEqualsFilter filter) {
        return [NSPredicate predicateWithFormat:@"%K != %@", @"$type", getFeatureTypeString(filter.value)];
    }
    
    NSPredicate *operator()(mbgl::style::TypeInFilter filter) {
        return [NSPredicate predicateWithFormat:@"%K IN %@", @"$type", getFeatureTypeStrings(filter.values)];
    }
    
    NSPredicate *operator()(mbgl::style::TypeNotInFilter filter) {
        return [NSPredicate predicateWithFormat:@"NOT %K IN %@", @"$type", getFeatureTypeStrings(filter.values)];
    }
    
    NSPredicate *operator()(mbgl::style::IdentifierEqualsFilter filter) {
        return [NSPredicate predicateWithFormat:@"%K == %@", @"$id", mbgl::FeatureIdentifier::visit(filter.value, ValueEvaluator())];
    }
    
    NSPredicate *operator()(mbgl::style::IdentifierNotEqualsFilter filter) {
        return [NSPredicate predicateWithFormat:@"%K != %@", @"$id", mbgl::FeatureIdentifier::visit(filter.value, ValueEvaluator())];
    }
    
    NSPredicate *operator()(mbgl::style::IdentifierInFilter filter) {
        return [NSPredicate predicateWithFormat:@"%K IN %@", @"$id", getValues(filter.values)];
    }
    
    NSPredicate *operator()(mbgl::style::IdentifierNotInFilter filter) {
        return [NSPredicate predicateWithFormat:@"NOT %K IN %@", @"$id", getValues(filter.values)];
    }

    NSPredicate *operator()(mbgl::style::AnyFilter filter) {
        NSArray *subpredicates = getPredicates(filter.filters);
        if (subpredicates.count) {
            return [NSCompoundPredicate orPredicateWithSubpredicates:subpredicates];
        }
        return [NSPredicate predicateWithValue:NO];
    }

    NSPredicate *operator()(mbgl::style::AllFilter filter) {
        // Convert [all, [>=, key, lower], [<=, key, upper]] to key BETWEEN {lower, upper}
        if (filter.filters.size() == 2) {
            auto leftFilter = filter.filters[0];
            auto rightFilter = filter.filters[1];

            std::string lowerKey;
            std::string upperKey;
            mbgl::Value lowerBound;
            mbgl::Value upperBound;
            if (leftFilter.is<mbgl::style::GreaterThanEqualsFilter>()) {
                lowerKey = leftFilter.get<mbgl::style::GreaterThanEqualsFilter>().key;
                lowerBound = leftFilter.get<mbgl::style::GreaterThanEqualsFilter>().value;
            } else if (rightFilter.is<mbgl::style::GreaterThanEqualsFilter>()) {
                lowerKey = rightFilter.get<mbgl::style::GreaterThanEqualsFilter>().key;
                lowerBound = rightFilter.get<mbgl::style::GreaterThanEqualsFilter>().value;
            }

            if (leftFilter.is<mbgl::style::LessThanEqualsFilter>()) {
                upperKey = leftFilter.get<mbgl::style::LessThanEqualsFilter>().key;
                upperBound = leftFilter.get<mbgl::style::LessThanEqualsFilter>().value;
            } else if (rightFilter.is<mbgl::style::LessThanEqualsFilter>()) {
                upperKey = rightFilter.get<mbgl::style::LessThanEqualsFilter>().key;
                upperBound = rightFilter.get<mbgl::style::LessThanEqualsFilter>().value;
            }

            if (!lowerBound.is<mbgl::NullValue>() && !upperBound.is<mbgl::NullValue>()
                && lowerKey == upperKey) {
                return [NSPredicate predicateWithFormat:@"%K BETWEEN {%@, %@}",
                        @(lowerKey.c_str()),
                        mbgl::Value::visit(lowerBound, ValueEvaluator()),
                        mbgl::Value::visit(upperBound, ValueEvaluator())];
            }
        }

        NSArray *subpredicates = getPredicates(filter.filters);
        if (subpredicates.count) {
            return [NSCompoundPredicate andPredicateWithSubpredicates:subpredicates];
        }
        return [NSPredicate predicateWithValue:YES];
    }

    NSPredicate *operator()(mbgl::style::NoneFilter filter) {
        NSArray *subpredicates = getPredicates(filter.filters);
        if (subpredicates.count > 1) {
            NSCompoundPredicate *predicate = [NSCompoundPredicate orPredicateWithSubpredicates:subpredicates];
            return [NSCompoundPredicate notPredicateWithSubpredicate:predicate];
        } else if (subpredicates.count) {
            return [NSCompoundPredicate notPredicateWithSubpredicate:subpredicates.firstObject];
        } else {
            return [NSPredicate predicateWithValue:YES];
        }
    }

    NSPredicate *operator()(mbgl::style::HasFilter filter) {
        return [NSPredicate predicateWithFormat:@"%K != nil", @(filter.key.c_str())];
    }

    NSPredicate *operator()(mbgl::style::NotHasFilter filter) {
        return [NSPredicate predicateWithFormat:@"%K == nil", @(filter.key.c_str())];
    }
    
    NSPredicate *operator()(mbgl::style::HasIdentifierFilter filter) {
        return [NSPredicate predicateWithFormat:@"%K != nil", @"$id"];
    }
    
    NSPredicate *operator()(mbgl::style::NotHasIdentifierFilter filter) {
        return [NSPredicate predicateWithFormat:@"%K == nil", @"$id"];
    }
};

@implementation NSPredicate (MGLAdditions)

- (mbgl::style::Filter)mgl_filter
{
    if ([self isEqual:[NSPredicate predicateWithValue:YES]])
    {
        return mbgl::style::AllFilter();
    }

    if ([self isEqual:[NSPredicate predicateWithValue:NO]])
    {
        return mbgl::style::AnyFilter();
    }

    if ([self.predicateFormat hasPrefix:@"BLOCKPREDICATE("])
    {
        [NSException raise:NSInvalidArgumentException
                    format:@"Block-based predicates are not supported."];
    }

    [NSException raise:NSInvalidArgumentException
                format:@"Unrecognized predicate type."];
    return {};
}

+ (instancetype)mgl_predicateWithFilter:(mbgl::style::Filter)filter
{
    FilterEvaluator evaluator;
    return mbgl::style::Filter::visit(filter, evaluator);
}

@end

@implementation NSPredicate (MGLExpressionAdditions)

NSArray *MGLSubpredicatesWithJSONObjects(NSArray *objects) {
    NSMutableArray *subpredicates = [NSMutableArray arrayWithCapacity:objects.count];
    for (id object in objects) {
        NSPredicate *predicate = [NSPredicate mgl_predicateWithJSONObject:object];
        [subpredicates addObject:predicate];
    }
    return subpredicates;
}

+ (instancetype)mgl_predicateWithJSONObject:(id)object {
    if ([object isEqual:@YES]) {
        return [NSPredicate predicateWithValue:YES];
    }
    if ([object isEqual:@NO]) {
        return [NSPredicate predicateWithValue:NO];
    }
    
    NSAssert([object isKindOfClass:[NSArray class]], @"Condition for case expression should be an expression.");
    NSArray *objects = (NSArray *)object;
    NSString *op = objects.firstObject;
    
    if ([op isEqualToString:@"=="]) {
        NSArray *subexpressions = MGLSubexpressionsWithJSONObjects([objects subarrayWithRange:NSMakeRange(1, objects.count - 1)]);
        return [NSPredicate predicateWithFormat:@"%@ == %@" argumentArray:subexpressions];
    }
    if ([op isEqualToString:@"!="]) {
        NSArray *subexpressions = MGLSubexpressionsWithJSONObjects([objects subarrayWithRange:NSMakeRange(1, objects.count - 1)]);
        return [NSPredicate predicateWithFormat:@"%@ != %@" argumentArray:subexpressions];
    }
    if ([op isEqualToString:@"<"]) {
        NSArray *subexpressions = MGLSubexpressionsWithJSONObjects([objects subarrayWithRange:NSMakeRange(1, objects.count - 1)]);
        return [NSPredicate predicateWithFormat:@"%@ < %@" argumentArray:subexpressions];
    }
    if ([op isEqualToString:@"<="]) {
        NSArray *subexpressions = MGLSubexpressionsWithJSONObjects([objects subarrayWithRange:NSMakeRange(1, objects.count - 1)]);
        return [NSPredicate predicateWithFormat:@"%@ <= %@" argumentArray:subexpressions];
    }
    if ([op isEqualToString:@">"]) {
        NSArray *subexpressions = MGLSubexpressionsWithJSONObjects([objects subarrayWithRange:NSMakeRange(1, objects.count - 1)]);
        return [NSPredicate predicateWithFormat:@"%@ > %@" argumentArray:subexpressions];
    }
    if ([op isEqualToString:@">="]) {
        NSArray *subexpressions = MGLSubexpressionsWithJSONObjects([objects subarrayWithRange:NSMakeRange(1, objects.count - 1)]);
        return [NSPredicate predicateWithFormat:@"%K >= %@" argumentArray:subexpressions];
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
        NSArray *subpredicates = MGLSubpredicatesWithJSONObjects([objects subarrayWithRange:NSMakeRange(1, objects.count - 1)]);
        return [NSCompoundPredicate andPredicateWithSubpredicates:subpredicates];
    }
    if ([op isEqualToString:@"any"]) {
        NSArray *subpredicates = MGLSubpredicatesWithJSONObjects([objects subarrayWithRange:NSMakeRange(1, objects.count - 1)]);
        return [NSCompoundPredicate orPredicateWithSubpredicates:subpredicates];
    }
    
    NSAssert(NO, @"Unrecognized expression conditional operator %@.", op);
    return nil;
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
