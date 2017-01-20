#import "NSImage+MGLAdditions.h"

@implementation NSImage (MGLAdditions)

- (std::unique_ptr<mbgl::SpriteImage>)mgl_spriteImage {
    // Create a bitmap image representation from the image, respecting backing
    // scale factor and any resizing done on the image at runtime.
    // http://www.cocoabuilder.com/archive/cocoa/82430-nsimage-getting-raw-bitmap-data.html#82431
    [self lockFocus];
    NSBitmapImageRep *rep = [[NSBitmapImageRep alloc] initWithFocusedViewRect:{ NSZeroPoint, self.size }];
    [self unlockFocus];

    // Get the image’s raw pixel data as an RGBA buffer.
    std::string pixelString((const char *)rep.bitmapData, rep.pixelsWide * rep.pixelsHigh * 4 /* RGBA */);

    mbgl::PremultipliedImage cPremultipliedImage({ static_cast<uint32_t>(rep.pixelsWide), static_cast<uint32_t>(rep.pixelsHigh) });
    std::copy(rep.bitmapData, rep.bitmapData + cPremultipliedImage.bytes(), cPremultipliedImage.data.get());
    return std::make_unique<mbgl::SpriteImage>(std::move(cPremultipliedImage),
                                               (float)(rep.pixelsWide / self.size.width));
}

@end
