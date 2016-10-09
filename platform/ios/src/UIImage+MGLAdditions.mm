#import "UIImage+MGLAdditions.h"

@implementation UIImage (MGLAdditions)

+ (std::unique_ptr<mbgl::SpriteImage>)mbgl_spriteImageFromImage:(UIImage *)image
{
    CGImageRef cgImage = image.CGImage;
    size_t width = CGImageGetWidth(cgImage);
    size_t height = CGImageGetHeight(cgImage);
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    mbgl::PremultipliedImage cPremultipliedImage(width, height);
    size_t bytesPerPixel = 4;
    size_t bytesPerRow = bytesPerPixel * width;
    size_t bitsPerComponent = 8;

    CGContextRef context = CGBitmapContextCreate(cPremultipliedImage.data.get(),
      width, height, bitsPerComponent, bytesPerRow,
      colorSpace, kCGImageAlphaPremultipliedLast);

    CGContextDrawImage(context, CGRectMake(0, 0, width, height), cgImage);
    CGContextRelease(context);
    CGColorSpaceRelease(colorSpace);

    return std::make_unique<mbgl::SpriteImage>(std::move(cPremultipliedImage), float(image.scale));
}

@end
