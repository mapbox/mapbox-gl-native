#import <Foundation/Foundation.h>

#import "MGLStyleValue.h"

#import "NSValue+MGLStyleAttributeAdditions.h"
#import "MGLTypes.h"
#import "MGLLineStyleLayer.h"
#import <mbgl/util/enum.hpp>

#if TARGET_OS_IPHONE
    #import "UIColor+MGLAdditions.h"
#else
    #import "NSColor+MGLAdditions.h"
#endif

#include <array>

template <typename MBGLType, typename ObjCType, typename MBGLElement = MBGLType, typename ObjCEnum = ObjCType>
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

    template <typename MBGLEnum = MBGLType,
              class = typename std::enable_if<std::is_enum<MBGLEnum>::value>::type,
              typename MGLEnum = ObjCEnum,
              class = typename std::enable_if<std::is_enum<MGLEnum>::value>::type>
    MGLStyleValue<ObjCType> *toEnumStyleValue(const mbgl::style::PropertyValue<MBGLEnum> &mbglValue) {
        if (mbglValue.isConstant()) {
            return toEnumStyleConstantValue<>(mbglValue.asConstant());
        } else if (mbglValue.isFunction()) {
            const auto &mbglStops = mbglValue.asFunction().getStops();
            NSMutableDictionary *stops = [NSMutableDictionary dictionaryWithCapacity:mbglStops.size()];
            for (const auto &mbglStop : mbglStops) {
                stops[@(mbglStop.first)] = toEnumStyleConstantValue<>(mbglStop.second);
            }
            return [MGLStyleFunction<NSValue *> functionWithBase:mbglValue.asFunction().getBase() stops:stops];
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
        } else if (value) {
            [NSException raise:@"MGLAbstractClassException" format:
             @"The style value %@ cannot be applied to the style. "
             @"Make sure the style value was created as a member of a concrete subclass of MGLStyleValue.",
             NSStringFromClass([value class])];
            return {};
        } else {
            return {};
        }
    }

    template <typename MBGLEnum = MBGLType,
              class = typename std::enable_if<std::is_enum<MBGLEnum>::value>::type,
              typename MGLEnum = ObjCEnum,
              class = typename std::enable_if<std::is_enum<MGLEnum>::value>::type>
    mbgl::style::PropertyValue<MBGLEnum> toEnumPropertyValue(MGLStyleValue<ObjCType> *value) {
        if ([value isKindOfClass:[MGLStyleConstantValue class]]) {
            MBGLEnum mbglValue;
            getMBGLValue([(MGLStyleConstantValue<ObjCType> *)value rawValue], mbglValue);
            return mbglValue;
        } else if ([value isKindOfClass:[MGLStyleFunction class]]) {
            MGLStyleFunction<NSValue *> *function = (MGLStyleFunction<NSValue *> *)value;
            __block std::vector<std::pair<float, MBGLEnum>> mbglStops;
            [function.stops enumerateKeysAndObjectsUsingBlock:^(NSNumber * _Nonnull zoomKey, MGLStyleValue<NSValue *> * _Nonnull stopValue, BOOL * _Nonnull stop) {
                NSCAssert([stopValue isKindOfClass:[MGLStyleValue class]], @"Stops should be MGLStyleValues");
                auto mbglStopValue = toEnumPropertyValue(stopValue);
                NSCAssert(mbglStopValue.isConstant(), @"Stops must be constant");
                mbglStops.emplace_back(zoomKey.floatValue, mbglStopValue.asConstant());
            }];
            return mbgl::style::Function<MBGLEnum>({{mbglStops}}, function.base);
        } else if (value) {
            [NSException raise:@"MGLAbstractClassException" format:
             @"The style value %@ cannot be applied to the style. "
             @"Make sure the style value was created as a member of a concrete subclass of MGLStyleValue.",
             NSStringFromClass([value class])];
            return {};
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

    template <typename MBGLEnum = MBGLType,
              class = typename std::enable_if<std::is_enum<MBGLEnum>::value>::type,
              typename MGLEnum = ObjCEnum,
              class = typename std::enable_if<std::is_enum<MGLEnum>::value>::type>
    MGLStyleConstantValue<ObjCType> *toEnumStyleConstantValue(const MBGLEnum mbglValue) {
        auto str = mbgl::Enum<MBGLEnum>::toString(mbglValue);
        MGLEnum mglType = *mbgl::Enum<MGLEnum>::toEnum(str);
        return [MGLStyleConstantValue<ObjCType> valueWithRawValue:[NSValue value:&mglType withObjCType:@encode(MGLEnum)]];
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
    
    MGLColor *toMGLRawStyleValue(const mbgl::Color mbglStopValue) {
        return [MGLColor mgl_colorWithColor:mbglStopValue];
    }
    
    ObjCType toMGLRawStyleValue(const std::vector<MBGLElement> &mbglStopValue) {
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
        mbglValue = rawValue.floatValue;
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
    
    void getMBGLValue(MGLColor *rawValue, mbgl::Color &mbglValue) {
        mbglValue = rawValue.mgl_color;
    }
    
    void getMBGLValue(ObjCType rawValue, std::vector<MBGLElement> &mbglValue) {
        mbglValue.reserve(rawValue.count);
        for (id obj in rawValue) {
            MBGLElement mbglElement;
            getMBGLValue(obj, mbglElement);
            mbglValue.push_back(mbglElement);
        }
    }

    // Enumerations
    template <typename MBGLEnum = MBGLType,
    class = typename std::enable_if<std::is_enum<MBGLEnum>::value>::type,
    typename MGLEnum = ObjCEnum,
    class = typename std::enable_if<std::is_enum<MGLEnum>::value>::type>
    void getMBGLValue(ObjCType rawValue, MBGLEnum &mbglValue) {
        MGLEnum mglEnum;
        [rawValue getValue:&mglEnum];
        auto str = mbgl::Enum<MGLEnum>::toString(mglEnum);
        mbglValue = *mbgl::Enum<MBGLEnum>::toEnum(str);
    }
};
