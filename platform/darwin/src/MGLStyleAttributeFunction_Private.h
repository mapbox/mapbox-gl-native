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
    } else if (name) { \
        NSAssert([name isKindOfClass:[NSValue class]], @"" #name @"should be an NSValue"); \
        ObjCType value; \
        [(NSValue *)name getValue:&value]; \
        self.layer->set##Name({ static_cast<mbgl::style::MBGLType>(value) }); \
    } else { \
        self.layer->set##Name({}); \
    }

#define MGLGetEnumProperty(Name, MBGLType, ObjCType) \
    const char *type = @encode(ObjCType); \
    mbgl::style::PropertyValue<mbgl::style::MBGLType> property = self.layer->get##Name() ?: self.layer->getDefault##Name(); \
    if (property.isConstant()) { \
        ObjCType value = static_cast<ObjCType>(property.asConstant()); \
        return [NSValue value:&value withObjCType:type]; \
    } else if (property.isFunction()) { \
        MGLStyleAttributeFunction *function = [[MGLStyleAttributeFunction alloc] init]; \
        auto stops = property.asFunction().getStops(); \
        NSMutableDictionary *convertedStops = [NSMutableDictionary dictionaryWithCapacity:stops.size()]; \
        for (auto stop : stops) { \
            ObjCType value = static_cast<ObjCType>(stop.second); \
            convertedStops[@(stop.first)] = [NSValue value:&value withObjCType:type]; \
        } \
        function.base = @(property.asFunction().getBase()); \
        function.stops = convertedStops; \
        return function; \
    } else { \
        return nil; \
    }

@interface MGLStyleAttributeFunction(Private)

+ (instancetype)functionWithColorPropertyValue:(mbgl::style::Function<mbgl::Color>)property;

+ (instancetype)functionWithNumberPropertyValue:(mbgl::style::Function<float>)property;

+ (instancetype)functionWithBoolPropertyValue:(mbgl::style::Function<bool>)property;

+ (instancetype)functionWithStringPropertyValue:(mbgl::style::Function<std::string>)property;

+ (instancetype)functionWithOffsetPropertyValue:(mbgl::style::Function<std::array<float, 2>>)property;

+ (instancetype)functionWithPaddingPropertyValue:(mbgl::style::Function<std::array<float, 4>>)property;

+ (instancetype)functionWithStringArrayPropertyValue:(mbgl::style::Function<std::vector<std::string>>)property;

+ (instancetype)functionWithNumberArrayPropertyValue:(mbgl::style::Function<std::vector<float>>)property;

+ (instancetype)functionWithEnumProperyValue:(mbgl::style::Function<bool>)property type:(const char *)type;

@end;
