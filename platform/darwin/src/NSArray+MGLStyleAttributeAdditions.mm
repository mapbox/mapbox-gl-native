#import "NSArray+MGLStyleAttributeAdditions.h"

#import "NSArray+MGLStyleAttributeAdditions_Private.hpp"
#import "MGLStyleAttributeValue_Private.hpp"

#include <array>

@interface  NSArray(Private) <MGLStyleAttributeValue_Private>
@end

@implementation NSArray (MGLStyleAttributeAdditions)

- (mbgl::style::PropertyValue<std::array<float, 2>>)mbgl_offsetPropertyValue
{
    NSAssert(self.count == 2, @"Offset must contain 2 values (dx, dy)");
    NSNumber *dx = self[0];
    NSNumber *dy = self[1];
    return {{dx.floatValue, dy.floatValue}};
}

- (mbgl::style::PropertyValue<std::array<float, 4> >)mbgl_paddingPropertyValue
{
    NSAssert(self.count == 4, @"Padding must contain 4 values (top, left, bottom & right)");
    NSNumber *top = self[0];
    NSNumber *left = self[1];
    NSNumber *bottom = self[2];
    NSNumber *right = self[3];
    return {{top.floatValue, left.floatValue, bottom.floatValue, right.floatValue}};
}

- (mbgl::style::PropertyValue<std::vector<std::string> >)mbgl_fontPropertyValue
{
    std::vector<std::string>fonts;
    
    for (NSString *font in self) {
        fonts.emplace_back(font.UTF8String);
    }
    
    return {{fonts}};
}

- (mbgl::style::PropertyValue<std::vector<float> >)mbgl_dashArrayPropertyValue
{
    std::vector<float>values;
    
    for (NSNumber *n in self) {
        values.emplace_back(n.floatValue);
    }
    
    return {{values}};
}

@end
