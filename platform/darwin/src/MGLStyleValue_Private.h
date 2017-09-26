#import <Foundation/Foundation.h>

#import "MGLStyleValue.h"

#import "NSValue+MGLStyleAttributeAdditions.h"
#import "NSValue+MGLAdditions.h"
#import "MGLTypes.h"

#import "MGLConversion.h"
#include <mbgl/style/conversion/data_driven_property_value.hpp>
#include <mbgl/style/conversion.hpp>
#import <mbgl/style/types.hpp>

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
                        format:@"This property can only be set to camera functions. Use +[MGLStyleValue valueWithInterpolationMode:cameraStops:options:] instead."];
            return {};
        }

        if ([value isKindOfClass:[MGLConstantStyleValue class]]) {
            return toMBGLConstantValue((MGLConstantStyleValue<ObjCType> *)value);
        } else if ([value isKindOfClass:[MGLCameraStyleFunction class]]) {
            MGLCameraStyleFunction<ObjCType> *cameraStyleFunction = (MGLCameraStyleFunction<ObjCType> *)value;
            // Intentionally ignore the stop type set by the developer becuase non interpolatable property values
            // can only have interval stops. This also allows for backwards compatiblity when the developer uses
            // a deprecated MGLStyleValue method (that used to create an MGLStyleFunction) to create a function
            // for properties that are piecewise-constant (i.e. enum, bool, string)
            return toMBGLIntervalCameraFunction(cameraStyleFunction);
        } else if ([value isMemberOfClass:[MGLStyleFunction class]]) {
            MGLStyleFunction<ObjCType> *styleFunction = (MGLStyleFunction<ObjCType> *)value;
            return toMBGLIntervalCameraFunction(styleFunction);
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
                        format:@"This property can only be set to camera functions. Use +[MGLStyleValue valueWithInterpolationMode:cameraStops:options:] instead."];
            return {};
        }

        if ([value isKindOfClass:[MGLConstantStyleValue class]]) {
            return toMBGLConstantValue((MGLConstantStyleValue<ObjCType> *)value);
        } else if ([value isMemberOfClass:[MGLStyleFunction class]]) {
            MGLStyleFunction<ObjCType> *styleFunction = (MGLStyleFunction<ObjCType> *)value;
            return toMBGLExponentialCameraFunction(styleFunction);
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
        if ([value isKindOfClass:[MGLConstantStyleValue class]]) {
            return toMBGLConstantValue((MGLConstantStyleValue<ObjCType> *)value);
        } else if ([value isKindOfClass:[MGLStyleFunction class]]) {
            mbgl::style::conversion::Error error;
            auto result = mbgl::style::conversion::convert<mbgl::style::DataDrivenPropertyValue<MBGLType>>(
                mbgl::style::conversion::makeValue(toRawStyleSpecValue((MGLStyleFunction<ObjCType> *) value)), error);
            NSCAssert(result, @(error.message.c_str()));
            return *result;
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
                        format:@"This property can only be set to camera functions. Use +[MGLStyleValue valueWithInterpolationMode:cameraStops:options:] instead."];
            return {};
        }

        if ([value isKindOfClass:[MGLConstantStyleValue class]]) {
            MBGLEnum mbglValue;
            getMBGLValue([(MGLConstantStyleValue<ObjCType> *)value rawValue], mbglValue);
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

    MBGLType toMBGLConstantValue(MGLConstantStyleValue<ObjCType> *value) {
        MBGLType mbglValue;
        getMBGLValue(value.rawValue, mbglValue);
        return mbglValue;
    }
    
    /**
     As hack to allow converting enum => string values, we accept a second, dummy parameter in
     the toRawStyleSpecValue() methods for converting 'atomic' (non-style-function) values.
     This allows us to use `std::enable_if` to test (at compile time) whether or not MBGLType is an Enum.
     */
    template <typename MBGLEnum = MBGLType,
    class = typename std::enable_if<!std::is_enum<MBGLEnum>::value>::type,
    typename MGLEnum = ObjCEnum,
    class = typename std::enable_if<!std::is_enum<MGLEnum>::value>::type>
    NSObject* toRawStyleSpecValue(NSObject *rawMGLValue, MBGLEnum &mbglValue) {
        if ([rawMGLValue isKindOfClass:[NSValue class]]) {
            const auto rawNSValue = (NSValue *)rawMGLValue;
            if (strcmp([rawNSValue objCType], @encode(CGVector)) == 0) {
                // offset [x, y]
                std::array<float, 2> mglValue = rawNSValue.mgl_offsetArrayValue;
                return [NSArray arrayWithObjects:@(mglValue[0]), @(mglValue[1]), nil];
            }
        }
        // noop pass-through plain NSObject-based items
        return rawMGLValue;
    }
    
    template <typename MBGLEnum = MBGLType,
    class = typename std::enable_if<std::is_enum<MBGLEnum>::value>::type,
    typename MGLEnum = ObjCEnum,
    class = typename std::enable_if<std::is_enum<MGLEnum>::value>::type>
    NSString* toRawStyleSpecValue(ObjCType rawValue, MBGLEnum &mbglValue) {
        MGLEnum mglEnum;
        [rawValue getValue:&mglEnum];
        return @(mbgl::Enum<MGLEnum>::toString(mglEnum));
    }
    
    NSObject* toRawStyleSpecValue(MGLColor *color, MBGLType &mbglValue) {
        return @(color.mgl_color.stringify().c_str());
    }

    
    NSObject* toRawStyleSpecValue(MGLStyleFunction<ObjCType>* styleFunction) {
        NSMutableDictionary * rawFunction = [NSMutableDictionary new];
        // interpolationMode => type
        switch (styleFunction.interpolationMode) {
            case MGLInterpolationModeExponential:
                rawFunction[@"type"] = @"exponential";
                break;
            case MGLInterpolationModeInterval:
                rawFunction[@"type"] = @"interval";
                break;
            case MGLInterpolationModeCategorical:
                rawFunction[@"type"] = @"categorical";
                break;
            case MGLInterpolationModeIdentity:
                rawFunction[@"type"] = @"identity";
                break;
        }
        
        // interpolationBase => base
        if (styleFunction.interpolationBase) {
            rawFunction[@"base"] = @(styleFunction.interpolationBase);
        }
        
        // stops and default value
        if ([styleFunction isKindOfClass:[MGLCameraStyleFunction class]]) {
            // zoom-only function (no default value)
            __block NSMutableArray *stops = [[NSMutableArray alloc] init];
            [styleFunction.stops enumerateKeysAndObjectsUsingBlock:^(NSNumber * _Nonnull zoomKey, MGLConstantStyleValue<ObjCType> * _Nonnull outputValue, BOOL * _Nonnull stop) {
                MBGLType dummyMbglValue;
                NSArray *rawStop = @[zoomKey, toRawStyleSpecValue([outputValue rawValue], dummyMbglValue)];
                [stops addObject:rawStop];
            }];
            rawFunction[@"stops"] = stops;

        } else if ([styleFunction isKindOfClass:[MGLSourceStyleFunction class]]) {
            auto sourceStyleFunction = (MGLSourceStyleFunction<ObjCType> *)styleFunction;
            rawFunction[@"property"] = sourceStyleFunction.attributeName;
            // property-only function
            __block NSMutableArray *stops = [[NSMutableArray alloc] init];
            [styleFunction.stops enumerateKeysAndObjectsUsingBlock:^(NSObject * _Nonnull propertyKey, MGLConstantStyleValue<ObjCType> * _Nonnull outputValue, BOOL * _Nonnull stop) {
                MBGLType dummyMbglValue;
                NSArray *rawStop = @[propertyKey, toRawStyleSpecValue([outputValue rawValue], dummyMbglValue)];
                [stops addObject:rawStop];
            }];
            rawFunction[@"stops"] = stops;
            
            // defaultValue => default
            if (sourceStyleFunction.defaultValue) {
                NSCAssert([sourceStyleFunction.defaultValue isKindOfClass:[MGLConstantStyleValue class]], @"Default value must be constant");
                MBGLType dummyMbglValue;
                rawFunction[@"default"] = toRawStyleSpecValue([(MGLConstantStyleValue<ObjCType> *)sourceStyleFunction.defaultValue rawValue], dummyMbglValue);
            }
        } else if ([styleFunction isKindOfClass:[MGLCompositeStyleFunction class]]) {
            // zoom-and-property function
            auto compositeStyleFunction = (MGLCompositeStyleFunction<ObjCType> *)styleFunction;
            rawFunction[@"property"] = compositeStyleFunction.attributeName;
            
            __block NSMutableArray *stops = [[NSMutableArray alloc] init];
            [compositeStyleFunction.stops enumerateKeysAndObjectsUsingBlock:^(NSNumber * _Nonnull zoomKey, NSDictionary * _Nonnull stopValue, BOOL * _Nonnull stop) {
                for (NSObject *valueKey in stopValue.allKeys) {
                    NSDictionary *stopKey = @{
                         @"zoom": zoomKey,
                         @"value": valueKey
                     };
                    MGLConstantStyleValue<ObjCType> *outputValue = stopValue[valueKey];
                    NSCAssert([outputValue isKindOfClass:[MGLConstantStyleValue<ObjCType> class]], @"Stop outputs should be MGLConstantStyleValues");
                    MBGLType dummyMbglValue;
                    NSArray *rawStop = @[stopKey, toRawStyleSpecValue([outputValue rawValue], dummyMbglValue)];
                    [stops addObject:rawStop];
                }
            }];
            rawFunction[@"stops"] = stops;
            
            // defaultValue => default
            if (compositeStyleFunction.defaultValue) {
                NSCAssert([compositeStyleFunction.defaultValue isKindOfClass:[MGLConstantStyleValue class]], @"Default value must be constant");
                MBGLType dummyMbglValue;
                rawFunction[@"default"] = toRawStyleSpecValue([(MGLConstantStyleValue<ObjCType> *)compositeStyleFunction.defaultValue rawValue], dummyMbglValue);
            }
        }
        
        return rawFunction;
    }

    mbgl::style::CameraFunction<MBGLType> toMBGLExponentialCameraFunction(MGLStyleFunction<ObjCType> *styleFunction) {
        __block std::map<float, MBGLType> stops = {};
        [styleFunction.stops enumerateKeysAndObjectsUsingBlock:^(NSNumber * _Nonnull zoomKey, MGLStyleValue<ObjCType> * _Nonnull stopValue, BOOL * _Nonnull stop) {
            NSCAssert([stopValue isKindOfClass:[MGLStyleValue class]], @"Stops should be MGLStyleValues");
            auto mbglStopValue = toPropertyValue(stopValue);
            NSCAssert(mbglStopValue.isConstant(), @"Stops must be constant");
            stops[zoomKey.floatValue] = mbglStopValue.asConstant();
        }];

        // Camera function with Exponential stops
        mbgl::style::ExponentialStops<MBGLType> exponentialStops = {stops, (float)styleFunction.interpolationBase};
        mbgl::style::CameraFunction<MBGLType> cameraFunction = {exponentialStops};

        return cameraFunction;
    }

    mbgl::style::CameraFunction<MBGLType> toMBGLIntervalCameraFunction(MGLStyleFunction<ObjCType> *styleFunction) {
        __block std::map<float, MBGLType> stops = {};
        [styleFunction.stops enumerateKeysAndObjectsUsingBlock:^(NSNumber * _Nonnull zoomKey, MGLStyleValue<ObjCType> * _Nonnull stopValue, BOOL * _Nonnull stop) {
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

    void setDefaultMBGLValue(MGLSourceStyleFunction<ObjCType> *sourceStyleFunction, mbgl::style::SourceFunction<MBGLType> &sourceFunction) {
        if (sourceStyleFunction.defaultValue) {
            NSCAssert([sourceStyleFunction.defaultValue isKindOfClass:[MGLConstantStyleValue class]], @"Default value must be constant");
            MBGLType mbglValue;
            id mglValue = [(MGLConstantStyleValue<ObjCType> *)sourceStyleFunction.defaultValue rawValue];
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
    
    void getMBGLValue(NSValue *rawValue, mbgl::style::Position &mbglValue) {
        auto spherical = rawValue.mgl_lightPositionArrayValue;
        mbgl::style::Position position(spherical);
        mbglValue = position;
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
    
    static NSValue *toMGLRawStyleValue(const mbgl::style::Position &mbglStopValue) {
        std::array<float, 3> spherical = mbglStopValue.getSpherical();
        MGLSphericalPosition position = MGLSphericalPositionMake(spherical[0], spherical[1], spherical[2]);
        return [NSValue valueWithMGLSphericalPosition:position];
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
            return [MGLConstantStyleValue<ObjCType> valueWithRawValue:[NSValue value:&mglType withObjCType:@encode(MGLEnum)]];
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
        id operator()(const mbgl::style::CompositeExponentialStops<MBGLType> &mbglStops) {
            NSMutableDictionary *stops = [NSMutableDictionary dictionaryWithCapacity:mbglStops.stops.size()];
            for (auto const& outerStop: mbglStops.stops) {
                stops[@(outerStop.first)] = toConvertedStops(outerStop.second);
            }
            MGLCompositeStyleFunction *compositeFunction = [MGLCompositeStyleFunction functionWithInterpolationMode:MGLInterpolationModeExponential
                                                                                                     stops:stops
                                                                                             attributeName:@(mbglFunction.property.c_str())
                                                                                                   options:@{MGLStyleFunctionOptionInterpolationBase: @(mbglStops.base)}];
            if (mbglFunction.defaultValue) {
                compositeFunction.defaultValue = [MGLStyleValue valueWithRawValue:toMGLRawStyleValue(*mbglFunction.defaultValue)];
            }
            return compositeFunction;
        }

        id operator()(const mbgl::style::CompositeIntervalStops<MBGLType> &mbglStops) {
            NSMutableDictionary *stops = [NSMutableDictionary dictionaryWithCapacity:mbglStops.stops.size()];
            for (auto const& outerStop: mbglStops.stops) {
                stops[@(outerStop.first)] = toConvertedStops(outerStop.second);
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

        id operator()(const mbgl::style::CompositeCategoricalStops<MBGLType> &mbglStops) {
            NSMutableDictionary *stops = [NSMutableDictionary dictionaryWithCapacity:mbglStops.stops.size()];
            for (auto const& outerStop: mbglStops.stops) {
                    NSMutableDictionary *innerStops = [NSMutableDictionary dictionaryWithCapacity:outerStop.second.size()];
                for (const auto &mbglStop : outerStop.second) {
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
            return [MGLConstantStyleValue<ObjCType> valueWithRawValue:rawValue];
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
