#import <Cocoa/Cocoa.h>

#include <mbgl/sprite/sprite_image.hpp>

@interface NSImage (MGLAdditions)

+ (std::unique_ptr<mbgl::SpriteImage>)mbgl_spriteImageFromImage:(NSImage *)image;

@end
