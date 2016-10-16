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
        if ((strcmp([number objCType], @encode(char)) == 0) ||
            (strcmp([number objCType], @encode(BOOL)) == 0)) {
            // char: 32-bit boolean
            // BOOL: 64-bit boolean
            return { (bool)number.boolValue };
        } else if (strcmp([number objCType], @encode(double)) == 0) {
            // Double values on all platforms are interpreted precisely.
            return { (double)number.doubleValue };
        } else if (strcmp([number objCType], @encode(float)) == 0) {
            // Float values when taken as double introduce precision problems,
            // so warn the user to avoid them. This would require them to
            // explicitly use -[NSNumber numberWithFloat:] arguments anyway.
            // We still do this conversion in order to provide a valid value.
            static dispatch_once_t onceToken;
            dispatch_once(&onceToken, ^{
                NSLog(@"One-time warning: Float values are converted to double and can introduce imprecision; please use double values explicitly in predicate arguments.");
            });
            return { (double)number.doubleValue };
        } else if ([number compare:@(0)] == NSOrderedDescending ||
                   [number compare:@(0)] == NSOrderedSame) {
            // Positive integer or zero; use uint64_t per mbgl::Value definition.
            // We use unsigned long long here to avoid any truncation.
            return { (uint64_t)number.unsignedLongLongValue };
        } else if ([number compare:@(0)] == NSOrderedAscending) {
            // Negative integer; use int64_t per mbgl::Value definition.
            // We use long long here to avoid any truncation.
            return { (int64_t)number.longLongValue };
        }
    }
    [NSException raise:@"Value not handled"
                format:@"Can’t convert %s:%@ to mbgl::Value", [value objCType], value];
    return { };
}

- (mbgl::FeatureIdentifier)mgl_featureIdentifier
{
    id value = self.constantValue;
    mbgl::Value mbglValue = [self mgl_filterValue];
    
    if ([value isKindOfClass:NSString.class]) {
        return mbglValue.get<std::string>();
    } else if ([value isKindOfClass:NSNumber.class]) {
        NSNumber *number = (NSNumber *)value;
        if ((strcmp([number objCType], @encode(char)) == 0) ||
            (strcmp([number objCType], @encode(BOOL)) == 0)) {
            return mbglValue.get<bool>();
        } else if ( strcmp([number objCType], @encode(double)) == 0 ||
                    strcmp([number objCType], @encode(float)) == 0) {
            return mbglValue.get<double>();
        } else if ([number compare:@(0)] == NSOrderedDescending ||
                   [number compare:@(0)] == NSOrderedSame) {
            return mbglValue.get<uint64_t>();
        } else if ([number compare:@(0)] == NSOrderedAscending) {
            return mbglValue.get<int64_t>();
        }
    }
    
    return {};
}

@end
