#ifndef LOG_H
#define LOG_H

#include <android/log.h>

#define S(x) #x
#define S_(x) S(x)
#define S__LINE__ S_(__LINE__)

#ifdef DEBUG
#define LOG_VERBOSE(fmt, args...) __android_log_print(ANDROID_LOG_VERBOSE, __FILE__ ":" S__LINE__, fmt, ##args)
#define LOG_DEBUG(fmt, args...) __android_log_print(ANDROID_LOG_DEBUG, __FILE__ ":" S__LINE__, fmt, ##args)
#define LOG_ASSERT(cond, ...) ((cond)?(void)0:__android_log_assert(#cond, __FILE__ ":" S__LINE__, NULL))
#define LOG_ASSERTMSG(cond, fmt, args...) ((cond)?(void)0:__android_log_assert(#cond, __FILE__ ":" S__LINE__, fmt, ##args))
#else
#define LOG_VERBOSE(fmt, args...)
#define LOG_DEBUG(fmt, args...)
#define LOG_ASSERT(cond, ...)
#define LOG_ASSERTMSG(cond, fmt, args...)
#endif
#define LOG_INFO(fmt, args...) __android_log_print(ANDROID_LOG_INFO, __FILE__ ":" S__LINE__, fmt, ##args)
#define LOG_WARN(fmt, args...) __android_log_print(ANDROID_LOG_WARN, __FILE__ ":" S__LINE__, fmt, ##args)
#define LOG_ERROR(fmt, args...) __android_log_print(ANDROID_LOG_ERROR, __FILE__ ":" S__LINE__, fmt, ##args)

#endif // LOG_H
