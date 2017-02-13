#include <string>

class MGLConversionValue {
public:
    MGLConversionValue(NSObject* _value) : value(_value) {}
    
    bool isNull() const {
        return !value || value == [NSNull null];
    }
    
    bool isArray() const {
        return [value isKindOfClass:[NSArray class]];
    }
    
    bool isObject() const {
        return [value isKindOfClass:[NSDictionary class]];
    }
    
    bool isString() const {
        return [value isKindOfClass:[NSString class]];
    }
    
    bool isBool() const {
        if (![value isKindOfClass:[NSNumber class]]) return false;
        // char: 32-bit boolean
        // BOOL: 64-bit boolean
        auto number = (NSNumber *)value;
        return ((strcmp([number objCType], @encode(char)) == 0) ||
                (strcmp([number objCType], @encode(BOOL)) == 0));
    }
    
    bool isNumber() const {
        return [value isKindOfClass:[NSNumber class]] && !isBool();
    }
    
    std::string toString() const {
        return std::string([((NSString *)value) UTF8String]);
    };
    
    double toNumber() const {
        return ((NSNumber *)value).doubleValue;
    };
    
    bool toBool() const {
        return ((NSNumber *)value).boolValue;
    };
    
    MGLConversionValue get(const char* key) const {
        NSCAssert([value isKindOfClass:[NSDictionary class]], @"Value must be an NSDictionary for get(string).");
        auto dict = (NSDictionary *)value;
        return { dict[@(key)] };
    };
    
    size_t getLength() const {
        NSCAssert([value isKindOfClass:[NSDictionary class]], @"Value must be an NSArray for getLength().");
        NSArray * array = (NSArray *)value;
        return [array count];
    };
    
    MGLConversionValue get(const std::size_t index ) const {
        NSCAssert([value isKindOfClass:[NSDictionary class]], @"Value must be an NSArray for get(int).");
        NSArray * array = (NSArray *)value;
        return { array[index] };
    };
    
private:
    NSObject * value;
};
