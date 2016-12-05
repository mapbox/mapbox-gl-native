#import "UIImage+MGLAdditions.h"

@implementation UIImage (MGLAdditions)

- (nullable instancetype)initWithMGLSpriteImage:(const mbgl::SpriteImage *)spriteImage
{
    std::string png = encodePNG(spriteImage->image);
    NSData *data = [[NSData alloc] initWithBytes:png.data() length:png.size()];
    return [self initWithData:data scale:spriteImage->pixelRatio];
}

- (std::unique_ptr<mbgl::SpriteImage>)mgl_spriteImage
{
    CGImageRef cgImage = self.CGImage;
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

    return std::make_unique<mbgl::SpriteImage>(std::move(cPremultipliedImage), float(self.scale));
}

@end
