#import "UIImage+MGLAdditions.h"

#include <mbgl/util/image+MGLAdditions.hpp>

@implementation UIImage (MGLAdditions)

- (nullable instancetype)initWithMGLSpriteImage:(const mbgl::SpriteImage *)spriteImage
{
    CGImageRef image = CGImageFromMGLPremultipliedImage(spriteImage->image.clone());
    if (!image) {
        return nil;
    }

    if (self = [self initWithCGImage:image scale:spriteImage->pixelRatio orientation:UIImageOrientationUp])
    {
        if (spriteImage->sdf)
        {
            self = [self imageWithRenderingMode:UIImageRenderingModeAlwaysTemplate];
        }
    }
    CGImageRelease(image);
    return self;
}

- (std::unique_ptr<mbgl::SpriteImage>)mgl_spriteImage {
    BOOL isTemplate = self.renderingMode == UIImageRenderingModeAlwaysTemplate;
    return std::make_unique<mbgl::SpriteImage>(MGLPremultipliedImageFromCGImage(self.CGImage),
                                               float(self.scale), isTemplate);
}

@end
