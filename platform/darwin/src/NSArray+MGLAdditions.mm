#import "NSArray+MGLAdditions.h"

#import "NSDictionary+MGLAdditions.h"
#import "NSExpression+MGLPrivateAdditions.h"

@implementation NSArray (MGLAdditions)

- (std::vector<mbgl::Value>)mgl_vector {
    std::vector<mbgl::Value> vector;
    vector.reserve(self.count);
    for (id value in self) {
        if ([value isKindOfClass:[NSArray class]]) {
            std::vector<mbgl::Value> innerVector = [value mgl_vector];
            vector.push_back(innerVector);
        } else if ([value isKindOfClass:[NSDictionary class]]) {
            mbgl::PropertyMap propertyMap = [value mgl_propertyMap];
            vector.push_back(propertyMap);
        } else {
            NSExpression *expression = [NSExpression expressionForConstantValue:value];
            vector.push_back(expression.mgl_constantMBGLValue);
        }
    }
    return vector;
}

- (NSAttributedString *)mgl_attributedComponentsJoinedByString:(NSString *)separator {
    NSAttributedString *attributedSeparator = [[NSAttributedString alloc] initWithString:separator];
    NSMutableAttributedString *attributedString = [[NSMutableAttributedString alloc] init];
    BOOL isSubsequentItem = NO;
    for (NSAttributedString *component in self) {
        NSAssert([component isKindOfClass:[NSAttributedString class]], @"Items in array must be attributed strings.");
        if (isSubsequentItem) {
            [attributedString appendAttributedString:attributedSeparator];
        }
        isSubsequentItem = YES;
        [attributedString appendAttributedString:component];
    }
    return attributedString;
}

+ (NSArray *)mgl_coordinatesFromCoordinates:(std::vector<CLLocationCoordinate2D>)coords {
    NSMutableArray *coordinates = [NSMutableArray array];
    for (auto coord : coords) {
        [coordinates addObject:@{@"latitude": @(coord.latitude),
                                 @"longitude": @(coord.longitude)}];
    }
    return coordinates;
}

- (std::vector<CLLocationCoordinate2D>)mgl_coordinates {
    NSUInteger numberOfCoordinates = [self count];
    CLLocationCoordinate2D *coords = (CLLocationCoordinate2D *)malloc(numberOfCoordinates * sizeof(CLLocationCoordinate2D));

    for (NSUInteger i = 0; i < numberOfCoordinates; i++) {
        coords[i] = CLLocationCoordinate2DMake([self[i][@"latitude"] doubleValue],
                                               [self[i][@"longitude"] doubleValue]);
    }

    std::vector<CLLocationCoordinate2D> coordinates = { coords, coords + numberOfCoordinates };
    free(coords);

    return coordinates;
}

@end
