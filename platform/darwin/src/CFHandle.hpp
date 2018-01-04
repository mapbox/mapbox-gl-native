#pragma once

/*
  CFHandle is a minimal wrapper designed to hold and release CoreFoundation-style handles
  It is non-transferrable: wrap it in something like a unique_ptr if you need to pass it around,
  or just use unique_ptr with a custom deleter.
  CFHandle has no special treatment for null handles -- be careful not to let it hold a null
  handle if the behavior of the Releaser isn't defined for null.
 
  ex:
   using CFDataHandle = CFHandle<CFDataRef, CFTypeRef, CFRelease>;
 
   CFDataHandle data(CFDataCreateWithBytesNoCopy(
        kCFAllocatorDefault, reinterpret_cast<const unsigned char*>(source.data()), source.size(),
        kCFAllocatorNull));
*/

namespace {

template <typename HandleType, typename ReleaserArgumentType, void (*Releaser)(ReleaserArgumentType)>
struct CFHandle {
    CFHandle(HandleType handle_): handle(handle_) {}
    ~CFHandle() { Releaser(handle); }
    HandleType operator*() { return handle; }
    operator bool() { return handle; }
private:
    HandleType handle;
};

} // namespace

