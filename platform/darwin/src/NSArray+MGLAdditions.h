#import <Foundation/Foundation.h>

#import <mbgl/util/feature.hpp>

@interface NSArray (MGLAdditions)

- (std::vector<mbgl::Value>)mgl_vector;

@end
