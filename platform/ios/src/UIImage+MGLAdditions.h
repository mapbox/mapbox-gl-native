#import <UIKit/UIKit.h>

#include <mbgl/style/image.hpp>

NS_ASSUME_NONNULL_BEGIN

@interface UIImage (MGLAdditions)

- (nullable instancetype)initWithMGLStyleImage:(const mbgl::style::Image *)styleImage;

- (std::unique_ptr<mbgl::style::Image>)mgl_styleImage;

@end

NS_ASSUME_NONNULL_END
