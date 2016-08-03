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

@end
