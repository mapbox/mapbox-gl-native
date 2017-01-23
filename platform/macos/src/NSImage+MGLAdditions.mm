#import "NSImage+MGLAdditions.h"

@implementation NSImage (MGLAdditions)

- (nullable instancetype)initWithMGLSpriteImage:(const mbgl::SpriteImage *)spriteImage {
    std::string png = encodePNG(spriteImage->image);
    NSData *data = [[NSData alloc] initWithBytes:png.data() length:png.size()];
    NSBitmapImageRep *rep = [NSBitmapImageRep imageRepWithData:data];
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
