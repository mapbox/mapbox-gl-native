//
//  MGLConversionValue.h
//  ios
//
//  Created by Anand Thakker on 2/10/17.
//  Copyright © 2017 Mapbox. All rights reserved.
//

#ifndef MGLConversionValue_h
#define MGLConversionValue_h

class MGLConversionValue {
public:
    Value(NSObject * _value) : value(_value) {}
    
    bool isNull() const {
        return value;
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
    
    float toNumber() const {
        return (float) ((NSNumber *)value).floatValue;
    };
    
    bool toBool() const {
        return (bool) ((NSNumber *)value).boolValue;
    };
    
    Value get(const char* key) const {
        NSDictionary *dict = (NSDictionary *)value;
        return Value(value[key]);
    };
    
    int getLength() const {
        NSArray * array = (NSArray *)value;
        return [size count];
    };
    
    Value get(const int index ) const {
        NSArray * array = (NSArray *)value;
        return array[index];
    };
    
private:
    NSObject * value;
};

#endif /* MGLConversionValue_h */
