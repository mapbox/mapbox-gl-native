#import <Foundation/Foundation.h>

#import "MGLStyleValue.h"

#import "NSValue+MGLStyleAttributeAdditions.h"
#import "NSValue+MGLAdditions.h"
#import "NSExpression+MGLPrivateAdditions.h"
#import "MGLTypes.h"

#import "MGLConversion.h"
#include <mbgl/style/conversion/property_value.hpp>
#include <mbgl/style/conversion/data_driven_property_value.hpp>
#include <mbgl/style/conversion/heatmap_color_property_value.hpp>
#include <mbgl/style/conversion/position.hpp>
#import <mbgl/style/types.hpp>

#import <mbgl/util/enum.hpp>

#include <mbgl/style/data_driven_property_value.hpp>

#include <mbgl/util/interpolate.hpp>

#if TARGET_OS_IPHONE
    #import "UIColor+MGLAdditions.h"
#else
    #import "NSColor+MGLAdditions.h"
#endif

namespace mbgl {
    namespace style {
        namespace expression {
            class Expression;
        }
    }
}

id MGLJSONObjectFromMBGLExpression(const mbgl::style::expression::Expression &mbglExpression);

template <typename MBGLType, typename ObjCType, typename MBGLElement = MBGLType, typename ObjCEnum = ObjCType>
class MGLStyleValueTransformer {
public:
    
    /// Convert an mbgl property value into an mgl style value
    NSExpression *toExpression(const mbgl::style::PropertyValue<MBGLType> &mbglValue) {
        PropertyExpressionEvaluator evaluator;
        return mbglValue.evaluate(evaluator);
    }
    
    /// Convert an mbgl data driven property value into an mgl style value
    template <typename MBGLEnum = MBGLType, typename MGLEnum = ObjCEnum>
    NSExpression *toExpression(const mbgl::style::DataDrivenPropertyValue<MBGLEnum> &mbglValue) {
        PropertyExpressionEvaluator evaluator;
        return mbglValue.evaluate(evaluator);
    }
    
    // Convert an mbgl heatmap color property value into an mgl style value
    NSExpression *toExpression(const mbgl::style::HeatmapColorPropertyValue &mbglValue) {
        if (mbglValue.isUndefined()) {
            return nil;
        }
        return [NSExpression mgl_expressionWithJSONObject:MGLJSONObjectFromMBGLExpression(mbglValue.getExpression())];
    }

    /**
     Converts an NSExpression to an mbgl property value.
     */
    template <typename MBGLValue>
    typename std::enable_if_t<!std::is_same<MBGLValue, mbgl::style::HeatmapColorPropertyValue>::value,
    MBGLValue> toPropertyValue(NSExpression *expression) {
        if (!expression) {
            return {};
        }
        
        if (expression.expressionType == NSConstantValueExpressionType) {
            MBGLType mbglValue;
            getMBGLValue(expression.constantValue, mbglValue);
            return mbglValue;
        }
        if (expression.expressionType == NSAggregateExpressionType) {
            MBGLType mbglValue;
            getMBGLValue(expression.collection, mbglValue);
            return mbglValue;
        }
        
        NSArray *jsonExpression = expression.mgl_jsonExpressionObject;
        
        mbgl::style::conversion::Error valueError;
        auto value = mbgl::style::conversion::convert<MBGLValue>(
            mbgl::style::conversion::makeConvertible(jsonExpression), valueError);
        if (!value) {
            [NSException raise:NSInvalidArgumentException
                        format:@"Invalid property value: %@", @(valueError.message.c_str())];
            return {};
        }
        
        return *value;
    }
    
    /**
     Converts an NSExpression to an mbgl property value.
     */
    template <typename MBGLValue>
    typename std::enable_if_t<std::is_same<MBGLValue, mbgl::style::HeatmapColorPropertyValue>::value,
    MBGLValue> toPropertyValue(NSExpression *expression) {
        if (!expression) {
            return {};
        }
        
        NSArray *jsonExpression = expression.mgl_jsonExpressionObject;
        
        mbgl::style::conversion::Error valueError;
        auto value = mbgl::style::conversion::convert<mbgl::style::HeatmapColorPropertyValue>(
            mbgl::style::conversion::makeConvertible(jsonExpression), valueError);
        if (!value) {
            [NSException raise:NSInvalidArgumentException
                        format:@"Invalid property value: %@", @(valueError.message.c_str())];
            return {};
        }
        
        return *value;
    }

private: // Private utilities for converting from mgl to mbgl values
    
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
    void getMBGLValue(id rawValue, std::array<float, 2> &mbglValue) {
        if ([rawValue isKindOfClass:[NSValue class]]) {
            mbglValue = [rawValue mgl_offsetArrayValue];
        } else if ([rawValue isKindOfClass:[NSArray class]]) {
            NSArray *array = (NSArray *)rawValue;
            getMBGLValue(array[0], mbglValue[0]);
            getMBGLValue(array[1], mbglValue[1]);
        }
    }

    // Padding
    void getMBGLValue(id rawValue, std::array<float, 4> &mbglValue) {
        if ([rawValue isKindOfClass:[NSValue class]]) {
            mbglValue = [rawValue mgl_paddingArrayValue];
        } else if ([rawValue isKindOfClass:[NSArray class]]) {
            NSArray *array = (NSArray *)rawValue;
            getMBGLValue(array[0], mbglValue[0]);
            getMBGLValue(array[1], mbglValue[1]);
            getMBGLValue(array[2], mbglValue[2]);
            getMBGLValue(array[3], mbglValue[3]);
            getMBGLValue(array[4], mbglValue[4]);
        }
    }

    // Color
    void getMBGLValue(MGLColor *rawValue, mbgl::Color &mbglValue) {
        mbglValue = rawValue.mgl_color;
    }

    // Array
    void getMBGLValue(ObjCType rawValue, std::vector<MBGLElement> &mbglValue) {
        mbglValue.reserve(rawValue.count);
        for (id obj in rawValue) {
            id constantObject = obj;
            if ([obj isKindOfClass:[NSExpression class]] && [obj expressionType] == NSConstantValueExpressionType) {
                constantObject = [constantObject constantValue];
            }
            MBGLElement mbglElement;
            getMBGLValue(constantObject, mbglElement);
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
    void getMBGLValue(id rawValue, MBGLEnum &mbglValue) {
        if ([rawValue isKindOfClass:[NSString class]]) {
            mbglValue = *mbgl::Enum<MBGLEnum>::toEnum([(NSString *)rawValue UTF8String]);
        } else {
            MGLEnum mglEnum;
            [(NSValue *)rawValue getValue:&mglEnum];
            auto str = mbgl::Enum<MGLEnum>::toString(mglEnum);
            mbglValue = *mbgl::Enum<MBGLEnum>::toEnum(str);
        }
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
            [array addObject:[NSExpression expressionForConstantValue:toMGLRawStyleValue(mbglElement)]];
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

    /// Converts all types of mbgl property values into an equivalent NSExpression.
    class PropertyExpressionEvaluator {
    public:
        NSExpression *operator()(const mbgl::style::Undefined) const {
            return nil;
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
        NSExpression *operator()(const MBGLType &value) const {
            id constantValue = toMGLRawStyleValue(value);
            if ([constantValue isKindOfClass:[NSArray class]]) {
                return [NSExpression expressionForAggregate:constantValue];
            }
            return [NSExpression expressionForConstantValue:constantValue];
        }
        
        template <typename MBGLEnum = MBGLType,
            class = typename std::enable_if<std::is_enum<MBGLEnum>::value>::type,
        typename MGLEnum = ObjCEnum,
            class = typename std::enable_if<std::is_enum<MGLEnum>::value>::type>
        NSExpression *operator()(const MBGLEnum &value) const {
            NSString *constantValue = @(mbgl::Enum<MBGLEnum>::toString(value));
            return [NSExpression expressionForConstantValue:constantValue];
        }

        NSExpression *operator()(const mbgl::style::CameraFunction<MBGLType> &mbglValue) const {
            return [NSExpression mgl_expressionWithJSONObject:MGLJSONObjectFromMBGLExpression(mbglValue.getExpression())];
        }

        NSExpression *operator()(const mbgl::style::SourceFunction<MBGLType> &mbglValue) const {
            return [NSExpression mgl_expressionWithJSONObject:MGLJSONObjectFromMBGLExpression(mbglValue.getExpression())];
        }

        NSExpression *operator()(const mbgl::style::CompositeFunction<MBGLType> &mbglValue) const {
            return [NSExpression mgl_expressionWithJSONObject:MGLJSONObjectFromMBGLExpression(mbglValue.getExpression())];
        }
    };
};
