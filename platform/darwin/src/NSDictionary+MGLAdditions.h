#import <Foundation/Foundation.h>

#import <mbgl/util/feature.hpp>

NS_ASSUME_NONNULL_BEGIN

@interface NSDictionary (MGLAdditions)

- (mbgl::PropertyMap)mgl_propertyMap;

@end

NS_ASSUME_NONNULL_END
