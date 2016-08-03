#import "MGLStyleAttributeFunction.h"

#include <mbgl/util/color.hpp>
#include <mbgl/style/function.hpp>
#include <mbgl/style/property_value.hpp>

#define MGLSetEnumProperty(name, Name, MBGLType, ObjCType) \
    if (name.isFunction) { \
        NSAssert([name isKindOfClass:[MGLStyleAttributeFunction class]], @"" #name @" should be a function"); \
        \
        __block std::vector<std::pair<float, mbgl::style::MBGLType>> stops; \
        [[(MGLStyleAttributeFunction *)name stops] enumerateKeysAndObjectsUsingBlock:^(NSNumber * _Nonnull key, NSValue * _Nonnull obj, BOOL * _Nonnull stop) { \
            NSAssert([obj isKindOfClass:[NSValue class]], @"Stops in " #name @" should be NSValues"); \
            ObjCType value; \
            [obj getValue:&value]; \
            stops.emplace_back(key.floatValue, static_cast<mbgl::style::MBGLType>(value)); \
        }]; \
        auto function = mbgl::style::Function<mbgl::style::MBGLType> { \
            stops, \
            [(MGLStyleAttributeFunction *)name base].floatValue, \
        }; \
        self.layer->set##Name(function); \
    } else { \
        NSAssert([name isKindOfClass:[NSValue class]], @"" #name @"should be an NSValue"); \
        ObjCType value; \
        [(NSValue *)name getValue:&value]; \
        self.layer->set##Name({ static_cast<mbgl::style::MBGLType>(value) }); \
    }

@interface MGLStyleAttributeFunction(Private)

+ (instancetype)functionWithColorPropertyValue:(mbgl::style::Function<mbgl::Color>)property;

+ (instancetype)functionWithNumberPropertyValue:(mbgl::style::Function<float>)property;

+ (instancetype)functionWithBoolPropertyValue:(mbgl::style::Function<bool>)property;

@end;
