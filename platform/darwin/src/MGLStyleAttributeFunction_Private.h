#import "MGLStyleAttributeFunction.h"

#import "NSValue+MGLStyleAttributeAdditions_Private.h"

#if TARGET_OS_IPHONE
    #import "UIColor+MGLAdditions.h"
#else
    #import "NSColor+MGLAdditions.h"
#endif

#include <mbgl/util/color.hpp>
#include <mbgl/util/variant.hpp>
#include <mbgl/style/function.hpp>
#include <mbgl/style/property_value.hpp>

#include <array>

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

@end

template <typename MBGLType, typename ObjCType>
class MGLStyleValueTransformer {
public:
    
    MGLStyleValue<ObjCType> *toStyleValue(const mbgl::style::PropertyValue<MBGLType> &mbglValue) {
        if (mbglValue.isConstant()) {
            return toStyleConstantValue(mbglValue.asConstant());
        } else if (mbglValue.isFunction()) {
            return toStyleFunction(mbglValue.asFunction());
        } else {
            return nil;
        }
    }
    
    mbgl::style::PropertyValue<MBGLType> toPropertyValue(MGLStyleValue<ObjCType> *value) {
        if ([value isKindOfClass:[MGLStyleConstantValue class]]) {
            MBGLType mbglValue;
            getMBGLValue([(MGLStyleConstantValue<ObjCType> *)value rawValue], mbglValue);
            return mbglValue;
        } else if ([value isKindOfClass:[MGLStyleFunction class]]) {
            MGLStyleFunction<ObjCType> *function = (MGLStyleFunction<ObjCType> *)value;
            __block std::vector<std::pair<float, MBGLType>> mbglStops;
            [function.stops enumerateKeysAndObjectsUsingBlock:^(NSNumber * _Nonnull zoomKey, MGLStyleValue<ObjCType> * _Nonnull stopValue, BOOL * _Nonnull stop) {
                NSCAssert([stopValue isKindOfClass:[MGLStyleValue class]], @"Stops should be MGLStyleValues");
                auto mbglStopValue = toPropertyValue(stopValue);
                NSCAssert(mbglStopValue.isConstant(), @"Stops must be constant");
                mbglStops.emplace_back(zoomKey.floatValue, mbglStopValue.asConstant());
            }];
            return mbgl::style::Function<MBGLType>({{mbglStops}}, function.base);
        } else {
            return {};
        }
    }
    
private:
    
    MGLStyleConstantValue<ObjCType> *toStyleConstantValue(const MBGLType mbglValue) {
        auto rawValue = toMGLRawStyleValue(mbglValue);
        return [MGLStyleConstantValue<ObjCType> valueWithRawValue:rawValue];
    }
    
    MGLStyleFunction<ObjCType> *toStyleFunction(const mbgl::style::Function<MBGLType> &mbglFunction) {
        const auto &mbglStops = mbglFunction.getStops();
        NSMutableDictionary *stops = [NSMutableDictionary dictionaryWithCapacity:mbglStops.size()];
        for (const auto &mbglStop : mbglStops) {
            auto rawValue = toMGLRawStyleValue(mbglStop.second);
            stops[@(mbglStop.first)] = [MGLStyleValue valueWithRawValue:rawValue];
        }
        return [MGLStyleFunction<ObjCType> functionWithBase:mbglFunction.getBase() stops:stops];
    }
    
    NSNumber *toMGLRawStyleValue(const bool mbglStopValue) {
        return @(mbglStopValue);
    }
    
    NSNumber *toMGLRawStyleValue(const float mbglStopValue) {
        return @(mbglStopValue);
    }
    
    NSString *toMGLRawStyleValue(const std::string &mbglStopValue) {
        return @(mbglStopValue.c_str());
    }
    
    // Offsets
    NSValue *toMGLRawStyleValue(const std::array<float, 2> &mbglStopValue) {
        return [NSValue mgl_valueWithOffsetArray:mbglStopValue];
    }
    
    // Padding
    NSValue *toMGLRawStyleValue(const std::array<float, 4> &mbglStopValue) {
        return [NSValue mgl_valueWithPaddingArray:mbglStopValue];
    }
    
    // Enumerations
    template <typename MBGLEnum = MBGLType, class = typename std::enable_if<std::is_enum<MBGLEnum>::value>::type>
    ObjCType toMGLRawStyleValue(const MBGLType &mbglStopValue) {
        NSUInteger rawValue = static_cast<NSUInteger>(mbglStopValue);
        return [NSValue value:&rawValue withObjCType:@encode(NSUInteger)];
    }
    
    MGLColor *toMGLRawStyleValue(const mbgl::Color mbglStopValue) {
        return [MGLColor mbgl_colorWithColor:mbglStopValue];
    }
    
    ObjCType toMGLRawStyleValue(const std::vector<MBGLType> &mbglStopValue) {
        NSMutableArray *array = [NSMutableArray arrayWithCapacity:mbglStopValue.size()];
        for (const auto &mbglElement: mbglStopValue) {
            [array addObject:toMGLRawStyleValue(mbglElement)];
        }
        return array;
    }
    
private:
    
    void getMBGLValue(NSNumber *rawValue, bool &mbglValue) {
        mbglValue = !!rawValue.boolValue;
    }
    
    void getMBGLValue(NSNumber *rawValue, float &mbglValue) {
        mbglValue = !!rawValue.boolValue;
    }
    
    void getMBGLValue(NSString *rawValue, std::string &mbglValue) {
        mbglValue = rawValue.UTF8String;
    }
    
    // Offsets
    void getMBGLValue(NSValue *rawValue, std::array<float, 2> &mbglValue) {
        mbglValue = rawValue.mgl_offsetArrayValue;
    }
    
    // Padding
    void getMBGLValue(NSValue *rawValue, std::array<float, 4> &mbglValue) {
        mbglValue = rawValue.mgl_paddingArrayValue;
    }
    
    // Enumerations
    template <typename MBGLEnum = MBGLType, class = typename std::enable_if<std::is_enum<MBGLEnum>::value>::type>
    void getMBGLValue(ObjCType rawValue, MBGLType &mbglValue) {
        [rawValue getValue:&mbglValue];
    }
    
    void getMBGLValue(MGLColor *rawValue, mbgl::Color &mbglValue) {
        mbglValue = rawValue.mbgl_color;
    }
    
    void getMBGLValue(ObjCType rawValue, std::vector<MBGLType> &mbglValue) {
        mbglValue.reserve(rawValue.count);
        for (id obj in rawValue) {
            MBGLType mbglElement;
            getMBGLValue(obj, mbglElement);
            mbglValue.push_back(mbglElement);
        }
    }
};
