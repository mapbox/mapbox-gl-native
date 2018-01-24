#import "NSCompoundPredicate+MGLAdditions.h"

#import "NSPredicate+MGLAdditions.h"
#import "NSExpression+MGLPrivateAdditions.h"

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
            NSAssert(self.subpredicates.count <= 1, @"NOT predicate cannot have multiple subpredicates.");
            NSPredicate *subpredicate = self.subpredicates.firstObject;
            mbgl::style::Filter subfilter = subpredicate.mgl_filter;

            // Convert NOT(!= nil) to NotHasFilter.
            if (subfilter.is<mbgl::style::HasFilter>()) {
                auto hasFilter = subfilter.get<mbgl::style::HasFilter>();
                return mbgl::style::NotHasFilter { .key = hasFilter.key };
            }

            // Convert NOT(== nil) to HasFilter.
            if (subfilter.is<mbgl::style::NotHasFilter>()) {
                auto hasFilter = subfilter.get<mbgl::style::NotHasFilter>();
                return mbgl::style::HasFilter { .key = hasFilter.key };
            }

            // Convert NOT(IN) or NOT(CONTAINS) to NotInFilter.
            if (subfilter.is<mbgl::style::InFilter>()) {
                auto inFilter = subfilter.get<mbgl::style::InFilter>();
                mbgl::style::NotInFilter notInFilter;
                notInFilter.key = inFilter.key;
                notInFilter.values = inFilter.values;
                return notInFilter;
            }

            // Convert NOT(), NOT(AND), NOT(NOT), NOT(==), etc. into NoneFilter.
            mbgl::style::NoneFilter noneFilter;
            if (subfilter.is<mbgl::style::AnyFilter>()) {
                // Flatten NOT(OR).
                noneFilter.filters = subfilter.get<mbgl::style::AnyFilter>().filters;
            } else if (subpredicate) {
                noneFilter.filters = { subfilter };
            }
            return noneFilter;
        }
        case NSAndPredicateType: {
            mbgl::style::AllFilter filter;
            filter.filters = self.mgl_subfilters;
            return filter;
        }
        case NSOrPredicateType: {
            mbgl::style::AnyFilter filter;
            filter.filters = self.mgl_subfilters;
            return filter;
        }
    }

    [NSException raise:@"Compound predicate type not handled"
                format:@""];
    return {};
}

@end

@implementation NSCompoundPredicate (MGLExpressionAdditions)

- (id)mgl_jsonExpressionObject {
    switch (self.compoundPredicateType) {
        case NSNotPredicateType: {
            NSAssert(self.subpredicates.count <= 1, @"NOT predicate cannot have multiple subpredicates.");
            NSPredicate *subpredicate = self.subpredicates.firstObject;
            return @[@"!", subpredicate.mgl_jsonExpressionObject];
        }
            
        case NSAndPredicateType: {
            NSArray *subarrays = [self.subpredicates valueForKeyPath:@"mgl_jsonExpressionObject"];
            return [@[@"all"] arrayByAddingObjectsFromArray:subarrays];
        }
            
        case NSOrPredicateType: {
            NSArray *subarrays = [self.subpredicates valueForKeyPath:@"mgl_jsonExpressionObject"];
            return [@[@"any"] arrayByAddingObjectsFromArray:subarrays];
        }
    }
    
    [NSException raise:@"Compound predicate type not handled"
                format:@""];
    return nil;
}

@end
