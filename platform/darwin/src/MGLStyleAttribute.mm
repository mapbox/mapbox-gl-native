#import "MGLStyleAttribute.h"

#import "MGLStyleAttributeValue_Private.h"
#import "MGLStyleAttributeFunction_Private.h"
#import "NSValue+MGLStyleAttributeAdditions_Private.h"

@interface MGLStyleAttribute()
@end

@implementation MGLStyleAttribute

+ (id<MGLStyleAttributeValue>)mbgl_colorWithPropertyValueColor:(mbgl::style::PropertyValue<mbgl::Color>)property
{
    if (property.isConstant()) {
        return [MGLColor mbgl_colorWithColor:property.asConstant()];
    } else if (property.isFunction()) {
        return [MGLStyleAttributeFunction functionWithColorPropertyValue:property.asFunction()];
    } else {
        return nil;
    }
}

+ (id <MGLStyleAttributeValue>)mbgl_numberWithPropertyValueNumber:(mbgl::style::PropertyValue<float>)property
{
    if (property.isConstant()) {
        return @(property.asConstant());
    } else if (property.isFunction()) {
        return [MGLStyleAttributeFunction functionWithNumberPropertyValue:property.asFunction()];
    } else {
        return nil;
    }
}

+ (id<MGLStyleAttributeValue>)mbgl_boolWithPropertyValueBool:(mbgl::style::PropertyValue<bool>)property
{
    if (property.isConstant()) {
        return @(property.asConstant());
    } else if (property.isFunction()) {
        return [MGLStyleAttributeFunction functionWithBoolPropertyValue:property.asFunction()];
    } else {
        return nil;
    }
}

+ (id<MGLStyleAttributeValue>)mbgl_stringWithPropertyValueString:(mbgl::style::PropertyValue<std::string>)property
{
    if (property.isConstant()) {
        return @(property.asConstant().c_str());
    } else if (property.isFunction()) {
        return [MGLStyleAttributeFunction functionWithStringPropertyValue:property.asFunction()];
    } else {
        return nil;
    }
}

+ (id<MGLStyleAttributeValue>)mbgl_offsetWithPropertyValueOffset:(mbgl::style::PropertyValue<std::array<float, 2> >)property
{
    if (property.isConstant()) {
        auto offset = property.asConstant();
        return [NSValue mgl_valueWithOffsetArray:offset];
    } else if (property.isFunction()) {
        return [MGLStyleAttributeFunction functionWithOffsetPropertyValue:property.asFunction()];
    } else {
        return nil;
    }
}

+ (id<MGLStyleAttributeValue>)mbgl_paddingWithPropertyValuePadding:(mbgl::style::PropertyValue<std::array<float, 4> >)property
{
    if (property.isConstant()) {
        auto padding = property.asConstant();
        return [NSValue mgl_valueWithPaddingArray:padding];
    } else if (property.isFunction()) {
        return [MGLStyleAttributeFunction functionWithPaddingPropertyValue:property.asFunction()];
    } else {
        return nil;
    }
}

+ (id<MGLStyleAttributeValue>)mbgl_stringArrayWithPropertyValueStringArray:(mbgl::style::PropertyValue<std::vector<std::string> >)property
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

+ (id<MGLStyleAttributeValue>)mbgl_numberArrayWithPropertyValueNumberArray:(mbgl::style::PropertyValue<std::vector<float> >)property
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
