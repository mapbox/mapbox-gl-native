#import <UIKit/UIKit.h>

#include <mbgl/sprite/sprite_image.hpp>

@interface UIImage (MGLAdditions)

+ (std::unique_ptr<mbgl::SpriteImage>)mbgl_spriteImageFromImage:(UIImage *)image;

@end
