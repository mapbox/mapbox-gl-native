#import "NSArray+MGLAdditions.h"

#import "NSDictionary+MGLAdditions.h"
#import "NSExpression+MGLAdditions.mm"

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
            vector.push_back([expression mgl_filterValue]);
        }
    }    
    return vector;
}

@end
