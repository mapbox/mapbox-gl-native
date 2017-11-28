
namespace {

template <typename T, typename S, void (*Releaser)(S)>
struct CFHandle {
    CFHandle(T t_): t(t_) {}
    ~CFHandle() { Releaser(t); }
    T operator*() { return t; }
    operator bool() { return t; }
private:
    T t;
};

using CGImageHandle = CFHandle<CGImageRef, CGImageRef, CGImageRelease>;
using CFDataHandle = CFHandle<CFDataRef, CFTypeRef, CFRelease>;
using CGImageSourceHandle = CFHandle<CGImageSourceRef, CFTypeRef, CFRelease>;
using CGDataProviderHandle = CFHandle<CGDataProviderRef, CGDataProviderRef, CGDataProviderRelease>;
using CGColorSpaceHandle = CFHandle<CGColorSpaceRef, CGColorSpaceRef, CGColorSpaceRelease>;
using CGContextHandle = CFHandle<CGContextRef, CGContextRef, CGContextRelease>;

using CFStringRefHandle = CFHandle<CFStringRef, CFTypeRef, CFRelease>;
using CFAttributedStringRefHandle = CFHandle<CFAttributedStringRef, CFTypeRef, CFRelease>;
using CFDictionaryRefHandle = CFHandle<CFDictionaryRef, CFTypeRef, CFRelease>;


} // namespace

