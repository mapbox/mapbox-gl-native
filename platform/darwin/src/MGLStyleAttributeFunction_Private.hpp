#import "MGLStyleAttributeFunction.h"

#include <mbgl/util/color.hpp>
#include <mbgl/style/function.hpp>
#include <mbgl/style/property_value.hpp>

#define MGLSetEnumProperty(name, Name, EnumType) \
    if (name.isFunction) { \
        NSAssert([name isKindOfClass:[MGLStyleAttributeFunction class]], @"" #name @" should be a function"); \
        \
        std::vector<std::pair<float, mbgl::style::EnumType>> stops; \
        \
        for (MGLStyleAttributePair *stop in [(MGLStyleAttributeFunction *)name stops]) { \
            NSNumber *t1 = stop.t1; \
            NSNumber *t2 = stop.t2; \
            stops.emplace_back(std::make_pair(t1.floatValue, static_cast<mbgl::style::EnumType>(t2.unsignedIntegerValue))); \
        } \
        auto function = mbgl::style::Function<mbgl::style::EnumType> { \
            stops, \
            [(MGLStyleAttributeFunction *)name base].floatValue, \
        }; \
        self.layer->set##Name(function); \
    } else { \
        NSAssert([name isKindOfClass:[NSValue class]], @"" #name @"should be an NSValue"); \
        mbgl::style::EnumType value; \
        [(NSValue *)name getValue:&value]; \
        self.layer->set##Name({ value }); \
    }

@interface MGLStyleAttributeFunction(Private)
- (mbgl::style::Function<mbgl::Color>)colorFunction;
@end;
