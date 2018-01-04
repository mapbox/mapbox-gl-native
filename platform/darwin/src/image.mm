#include <mbgl/util/image+MGLAdditions.hpp>

#import <ImageIO/ImageIO.h>

#import "CFHandle.hpp"

using CGImageHandle = CFHandle<CGImageRef, CGImageRef, CGImageRelease>;
using CFDataHandle = CFHandle<CFDataRef, CFTypeRef, CFRelease>;
using CGImageSourceHandle = CFHandle<CGImageSourceRef, CFTypeRef, CFRelease>;
using CGDataProviderHandle = CFHandle<CGDataProviderRef, CGDataProviderRef, CGDataProviderRelease>;
using CGColorSpaceHandle = CFHandle<CGColorSpaceRef, CGColorSpaceRef, CGColorSpaceRelease>;
using CGContextHandle = CFHandle<CGContextRef, CGContextRef, CGContextRelease>;

CGImageRef CGImageCreateWithMGLPremultipliedImage(mbgl::PremultipliedImage&& src) {
    // We're converting the PremultipliedImage's backing store to a CGDataProvider, and are taking
    // over ownership of the memory.
    CGDataProviderHandle provider(CGDataProviderCreateWithData(
        NULL, src.data.get(), src.bytes(), [](void*, const void* data, size_t) {
            delete[] reinterpret_cast<const decltype(src.data)::element_type*>(data);
        }));
    if (!provider) {
        return nil;
    }

    // If we successfully created the provider, it will take over management of the memory segment.
    src.data.release();

    CGColorSpaceHandle colorSpace(CGColorSpaceCreateDeviceRGB());
    if (!colorSpace) {
        return nil;
    }

    constexpr const size_t bitsPerComponent = 8;
    constexpr const size_t bytesPerPixel = 4;
    constexpr const size_t bitsPerPixel = bitsPerComponent * bytesPerPixel;
    const size_t bytesPerRow = bytesPerPixel * src.size.width;

    return CGImageCreate(src.size.width, src.size.height, bitsPerComponent, bitsPerPixel,
                         bytesPerRow, *colorSpace,
                         kCGBitmapByteOrderDefault | kCGImageAlphaPremultipliedLast, *provider,
                         NULL, false, kCGRenderingIntentDefault);
}

mbgl::PremultipliedImage MGLPremultipliedImageFromCGImage(CGImageRef src) {
    const size_t width = CGImageGetWidth(src);
    const size_t height = CGImageGetHeight(src);

    mbgl::PremultipliedImage image({ static_cast<uint32_t>(width), static_cast<uint32_t>(height) });

    CGColorSpaceHandle colorSpace(CGColorSpaceCreateDeviceRGB());
    if (!colorSpace) {
        throw std::runtime_error("CGColorSpaceCreateDeviceRGB failed");
    }

    constexpr const size_t bitsPerComponent = 8;
    constexpr const size_t bytesPerPixel = 4;
    const size_t bytesPerRow = bytesPerPixel * width;

    CGContextHandle context(CGBitmapContextCreate(
        image.data.get(), width, height, bitsPerComponent, bytesPerRow, *colorSpace,
        kCGBitmapByteOrderDefault | kCGImageAlphaPremultipliedLast));
    if (!context) {
        throw std::runtime_error("CGBitmapContextCreate failed");
    }

    CGContextSetBlendMode(*context, kCGBlendModeCopy);
    CGContextDrawImage(*context, CGRectMake(0, 0, width, height), src);

    return image;
}

namespace mbgl {

PremultipliedImage decodeImage(const std::string& source) {
    CFDataHandle data(CFDataCreateWithBytesNoCopy(
        kCFAllocatorDefault, reinterpret_cast<const unsigned char*>(source.data()), source.size(),
        kCFAllocatorNull));
    if (!data) {
        throw std::runtime_error("CFDataCreateWithBytesNoCopy failed");
    }

    CGImageSourceHandle imageSource(CGImageSourceCreateWithData(*data, NULL));
    if (!imageSource) {
        throw std::runtime_error("CGImageSourceCreateWithData failed");
    }

    CGImageHandle image(CGImageSourceCreateImageAtIndex(*imageSource, 0, NULL));
    if (!image) {
        throw std::runtime_error("CGImageSourceCreateImageAtIndex failed");
    }

    return MGLPremultipliedImageFromCGImage(*image);
}

} // namespace mbgl
