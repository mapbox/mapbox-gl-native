#ifndef LLMR_UTIL_STD
#define LLMR_UTIL_STD

#include <memory>

namespace std {

// C++14 backfill from http://herbsutter.com/gotw/_102/
template<typename T, typename ...Args>
::std::unique_ptr<T> make_unique(Args&& ...args) {
    return ::std::unique_ptr<T>(new T(::std::forward<Args>(args)...));
}

}

#endif
