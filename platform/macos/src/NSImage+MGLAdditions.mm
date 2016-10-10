#import "NSImage+MGLAdditions.h"

@implementation NSImage (MGLAdditions)

+ (std::unique_ptr<mbgl::SpriteImage>)mgl_spriteImage:(NSImage *)image
{
    // Create a bitmap image representation from the image, respecting backing
    // scale factor and any resizing done on the image at runtime.
    // http://www.cocoabuilder.com/archive/cocoa/82430-nsimage-getting-raw-bitmap-data.html#82431
    [image lockFocus];
    NSBitmapImageRep *rep = [[NSBitmapImageRep alloc] initWithFocusedViewRect:{ NSZeroPoint, image.size }];
    [image unlockFocus];

    // Get the image’s raw pixel data as an RGBA buffer.
    std::string pixelString((const char *)rep.bitmapData, rep.pixelsWide * rep.pixelsHigh * 4 /* RGBA */);

    mbgl::PremultipliedImage cPremultipliedImage(rep.pixelsWide, rep.pixelsHigh);
    std::copy(rep.bitmapData, rep.bitmapData + cPremultipliedImage.size(), cPremultipliedImage.data.get());
    return std::make_unique<mbgl::SpriteImage>(std::move(cPremultipliedImage),
      (float)(rep.pixelsWide / image.size.width));
}

@end
