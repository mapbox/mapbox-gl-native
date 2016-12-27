#import "FeatureAttributePredicateEditorRowTemplate.h"

#import <Mapbox/Mapbox.h>

@implementation MGLStyleLayer (MBXAdditions)

- (NSPredicate *)predicate {
    return nil;
}

- (void)setPredicate:(NSPredicate *)predicate {
}

@end

@implementation FeatureAttributePredicateEditorRowTemplate {
    NSTextField *_keyPathTextField;
    NSPopUpButton *_operatorPopUpButton;
    NSTextField *_valueTextField;
    NSTokenField *_valueTokenField;
}

- (double)matchForPredicate:(NSComparisonPredicate *)predicate {
    if (![predicate isKindOfClass:[NSComparisonPredicate class]]) {
        return 0;
    }
    
    id constantValue = predicate.rightExpression.constantValue;
    if ([constantValue isKindOfClass:[NSArray class]]) {
        constantValue = [[constantValue firstObject] constantValue];
    }
    switch (self.rightExpressionAttributeType) {
        case NSBooleanAttributeType:
            return ([constantValue isKindOfClass:[NSNumber class]]
                    && (strcmp([constantValue objCType], @encode(char)) == 0
                        || strcmp([constantValue objCType], @encode(BOOL)) == 0)) ? 1 : 0;
            
        case NSDoubleAttributeType:
        case NSFloatAttributeType:
            return ([constantValue isKindOfClass:[NSNumber class]]
                    && (strcmp([constantValue objCType], @encode(double)) == 0
                        || strcmp([constantValue objCType], @encode(float)) == 0)) ? 1 : 0;
            
        case NSInteger16AttributeType:
        case NSInteger32AttributeType:
        case NSInteger64AttributeType:
            return ([constantValue isKindOfClass:[NSNumber class]]
                    && strcmp([constantValue objCType], @encode(double)) != 0
                    && strcmp([constantValue objCType], @encode(float)) != 0
                    && strcmp([constantValue objCType], @encode(char)) != 0
                    && strcmp([constantValue objCType], @encode(BOOL)) != 0) ? 1 : 0;
            
        case NSStringAttributeType:
            return [constantValue isKindOfClass:[NSString class]] ? 0.8 : 0;
            
        default:
            return [super matchForPredicate:predicate];
    }
}

- (NS_ARRAY_OF(NSView *) *)templateViews {
    NSMutableArray *views = super.templateViews.mutableCopy;
    views[0] = self.keyPathTextField;
    _operatorPopUpButton = views[1];
    _valueTextField = views[2];
    
    BOOL isInPredicate = [_operatorPopUpButton.selectedItem.representedObject unsignedIntegerValue] == NSInPredicateOperatorType;
    if (isInPredicate) {
        [views insertObject:self.valueTokenField atIndex:2];
    } else {
        [views addObject:self.valueTokenField];
    }
    [views[2] setHidden:NO];
    [views[3] setHidden:YES];
    
    return views;
}

- (NSTextField *)keyPathTextField {
    if (!_keyPathTextField) {
        _keyPathTextField = [[NSTextField alloc] initWithFrame:NSZeroRect];
        _keyPathTextField.editable = NO;
        _keyPathTextField.bordered = NO;
        _keyPathTextField.backgroundColor = nil;
        _keyPathTextField.font = [NSFont systemFontOfSize:[NSFont systemFontSizeForControlSize:NSSmallControlSize]];
    }
    [_keyPathTextField sizeToFit];
    return _keyPathTextField;
}

- (NSTokenField *)valueTokenField {
    if (!_valueTokenField) {
        _valueTokenField = [[NSTokenField alloc] initWithFrame:NSZeroRect];
        _valueTokenField.font = [NSFont systemFontOfSize:[NSFont systemFontSizeForControlSize:NSSmallControlSize]];
    }
    [_valueTokenField sizeToFit];
    NSRect frame = _valueTokenField.frame;
    frame.size.width = _valueTextField.frame.size.width;
    _valueTokenField.frame = frame;
    return _valueTokenField;
}

- (NSPredicate *)predicateWithSubpredicates:(NS_ARRAY_OF(NSPredicate *) *)subpredicates {
    NSComparisonPredicate *predicate = (NSComparisonPredicate *)[super predicateWithSubpredicates:subpredicates];
    NSAssert([predicate isKindOfClass:[NSComparisonPredicate class]], @"FeatureAttributePredicateEditorRowTemplate only works with comparison predicates.");
    
    NSExpression *leftExpression = [NSExpression expressionForKeyPath:self.keyPathTextField.stringValue];
    NSExpression *rightExpression = predicate.rightExpression;
    if (predicate.predicateOperatorType == NSInPredicateOperatorType) {
        rightExpression = [NSExpression expressionForAggregate:
                           [_valueTokenField.stringValue componentsSeparatedByString:@","]];
    }
    return [NSComparisonPredicate predicateWithLeftExpression:leftExpression
                                              rightExpression:rightExpression
                                                     modifier:predicate.comparisonPredicateModifier
                                                         type:predicate.predicateOperatorType
                                                      options:predicate.options];
}

- (void)setPredicate:(NSComparisonPredicate *)predicate {
    NSAssert([predicate isKindOfClass:[NSComparisonPredicate class]], @"FeatureAttributePredicateEditorRowTemplate only works with comparison predicates.");
    
    self.keyPathTextField.stringValue = predicate.leftExpression.keyPath;
    [self.keyPathTextField sizeToFit];
    
    BOOL isInPredicate = predicate.predicateOperatorType == NSInPredicateOperatorType;
    _operatorPopUpButton.enabled = !isInPredicate;
    _valueTextField.hidden = isInPredicate;
    self.valueTokenField.hidden = !isInPredicate;
    NSExpression *rightExpression = predicate.rightExpression;
    if (isInPredicate) {
        NSArray *values = [rightExpression.constantValue valueForKeyPath:@"constantValue"];
        self.valueTokenField.stringValue = [values componentsJoinedByString:@","];
        rightExpression = [NSExpression expressionForConstantValue:nil];
    } else {
        self.valueTokenField.stringValue = @"";
    }
    
    predicate = [NSComparisonPredicate predicateWithLeftExpression:self.leftExpressions.firstObject
                                                   rightExpression:rightExpression
                                                          modifier:predicate.comparisonPredicateModifier
                                                              type:predicate.predicateOperatorType
                                                           options:predicate.options];
    [super setPredicate:predicate];
}

@end
