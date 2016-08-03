#import "MGLStyleAttribute.hpp"

#import "MGLStyleAttributeValue_Private.hpp"
#import "MGLStyleAttributeFunction_Private.hpp"

@interface MGLStyleAttribute()
@end

@implementation MGLStyleAttribute

+ (id <MGLStyleAttributeValue>)mbgl_colorPropertyValueWith:(mbgl::style::PropertyValue<mbgl::Color>)color
{
    if (color.isConstant()) {
        return [MGLColor mbgl_colorWithColor:color.asConstant()];
    } else if (color.isFunction()) {
        return [MGLStyleAttributeFunction functionWithColorPropertyValue:color.asFunction()];
    } else {
        return nil;
    }
}

@end
