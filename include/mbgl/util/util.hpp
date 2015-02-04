#ifndef MBGL_UTIL_UTIL
#define MBGL_UTIL_UTIL

#ifdef DEBUG

#include <thread>
#define MBGL_STORE_THREAD(tid) const std::thread::id tid = std::this_thread::get_id();
#define MBGL_VERIFY_THREAD(tid) assert(tid == std::this_thread::get_id());

#else

#define MBGL_STORE_THREAD(tid)
#define MBGL_VERIFY_THREAD(tid)

#endif

#endif
