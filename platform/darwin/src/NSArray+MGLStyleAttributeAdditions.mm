#import "NSArray+MGLStyleAttributeAdditions.h"

#import "NSArray+MGLStyleAttributeAdditions_Private.h"
#import "MGLStyleAttributeValue_Private.h"

#include <array>

@interface  NSArray(Private) <MGLStyleAttributeValue_Private>
@end

@implementation NSArray (MGLStyleAttributeAdditions)

- (mbgl::style::PropertyValue<std::vector<std::string> >)mbgl_stringArrayPropertyValue
{
    std::vector<std::string>fonts;
    
    for (NSString *font in self) {
        fonts.emplace_back(font.UTF8String);
    }
    
    return {{fonts}};
}

- (mbgl::style::PropertyValue<std::vector<float> >)mbgl_numberArrayPropertyValue
{
    std::vector<float>values;
    
    for (NSNumber *n in self) {
        values.emplace_back(n.floatValue);
    }
    
    return {{values}};
}


@end
