#import "NSComparisonPredicate+MGLAdditions.h"

#import "NSPredicate+MGLAdditions.h"
#import "NSExpression+MGLAdditions.h"

@implementation NSComparisonPredicate (MGLAdditions)

- (mbgl::style::Filter)mgl_filter
{
    switch (self.predicateOperatorType) {
        case NSEqualToPredicateOperatorType: {
            auto filter = mbgl::style::EqualsFilter();
            filter.key = self.leftExpression.keyPath.UTF8String;
            filter.value = self.rightExpression.mgl_filterValue;
            return filter;
        }
        case NSNotEqualToPredicateOperatorType: {
            auto filter = mbgl::style::NotEqualsFilter();
            filter.key = self.leftExpression.keyPath.UTF8String;
            filter.value = self.rightExpression.mgl_filterValue;
            return filter;
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
        case NSMatchesPredicateOperatorType:
        case NSLikePredicateOperatorType:
        case NSBeginsWithPredicateOperatorType:
        case NSEndsWithPredicateOperatorType:
        case NSCustomSelectorPredicateOperatorType:
        case NSContainsPredicateOperatorType:
        case NSBetweenPredicateOperatorType:
            [NSException raise:@"Unsupported operator type"
                        format:@"NSPredicateOperatorType:%lu is not supported.", (unsigned long)self.predicateOperatorType];
    }
    
    return {};
}

@end
