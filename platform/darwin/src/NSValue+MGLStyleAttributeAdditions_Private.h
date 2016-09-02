#import <Foundation/Foundation.h>

#import "MGLStyleAttributeValue.h"

#include <mbgl/style/property_value.hpp>

@interface NSValue (MGLStyleAttributeAdditions_Private) <MGLStyleAttributeValue>

+ (instancetype)mgl_valueWithOffsetArray:(std::array<float, 2>)offsetArray;
+ (instancetype)mgl_valueWithPaddingArray:(std::array<float, 4>)paddingArray;

- (std::array<float, 2>)mgl_offsetArrayValue;
- (std::array<float, 4>)mgl_paddingArrayValue;

@end
