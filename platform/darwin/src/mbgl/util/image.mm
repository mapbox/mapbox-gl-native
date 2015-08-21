#include <mbgl/util/image.hpp>

#import <ImageIO/ImageIO.h>

#if TARGET_OS_IPHONE
#import <MobileCoreServices/MobileCoreServices.h>
#else
#import <CoreServices/CoreServices.h>
#endif

namespace mbgl {
namespace util {

std::string compress_png(int width, int height, const void *rgba) {
    CGDataProviderRef provider = CGDataProviderCreateWithData(NULL, rgba, width * height * 4, NULL);
    if (!provider) {
        return "";
    }

    CGColorSpaceRef color_space = CGColorSpaceCreateDeviceRGB();
    if (!color_space) {
        CGDataProviderRelease(provider);
        return "";
    }

    CGImageRef image = CGImageCreate(width, height, 8, 32, 4 * width, color_space,
        kCGBitmapByteOrderDefault | kCGImageAlphaLast, provider, NULL, false,
        kCGRenderingIntentDefault);
    if (!image) {
        CGColorSpaceRelease(color_space);
        CGDataProviderRelease(provider);
        return "";
    }

    CFMutableDataRef data = CFDataCreateMutable(kCFAllocatorDefault, 0);
    if (!data) {
        CGImageRelease(image);
        CGColorSpaceRelease(color_space);
        CGDataProviderRelease(provider);
        return "";
    }

    CGImageDestinationRef image_destination = CGImageDestinationCreateWithData(data, kUTTypePNG, 1, NULL);
    if (!image_destination) {
        CFRelease(data);
        CGImageRelease(image);
        CGColorSpaceRelease(color_space);
        CGDataProviderRelease(provider);
        return "";
    }

    CGImageDestinationAddImage(image_destination, image, NULL);
    CGImageDestinationFinalize(image_destination);

    const std::string result {
        reinterpret_cast<const char *>(CFDataGetBytePtr(data)),
        static_cast<size_t>(CFDataGetLength(data))
    };

    CFRelease(image_destination);
    CFRelease(data);
    CGImageRelease(image);
    CGColorSpaceRelease(color_space);
    CGDataProviderRelease(provider);

    return result;
}

Image::Image(const std::string &source_data) {
    CFDataRef data = CFDataCreateWithBytesNoCopy(kCFAllocatorDefault, reinterpret_cast<const unsigned char *>(source_data.data()), source_data.size(), kCFAllocatorNull);
    if (!data) {
        return;
    }

    CGImageSourceRef image_source = CGImageSourceCreateWithData(data, NULL);
    if (!image_source) {
        CFRelease(data);
        return;
    }

    CGImageRef image = CGImageSourceCreateImageAtIndex(image_source, 0, NULL);
    if (!image) {
        CFRelease(image_source);
        CFRelease(data);
        return;
    }

    CGColorSpaceRef color_space = CGColorSpaceCreateDeviceRGB();
    if (!color_space) {
        CGImageRelease(image);
        CFRelease(image_source);
        CFRelease(data);
        return;
    }

    width = uint32_t(CGImageGetWidth(image));
    height = uint32_t(CGImageGetHeight(image));
    CGRect rect = {{ 0, 0 }, { static_cast<CGFloat>(width), static_cast<CGFloat>(height) }};

    img = std::make_unique<char[]>(width * height * 4);
    CGContextRef context = CGBitmapContextCreate(img.get(), width, height, 8, width * 4,
        color_space, kCGImageAlphaPremultipliedLast);
    if (!context) {
        CGColorSpaceRelease(color_space);
        CGImageRelease(image);
        CFRelease(image_source);
        CFRelease(data);
        width = 0;
        height = 0;
        return;
    }

    CGContextSetBlendMode(context, kCGBlendModeCopy);
    CGContextDrawImage(context, rect, image);

    CGContextRelease(context);
    CGColorSpaceRelease(color_space);
    CGImageRelease(image);
    CFRelease(image_source);
    CFRelease(data);
}

}
}