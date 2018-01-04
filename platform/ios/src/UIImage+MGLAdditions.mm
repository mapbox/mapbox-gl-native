#import "UIImage+MGLAdditions.h"

#include <mbgl/util/image+MGLAdditions.hpp>

@implementation UIImage (MGLAdditions)

- (nullable instancetype)initWithMGLStyleImage:(const mbgl::style::Image *)styleImage
{
    CGImageRef image = CGImageCreateWithMGLPremultipliedImage(styleImage->getImage().clone());
    if (!image) {
        return nil;
    }

    if (self = [self initWithCGImage:image scale:styleImage->getPixelRatio() orientation:UIImageOrientationUp])
    {
        if (styleImage->isSdf())
        {
            self = [self imageWithRenderingMode:UIImageRenderingModeAlwaysTemplate];
        }
    }
    CGImageRelease(image);
    return self;
}

- (nullable instancetype)initWithMGLPremultipliedImage:(const mbgl::PremultipliedImage&&)mbglImage scale:(CGFloat)scale
{
    CGImageRef image = CGImageCreateWithMGLPremultipliedImage(mbglImage.clone());
    if (!image) {
        return nil;
    }

    self = [self initWithCGImage:image scale:scale orientation:UIImageOrientationUp];
    
    CGImageRelease(image);
    return self;
}

- (std::unique_ptr<mbgl::style::Image>)mgl_styleImageWithIdentifier:(NSString *)identifier {
    BOOL isTemplate = self.renderingMode == UIImageRenderingModeAlwaysTemplate;
    return std::make_unique<mbgl::style::Image>([identifier UTF8String],
                                                self.mgl_premultipliedImage,
                                                float(self.scale), isTemplate);
}

-(mbgl::PremultipliedImage)mgl_premultipliedImage {
    return MGLPremultipliedImageFromCGImage(self.CGImage);
}
@end
