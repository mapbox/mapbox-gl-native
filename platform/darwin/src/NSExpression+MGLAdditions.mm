#import "NSExpression+MGLAdditions.h"

@implementation NSExpression (MGLAdditions)

- (std::vector<mbgl::Value>)mgl_filterValues
{
    if ([self.constantValue isKindOfClass:NSArray.class]) {
        NSArray *values = self.constantValue;
        std::vector<mbgl::Value>convertedValues;
        for (id value in values) {
            convertedValues.push_back([self mgl_convertedValueWithValue:value]);
        }
        return convertedValues;
    }
    [NSException raise:@"Values not handled" format:@""];
    return { };
}

- (mbgl::Value)mgl_filterValue
{
    return [self mgl_convertedValueWithValue:self.constantValue];
}

- (mbgl::Value)mgl_convertedValueWithValue:(id)value
{
    if ([value isKindOfClass:NSString.class]) {
        return { std::string([(NSString *)value UTF8String]) };
    } else if ([value isKindOfClass:NSNumber.class]) {
        NSNumber *number = (NSNumber *)value;
        if((strcmp([number objCType], @encode(int))) == 0) {
            return { number.intValue };
        } else if ((strcmp([number objCType], @encode(double))) == 0) {
            return { number.doubleValue };
        } else if ((strcmp([number objCType], @encode(bool))) == 0) {
            return { number.boolValue };
        }
    }
    [NSException raise:@"Value not handled"
                format:@"Can’t convert %s:%@ to mbgl::Value", [value objCType], value];
    return { };
}

@end
