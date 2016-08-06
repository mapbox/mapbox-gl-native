#pragma once

#ifndef NDEBUG

#include <thread>
#define MBGL_STORE_THREAD(tid) const std::thread::id tid = std::this_thread::get_id();
#define MBGL_VERIFY_THREAD(tid) assert(tid == std::this_thread::get_id());

#else

#define MBGL_STORE_THREAD(tid)
#define MBGL_VERIFY_THREAD(tid)

#endif
