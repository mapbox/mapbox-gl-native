#import "NSImage+MGLAdditions.h"

#include <mbgl/util/image+MGLAdditions.hpp>

@implementation NSImage (MGLAdditions)

- (nullable instancetype)initWithMGLPremultipliedImage:(mbgl::PremultipliedImage&&)src {
    CGImageRef image = CGImageFromMGLPremultipliedImage(std::move(src));
    if (!image) {
        return nil;
    }

    self = [self initWithCGImage:image size:NSZeroSize];
    CGImageRelease(image);
    return self;
}

- (nullable instancetype)initWithMGLSpriteImage:(const mbgl::SpriteImage *)spriteImage {
    CGImageRef image = CGImageFromMGLPremultipliedImage(spriteImage->image.clone());
    if (!image) {
        return nil;
    }

    NSBitmapImageRep *rep = [[NSBitmapImageRep alloc] initWithCGImage:image];
    CGImageRelease(image);
    if (self = [self initWithSize:NSMakeSize(spriteImage->getWidth(), spriteImage->getHeight())]) {
        [self addRepresentation:rep];
        [self setTemplate:spriteImage->sdf];
    }
    return self;
}

- (std::unique_ptr<mbgl::SpriteImage>)mgl_spriteImage {
    // Create a bitmap image representation from the image, respecting backing
    // scale factor and any resizing done on the image at runtime.
    // http://www.cocoabuilder.com/archive/cocoa/82430-nsimage-getting-raw-bitmap-data.html#82431
    [self lockFocus];
    NSBitmapImageRep *rep = [[NSBitmapImageRep alloc] initWithFocusedViewRect:{ NSZeroPoint, self.size }];
    [self unlockFocus];

    mbgl::PremultipliedImage cPremultipliedImage({ static_cast<uint32_t>(rep.pixelsWide), static_cast<uint32_t>(rep.pixelsHigh) });
    std::copy(rep.bitmapData, rep.bitmapData + cPremultipliedImage.bytes(), cPremultipliedImage.data.get());
    return std::make_unique<mbgl::SpriteImage>(std::move(cPremultipliedImage),
                                               (float)(rep.pixelsWide / self.size.width),
                                               [self isTemplate]);
}

@end
