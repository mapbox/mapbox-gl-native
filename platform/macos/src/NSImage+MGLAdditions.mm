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

- (nullable instancetype)initWithMGLStyleImage:(const mbgl::style::Image *)styleImage {
    CGImageRef image = CGImageFromMGLPremultipliedImage(styleImage->getImage().clone());
    if (!image) {
        return nil;
    }

    NSBitmapImageRep *rep = [[NSBitmapImageRep alloc] initWithCGImage:image];
    CGImageRelease(image);
    CGFloat w = styleImage->getImage().size.width / styleImage->getPixelRatio();
    CGFloat h = styleImage->getImage().size.height / styleImage->getPixelRatio();
    if (self = [self initWithSize:NSMakeSize(w, h)]) {
        [self addRepresentation:rep];
        [self setTemplate:styleImage->isSdf()];
    }
    return self;
}

- (std::unique_ptr<mbgl::style::Image>)mgl_styleImageWithIdentifier:(NSString *)identifier {
    mbgl::PremultipliedImage cPremultipliedImage = self.mgl_premultipliedImage;
    auto imageWidth = cPremultipliedImage.size.width;
    return std::make_unique<mbgl::style::Image>([identifier UTF8String],
                                                std::move(cPremultipliedImage),
                                                (float)(imageWidth / self.size.width),
                                                [self isTemplate]);
}

- (mbgl::PremultipliedImage)mgl_premultipliedImage {
    // Create a bitmap image representation from the image, respecting backing
    // scale factor and any resizing done on the image at runtime.
    // http://www.cocoabuilder.com/archive/cocoa/82430-nsimage-getting-raw-bitmap-data.html#82431
    [self lockFocus];
    NSBitmapImageRep *rep = [[NSBitmapImageRep alloc] initWithFocusedViewRect:{ NSZeroPoint, self.size }];
    [self unlockFocus];

    mbgl::PremultipliedImage cPremultipliedImage({ static_cast<uint32_t>(rep.pixelsWide), static_cast<uint32_t>(rep.pixelsHigh) });
    std::copy(rep.bitmapData, rep.bitmapData + cPremultipliedImage.bytes(), cPremultipliedImage.data.get());
    return cPremultipliedImage;
}
@end
