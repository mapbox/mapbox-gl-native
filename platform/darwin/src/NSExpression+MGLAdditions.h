#import <Foundation/Foundation.h>

#include <mbgl/style/filter.hpp>

@interface NSExpression (MGLAdditions)

- (mbgl::Value)mgl_filterValue;
- (std::vector<mbgl::Value>)mgl_filterValues;
- (mbgl::FeatureIdentifier)mgl_featureIdentifier;

@end
