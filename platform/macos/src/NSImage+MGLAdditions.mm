#import "NSImage+MGLAdditions.h"

#include <mbgl/util/image+MGLAdditions.hpp>

@implementation NSImage (MGLAdditions)

- (nullable instancetype)initWithMGLPremultipliedImage:(mbgl::PremultipliedImage&&)src {
    CGImageRef image = CGImageCreateWithMGLPremultipliedImage(std::move(src));
    if (!image) {
        return nil;
    }

    self = [self initWithCGImage:image size:NSZeroSize];
    CGImageRelease(image);
    return self;
}

- (nullable instancetype)initWithMGLStyleImage:(const mbgl::style::Image *)styleImage {
    CGImageRef image = CGImageCreateWithMGLPremultipliedImage(styleImage->getImage().clone());
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
    CGImageRef ref = [self CGImageForProposedRect:nullptr context:nullptr hints:nullptr];
    return MGLPremultipliedImageFromCGImage(ref);
}

@end
