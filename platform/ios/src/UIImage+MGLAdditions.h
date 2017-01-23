#import <UIKit/UIKit.h>

#include <mbgl/sprite/sprite_image.hpp>

NS_ASSUME_NONNULL_BEGIN

@interface UIImage (MGLAdditions)

- (nullable instancetype)initWithMGLSpriteImage:(const mbgl::SpriteImage *)spriteImage;

- (std::unique_ptr<mbgl::SpriteImage>)mgl_spriteImage;

@end

NS_ASSUME_NONNULL_END
