#import "NSCompoundPredicate+MGLAdditions.h"

#import "MGLStyleValue_Private.h"

#import "NSPredicate+MGLAdditions.h"
#import "NSExpression+MGLPrivateAdditions.h"

#include <mbgl/style/conversion/property_value.hpp>

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
        case NSNotPredicateType:
        case NSOrPredicateType:
        case NSAndPredicateType: {
            mbgl::style::conversion::Error valueError;
            NSArray *jsonObject = self.mgl_jsonExpressionObject;
            auto value = mbgl::style::conversion::convert<std::unique_ptr<mbgl::style::expression::Expression>>(mbgl::style::conversion::makeConvertible(jsonObject), valueError, mbgl::style::expression::type::Boolean);
            mbgl::style::ExpressionFilter filter;
            if (!value) {
                [NSException raise:NSInvalidArgumentException
                            format:@"Invalid property value: %@", @(valueError.message.c_str())];
                return {};
            }
            filter.expression = std::move(*value);
            
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
