#ifndef MBGL_UTIL_NONCOPYABLE
#define MBGL_UTIL_NONCOPYABLE

namespace mbgl { namespace util {

namespace non_copyable_
{

class noncopyable
{
protected:
    constexpr noncopyable() = default;
    ~noncopyable() = default;
    noncopyable( noncopyable const& ) = delete;
    noncopyable& operator=(noncopyable const& ) = delete;
};
}

typedef non_copyable_::noncopyable noncopyable;

}}

#endif
