#ifndef MBGL_UTIL_NONCOPYABLE
#define MBGL_UTIL_NONCOPYABLE

namespace mbgl {
namespace util {

class noncopyable {
protected:
    constexpr noncopyable() = default;
    virtual ~noncopyable() = default;

    // Disable copy semantics.
    noncopyable(noncopyable const&) = delete;
    noncopyable& operator=(noncopyable const&) = delete;
};

} // namespace util
} // namespace mbgl

#endif // MBGL_UTIL_NONCOPYABLE
