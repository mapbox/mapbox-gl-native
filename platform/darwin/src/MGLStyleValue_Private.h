#import <Foundation/Foundation.h>

#import "MGLStyleValue.h"

#import "NSValue+MGLStyleAttributeAdditions.h"
#import "MGLTypes.h"

#import <mbgl/util/enum.hpp>

#include <mbgl/style/data_driven_property_value.hpp>

#include <mbgl/util/interpolate.hpp>

#if TARGET_OS_IPHONE
    #import "UIColor+MGLAdditions.h"
#else
    #import "NSColor+MGLAdditions.h"
#endif

template <typename MBGLType, typename ObjCType, typename MBGLElement = MBGLType, typename ObjCEnum = ObjCType>
class MGLStyleValueTransformer {
public:

    // Convert an mbgl property value into an mgl style value
    MGLStyleValue<ObjCType> *toStyleValue(const mbgl::style::PropertyValue<MBGLType> &mbglValue) {
        PropertyValueEvaluator evaluator;
        return mbglValue.evaluate(evaluator);
    }

    // Convert an mbgl data driven property value into an mgl style value
    MGLStyleValue<ObjCType> *toDataDrivenStyleValue(const mbgl::style::DataDrivenPropertyValue<MBGLType> &mbglValue) {
        PropertyValueEvaluator evaluator;
        return mbglValue.evaluate(evaluator);
    }

    // Convert an mbgl property value containing an enum into an mgl style value
    template <typename MBGLEnum = MBGLType,
              class = typename std::enable_if<std::is_enum<MBGLEnum>::value>::type,
              typename MGLEnum = ObjCEnum,
              class = typename std::enable_if<std::is_enum<MGLEnum>::value>::type>
    MGLStyleValue<ObjCType> *toEnumStyleValue(const mbgl::style::PropertyValue<MBGLEnum> &mbglValue) {
        EnumPropertyValueEvaluator<MBGLEnum, ObjCEnum> evaluator;
        return mbglValue.evaluate(evaluator);
    }

    // Convert an mgl style value into a non interpolatable (camera with interval stops) mbgl property value
    mbgl::style::PropertyValue<MBGLType> toPropertyValue(MGLStyleValue<ObjCType> *value) {
        if ([value isKindOfClass:[MGLSourceStyleFunction class]] || [value isKindOfClass:[MGLCompositeStyleFunction class]]) {
            [NSException raise:NSInvalidArgumentException
                        format:@"This property can only be set to camera functions. Use +[MGLStyleValue cameraFunctionValueWithinterpolationMode:stops:options:] instead."];
            return {};
        }

        if ([value isKindOfClass:[MGLStyleConstantValue class]]) {
            return toMBGLConstantValue((MGLStyleConstantValue<ObjCType> *)value);
        } else if ([value isKindOfClass:[MGLCameraStyleFunction class]]) {
            MGLCameraStyleFunction<ObjCType> *cameraStyleFunction = (MGLCameraStyleFunction<ObjCType> *)value;
            // Intentionally ignore the stop type set by the developer becuase non interpolatable property values
            // can only have interval stops. This also allows for backwards compatiblity when the developer uses
            // a deprecated MGLStyleValue method (that used to create an MGLStyleFunction) to create a function
            // for properties that are piecewise-constant (i.e. enum, bool, string)
            return toMBGLIntervalCameraFunction(cameraStyleFunction);
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

    // Convert an mgl style value into a non interpolatable (camera with exponential or interval stops) mbgl property value
    mbgl::style::PropertyValue<MBGLType> toInterpolatablePropertyValue(MGLStyleValue<ObjCType> *value) {
        if ([value isKindOfClass:[MGLSourceStyleFunction class]] || [value isKindOfClass:[MGLCompositeStyleFunction class]]) {
            [NSException raise:NSInvalidArgumentException
                        format:@"This property can only be set to camera functions. Use +[MGLStyleValue cameraFunctionValueWithinterpolationMode:stops:options:] instead."];
            return {};
        }

        if ([value isKindOfClass:[MGLStyleConstantValue class]]) {
            return toMBGLConstantValue((MGLStyleConstantValue<ObjCType> *)value);
        } else if ([value isKindOfClass:[MGLCameraStyleFunction class]]) {
            MGLCameraStyleFunction<ObjCType> *cameraStyleFunction = (MGLCameraStyleFunction<ObjCType> *)value;
            switch (cameraStyleFunction.interpolationMode) {
                case MGLInterpolationModeExponential:
                    return toMBGLExponentialCameraFunction(cameraStyleFunction);
                    break;
                case MGLInterpolationModeInterval:
                    return toMBGLIntervalCameraFunction(cameraStyleFunction);
                    break;
                default:
                    [NSException raise:NSInvalidArgumentException
                                format:@"A camera function must use either exponential or interval stops."];
                    break;
            }
            return {};
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

    // Convert an mgl style value into a mbgl data driven property value
    mbgl::style::DataDrivenPropertyValue<MBGLType> toDataDrivenPropertyValue(MGLStyleValue<ObjCType> *value) {
        if ([value isKindOfClass:[MGLStyleConstantValue class]]) {
            return toMBGLConstantValue((MGLStyleConstantValue<ObjCType> *)value);
        } else if ([value isKindOfClass:[MGLCameraStyleFunction class]]) {
            MGLCameraStyleFunction<ObjCType> *cameraStyleFunction = (MGLCameraStyleFunction<ObjCType> *)value;
            switch (cameraStyleFunction.interpolationMode) {
                case MGLInterpolationModeExponential:
                    return toMBGLExponentialCameraFunction(cameraStyleFunction);
                    break;
                case MGLInterpolationModeInterval:
                    return toMBGLIntervalCameraFunction(cameraStyleFunction);
                    break;
                default:
                    [NSException raise:NSInvalidArgumentException
                                format:@"A camera function must use either exponential or interval stops."];
                    return {};
            }
        } else if ([value isKindOfClass:[MGLSourceStyleFunction class]]) {
            MGLSourceStyleFunction<ObjCType> *sourceStyleFunction = (MGLSourceStyleFunction<ObjCType> *)value;
            switch (sourceStyleFunction.interpolationMode) {
                case MGLInterpolationModeExponential:
                    return toMBGLExponentialSourceFunction(sourceStyleFunction);
                    break;
                case MGLInterpolationModeInterval:
                    return toMBGLIntervalSourceFunction(sourceStyleFunction);
                    break;
                case MGLInterpolationModeCategorical:
                    return toMBGLCategoricalSourceFunction(sourceStyleFunction);
                    break;
                case MGLInterpolationModeIdentity:
                    return toMBGLIdentitySourceFunction(sourceStyleFunction);
                    break;
                default:
                    [NSException raise:NSInvalidArgumentException
                                format:@"A camera function must use exponential, interval, categorical, or identity stops."];
                    return {};
            }
        } else if ([value isKindOfClass:[MGLCompositeStyleFunction class]]) {
            MGLCompositeStyleFunction<ObjCType> *compositeStyleFunction = (MGLCompositeStyleFunction<ObjCType> *)value;
            switch (compositeStyleFunction.interpolationMode) {
                case MGLInterpolationModeExponential: {
                    __block std::map<float, mbgl::style::ExponentialStops<MBGLType>> outerStops = {};
                    [compositeStyleFunction.stops enumerateKeysAndObjectsUsingBlock:^(NSNumber * _Nonnull zoomKey, NSDictionary * _Nonnull stopValue, BOOL * _Nonnull stop) {
                        std::map<float, MBGLType> stops = {};
                        for (NSNumber *key in stopValue.allKeys) {
                            NSCAssert([key isKindOfClass:[NSNumber class]], @"Stop keys should be NSNumbers");
                            MGLStyleValue<ObjCType> *value = stopValue[key];
                            NSCAssert([value isKindOfClass:[MGLStyleValue class]], @"Stops should be MGLStyleValues");
                            auto mbglStopValue = toPropertyValue(value);
                            NSCAssert(mbglStopValue.isConstant(), @"Stops must be constant");
                            stops[key.floatValue] = mbglStopValue.asConstant();
                        }
                        mbgl::style::ExponentialStops<MBGLType> innerStops = {stops, (float)compositeStyleFunction.interpolationBase};
                        outerStops[zoomKey.floatValue] = innerStops;
                    }];
                    mbgl::style::CompositeFunction<MBGLType> compositeFunction = {compositeStyleFunction.attributeName.UTF8String, outerStops};
                    if (compositeStyleFunction.defaultValue) {
                        NSCAssert([compositeStyleFunction.defaultValue isKindOfClass:[MGLStyleConstantValue class]], @"Default value must be constant");
                        MBGLType mbglValue;
                        id mglValue = [(MGLStyleConstantValue<ObjCType> *)compositeStyleFunction.defaultValue rawValue];
                        getMBGLValue(mglValue, mbglValue);
                        compositeFunction.defaultValue = mbglValue;
                    }
                    return compositeFunction;
                }
                    break;
                case MGLInterpolationModeInterval: {
                    __block std::map<float, mbgl::style::IntervalStops<MBGLType>> outerStops = {};
                    [compositeStyleFunction.stops enumerateKeysAndObjectsUsingBlock:^(NSNumber * _Nonnull zoomKey, NSDictionary * _Nonnull stopValue, BOOL * _Nonnull stop) {
                        std::map<float, MBGLType> stops = {};
                        for (NSNumber *key in stopValue.allKeys) {
                            NSCAssert([key isKindOfClass:[NSNumber class]], @"Stop keys should be NSNumbers");
                            MGLStyleValue<ObjCType> *value = stopValue[key];
                            NSCAssert([value isKindOfClass:[MGLStyleValue class]], @"Stops should be MGLStyleValues");
                            auto mbglStopValue = toPropertyValue(value);
                            NSCAssert(mbglStopValue.isConstant(), @"Stops must be constant");
                            stops[key.floatValue] = mbglStopValue.asConstant();
                        }
                        mbgl::style::IntervalStops<MBGLType> innerStops = {stops};
                        outerStops[zoomKey.floatValue] = innerStops;
                    }];
                    mbgl::style::CompositeFunction<MBGLType> compositeFunction = {compositeStyleFunction.attributeName.UTF8String, outerStops};
                    if (compositeStyleFunction.defaultValue) {
                        NSCAssert([compositeStyleFunction.defaultValue isKindOfClass:[MGLStyleConstantValue class]], @"Default value must be constant");
                        MBGLType mbglValue;
                        id mglValue = [(MGLStyleConstantValue<ObjCType> *)compositeStyleFunction.defaultValue rawValue];
                        getMBGLValue(mglValue, mbglValue);
                        compositeFunction.defaultValue = mbglValue;
                    }
                    return compositeFunction;
                }
                    break;
                case MGLInterpolationModeCategorical: {
                    __block std::map<float, mbgl::style::CategoricalStops<MBGLType>> outerStops = {};
                    [compositeStyleFunction.stops enumerateKeysAndObjectsUsingBlock:^(NSNumber * _Nonnull zoomKey, NSDictionary * _Nonnull stopValue, BOOL * _Nonnull stop) {
                        __block std::map<mbgl::style::CategoricalValue, MBGLType> stops = {};
                        [stopValue enumerateKeysAndObjectsUsingBlock:^(id  _Nonnull categoryKey, MGLStyleValue<ObjCType> *  _Nonnull innerValue, BOOL * _Nonnull stop) {
                            NSCAssert([innerValue isKindOfClass:[MGLStyleValue class]], @"Stops should be MGLStyleValues");
                            auto mbglStopValue = toPropertyValue(innerValue);
                            NSCAssert(mbglStopValue.isConstant(), @"Stops must be constant");

                            if ([categoryKey isKindOfClass:[NSString class]]) {
                                const std::string& convertedValueKey = [((NSString *)categoryKey) UTF8String];
                                stops[mbgl::style::CategoricalValue(convertedValueKey)] = mbglStopValue.asConstant();
                            } else if ([categoryKey isKindOfClass:[NSNumber class]]) {
                                NSNumber *key = (NSNumber *)categoryKey;
                                if ((strcmp([key objCType], @encode(char)) == 0) ||
                                    (strcmp([key objCType], @encode(BOOL)) == 0)) {
                                    stops[mbgl::style::CategoricalValue((bool)[key boolValue])] = mbglStopValue.asConstant();
                                } else if (strcmp([key objCType], @encode(double)) == 0 ||
                                           strcmp([key objCType], @encode(float)) == 0) {
                                    NSCAssert(mbglStopValue.isConstant(), @"Categorical stop keys must be strings, booleans, or integers");
                                } else if ([key compare:@(0)] == NSOrderedDescending ||
                                           [key compare:@(0)] == NSOrderedSame ||
                                           [key compare:@(0)] == NSOrderedAscending) {
                                    stops[mbgl::style::CategoricalValue((int64_t)[key integerValue])] = mbglStopValue.asConstant();
                                }
                            }
                        }];
                        mbgl::style::CategoricalStops<MBGLType> categoricalStops = {stops};
                        outerStops[zoomKey.floatValue] = categoricalStops;
                    }];
                    mbgl::style::CompositeFunction<MBGLType> compositeFunction = {compositeStyleFunction.attributeName.UTF8String, outerStops};
                    if (compositeStyleFunction.defaultValue) {
                        NSCAssert([compositeStyleFunction.defaultValue isKindOfClass:[MGLStyleConstantValue class]], @"Default value must be constant");
                        MBGLType mbglValue;
                        id mglValue = [(MGLStyleConstantValue<ObjCType> *)compositeStyleFunction.defaultValue rawValue];
                        getMBGLValue(mglValue, mbglValue);
                        compositeFunction.defaultValue = mbglValue;
                    }
                    return compositeFunction;
                }
                    break;
                default:
                    [NSException raise:NSInvalidArgumentException
                                format:@"A composite function must use exponential, interval, or categorical stops."];
                    return {};
            }
            return {};
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

    // Convert an mgl style value containing an enum into a mbgl property value containing an enum
    template <typename MBGLEnum = MBGLType,
              class = typename std::enable_if<std::is_enum<MBGLEnum>::value>::type,
              typename MGLEnum = ObjCEnum,
              class = typename std::enable_if<std::is_enum<MGLEnum>::value>::type>
    mbgl::style::PropertyValue<MBGLEnum> toEnumPropertyValue(MGLStyleValue<ObjCType> *value) {
        if ([value isKindOfClass:[MGLSourceStyleFunction class]] || [value isKindOfClass:[MGLCompositeStyleFunction class]]) {
            [NSException raise:NSInvalidArgumentException
                        format:@"This property can only be set to camera functions. Use +[MGLStyleValue cameraFunctionValueWithinterpolationMode:stops:options:] instead."];
            return {};
        }

        if ([value isKindOfClass:[MGLStyleConstantValue class]]) {
            MBGLEnum mbglValue;
            getMBGLValue([(MGLStyleConstantValue<ObjCType> *)value rawValue], mbglValue);
            return mbglValue;
        } else if ([value isKindOfClass:[MGLCameraStyleFunction class]]) {
            MGLCameraStyleFunction<NSValue *> *cameraStyleFunction = (MGLCameraStyleFunction<NSValue *> *)value;
            __block std::map<float, MBGLType> stops = {};
            [cameraStyleFunction.stops enumerateKeysAndObjectsUsingBlock:^(NSNumber * _Nonnull zoomKey, MGLStyleValue<NSValue *> * _Nonnull stopValue, BOOL * _Nonnull stop) {
                NSCAssert([stopValue isKindOfClass:[MGLStyleValue class]], @"Stops should be MGLStyleValues");
                auto mbglStopValue = toEnumPropertyValue(stopValue);
                NSCAssert(mbglStopValue.isConstant(), @"Stops must be constant");
                stops[zoomKey.floatValue] = mbglStopValue.asConstant();
            }];

            // Enumerations can only ever use interval stops.
            mbgl::style::IntervalStops<MBGLType> intervalStops = {stops};

            mbgl::style::CameraFunction<MBGLType> cameraFunction = {intervalStops};
            return cameraFunction;
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

private: // Private utilities for converting from mgl to mbgl values

    MBGLType toMBGLConstantValue(MGLStyleConstantValue<ObjCType> *value) {
        MBGLType mbglValue;
        getMBGLValue(value.rawValue, mbglValue);
        return mbglValue;
    }

    mbgl::style::CameraFunction<MBGLType> toMBGLExponentialCameraFunction(MGLCameraStyleFunction<ObjCType> *cameraStyleFunction) {
        __block std::map<float, MBGLType> stops = {};
        [cameraStyleFunction.stops enumerateKeysAndObjectsUsingBlock:^(NSNumber * _Nonnull zoomKey, MGLStyleValue<ObjCType> * _Nonnull stopValue, BOOL * _Nonnull stop) {
            NSCAssert([stopValue isKindOfClass:[MGLStyleValue class]], @"Stops should be MGLStyleValues");
            auto mbglStopValue = toPropertyValue(stopValue);
            NSCAssert(mbglStopValue.isConstant(), @"Stops must be constant");
            stops[zoomKey.floatValue] = mbglStopValue.asConstant();
        }];

        // Camera function with Exponential stops
        mbgl::style::ExponentialStops<MBGLType> exponentialStops = {stops, (float)cameraStyleFunction.interpolationBase};
        mbgl::style::CameraFunction<MBGLType> cameraFunction = {exponentialStops};

        return cameraFunction;
    }

    mbgl::style::CameraFunction<MBGLType> toMBGLIntervalCameraFunction(MGLCameraStyleFunction<ObjCType> *cameraStyleFunction) {
        __block std::map<float, MBGLType> stops = {};
        [cameraStyleFunction.stops enumerateKeysAndObjectsUsingBlock:^(NSNumber * _Nonnull zoomKey, MGLStyleValue<ObjCType> * _Nonnull stopValue, BOOL * _Nonnull stop) {
            NSCAssert([stopValue isKindOfClass:[MGLStyleValue class]], @"Stops should be MGLStyleValues");
            auto mbglStopValue = toPropertyValue(stopValue);
            NSCAssert(mbglStopValue.isConstant(), @"Stops must be constant");
            stops[zoomKey.floatValue] = mbglStopValue.asConstant();
        }];

        // Camera function with Interval stops
        mbgl::style::IntervalStops<MBGLType> intervalStops = {stops};
        mbgl::style::CameraFunction<MBGLType> cameraFunction = {intervalStops};

        return cameraFunction;
    }

    mbgl::style::SourceFunction<MBGLType> toMBGLCategoricalSourceFunction(MGLSourceStyleFunction<ObjCType> *sourceStyleFunction) {
        __block std::map<mbgl::style::CategoricalValue, MBGLType> stops = {};
        [sourceStyleFunction.stops enumerateKeysAndObjectsUsingBlock:^(id categoryKey, MGLStyleValue<ObjCType> *stopValue, BOOL *stop) {
            NSCAssert([stopValue isKindOfClass:[MGLStyleValue class]], @"Stops should be MGLStyleValues");
            auto mbglStopValue = toPropertyValue(stopValue);
            NSCAssert(mbglStopValue.isConstant(), @"Stops must be constant");

            if ([categoryKey isKindOfClass:[NSString class]]) {
                const std::string& convertedValueKey = [((NSString *)categoryKey) UTF8String];
                stops[mbgl::style::CategoricalValue(convertedValueKey)] = mbglStopValue.asConstant();
            } else if ([categoryKey isKindOfClass:[NSNumber class]]) {
                NSNumber *key = (NSNumber *)categoryKey;
                if ((strcmp([key objCType], @encode(char)) == 0) ||
                    (strcmp([key objCType], @encode(BOOL)) == 0)) {
                    stops[mbgl::style::CategoricalValue((bool)[key boolValue])] = mbglStopValue.asConstant();
                } else if (strcmp([key objCType], @encode(double)) == 0 ||
                           strcmp([key objCType], @encode(float)) == 0) {
                    NSCAssert(mbglStopValue.isConstant(), @"Categorical stop keys must be strings, booleans, or integers");
                } else if ([key compare:@(0)] == NSOrderedDescending ||
                           [key compare:@(0)] == NSOrderedSame ||
                           [key compare:@(0)] == NSOrderedAscending) {
                    stops[mbgl::style::CategoricalValue((int64_t)[key integerValue])] = mbglStopValue.asConstant();
                }
            }
        }];
        mbgl::style::CategoricalStops<MBGLType> categoricalStops = {stops};
        mbgl::style::SourceFunction<MBGLType> sourceFunction = {sourceStyleFunction.attributeName.UTF8String, categoricalStops};
        setDefaultMBGLValue(sourceStyleFunction, sourceFunction);
        return sourceFunction;
    }

    mbgl::style::SourceFunction<MBGLType> toMBGLExponentialSourceFunction(MGLSourceStyleFunction<ObjCType> *sourceStyleFunction) {
        __block std::map<float, MBGLType> stops = {};
        [sourceStyleFunction.stops enumerateKeysAndObjectsUsingBlock:^(NSNumber * _Nonnull zoomKey, MGLStyleValue<ObjCType> * _Nonnull stopValue, BOOL * _Nonnull stop) {
            NSCAssert([stopValue isKindOfClass:[MGLStyleValue class]], @"Stops should be MGLStyleValues");
            auto mbglStopValue = toPropertyValue(stopValue);
            NSCAssert(mbglStopValue.isConstant(), @"Stops must be constant");
            stops[zoomKey.floatValue] = mbglStopValue.asConstant();
        }];
        mbgl::style::ExponentialStops<MBGLType> exponentialStops = {stops, (float)sourceStyleFunction.interpolationBase};
        mbgl::style::SourceFunction<MBGLType> sourceFunction = {sourceStyleFunction.attributeName.UTF8String, exponentialStops};
        setDefaultMBGLValue(sourceStyleFunction, sourceFunction);
        return sourceFunction;
    }

    mbgl::style::SourceFunction<MBGLType> toMBGLIntervalSourceFunction(MGLSourceStyleFunction<ObjCType> *sourceStyleFunction) {
        __block std::map<float, MBGLType> stops = {};
        [sourceStyleFunction.stops enumerateKeysAndObjectsUsingBlock:^(NSNumber * _Nonnull zoomKey, MGLStyleValue<ObjCType> * _Nonnull stopValue, BOOL * _Nonnull stop) {
            NSCAssert([stopValue isKindOfClass:[MGLStyleValue class]], @"Stops should be MGLStyleValues");
            auto mbglStopValue = toPropertyValue(stopValue);
            NSCAssert(mbglStopValue.isConstant(), @"Stops must be constant");
            stops[zoomKey.floatValue] = mbglStopValue.asConstant();
        }];
        mbgl::style::IntervalStops<MBGLType> intervalStops = {stops};
        mbgl::style::SourceFunction<MBGLType> sourceFunction = {sourceStyleFunction.attributeName.UTF8String, intervalStops};
        setDefaultMBGLValue(sourceStyleFunction, sourceFunction);
        return sourceFunction;
    }

    mbgl::style::SourceFunction<MBGLType> toMBGLIdentitySourceFunction(MGLSourceStyleFunction<ObjCType> *sourceStyleFunction) {
        mbgl::style::IdentityStops<MBGLType> identityStops;
        mbgl::style::SourceFunction<MBGLType> sourceFunction = {sourceStyleFunction.attributeName.UTF8String, identityStops};
        setDefaultMBGLValue(sourceStyleFunction, sourceFunction);
        return sourceFunction;
    }

    void setDefaultMBGLValue(MGLSourceStyleFunction<ObjCType> *sourceStyleFunction, mbgl::style::SourceFunction<MBGLType> &sourceFunction) {
        if (sourceStyleFunction.defaultValue) {
            NSCAssert([sourceStyleFunction.defaultValue isKindOfClass:[MGLStyleConstantValue class]], @"Default value must be constant");
            MBGLType mbglValue;
            id mglValue = [(MGLStyleConstantValue<ObjCType> *)sourceStyleFunction.defaultValue rawValue];
            getMBGLValue(mglValue, mbglValue);
            sourceFunction.defaultValue = mbglValue;
        }
    }

    // Bool
    void getMBGLValue(NSNumber *rawValue, bool &mbglValue) {
        mbglValue = !!rawValue.boolValue;
    }

    // Float
    void getMBGLValue(NSNumber *rawValue, float &mbglValue) {
        mbglValue = rawValue.floatValue;
    }

    // String
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

    // Color
    void getMBGLValue(MGLColor *rawValue, mbgl::Color &mbglValue) {
        mbglValue = rawValue.mgl_color;
    }

    // Array
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

private: // Private utilities for converting from mbgl to mgl values

    // Bool
    static NSNumber *toMGLRawStyleValue(const bool mbglStopValue) {
        return @(mbglStopValue);
    }

    // Float
    static NSNumber *toMGLRawStyleValue(const float mbglStopValue) {
        return @(mbglStopValue);
    }

    // Integer
    static NSNumber *toMGLRawStyleValue(const int64_t mbglStopValue) {
        return @(mbglStopValue);
    }

    // String
    static NSString *toMGLRawStyleValue(const std::string &mbglStopValue) {
        return @(mbglStopValue.c_str());
    }

    // Offsets
    static NSValue *toMGLRawStyleValue(const std::array<float, 2> &mbglStopValue) {
        return [NSValue mgl_valueWithOffsetArray:mbglStopValue];
    }

    // Padding
    static NSValue *toMGLRawStyleValue(const std::array<float, 4> &mbglStopValue) {
        return [NSValue mgl_valueWithPaddingArray:mbglStopValue];
    }

    // Color
    static MGLColor *toMGLRawStyleValue(const mbgl::Color mbglStopValue) {
        return [MGLColor mgl_colorWithColor:mbglStopValue];
    }

    // Array
    static ObjCType toMGLRawStyleValue(const std::vector<MBGLElement> &mbglStopValue) {
        NSMutableArray *array = [NSMutableArray arrayWithCapacity:mbglStopValue.size()];
        for (const auto &mbglElement: mbglStopValue) {
            [array addObject:toMGLRawStyleValue(mbglElement)];
        }
        return array;
    }

    // Enumerations
    template <typename MBGLEnum = MBGLType, typename MGLEnum = ObjCEnum>
    static NSValue *toMGLRawStyleValue(const MBGLEnum &value) {
        auto str = mbgl::Enum<MBGLEnum>::toString(value);
        MGLEnum mglType = *mbgl::Enum<MGLEnum>::toEnum(str);
        return [NSValue value:&mglType withObjCType:@encode(MGLEnum)];
    }

    // Converts mbgl stops to an equivilent NSDictionary for mgl
    static NSMutableDictionary *toConvertedStops(const std::map<float, MBGLType> &mbglStops) {
        NSMutableDictionary *stops = [NSMutableDictionary dictionaryWithCapacity:mbglStops.size()];
        for (const auto &mbglStop : mbglStops) {
            auto rawValue = toMGLRawStyleValue(mbglStop.second);
            stops[@(mbglStop.first)] = [MGLStyleValue valueWithRawValue:rawValue];
        }
        return stops;
    }

    // Converts mbgl interval stop categorical values to an equivilant object for mgl
    class CategoricalValueVisitor {
    public:
        id operator()(const bool value) {
            return toMGLRawStyleValue(value);
        }

        id operator()(const int64_t value) {
            return toMGLRawStyleValue(value);
        }

        id operator()(const std::string value) {
            return toMGLRawStyleValue(value);
        }
    };

    // Converts all types of mbgl property values containing enumerations into an equivilant mgl style value
    template <typename MBGLEnum = MBGLType, typename MGLEnum = ObjCEnum>
    class EnumPropertyValueEvaluator {
    public:
        id operator()(const mbgl::style::Undefined) const {
            return nil;
        }

        id operator()(const MBGLEnum &value) const {
            auto str = mbgl::Enum<MBGLEnum>::toString(value);
            MGLEnum mglType = *mbgl::Enum<MGLEnum>::toEnum(str);
            return [MGLStyleConstantValue<ObjCType> valueWithRawValue:[NSValue value:&mglType withObjCType:@encode(MGLEnum)]];
        }

        id operator()(const mbgl::style::CameraFunction<MBGLEnum> &mbglValue) const {
            CameraFunctionStopsVisitor visitor;
            return apply_visitor(visitor, mbglValue.stops);
        }
    };

    // Converts all possible mbgl camera function stops into an equivilant mgl style value
    class CameraFunctionStopsVisitor {
    public:
        id operator()(const mbgl::style::ExponentialStops<MBGLType> &mbglStops) {
            return [MGLCameraStyleFunction functionWithInterpolationMode:MGLInterpolationModeExponential
                                                          stops:toConvertedStops(mbglStops.stops)
                                                        options:@{MGLStyleFunctionOptionInterpolationBase: @(mbglStops.base)}];
        }

        id operator()(const mbgl::style::IntervalStops<MBGLType> &mbglStops) {
            return [MGLCameraStyleFunction functionWithInterpolationMode:MGLInterpolationModeInterval
                                                          stops:toConvertedStops(mbglStops.stops)
                                                        options:nil];
        }
    };

    // Converts a source function and all possible mbgl source function stops into an equivilant mgl style value
    class SourceFunctionStopsVisitor {
    public:
        id operator()(const mbgl::style::ExponentialStops<MBGLType> &mbglStops) {
            MGLSourceStyleFunction *sourceFunction = [MGLSourceStyleFunction functionWithInterpolationMode:MGLInterpolationModeExponential
                                                                                            stops:toConvertedStops(mbglStops.stops)
                                                                                    attributeName:@(mbglFunction.property.c_str())
                                                                                          options:@{MGLStyleFunctionOptionInterpolationBase: @(mbglStops.base)}];
            if (mbglFunction.defaultValue) {
                sourceFunction.defaultValue = [MGLStyleValue valueWithRawValue:toMGLRawStyleValue(*mbglFunction.defaultValue)];
            }
            return sourceFunction;
        }

        id operator()(const mbgl::style::IntervalStops<MBGLType> &mbglStops) {
            MGLSourceStyleFunction *sourceFunction = [MGLSourceStyleFunction functionWithInterpolationMode:MGLInterpolationModeInterval
                                                                                            stops:toConvertedStops(mbglStops.stops)
                                                                                    attributeName:@(mbglFunction.property.c_str())
                                                                                          options:nil];
            if (mbglFunction.defaultValue) {
                sourceFunction.defaultValue = [MGLStyleValue valueWithRawValue:toMGLRawStyleValue(*mbglFunction.defaultValue)];
            }
            return sourceFunction;
        }

        id operator()(const mbgl::style::CategoricalStops<MBGLType> &mbglStops) {
            NSMutableDictionary *stops = [NSMutableDictionary dictionaryWithCapacity:mbglStops.stops.size()];
            for (const auto &mbglStop : mbglStops.stops) {
                auto categoricalValue = mbglStop.first;
                auto rawValue = toMGLRawStyleValue(mbglStop.second);
                CategoricalValueVisitor categoricalValueVisitor;
                id stopKey = apply_visitor(categoricalValueVisitor, categoricalValue);
                stops[stopKey] = [MGLStyleValue valueWithRawValue:rawValue];
            }

            MGLSourceStyleFunction *sourceFunction = [MGLSourceStyleFunction functionWithInterpolationMode:MGLInterpolationModeCategorical
                                                                                            stops:stops
                                                                                    attributeName:@(mbglFunction.property.c_str())
                                                                                          options:nil];
            if (mbglFunction.defaultValue) {
                sourceFunction.defaultValue = [MGLStyleValue valueWithRawValue:toMGLRawStyleValue(*mbglFunction.defaultValue)];
            }
            return sourceFunction;

        }

        id operator()(const mbgl::style::IdentityStops<MBGLType> &mbglStops) {
            MGLSourceStyleFunction *sourceFunction = [MGLSourceStyleFunction functionWithInterpolationMode:MGLInterpolationModeIdentity
                                                                                                     stops:nil
                                                                                             attributeName:@(mbglFunction.property.c_str()) options:nil];
            if (mbglFunction.defaultValue) {
                sourceFunction.defaultValue = [MGLStyleValue valueWithRawValue:toMGLRawStyleValue(*mbglFunction.defaultValue)];
            }
            return sourceFunction;
        }

        const mbgl::style::SourceFunction<MBGLType> &mbglFunction;
    };

    // Converts a composite function and all possible mbgl stops into an equivilant mgl style value
    class CompositeFunctionStopsVisitor {
    public:
        id operator()(const std::map<float, mbgl::style::ExponentialStops<MBGLType>> &mbglStops) {
            float base = 1.0f;
            NSMutableDictionary *stops = [NSMutableDictionary dictionaryWithCapacity:mbglStops.size()];
            for (auto const& outerStop: mbglStops) {
                stops[@(outerStop.first)] = toConvertedStops(outerStop.second.stops);
                base = outerStop.second.base;
            }
            MGLCompositeStyleFunction *compositeFunction = [MGLCompositeStyleFunction functionWithInterpolationMode:MGLInterpolationModeExponential
                                                                                                     stops:stops
                                                                                             attributeName:@(mbglFunction.property.c_str())
                                                                                                   options:@{MGLStyleFunctionOptionInterpolationBase: @(base)}];
            if (mbglFunction.defaultValue) {
                compositeFunction.defaultValue = [MGLStyleValue valueWithRawValue:toMGLRawStyleValue(*mbglFunction.defaultValue)];
            }
            return compositeFunction;
        }

        id operator()(const std::map<float, mbgl::style::IntervalStops<MBGLType>> &mbglStops) {
            NSMutableDictionary *stops = [NSMutableDictionary dictionaryWithCapacity:mbglStops.size()];
            for (auto const& outerStop: mbglStops) {
                stops[@(outerStop.first)] = toConvertedStops(outerStop.second.stops);
            }
            MGLCompositeStyleFunction *compositeFunction = [MGLCompositeStyleFunction functionWithInterpolationMode:MGLInterpolationModeInterval
                                                                                                     stops:stops
                                                                                             attributeName:@(mbglFunction.property.c_str())
                                                                                                   options:nil];
            if (mbglFunction.defaultValue) {
                compositeFunction.defaultValue = [MGLStyleValue valueWithRawValue:toMGLRawStyleValue(*mbglFunction.defaultValue)];
            }
            return compositeFunction;
        }

        id operator()(const std::map<float, mbgl::style::CategoricalStops<MBGLType>> &mbglStops) {
            NSMutableDictionary *stops = [NSMutableDictionary dictionaryWithCapacity:mbglStops.size()];
            for (auto const& outerStop: mbglStops) {
                    NSMutableDictionary *innerStops = [NSMutableDictionary dictionaryWithCapacity:outerStop.second.stops.size()];
                for (const auto &mbglStop : outerStop.second.stops) {
                    auto categoricalValue = mbglStop.first;
                    auto rawValue = toMGLRawStyleValue(mbglStop.second);
                    CategoricalValueVisitor categoricalValueVisitor;
                    id stopKey = apply_visitor(categoricalValueVisitor, categoricalValue);
                    innerStops[stopKey] = [MGLStyleValue valueWithRawValue:rawValue];
                }
                stops[@(outerStop.first)] = innerStops;
            }

            MGLCompositeStyleFunction *compositeFunction = [MGLCompositeStyleFunction functionWithInterpolationMode:MGLInterpolationModeCategorical
                                                                                                     stops:stops attributeName:@(mbglFunction.property.c_str())
                                                                                                   options:nil];
            if (mbglFunction.defaultValue) {
                compositeFunction.defaultValue = [MGLStyleValue valueWithRawValue:toMGLRawStyleValue(*mbglFunction.defaultValue)];
            }
            return compositeFunction;
        }

        const mbgl::style::CompositeFunction<MBGLType> &mbglFunction;
    };


    // Converts all types of mbgl property values that don't contain enumerations into an equivilant mgl style value
    class PropertyValueEvaluator {
    public:
        id operator()(const mbgl::style::Undefined) const {
            return nil;
        }

        id operator()(const MBGLType &value) const {
            auto rawValue = toMGLRawStyleValue(value);
            return [MGLStyleConstantValue<ObjCType> valueWithRawValue:rawValue];
        }

        id operator()(const mbgl::style::CameraFunction<MBGLType> &mbglValue) const {
            CameraFunctionStopsVisitor visitor;
            return apply_visitor(visitor, mbglValue.stops);
        }

        id operator()(const mbgl::style::SourceFunction<MBGLType> &mbglValue) const {
            SourceFunctionStopsVisitor visitor { mbglValue };
            return apply_visitor(visitor, mbglValue.stops);
        }

        MGLCompositeStyleFunction<ObjCType> * operator()(const mbgl::style::CompositeFunction<MBGLType> &mbglValue) const {
            CompositeFunctionStopsVisitor visitor { mbglValue };
            return apply_visitor(visitor, mbglValue.stops);
        }
    };
};
