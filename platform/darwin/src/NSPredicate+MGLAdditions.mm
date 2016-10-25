#import "NSPredicate+MGLAdditions.h"

#import "MGLValueEvaluator.h"

class FilterEvaluator {
public:
    
    NSArray* getPredicates(std::vector<mbgl::style::Filter> filters) {
        NSMutableArray *predicates = [NSMutableArray arrayWithCapacity:filters.size()];
        for (auto filter : filters) {
            [predicates addObject:mbgl::style::Filter::visit(filter, FilterEvaluator())];
        }
        return predicates;
    }
    
    NSArray* getValues(std::vector<mbgl::Value> values) {
        NSMutableArray *array = [NSMutableArray arrayWithCapacity:values.size()];
        for (auto value : values) {
            [array addObject:mbgl::Value::visit(value, ValueEvaluator())];
        }
        return array;
    }
    
    NSPredicate* operator()(mbgl::style::NullFilter filter) {
        return nil;
    }
    
    NSPredicate* operator()(mbgl::style::EqualsFilter filter) {
        return [NSPredicate predicateWithFormat:@"%K == %@", @(filter.key.c_str()), mbgl::Value::visit(filter.value, ValueEvaluator())];
    }
    
    NSPredicate* operator()(mbgl::style::NotEqualsFilter filter) {
        return [NSPredicate predicateWithFormat:@"%K != %@", @(filter.key.c_str()), mbgl::Value::visit(filter.value, ValueEvaluator())];
    }
    
    NSPredicate* operator()(mbgl::style::GreaterThanFilter filter) {
        return [NSPredicate predicateWithFormat:@"%K > %@", @(filter.key.c_str()), mbgl::Value::visit(filter.value, ValueEvaluator())];
    }
    
    NSPredicate* operator()(mbgl::style::GreaterThanEqualsFilter filter) {
        return [NSPredicate predicateWithFormat:@"%K >= %@", @(filter.key.c_str()), mbgl::Value::visit(filter.value, ValueEvaluator())];
    }
    
    NSPredicate* operator()(mbgl::style::LessThanFilter filter) {
        return [NSPredicate predicateWithFormat:@"%K < %@", @(filter.key.c_str()), mbgl::Value::visit(filter.value, ValueEvaluator())];
    }
    
    NSPredicate* operator()(mbgl::style::LessThanEqualsFilter filter) {
        return [NSPredicate predicateWithFormat:@"%K <= %@", @(filter.key.c_str()), mbgl::Value::visit(filter.value, ValueEvaluator())];
    }
    
    NSPredicate* operator()(mbgl::style::InFilter filter) {
        return [NSPredicate predicateWithFormat:@"%K IN %@", @(filter.key.c_str()), getValues(filter.values)];
    }
    
    NSPredicate* operator()(mbgl::style::NotInFilter filter) {
        return [NSPredicate predicateWithFormat:@"NOT %K IN %@", @(filter.key.c_str()), getValues(filter.values)];
    }
    
    NSPredicate* operator()(mbgl::style::AnyFilter filter) {
        return [NSCompoundPredicate orPredicateWithSubpredicates:getPredicates(filter.filters)];
    }
    
    NSPredicate* operator()(mbgl::style::AllFilter filter) {
        return [NSCompoundPredicate andPredicateWithSubpredicates:getPredicates(filter.filters)];
    }
    
    NSPredicate* operator()(mbgl::style::NoneFilter filter) {
        NSArray *predicates = getPredicates(filter.filters);
        if (predicates.count > 1) {
            NSCompoundPredicate *predicate = [NSCompoundPredicate andPredicateWithSubpredicates:predicates];
            return [NSCompoundPredicate notPredicateWithSubpredicate:predicate];
        } else {
            return [NSCompoundPredicate notPredicateWithSubpredicate:predicates.firstObject];
        }
    }
    
    NSPredicate* operator()(mbgl::style::HasFilter filter) {
        return [NSPredicate predicateWithFormat:@"%K != nil", @(filter.key.c_str())];
    }
    
    NSPredicate* operator()(mbgl::style::NotHasFilter filter) {
        return [NSPredicate predicateWithFormat:@"%K == nil", @(filter.key.c_str())];
    }
    
};

@implementation NSPredicate (MGLAdditions)

- (mbgl::style::Filter)mgl_filter
{
    if ([self isEqual:[NSPredicate predicateWithValue:YES]])
    {
        auto filter = mbgl::style::AllFilter();
        return filter;
    }
    
    if ([self isEqual:[NSPredicate predicateWithValue:NO]])
    {
        auto filter = mbgl::style::AnyFilter();
        return filter;
    }
    
    [NSException raise:@"Not supported"
                format:@"Try with NSComparisonPredicate or NSCompoundPredicate instead."];
    return {};
}

+ (instancetype)mgl_predicateWithFilter:(mbgl::style::Filter)filter
{
    FilterEvaluator evaluator;
    return mbgl::style::Filter::visit(filter, evaluator);
}

@end
