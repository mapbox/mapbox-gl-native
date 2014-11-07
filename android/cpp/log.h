#ifndef LOG_H
#define LOG_H

#include <android/log.h>

#define S(x) #x
#define S_(x) S(x)
#define S__LINE__ S_(__LINE__)

#define VERBOSE(fmt, args...) __android_log_print(ANDROID_LOG_VERBOSE, __FILE__ ":" S__LINE__, fmt, ##args)
#define DEBUG(fmt, args...) __android_log_print(ANDROID_LOG_DEBUG, __FILE__ ":" S__LINE__, fmt, ##args)
#define INFO(fmt, args...) __android_log_print(ANDROID_LOG_INFO, __FILE__ ":" S__LINE__, fmt, ##args)
#define WARN(fmt, args...) __android_log_print(ANDROID_LOG_WARN, __FILE__ ":" S__LINE__, fmt, ##args)
#define ERROR(fmt, args...) __android_log_print(ANDROID_LOG_ERROR, __FILE__ ":" S__LINE__, fmt, ##args)
#define ASSERT(cond, ...) ((cond)?(void)0:__android_log_assert(#cond, __FILE__ ":" S__LINE__, NULL))
#define ASSERTMSG(cond, fmt, args...) ((cond)?(void)0:__android_log_assert(#cond, __FILE__ ":" S__LINE__, fmt, ##args))

#endif // LOG_H
