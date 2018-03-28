#import "NSExpression+MGLPrivateAdditions.h"

#import "MGLTypes.h"
#if TARGET_OS_IPHONE
    #import "UIColor+MGLAdditions.h"
    #define MGLEdgeInsets UIEdgeInsets
#else
    #import "NSColor+MGLAdditions.h"
    #define MGLEdgeInsets NSEdgeInsets
#endif
#import "NSPredicate+MGLAdditions.h"
#import "NSValue+MGLStyleAttributeAdditions.h"

#import <mbgl/style/expression/expression.hpp>

@implementation NSExpression (MGLPrivateAdditions)

- (std::vector<mbgl::Value>)mgl_aggregateMBGLValue {
    if ([self.constantValue isKindOfClass:[NSArray class]] || [self.constantValue isKindOfClass:[NSSet class]]) {
        std::vector<mbgl::Value> convertedValues;
        for (id value in self.constantValue) {
            NSExpression *expression = value;
            if (![expression isKindOfClass:[NSExpression class]]) {
                expression = [NSExpression expressionForConstantValue:expression];
            }
            convertedValues.push_back(expression.mgl_constantMBGLValue);
        }
        return convertedValues;
    }
    [NSException raise:NSInvalidArgumentException
                format:@"Constant value expression must contain an array or set."];
    return {};
}

- (mbgl::Value)mgl_constantMBGLValue {
    id value = self.constantValue;
    if ([value isKindOfClass:NSString.class]) {
        return { std::string([(NSString *)value UTF8String]) };
    } else if ([value isKindOfClass:NSNumber.class]) {
        NSNumber *number = (NSNumber *)value;
        if ((strcmp([number objCType], @encode(char)) == 0) ||
            (strcmp([number objCType], @encode(BOOL)) == 0)) {
            // char: 32-bit boolean
            // BOOL: 64-bit boolean
            return { (bool)number.boolValue };
        } else if (strcmp([number objCType], @encode(double)) == 0) {
            // Double values on all platforms are interpreted precisely.
            return { (double)number.doubleValue };
        } else if (strcmp([number objCType], @encode(float)) == 0) {
            // Float values when taken as double introduce precision problems,
            // so warn the user to avoid them. This would require them to
            // explicitly use -[NSNumber numberWithFloat:] arguments anyway.
            // We still do this conversion in order to provide a valid value.
            static dispatch_once_t onceToken;
            dispatch_once(&onceToken, ^{
                NSLog(@"Float value in expression will be converted to a double; some imprecision may result. "
                      @"Use double values explicitly when specifying constant expression values and "
                      @"when specifying arguments to predicate and expression format strings. "
                      @"This will be logged only once.");
            });
            return { (double)number.doubleValue };
        } else if ([number compare:@(0)] == NSOrderedDescending ||
                   [number compare:@(0)] == NSOrderedSame) {
            // Positive integer or zero; use uint64_t per mbgl::Value definition.
            // We use unsigned long long here to avoid any truncation.
            return { (uint64_t)number.unsignedLongLongValue };
        } else if ([number compare:@(0)] == NSOrderedAscending) {
            // Negative integer; use int64_t per mbgl::Value definition.
            // We use long long here to avoid any truncation.
            return { (int64_t)number.longLongValue };
        }
    } else if ([value isKindOfClass:[MGLColor class]]) {
        auto hexString = [(MGLColor *)value mgl_color].stringify();
        return { hexString };
    } else if (value && value != [NSNull null]) {
        [NSException raise:NSInvalidArgumentException
                    format:@"Can’t convert %s:%@ to mbgl::Value", [value objCType], value];
    }
    return {};
}

- (std::vector<mbgl::FeatureType>)mgl_aggregateFeatureType {
    if ([self.constantValue isKindOfClass:[NSArray class]] || [self.constantValue isKindOfClass:[NSSet class]]) {
        std::vector<mbgl::FeatureType> convertedValues;
        for (id value in self.constantValue) {
            NSExpression *expression = value;
            if (![expression isKindOfClass:[NSExpression class]]) {
                expression = [NSExpression expressionForConstantValue:expression];
            }
            convertedValues.push_back(expression.mgl_featureType);
        }
        return convertedValues;
    }
    [NSException raise:NSInvalidArgumentException
                format:@"Constant value expression must contain an array or set."];
    return {};
}

- (mbgl::FeatureType)mgl_featureType {
    id value = self.constantValue;
    if ([value isKindOfClass:NSString.class]) {
        if ([value isEqualToString:@"Point"]) {
            return mbgl::FeatureType::Point;
        }
        if ([value isEqualToString:@"LineString"]) {
            return mbgl::FeatureType::LineString;
        }
        if ([value isEqualToString:@"Polygon"]) {
            return mbgl::FeatureType::Polygon;
        }
    } else if ([value isKindOfClass:NSNumber.class]) {
        switch ([value integerValue]) {
            case 1:
                return mbgl::FeatureType::Point;
            case 2:
                return mbgl::FeatureType::LineString;
            case 3:
                return mbgl::FeatureType::Polygon;
            default:
                break;
        }
    }
    return mbgl::FeatureType::Unknown;
}

- (std::vector<mbgl::FeatureIdentifier>)mgl_aggregateFeatureIdentifier {
    if ([self.constantValue isKindOfClass:[NSArray class]] || [self.constantValue isKindOfClass:[NSSet class]]) {
        std::vector<mbgl::FeatureIdentifier> convertedValues;
        for (id value in self.constantValue) {
            NSExpression *expression = value;
            if (![expression isKindOfClass:[NSExpression class]]) {
                expression = [NSExpression expressionForConstantValue:expression];
            }
            convertedValues.push_back(expression.mgl_featureIdentifier);
        }
        return convertedValues;
    }
    [NSException raise:NSInvalidArgumentException
                format:@"Constant value expression must contain an array or set."];
    return {};
}

- (mbgl::FeatureIdentifier)mgl_featureIdentifier {
    mbgl::Value mbglValue = self.mgl_constantMBGLValue;

    if (mbglValue.is<std::string>()) {
        return mbglValue.get<std::string>();
    }
    if (mbglValue.is<double>()) {
        return mbglValue.get<double>();
    }
    if (mbglValue.is<uint64_t>()) {
        return mbglValue.get<uint64_t>();
    }
    if (mbglValue.is<int64_t>()) {
        return mbglValue.get<int64_t>();
    }

    return {};
}

@end

@implementation NSObject (MGLExpressionAdditions)

- (NSNumber *)mgl_number {
    return nil;
}

- (NSNumber *)mgl_numberWithFallbackValues:(id)fallbackValue, ... {
    if (self.mgl_number) {
        return self.mgl_number;
    }
    
    va_list fallbackValues;
    va_start(fallbackValues, fallbackValue);
    for (id value = fallbackValue; value; value = va_arg(fallbackValues, id)) {
        if ([value mgl_number]) {
            return [value mgl_number];
        }
    }
    
    return nil;
}

@end

@implementation NSNull (MGLExpressionAdditions)

- (id)mgl_jsonExpressionObject {
    return self;
}

@end

@implementation NSString (MGLExpressionAdditions)

- (id)mgl_jsonExpressionObject {
    return self;
}

- (NSNumber *)mgl_number {
    if (self.doubleValue || ![[NSDecimalNumber decimalNumberWithString:self] isEqual:[NSDecimalNumber notANumber]]) {
        return @(self.doubleValue);
    }
    
    return nil;
}

@end

@implementation NSNumber (MGLExpressionAdditions)

- (id)mgl_interpolateWithCurveType:(NSString *)curveType
                        parameters:(NSArray *)parameters
                             stops:(NSDictionary<NSNumber *, id> *)stops {
    [NSException raise:NSInvalidArgumentException
                format:@"Interpolation expressions lack underlying Objective-C implementations."];
    return nil;
}

- (id)mgl_stepWithMinimum:(id)minimum stops:(NSDictionary<NSNumber *, id> *)stops {
    [NSException raise:NSInvalidArgumentException
                format:@"Interpolation expressions lack underlying Objective-C implementations."];
    return nil;
}

- (NSNumber *)mgl_number {
    return self;
}

- (id)mgl_jsonExpressionObject {
    if ([self isEqualToNumber:@(M_E)]) {
        return @[@"e"];
    } else if ([self isEqualToNumber:@(M_PI)]) {
        return @[@"pi"];
    }
    return self;
}

@end

@implementation MGLColor (MGLExpressionAdditions)

- (id)mgl_jsonExpressionObject {
    auto color = [self mgl_color];
    if (color.a == 1) {
        return @[@"rgb", @(color.r * 255), @(color.g * 255), @(color.b * 255)];
    }
    return @[@"rgba", @(color.r * 255), @(color.g * 255), @(color.b * 255), @(color.a)];
}

@end

@implementation NSArray (MGLExpressionAdditions)

- (id)mgl_jsonExpressionObject {
    return [self valueForKeyPath:@"mgl_jsonExpressionObject"];
}

- (id)mgl_coalesce {
    [NSException raise:NSInvalidArgumentException
                format:@"Coalesce expressions lack underlying Objective-C implementations."];
    return nil;
}

@end

@implementation NSDictionary (MGLExpressionAdditions)

- (id)mgl_jsonExpressionObject {
    NSMutableDictionary *expressionObject = [NSMutableDictionary dictionaryWithCapacity:self.count];
    [self enumerateKeysAndObjectsUsingBlock:^(id _Nonnull key, id _Nonnull obj, BOOL * _Nonnull stop) {
        expressionObject[[key mgl_jsonExpressionObject]] = [obj mgl_jsonExpressionObject];
    }];
    
    return expressionObject;
}

- (id)mgl_has:(id)element {
    [NSException raise:NSInvalidArgumentException
                format:@"Has expressions lack underlying Objective-C implementations."];
    return nil;
}

@end

@implementation NSExpression (MGLExpressionAdditions)

- (NSExpression *)mgl_expressionWithContext:(NSDictionary<NSString *, NSExpression *> *)context {
    [NSException raise:NSInternalInconsistencyException
                format:@"Assignment expressions lack underlying Objective-C implementations."];
    return self;
}

- (id)mgl_has:(id)element {
    [NSException raise:NSInvalidArgumentException
                format:@"Has expressions lack underlying Objective-C implementations."];
    return nil;
}

@end

@implementation NSExpression (MGLAdditions)

static NSDictionary<NSString *, NSString *> *MGLFunctionNamesByExpressionOperator;
static NSDictionary<NSString *, NSString *> *MGLExpressionOperatorsByFunctionNames;

NSArray *MGLSubexpressionsWithJSONObjects(NSArray *objects) {
    NSMutableArray *subexpressions = [NSMutableArray arrayWithCapacity:objects.count];
    for (id object in objects) {
        NSExpression *expression = [NSExpression mgl_expressionWithJSONObject:object];
        [subexpressions addObject:expression];
    }
    return subexpressions;
}

+ (instancetype)mgl_expressionWithJSONObject:(id)object {
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        MGLFunctionNamesByExpressionOperator = @{
            @"+": @"add:to:",
            @"-": @"from:subtract:",
            @"*": @"multiply:by:",
            @"/": @"divide:by:",
            @"%": @"modulus:by:",
            @"sqrt": @"sqrt:",
            @"log10": @"log:",
            @"ln": @"ln:",
            @"^": @"raise:toPower:",
            @"upcase": @"uppercase:",
            @"downcase": @"lowercase:",
        };
    });
    
    if (!object || object == [NSNull null]) {
        return [NSExpression expressionForConstantValue:nil];
    }
    
    if ([object isKindOfClass:[NSString class]] ||
        [object isKindOfClass:[NSNumber class]] ||
        [object isKindOfClass:[NSValue class]] ||
        [object isKindOfClass:[MGLColor class]]) {
        return [NSExpression expressionForConstantValue:object];
    }
    
    if ([object isKindOfClass:[NSDictionary class]]) {
        NSMutableDictionary *dictionary = [NSMutableDictionary dictionaryWithCapacity:[object count]];
        [object enumerateKeysAndObjectsUsingBlock:^(id _Nonnull key, id _Nonnull obj, BOOL * _Nonnull stop) {
            dictionary[key] = [NSExpression mgl_expressionWithJSONObject:obj];
        }];
        return [NSExpression expressionForConstantValue:dictionary];
    }
    
    if ([object isKindOfClass:[NSArray class]]) {
        NSArray *array = (NSArray *)object;
        NSString *op = array.firstObject;
        
        NSArray *argumentObjects = [array subarrayWithRange:NSMakeRange(1, array.count - 1)];
        
        NSString *functionName = MGLFunctionNamesByExpressionOperator[op];
        if (functionName) {
            NSArray *subexpressions = MGLSubexpressionsWithJSONObjects(argumentObjects);
            if ([op isEqualToString:@"+"] && argumentObjects.count > 2) {
                NSExpression *subexpression = [NSExpression expressionForAggregate:subexpressions];
                return [NSExpression expressionForFunction:@"sum:"
                                                 arguments:@[subexpression]];
            } else if ([op isEqualToString:@"^"] && [argumentObjects.firstObject isEqual:@[@"e"]]) {
                functionName = @"exp:";
                subexpressions = [subexpressions subarrayWithRange:NSMakeRange(1, subexpressions.count - 1)];
            }
            
            return [NSExpression expressionForFunction:functionName
                                             arguments:subexpressions];
        } else if ([op isEqualToString:@"literal"]) {
            if ([argumentObjects.firstObject isKindOfClass:[NSArray class]]) {
                return [NSExpression expressionForAggregate:MGLSubexpressionsWithJSONObjects(argumentObjects.firstObject)];
            }
            return [NSExpression mgl_expressionWithJSONObject:argumentObjects.firstObject];
        } else if ([op isEqualToString:@"to-boolean"]) {
            NSExpression *operand = [NSExpression mgl_expressionWithJSONObject:argumentObjects.firstObject];
            return [NSExpression expressionForFunction:operand selectorName:@"boolValue" arguments:@[]];
        } else if ([op isEqualToString:@"to-number"]) {
            NSExpression *operand = [NSExpression mgl_expressionWithJSONObject:argumentObjects.firstObject];
            argumentObjects = [argumentObjects subarrayWithRange:NSMakeRange(1, argumentObjects.count - 1)];
            NSArray *subexpressions = MGLSubexpressionsWithJSONObjects(argumentObjects);
            return [NSExpression expressionForFunction:operand selectorName:@"mgl_numberWithFallbackValues:" arguments:subexpressions];
        } else if ([op isEqualToString:@"to-string"]) {
            NSExpression *operand = [NSExpression mgl_expressionWithJSONObject:argumentObjects.firstObject];
            return [NSExpression expressionForFunction:operand selectorName:@"stringValue" arguments:@[]];
        } else if ([op isEqualToString:@"get"]) {
            if (argumentObjects.count == 2) {
                NSExpression *operand = [NSExpression mgl_expressionWithJSONObject:argumentObjects.lastObject];
                if ([argumentObjects.firstObject isKindOfClass:[NSString class]]) {
                    return [NSExpression expressionWithFormat:@"%@.%K", operand, argumentObjects.firstObject];
                }
                NSExpression *key = [NSExpression mgl_expressionWithJSONObject:argumentObjects.firstObject];
                return [NSExpression expressionWithFormat:@"%@.%@", operand, key];
            }
            return [NSExpression expressionForKeyPath:argumentObjects.firstObject];
        } else if ([op isEqualToString:@"length"]) {
            NSArray *subexpressions = MGLSubexpressionsWithJSONObjects(argumentObjects);
            return [NSExpression expressionForFunction:@"count:" arguments:@[subexpressions.firstObject]];
        } else if ([op isEqualToString:@"min"]) {
            NSArray *subexpressions = MGLSubexpressionsWithJSONObjects(argumentObjects);
            NSExpression *subexpression = [NSExpression expressionForAggregate:subexpressions];
            return [NSExpression expressionForFunction:@"min:" arguments:@[subexpression]];
        } else if ([op isEqualToString:@"max"]) {
            NSArray *subexpressions = MGLSubexpressionsWithJSONObjects(argumentObjects);
            NSExpression *subexpression = [NSExpression expressionForAggregate:subexpressions];
            return [NSExpression expressionForFunction:@"max:" arguments:@[subexpression]];
        } else if ([op isEqualToString:@"e"]) {
            return [NSExpression expressionForConstantValue:@(M_E)];
        } else if ([op isEqualToString:@"pi"]) {
            return [NSExpression expressionForConstantValue:@(M_PI)];
        } else if ([op isEqualToString:@"concat"]) {
            NSArray *subexpressions = MGLSubexpressionsWithJSONObjects(argumentObjects);
            NSExpression *operand = subexpressions.firstObject;
            subexpressions = [subexpressions subarrayWithRange:NSMakeRange(1, subexpressions.count - 1)];
            return [NSExpression expressionForFunction:operand selectorName:@"stringByAppendingString:" arguments:subexpressions];
        }  else if ([op isEqualToString:@"at"]) {
            NSArray *subexpressions = MGLSubexpressionsWithJSONObjects(argumentObjects);
            NSExpression *index = subexpressions.firstObject;
            NSExpression *operand = subexpressions[1];
            return [NSExpression expressionForFunction:@"objectFrom:withIndex:" arguments:@[operand, index]];
        } else if ([op isEqualToString:@"has"]) {
            NSArray *subexpressions = MGLSubexpressionsWithJSONObjects(argumentObjects);
            NSExpression *operand = argumentObjects.count > 1 ? subexpressions[1] : [NSExpression expressionWithFormat:@"self"];
            NSExpression *property = subexpressions.firstObject;
            return [NSExpression expressionWithFormat:@"FUNCTION(%@, 'mgl_has:', %@)", operand, property];
        } else if ([op isEqualToString:@"interpolate"]) {
            NSArray *interpolationOptions = argumentObjects.firstObject;
            NSString *curveType = interpolationOptions.firstObject;
            NSExpression *curveTypeExpression = [NSExpression mgl_expressionWithJSONObject:curveType];
            id curveParameters;
            if ([curveType isEqual:@"exponential"]) {
                curveParameters = interpolationOptions[1];
            } else if ([curveType isEqualToString:@"cubic-bezier"]) {
                curveParameters = @[@"literal", [interpolationOptions subarrayWithRange:NSMakeRange(1, 4)]];
            }
            NSExpression *curveParameterExpression = [NSExpression mgl_expressionWithJSONObject:curveParameters];
            argumentObjects = [argumentObjects subarrayWithRange:NSMakeRange(1, argumentObjects.count - 1)];
            NSExpression *operand = [NSExpression mgl_expressionWithJSONObject:argumentObjects.firstObject];
            NSArray *stopExpressions = [argumentObjects subarrayWithRange:NSMakeRange(1, argumentObjects.count - 1)];
            NSMutableDictionary *stops = [NSMutableDictionary dictionaryWithCapacity:stopExpressions.count / 2];
            NSEnumerator *stopEnumerator = stopExpressions.objectEnumerator;
            while (NSNumber *key = stopEnumerator.nextObject) {
                NSExpression *valueExpression = stopEnumerator.nextObject;
                stops[key] = [NSExpression mgl_expressionWithJSONObject:valueExpression];
            }
            NSExpression *stopExpression = [NSExpression expressionForConstantValue:stops];
            return [NSExpression expressionForFunction:operand
                                          selectorName:@"mgl_interpolateWithCurveType:parameters:stops:"
                                             arguments:@[curveTypeExpression, curveParameterExpression, stopExpression]];
        } else if ([op isEqualToString:@"step"]) {
            NSExpression *operand = [NSExpression mgl_expressionWithJSONObject:argumentObjects[0]];
            NSArray *stopExpressions = [argumentObjects subarrayWithRange:NSMakeRange(1, argumentObjects.count - 1)];
            NSExpression *minimum;
            if (stopExpressions.count % 2) {
                minimum = [NSExpression mgl_expressionWithJSONObject:stopExpressions.firstObject];
                stopExpressions = [stopExpressions subarrayWithRange:NSMakeRange(1, stopExpressions.count - 1)];
            }
            NSMutableDictionary *stops = [NSMutableDictionary dictionaryWithCapacity:stopExpressions.count / 2];
            NSEnumerator *stopEnumerator = stopExpressions.objectEnumerator;
            while (NSNumber *key = stopEnumerator.nextObject) {
                NSExpression *valueExpression = stopEnumerator.nextObject;
                if (minimum) {
                    stops[key] = [NSExpression mgl_expressionWithJSONObject:valueExpression];
                } else {
                    minimum = [NSExpression mgl_expressionWithJSONObject:valueExpression];
                }
            }
            NSExpression *stopExpression = [NSExpression expressionForConstantValue:stops];
            return [NSExpression expressionForFunction:operand
                                          selectorName:@"mgl_stepWithMinimum:stops:"
                                             arguments:@[minimum, stopExpression]];
        } else if ([op isEqualToString:@"zoom"]) {
            return [NSExpression expressionForVariable:@"zoomLevel"];
        } else if ([op isEqualToString:@"heatmap-density"]) {
            return [NSExpression expressionForVariable:@"heatmapDensity"];
        } else if ([op isEqualToString:@"geometry-type"]) {
            return [NSExpression expressionForVariable:@"mgl_geometryType"];
        } else if ([op isEqualToString:@"id"]) {
            return [NSExpression expressionForVariable:@"mgl_featureIdentifier"];
        }  else if ([op isEqualToString:@"properties"]) {
            return [NSExpression expressionForVariable:@"mgl_featureProperties"];
        } else if ([op isEqualToString:@"let"]) {
            NSExpression *operand = [NSExpression mgl_expressionWithJSONObject:argumentObjects.lastObject];
            NSArray *bindingObjects = [argumentObjects subarrayWithRange:NSMakeRange(0, argumentObjects.count - 1)];
            NSMutableDictionary *context = [NSMutableDictionary dictionaryWithCapacity:bindingObjects.count / 2];
            NSEnumerator *bindingEnumerator = bindingObjects.objectEnumerator;
            while (NSString *key = bindingEnumerator.nextObject) {
                context[key] = [NSExpression mgl_expressionWithJSONObject:bindingEnumerator.nextObject];
            }
            return [NSExpression expressionForFunction:operand
                                          selectorName:@"mgl_expressionWithContext:"
                                             arguments:@[[NSExpression expressionForConstantValue:context]]];
        } else if ([op isEqualToString:@"var"]) {
            return [NSExpression expressionForVariable:argumentObjects.firstObject];
        } else if ([op isEqualToString:@"case"]) {
            NSArray *caseExpressions = argumentObjects;
            NSExpression *firstConditional = [NSExpression expressionWithFormat:@"%@", [NSPredicate mgl_predicateWithJSONObject:caseExpressions[0]]];
            NSMutableArray *arguments = [NSMutableArray array];
            
            for (NSUInteger index = 1; index < caseExpressions.count; index++) {
                if ([caseExpressions[index] isKindOfClass:[NSArray class]]) {
                    NSPredicate *conditional = [NSPredicate mgl_predicateWithJSONObject:caseExpressions[index]];
                    NSExpression *argument = [NSExpression expressionWithFormat:@"%@", conditional];
                    [arguments addObject:argument];
                } else {
                    [arguments addObject:[NSExpression mgl_expressionWithJSONObject:caseExpressions[index]]];
                }
            }
            
            return [NSExpression expressionForFunction:firstConditional selectorName:@"mgl_case:" arguments:arguments];
        } else if ([op isEqualToString:@"match"]) {
            NSExpression *operand = [NSExpression mgl_expressionWithJSONObject:argumentObjects[0]];
            NSArray *matchOptions = [argumentObjects subarrayWithRange:NSMakeRange(1, argumentObjects.count - 1)];

            NSMutableArray *optionsArray = [NSMutableArray array];
            NSEnumerator *optionsEnumerator = matchOptions.objectEnumerator;
            while (id object = optionsEnumerator.nextObject) {
                NSExpression *option = [NSExpression mgl_expressionWithJSONObject:object];
                [optionsArray addObject:option];
            }
        
            return [NSExpression expressionForFunction:operand
                                          selectorName:@"mgl_match:"
                                             arguments:optionsArray];
        } else if ([op isEqualToString:@"coalesce"]) {
            NSMutableArray *expressions = [NSMutableArray array];
            for (id operand in argumentObjects) {
                [expressions addObject:[NSExpression mgl_expressionWithJSONObject:operand]];
            }
            
            return [NSExpression expressionWithFormat:@"FUNCTION(%@, 'mgl_coalesce')", expressions];
        }else {
            [NSException raise:NSInvalidArgumentException
                        format:@"Expression operator %@ not yet implemented.", op];
        }
    }
    
    [NSException raise:NSInvalidArgumentException
                format:@"Unable to convert JSON object %@ to an NSExpression.", object];
    
    return nil;
}

- (id)mgl_jsonExpressionObject {
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        MGLExpressionOperatorsByFunctionNames = @{
            @"add:to:": @"+",
            @"from:subtract:": @"-",
            @"multiply:by:": @"*",
            @"divide:by:": @"/",
            @"modulus:by:": @"%",
            @"sqrt:": @"sqrt",
            @"log:": @"log10",
            @"ln:": @"ln",
            @"raise:toPower:": @"^",
            @"uppercase:": @"upcase",
            @"lowercase:": @"downcase",
        };
    });
    
    switch (self.expressionType) {
        case NSVariableExpressionType: {
            if ([self.variable isEqualToString:@"heatmapDensity"]) {
                return @[@"heatmap-density"];
            }
            if ([self.variable isEqualToString:@"zoomLevel"]) {
                return @[@"zoom"];
            }
            if ([self.variable isEqualToString:@"mgl_geometryType"]) {
                return @[@"geometry-type"];
            }
            if ([self.variable isEqualToString:@"mgl_featureIdentifier"]) {
                return @[@"id"];
            }
            if ([self.variable isEqualToString:@"mgl_featureProperties"]) {
                return @[@"properties"];
            }
            return @[@"var", self.variable];
        }
        
        case NSConstantValueExpressionType: {
            id constantValue = self.constantValue;
            if (!constantValue || constantValue == [NSNull null]) {
                return [NSNull null];
            }
            if ([constantValue isEqual:@(M_E)]) {
                return @[@"e"];
            }
            if ([constantValue isEqual:@(M_PI)]) {
                return @[@"pi"];
            }
            if ([constantValue isKindOfClass:[NSArray class]] ||
                [constantValue isKindOfClass:[NSDictionary class]]) {
                NSArray *collection = [constantValue mgl_jsonExpressionObject];
                return @[@"literal", collection];
            }
            if ([constantValue isKindOfClass:[MGLColor class]]) {
                auto color = [constantValue mgl_color];
                if (color.a == 1) {
                    return @[@"rgb", @(color.r * 255), @(color.g * 255), @(color.b * 255)];
                }
                return @[@"rgba", @(color.r * 255), @(color.g * 255), @(color.b * 255), @(color.a)];
            }
            if ([constantValue isKindOfClass:[NSValue class]]) {
                const auto boxedValue = (NSValue *)constantValue;
                if (strcmp([boxedValue objCType], @encode(CGVector)) == 0) {
                    // offset [x, y]
                    std::array<float, 2> mglValue = boxedValue.mgl_offsetArrayValue;
                    return @[@"literal", @[@(mglValue[0]), @(mglValue[1])]];
                }
                if (strcmp([boxedValue objCType], @encode(MGLEdgeInsets)) == 0) {
                    // padding [x, y]
                    std::array<float, 4> mglValue = boxedValue.mgl_paddingArrayValue;
                    return @[@"literal", @[@(mglValue[0]), @(mglValue[1]), @(mglValue[2]), @(mglValue[3])]];
                }
            }
            return self.constantValue;
        }
            
        case NSKeyPathExpressionType: {
            NSArray *expressionObject;
            for (NSString *pathComponent in self.keyPath.pathComponents.reverseObjectEnumerator) {
                if (expressionObject) {
                    expressionObject = @[@"get", pathComponent, expressionObject];
                } else {
                    expressionObject = @[@"get", pathComponent];
                }
            }
            return expressionObject;
        }
            
        case NSFunctionExpressionType: {
            NSString *function = self.function;
            NSString *op = MGLExpressionOperatorsByFunctionNames[function];
            if (op) {
                NSArray *arguments = self.arguments.mgl_jsonExpressionObject;
                return [@[op] arrayByAddingObjectsFromArray:arguments];
            } else if ([function isEqualToString:@"valueForKeyPath:"]) {
                return @[@"get", self.arguments.firstObject.mgl_jsonExpressionObject, self.operand.mgl_jsonExpressionObject];
            } else if ([function isEqualToString:@"average:"]) {
                NSExpression *sum = [NSExpression expressionForFunction:@"sum:" arguments:self.arguments];
                NSExpression *count = [NSExpression expressionForFunction:@"count:" arguments:self.arguments];
                return [NSExpression expressionForFunction:@"divide:by:" arguments:@[sum, count]].mgl_jsonExpressionObject;
            } else if ([function isEqualToString:@"sum:"]) {
                NSArray *arguments = [self.arguments.firstObject.collection valueForKeyPath:@"mgl_jsonExpressionObject"];
                return [@[@"+"] arrayByAddingObjectsFromArray:arguments];
            } else if ([function isEqualToString:@"count:"]) {
                NSArray *arguments = self.arguments.firstObject.mgl_jsonExpressionObject;
                return @[@"length", arguments];
            } else if ([function isEqualToString:@"min:"]) {
                NSArray *arguments = [self.arguments.firstObject.collection valueForKeyPath:@"mgl_jsonExpressionObject"];
                return [@[@"min"] arrayByAddingObjectsFromArray:arguments];
            } else if ([function isEqualToString:@"max:"]) {
                NSArray *arguments = [self.arguments.firstObject.collection valueForKeyPath:@"mgl_jsonExpressionObject"];
                return [@[@"max"] arrayByAddingObjectsFromArray:arguments];
            } else if ([function isEqualToString:@"exp:"]) {
                return [NSExpression expressionForFunction:@"raise:toPower:" arguments:@[@(M_E), self.arguments.firstObject]].mgl_jsonExpressionObject;
            } else if ([function isEqualToString:@"ceiling:"]) {
                return [NSExpression expressionWithFormat:@"trunc:(%@) + TERNARY(modulus:by:(%@, 1) > 0, 1, 0)",
                        self.arguments.firstObject, self.arguments.firstObject].mgl_jsonExpressionObject;
            } else if ([function isEqualToString:@"trunc:"]) {
                return [NSExpression expressionWithFormat:@"%@ - modulus:by:(%@, 1)",
                        self.arguments.firstObject, self.arguments.firstObject].mgl_jsonExpressionObject;
            } else if ([function isEqualToString:@"abs:"]) {
                return [NSExpression expressionWithFormat:@"%@ * TERNARY(%@ > 0, 1, -1)",
                        self.arguments.firstObject, self.arguments.firstObject].mgl_jsonExpressionObject;
            } else if ([function isEqualToString:@"floor:"]) {
                return [NSExpression expressionWithFormat:@"trunc:(%@) - TERNARY(modulus:by:(%@, 1) < 0, 1, 0)",
                        self.arguments.firstObject, self.arguments.firstObject].mgl_jsonExpressionObject;
            } else if ([function isEqualToString:@"stringByAppendingString:"]) {
                NSArray *arguments = self.arguments.mgl_jsonExpressionObject;
                return [@[@"concat", self.operand.mgl_jsonExpressionObject] arrayByAddingObjectsFromArray:arguments];
            } else if ([function isEqualToString:@"objectFrom:withIndex:"]) {
                return @[@"at", self.arguments[1].mgl_jsonExpressionObject, self.arguments[0].mgl_jsonExpressionObject];
            } else if ([function isEqualToString:@"boolValue"]) {
                return @[@"to-boolean", self.operand.mgl_jsonExpressionObject];
            } else if ([function isEqualToString:@"mgl_numberWithFallbackValues:"] ||
                       [function isEqualToString:@"decimalValue"] ||
                       [function isEqualToString:@"floatValue"] ||
                       [function isEqualToString:@"doubleValue"]) {
                NSArray *arguments = self.arguments.mgl_jsonExpressionObject;
                return [@[@"to-number", self.operand.mgl_jsonExpressionObject] arrayByAddingObjectsFromArray:arguments];
            } else if ([function isEqualToString:@"stringValue"]) {
                return @[@"to-string", self.operand.mgl_jsonExpressionObject];
            } else if ([function isEqualToString:@"noindex:"]) {
                return self.arguments.firstObject.mgl_jsonExpressionObject;
            } else if ([function isEqualToString:@"mgl_has:"]) {
                NSMutableArray *expressionObject = [NSMutableArray arrayWithObjects:@"has", self.arguments[0].mgl_jsonExpressionObject, nil];
                if (self.operand.expressionType != NSEvaluatedObjectExpressionType) {
                    [expressionObject addObject:self.operand.mgl_jsonExpressionObject];
                }
                return expressionObject;
            } else if ([function isEqualToString:@"mgl_interpolateWithCurveType:parameters:stops:"]) {
                if (self.arguments.count < 3) {
                    [NSException raise:NSInvalidArgumentException format:
                     @"Too few arguments to ‘mgl_interpolateWithCurveType:parameters:stops:’ function; expected 3 arguments."];
                } else if (self.arguments.count > 3) {
                    [NSException raise:NSInvalidArgumentException format:
                     @"%lu unexpected arguments to ‘mgl_interpolateWithCurveType:parameters:stops:’ function; expected 3 arguments.",
                     self.arguments.count - 3];
                }
                NSString *curveType = self.arguments.firstObject.constantValue;
                NSMutableArray *interpolationArray = [NSMutableArray arrayWithObject:curveType];
                if ([curveType isEqualToString:@"exponential"]) {
                    id base = [self.arguments[1] mgl_jsonExpressionObject];
                    [interpolationArray addObject:base];
                } else if ([curveType isEqualToString:@"cubic-bezier"]) {
                    NSArray *controlPoints = [self.arguments[1].collection mgl_jsonExpressionObject];
                    [interpolationArray addObjectsFromArray:controlPoints];
                }
                NSMutableArray *expressionObject = [NSMutableArray arrayWithObjects:@"interpolate", interpolationArray, self.operand.mgl_jsonExpressionObject, nil];
                NSDictionary<NSNumber *, NSExpression *> *stops = self.arguments[2].constantValue;
                for (NSNumber *key in [stops.allKeys sortedArrayUsingSelector:@selector(compare:)]) {
                    [expressionObject addObject:key];
                    [expressionObject addObject:[stops[key] mgl_jsonExpressionObject]];
                }
                return expressionObject;
            } else if ([function isEqualToString:@"mgl_stepWithMinimum:stops:"]) {
                id minimum = self.arguments.firstObject.mgl_jsonExpressionObject;
                NSMutableArray *expressionObject = [NSMutableArray arrayWithObjects:@"step", self.operand.mgl_jsonExpressionObject, minimum, nil];
                NSDictionary<NSNumber *, NSExpression *> *stops = self.arguments[1].constantValue;
                for (NSNumber *key in [stops.allKeys sortedArrayUsingSelector:@selector(compare:)]) {
                    [expressionObject addObject:key];
                    [expressionObject addObject:[stops[key] mgl_jsonExpressionObject]];
                }
                return expressionObject;
            } else if ([function isEqualToString:@"mgl_expressionWithContext:"]) {
                id context = self.arguments.firstObject;
                if ([context isKindOfClass:[NSExpression class]]) {
                    context = [context constantValue];
                }
                NSMutableArray *expressionObject = [NSMutableArray arrayWithObjects:@"let", nil];
                [context enumerateKeysAndObjectsUsingBlock:^(id _Nonnull key, NSExpression * _Nonnull obj, BOOL * _Nonnull stop) {
                    [expressionObject addObject:key];
                    [expressionObject addObject:obj.mgl_jsonExpressionObject];
                }];
                [expressionObject addObject:self.operand.mgl_jsonExpressionObject];
                return expressionObject;
            } else if ([function isEqualToString:@"mgl_case:"]) {
                NSPredicate *firstConditional = (NSPredicate *)self.operand.constantValue;
                NSMutableArray *expressionObject = [NSMutableArray arrayWithObjects:@"case", firstConditional.mgl_jsonExpressionObject, nil];

                for (NSExpression *option in self.arguments) {
                    if ([option respondsToSelector:@selector(constantValue)] && [option.constantValue isKindOfClass:[NSComparisonPredicate class]]) {
                        NSPredicate *predicate = (NSPredicate *)option.constantValue;
                        [expressionObject addObject:predicate.mgl_jsonExpressionObject];
                    } else {
                        [expressionObject addObject:option.mgl_jsonExpressionObject];
                    }
                }

                return expressionObject;
            } else if ([function isEqualToString:@"mgl_match:"]) {
                NSMutableArray *expressionObject = [NSMutableArray arrayWithObjects:@"match", self.operand.mgl_jsonExpressionObject, nil];
                

                for (NSExpression *option in self.arguments) {
                    [expressionObject addObject:option.mgl_jsonExpressionObject];
                }
                
                return expressionObject;
            } else if ([function isEqualToString:@"mgl_coalesce"]) {
                NSMutableArray *expressionObject = [NSMutableArray arrayWithObjects:@"coalesce", nil];
                
                for (NSExpression *expression in self.operand.constantValue) {
                    [expressionObject addObject:[expression mgl_jsonExpressionObject]];
                }
                
                return expressionObject;
            } else if ([function isEqualToString:@"median:"] ||
                       [function isEqualToString:@"mode:"] ||
                       [function isEqualToString:@"stddev:"] ||
                       [function isEqualToString:@"random"] ||
                       [function isEqualToString:@"randomn:"] ||
                       [function isEqualToString:@"now"] ||
                       [function isEqualToString:@"bitwiseAnd:with:"] ||
                       [function isEqualToString:@"bitwiseOr:with:"] ||
                       [function isEqualToString:@"bitwiseXor:with:"] ||
                       [function isEqualToString:@"leftshift:by:"] ||
                       [function isEqualToString:@"rightshift:by:"] ||
                       [function isEqualToString:@"onesComplement:"] ||
                       [function isEqualToString:@"distanceToLocation:fromLocation:"]) {
                [NSException raise:NSInvalidArgumentException
                            format:@"Expression function %@ not yet implemented.", function];
                return nil;
            } else {
                [NSException raise:NSInvalidArgumentException
                            format:@"Unrecognized expression function %@.", function];
                return nil;
            }
        }
            
        case NSConditionalExpressionType: {
            NSMutableArray *arguments = [NSMutableArray arrayWithObjects:self.predicate.mgl_jsonExpressionObject, nil];
            
            if (self.trueExpression.expressionType == NSConditionalExpressionType) {
                // Fold nested conditionals into a single case expression.
                NSArray *trueArguments = self.trueExpression.mgl_jsonExpressionObject;
                trueArguments = [trueArguments subarrayWithRange:NSMakeRange(1, trueArguments.count - 1)];
                [arguments addObjectsFromArray:trueArguments];
            } else {
                [arguments addObject:self.trueExpression.mgl_jsonExpressionObject];
            }
            
            if (self.falseExpression.expressionType == NSConditionalExpressionType) {
                // Fold nested conditionals into a single case expression.
                NSArray *falseArguments = self.falseExpression.mgl_jsonExpressionObject;
                falseArguments = [falseArguments subarrayWithRange:NSMakeRange(1, falseArguments.count - 1)];
                [arguments addObjectsFromArray:falseArguments];
            } else {
                [arguments addObject:self.falseExpression.mgl_jsonExpressionObject];
            }
            
            [arguments insertObject:@"case" atIndex:0];
            return arguments;
        }
            
        case NSAggregateExpressionType: {
            NSArray *collection = [self.collection valueForKeyPath:@"mgl_jsonExpressionObject"];
            return @[@"literal", collection];
        }
        
        case NSEvaluatedObjectExpressionType:
        case NSUnionSetExpressionType:
        case NSIntersectSetExpressionType:
        case NSMinusSetExpressionType:
        case NSSubqueryExpressionType:
        case NSAnyKeyExpressionType:
        case NSBlockExpressionType:
            [NSException raise:NSInvalidArgumentException
                        format:@"Expression type %lu not yet implemented.", self.expressionType];
    }
    
    // NSKeyPathSpecifierExpression
    if (self.expressionType == 10) {
        return self.description;
    }
    // An assignment expression type is present in the BNF grammar, but the
    // corresponding NSExpressionType value and property getters are missing.
    if (self.expressionType == 12) {
        [NSException raise:NSInvalidArgumentException
                    format:@"Assignment expressions not yet implemented."];
    }
    
    return nil;
}

@end
