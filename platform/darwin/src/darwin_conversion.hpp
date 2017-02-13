#pragma once

#include <mbgl/util/logging.hpp>
#include <mbgl/style/conversion.hpp>
#include <mbgl/util/feature.hpp>
#include <mbgl/util/optional.hpp>

namespace mbgl {
namespace style {
namespace conversion {

// A minimal wrapper class conforming to the requirements for `objectMember(v, name)` (see mbgl/style/conversion.hpp)
// This is necessary because using `NSObject*` as the value type in `optional<NSObject*>` causes problems for the ARC,
// due to things like `optional(const value_type& __v)`
class OptionalNSObjectValue {
public:
    OptionalNSObjectValue(NSObject *_value) : value(_value) {}
    OptionalNSObjectValue() {}
    
    explicit operator bool() const {
        return value;
    }
    
    NSObject* operator*() {
        assert(this);
        return value;
    }
private:
    NSObject* value;
};

inline bool isUndefined(const NSObject* value) {
    return !value || value == [NSNull null];
}

inline bool isArray(const NSObject* value) {
    return [value isKindOfClass:[NSArray class]];
}

inline bool isObject(const NSObject* value) {
    return [value isKindOfClass:[NSDictionary class]];
}

inline std::size_t arrayLength(const NSObject* value) {
    NSCAssert([value isKindOfClass:[NSArray class]], @"Value must be an NSArray for getLength().");
    NSArray * array = (NSArray *)value;
    return [array count];
}

inline NSObject* arrayMember(const NSObject* value, std::size_t i) {
    NSCAssert([value isKindOfClass:[NSArray class]], @"Value must be an NSArray for get(int).");
    NSArray * array = (NSArray *)value;
    return array[i];
}

inline OptionalNSObjectValue objectMember(const NSObject* value, const char* key) {
    NSCAssert([value isKindOfClass:[NSDictionary class]], @"Value must be an NSDictionary for get(string).");
    auto dict = (NSDictionary *)value;
    NSObject *member = dict[@(key)];

    if (value && value != [NSNull null]) {
        return { member };
    } else {
        return {};
    }
}

template <class Fn>
optional<Error> eachMember(const NSObject*, Fn&&) {
    mbgl::Log::Warning(mbgl::Event::General, "eachMember not implemented");
    return {};
}

inline bool _isBool(const NSObject* value) {
    if (![value isKindOfClass:[NSNumber class]]) return false;
    // char: 32-bit boolean
    // BOOL: 64-bit boolean
    auto number = (NSNumber *)value;
    return ((strcmp([number objCType], @encode(char)) == 0) ||
            (strcmp([number objCType], @encode(BOOL)) == 0));
}
    
inline bool _isNumber(const NSObject* value) {
    return [value isKindOfClass:[NSNumber class]] && !_isBool(value);
}
    
inline bool _isString(const NSObject* value) {
    return [value isKindOfClass:[NSString class]];
}

inline optional<bool> toBool(const NSObject* value) {
    if (_isBool(value)) {
        return ((NSNumber *)value).boolValue;
    } else {
        return {};
    }
}

inline optional<float> toNumber(const NSObject* value) {
    if (_isNumber(value)) {
        return ((NSNumber *)value).floatValue;
    } else {
        return {};
    }
}

inline optional<std::string> toString(const NSObject* value) {
    if (_isString(value)) {
        return std::string([((NSString *)value) UTF8String]);
    } else {
        return {};
    }
}

inline optional<mbgl::Value> toValue(const NSObject* value) {
    if (isUndefined(value)) {
        return {};
    } else if (_isBool(value)) {
        return { *toBool(value) };
    } else if ( _isString(value)) {
        return { *toString(value) };
    } else if (_isNumber(value)) {
        // Need to cast to a double here as the float is otherwise considered a bool...
       return { (double) *toNumber(value) };
    } else {
        return {};
    }
}

} // namespace conversion
} // namespace style
} // namespace mbgl
