#import <Foundation/Foundation.h>

#include "MGLConversion.h"

NS_ASSUME_NONNULL_BEGIN

namespace mbgl {
namespace style {
namespace conversion {

class Holder {
public:
    Holder(const id v) : value(v) {}
    const id value;
};

static Holder& cast(Storage& storage) {
    return *static_cast<Holder*>(static_cast<void*>(&storage));
}

static const Holder& cast(const Storage& storage) {
    return *static_cast<const Holder*>(static_cast<const void*>(&storage));
}

static void destroy(Storage& storage) {
    Holder& value = cast(storage);
    value.~Holder();
}

static void move(Storage&& src, Storage& dest) {
    new (static_cast<void*>(&dest)) Holder(std::move(cast(src)));
    destroy(src);
}

static bool isUndefined(const Storage& storage) {
    const id value = cast(storage).value;
    return !value || value == [NSNull null];
}

static bool isArray(const Storage& storage) {
    const id value = cast(storage).value;
    return [value isKindOfClass:[NSArray class]];
}

static bool isObject(const Storage& storage) {
    const id value = cast(storage).value;
    return [value isKindOfClass:[NSDictionary class]];
}

static std::size_t arrayLength(const Storage& storage) {
    const id value = cast(storage).value;
    NSCAssert([value isKindOfClass:[NSArray class]], @"Value must be an NSArray for getLength().");
    NSArray *array = value;
    auto length = [array count];
    NSCAssert(length <= std::numeric_limits<size_t>::max(), @"Array length out of bounds.");
    return length;
}

static Value arrayMember(const Storage& storage, std::size_t i) {
    const id value = cast(storage).value;
    NSCAssert([value isKindOfClass:[NSArray class]], @"Value must be an NSArray for get(int).");
    NSCAssert(i < NSUIntegerMax, @"Index must be less than NSUIntegerMax");
    return makeValue([value objectAtIndex: i]);
}

static optional<Value> objectMember(const Storage& storage, const char *key) {
    const id value = cast(storage).value;
    NSCAssert([value isKindOfClass:[NSDictionary class]], @"Value must be an NSDictionary for get(string).");
    NSObject *member = [value objectForKey: @(key)];
    if (member && member != [NSNull null]) {
        return makeValue(member);
    } else {
        return optional<Value>();
    }
}

static optional<Error> eachMember(const Storage& storage, const std::function<optional<Error> (const std::string&, const Value&)>& fn) {
    // Not implemented (unneeded for MGLStyleFunction conversion).
    NSCAssert(NO, @"eachMember not implemented");
    return {};
}

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

static optional<bool> toBool(const Storage& storage) {
    const id value = cast(storage).value;
    if (_isBool(value)) {
        return ((NSNumber *)value).boolValue;
    } else {
        return {};
    }
}

static optional<float> toNumber(const Storage& storage) {
    const id value = cast(storage).value;
    if (_isNumber(value)) {
        return ((NSNumber *)value).floatValue;
    } else {
        return {};
    }
}

static optional<double> toDouble(const Storage& storage) {
    const id value = cast(storage).value;
    if (_isNumber(value)) {
        return ((NSNumber *)value).doubleValue;
    } else {
        return {};
    }
}

static optional<std::string> toString(const Storage& storage) {
    const id value = cast(storage).value;
    if (_isString(value)) {
        return std::string(static_cast<const char *>([value UTF8String]));
    } else {
        return {};
    }
}

static optional<mbgl::Value> toValue(const Storage& storage) {
    const id value = cast(storage).value;
    if (isUndefined(storage)) {
        return {};
    } else if (_isBool(value)) {
        return { *toBool(storage) };
    } else if ( _isString(value)) {
        return { *toString(storage) };
    } else if (_isNumber(value)) {
        // Need to cast to a double here as the float is otherwise considered a bool...
       return { static_cast<double>(*toNumber(storage)) };
    } else {
        return {};
    }
}

static optional<GeoJSON> toGeoJSON(const Storage& storage, Error& error) {
    error = { "toGeoJSON not implemented" };
    return {};
}

Value makeValue(const id value) {
    static Value::VTable vtable = {
        move,
        destroy,
        isUndefined,
        isArray,
        arrayLength,
        arrayMember,
        isObject,
        objectMember,
        eachMember,
        toBool,
        toNumber,
        toDouble,
        toString,
        toValue,
        toGeoJSON
    };

    Storage storage;
    new (static_cast<void*>(&storage)) Holder(value);
    return Value(&vtable, std::move(storage));
}

} // namespace conversion
} // namespace style
} // namespace mbgl

NS_ASSUME_NONNULL_END

