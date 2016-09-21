#import "NSComparisonPredicate+MGLAdditions.h"

#import "NSPredicate+MGLAdditions.h"
#import "NSExpression+MGLAdditions.h"

@implementation NSComparisonPredicate (MGLAdditions)

- (mbgl::style::Filter)mgl_filter
{
    switch (self.predicateOperatorType) {
        case NSEqualToPredicateOperatorType: {
            if (self.rightExpression.constantValue)
            {
                auto filter = mbgl::style::EqualsFilter();
                filter.key = self.leftExpression.keyPath.UTF8String;
                filter.value = self.rightExpression.mgl_filterValue;
                return filter;
            }
            else
            {
                auto filter = mbgl::style::NotHasFilter();
                filter.key = self.leftExpression.keyPath.UTF8String;
                return filter;
            }
        }
        case NSNotEqualToPredicateOperatorType: {
            if (self.rightExpression.constantValue)
            {
                auto filter = mbgl::style::NotEqualsFilter();
                filter.key = self.leftExpression.keyPath.UTF8String;
                filter.value = self.rightExpression.mgl_filterValue;
                return filter;
            }
            else
            {
                auto filter = mbgl::style::HasFilter();
                filter.key = self.leftExpression.keyPath.UTF8String;
                return filter;
            }
        }
        case NSGreaterThanPredicateOperatorType: {
            auto filter = mbgl::style::GreaterThanFilter();
            filter.key = self.leftExpression.keyPath.UTF8String;
            filter.value = self.rightExpression.mgl_filterValue;
            return filter;
        }
        case NSGreaterThanOrEqualToPredicateOperatorType: {
            auto filter = mbgl::style::GreaterThanEqualsFilter();
            filter.key = self.leftExpression.keyPath.UTF8String;
            filter.value = self.rightExpression.mgl_filterValue;
            return filter;
        }
        case NSLessThanPredicateOperatorType: {
            auto filter = mbgl::style::LessThanFilter();
            filter.key = self.leftExpression.keyPath.UTF8String;
            filter.value = self.rightExpression.mgl_filterValue;
            return filter;
        }
        case NSLessThanOrEqualToPredicateOperatorType: {
            auto filter = mbgl::style::LessThanEqualsFilter();
            filter.key = self.leftExpression.keyPath.UTF8String;
            filter.value = self.rightExpression.mgl_filterValue;
            return filter;
        }
        case NSInPredicateOperatorType: {
            auto filter = mbgl::style::InFilter();
            filter.key = self.leftExpression.keyPath.UTF8String;
            filter.values = self.rightExpression.mgl_filterValues;
            return filter;
        }
        case NSContainsPredicateOperatorType: {
            auto filter = mbgl::style::InFilter();
            filter.key = [self.rightExpression.constantValue UTF8String];
            filter.values = self.leftExpression.mgl_filterValues;
            return filter;
        }
        case NSBetweenPredicateOperatorType: {
            auto filter = mbgl::style::AllFilter();
            auto gteFilter = mbgl::style::GreaterThanEqualsFilter();
            gteFilter.key = self.leftExpression.keyPath.UTF8String;
            gteFilter.value = self.rightExpression.mgl_filterValues[0];
            filter.filters.push_back(gteFilter);
            auto lteFilter = mbgl::style::LessThanEqualsFilter();
            lteFilter.key = self.leftExpression.keyPath.UTF8String;
            lteFilter.value = self.rightExpression.mgl_filterValues[1];
            filter.filters.push_back(lteFilter);
            return filter;
        }
        case NSMatchesPredicateOperatorType:
        case NSLikePredicateOperatorType:
        case NSBeginsWithPredicateOperatorType:
        case NSEndsWithPredicateOperatorType:
        case NSCustomSelectorPredicateOperatorType:
            [NSException raise:@"Unsupported operator type"
                        format:@"NSPredicateOperatorType:%lu is not supported.", (unsigned long)self.predicateOperatorType];
    }
    
    return {};
}

@end
