
class MGLConversionValue {
public:
    MGLConversionValue(NSObject* _value) : value(_value) {}
    
    bool isNull() const {
        return !value || value == [NSNull null]];
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
        // char: 32-bit boolean
        // BOOL: 64-bit boolean
        return [value isKindOfClass:[NSNumber class]] &&
                ((strcmp([value objCType], @encode(char)) == 0) ||
                (strcmp([value objCType], @encode(BOOL)) == 0));
    }
    
    bool isNumber() const {
        return [value isKindOfClass:[NSNumber class]] && !isBool();
    }
    
    std::string toString() const {
        return std::string([(NSString *)value UTF8String]);
    };
    
    double toNumber() const {
        return ((NSNumber *)value).doubleValue;
    };
    
    bool toBool() const {
        return ((NSNumber *)value).boolValue;
    };
    
    MGLConversionValue get(const char* key) const {
        NSAssert([value isKindOfClass: NSDictionary], @"Value must be an NSDictionary for get(string).");
        NSDictionary *dict = (NSDictionary *)value;
        return { value[@key] };
    };
    
    size_t getLength() const {
        NSAssert([value isKindOfClass: NSDictionary], @"Value must be an NSArray for getLength().");
        NSArray * array = (NSArray *)value;
        return [size count];
    };
    
    MGLConversionValue get(const int index ) const {
        NSAssert([value isKindOfClass: NSDictionary], @"Value must be an NSArray for get(int).");
        NSArray * array = (NSArray *)value;
        return { array[index] };
    };
    
private:
    NSObject * value;
};
