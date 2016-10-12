#import <UIKit/UIKit.h>

#include <mbgl/sprite/sprite_image.hpp>

@interface UIImage (MGLAdditions)

- (std::unique_ptr<mbgl::SpriteImage>)mgl_spriteImage;

@end
