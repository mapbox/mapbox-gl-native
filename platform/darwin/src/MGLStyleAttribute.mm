#import "MGLStyleAttribute.hpp"

#import "MGLStyleAttributeValue_Private.hpp"
#import "MGLStyleAttributeFunction_Private.hpp"

@interface MGLStyleAttribute()
@end

@implementation MGLStyleAttribute

+ (id <MGLStyleAttributeValue>)mbgl_colorPropertyValueWith:(mbgl::style::PropertyValue<mbgl::Color>)property
{
    if (property.isConstant()) {
        return [MGLColor mbgl_colorWithColor:property.asConstant()];
    } else if (property.isFunction()) {
        return [MGLStyleAttributeFunction functionWithColorPropertyValue:property.asFunction()];
    } else {
        return nil;
    }
}

+ (id <MGLStyleAttributeValue>)mbgl_numberPropertyValueWith:(mbgl::style::PropertyValue<float>)property
{
    if (property.isConstant()) {
        return @(property.asConstant());
    } else if (property.isFunction()) {
        return [MGLStyleAttributeFunction functionWithNumberPropertyValue:property.asFunction()];
    } else {
        return nil;
    }
}

+ (id<MGLStyleAttributeValue>)mbgl_boolPropertyValueWith:(mbgl::style::PropertyValue<bool>)property
{
    if (property.isConstant()) {
        return @(property.asConstant());
    } else if (property.isFunction()) {
        return [MGLStyleAttributeFunction functionWithBoolPropertyValue:property.asFunction()];
    } else {
        return nil;
    }
}

+ (id<MGLStyleAttributeValue>)mbgl_stringPropertyValueWith:(mbgl::style::PropertyValue<std::string>)property
{
    if (property.isConstant()) {
        return @(property.asConstant().c_str());
    } else if (property.isFunction()) {
        return [MGLStyleAttributeFunction functionWithStringPropertyValue:property.asFunction()];
    } else {
        return nil;
    }
}

+ (id<MGLStyleAttributeValue>)mbgl_offsetPropertyValueWith:(mbgl::style::PropertyValue<std::array<float, 2> >)property
{
    if (property.isConstant()) {
        auto offset = property.asConstant();
        return @[@(offset[0]), @(offset[1])];
    } else if (property.isFunction()) {
        return [MGLStyleAttributeFunction functionWithOffsetPropertyValue:property.asFunction()];
    } else {
        return nil;
    }
}

+ (id<MGLStyleAttributeValue>)mbgl_paddingPropertyValueWith:(mbgl::style::PropertyValue<std::array<float, 4> >)property
{
    if (property.isConstant()) {
        auto padding = property.asConstant();
        return @[@(padding[0]), @(padding[1]), @(padding[2]), @(padding[3])];
    } else if (property.isFunction()) {
        return [MGLStyleAttributeFunction functionWithPaddingPropertyValue:property.asFunction()];
    } else {
        return nil;
    }
}

+ (id<MGLStyleAttributeValue>)mbgl_stringArrayPropertyValueWith:(mbgl::style::PropertyValue<std::vector<std::string> >)property
{
    if (property.isConstant()) {
        auto strings = property.asConstant();
        NSMutableArray *convertedStrings = [[NSMutableArray alloc] initWithCapacity:strings.size()];
        for (auto string : strings) {
            [convertedStrings addObject:@(string.c_str())];
        }
        return convertedStrings;
    } else if (property.isFunction()) {
        return [MGLStyleAttributeFunction functionWithStringArrayPropertyValue:property.asFunction()];
    } else {
        return nil;
    }
}

+ (id<MGLStyleAttributeValue>)mbgl_numberArrayPropertyValueWith:(mbgl::style::PropertyValue<std::vector<float> >)property
{
    if (property.isConstant()) {
        auto numbers = property.asConstant();
        NSMutableArray *convertedNumbers = [NSMutableArray arrayWithCapacity:numbers.size()];
        for (auto number : numbers) {
            [convertedNumbers addObject:@(number)];
        }
        return convertedNumbers;
    } else if (property.isFunction()) {
        return [MGLStyleAttributeFunction functionWithNumberArrayPropertyValue:property.asFunction()];
    } else {
        return nil;
    }
}

@end
