#import <Foundation/Foundation.h>

#include <mbgl/util/logging.hpp>
#include <mbgl/style/conversion.hpp>
#include <mbgl/util/feature.hpp>
#include <mbgl/util/optional.hpp>

NS_ASSUME_NONNULL_BEGIN

namespace mbgl {
namespace style {
namespace conversion {

/**
 A minimal wrapper class conforming to the requirements for `objectMember(v, name)` (see mbgl/style/conversion.hpp)
 This is necessary because using `NSObject*` as the value type in `optional<NSObject*>` causes problems for the ARC,
 due to things like `optional(const value_type& __v)`
 */
class OptionalNSObjectValue {
public:
    OptionalNSObjectValue(NSObject * _Nullable _value) : value(_value) {}
    
    explicit operator bool() const {
        return value;
    }
    
    NSObject * _Nullable operator*() {
        NSCAssert(this, @"Expected non-null value.");
        return value;
    }
private:
    NSObject * _Nullable value;
};

inline bool isUndefined(const id value) {
    return !value || value == [NSNull null];
}

inline bool isArray(const id value) {
    return [value isKindOfClass:[NSArray class]];
}

inline bool isObject(const id value) {
    return [value isKindOfClass:[NSDictionary class]];
}

inline std::size_t arrayLength(const id value) {
    NSCAssert([value isKindOfClass:[NSArray class]], @"Value must be an NSArray for getLength().");
    NSArray *array = value;
    auto length = [array count];
    NSCAssert(length <= std::numeric_limits<size_t>::max(), @"Array length out of bounds.");
    return length;
}

inline NSObject *arrayMember(const id value, std::size_t i) {
    NSCAssert([value isKindOfClass:[NSArray class]], @"Value must be an NSArray for get(int).");
    NSCAssert(i < NSUIntegerMax, @"Index must be less than NSUIntegerMax");
    return [value objectAtIndex: i];
}

inline OptionalNSObjectValue objectMember(const id value, const char *key) {
    NSCAssert([value isKindOfClass:[NSDictionary class]], @"Value must be an NSDictionary for get(string).");
    NSObject *member = [value objectForKey: @(key)];
    if (member && member != [NSNull null]) {
        return { member };
    } else {
        return { nullptr };
    }
}

// Not implemented (unneeded for MGLStyleFunction conversion):
// optional<Error> eachMember(const NSObject*, Fn&&)

inline bool _isBool(const id value) {
    if (![value isKindOfClass:[NSNumber class]]) return false;
    // char: 32-bit boolean
    // BOOL: 64-bit boolean
    NSNumber *number = value;
    return ((strcmp([number objCType], @encode(char)) == 0) ||
            (strcmp([number objCType], @encode(BOOL)) == 0));
}
    
inline bool _isNumber(const id value) {
    return [value isKindOfClass:[NSNumber class]] && !_isBool(value);
}
    
inline bool _isString(const id value) {
    return [value isKindOfClass:[NSString class]];
}

inline optional<bool> toBool(const id value) {
    if (_isBool(value)) {
        return ((NSNumber *)value).boolValue;
    } else {
        return {};
    }
}

inline optional<float> toNumber(const id value) {
    if (_isNumber(value)) {
        return ((NSNumber *)value).floatValue;
    } else {
        return {};
    }
}

inline optional<std::string> toString(const id value) {
    if (_isString(value)) {
        return std::string(static_cast<const char *>([value UTF8String]));
    } else {
        return {};
    }
}

inline optional<mbgl::Value> toValue(const id value) {
    if (isUndefined(value)) {
        return {};
    } else if (_isBool(value)) {
        return { *toBool(value) };
    } else if ( _isString(value)) {
        return { *toString(value) };
    } else if (_isNumber(value)) {
        // Need to cast to a double here as the float is otherwise considered a bool...
       return { static_cast<double>(*toNumber(value)) };
    } else {
        return {};
    }
}

} // namespace conversion
} // namespace style
} // namespace mbgl

NS_ASSUME_NONNULL_END

