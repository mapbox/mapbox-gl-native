#import "NSCompoundPredicate+MGLAdditions.h"

#import "NSPredicate+MGLAdditions.h"
#import "NSExpression+MGLAdditions.h"

@implementation NSCompoundPredicate (MGLAdditions)

- (std::vector<mbgl::style::Filter>)mgl_subfilters
{
    std::vector<mbgl::style::Filter>filters;
    for (NSPredicate *predicate in self.subpredicates) {
        filters.push_back(predicate.mgl_filter);
    }
    return filters;
}

- (mbgl::style::Filter)mgl_filter
{
    switch (self.compoundPredicateType) {
        case NSNotPredicateType: {
            
            // Translate a nested NSComparisonPredicate with operator type NSInPredicateOperatorType into a flat mbgl::NotIn filter.
            NSArray<NSComparisonPredicate *> *comparisonPredicates = [self.subpredicates filteredArrayUsingPredicate:[NSPredicate predicateWithFormat:@"class == %@", [NSComparisonPredicate class]]];
            NSArray<NSComparisonPredicate *> *notInPredicates = [comparisonPredicates filteredArrayUsingPredicate:[NSPredicate predicateWithBlock:^BOOL(NSComparisonPredicate *_Nonnull predicate, NSDictionary<NSString *,id> * _Nullable bindings) {
                return predicate.predicateOperatorType == NSInPredicateOperatorType;
            }]];
            
            if (notInPredicates.count) {
                auto filter = mbgl::style::NotInFilter();
                filter.key = notInPredicates.firstObject.leftExpression.keyPath.UTF8String;
                filter.values = notInPredicates.firstObject.rightExpression.mgl_filterValues;
                return filter;
            } else {
                auto filter = mbgl::style::NoneFilter();
                filter.filters = [self mgl_subfilters];
                return filter;
            }
        }
        case NSAndPredicateType: {
            auto filter = mbgl::style::AllFilter();
            filter.filters = [self mgl_subfilters];
            return filter;
        }
        case NSOrPredicateType: {
            auto filter = mbgl::style::AnyFilter();
            filter.filters = [self mgl_subfilters];
            return filter;
        }
    }
    
    [NSException raise:@"Compound predicate type not handled"
                format:@""];
    return {};
}

@end
