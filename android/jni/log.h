#ifndef LOG_H
#define LOG_H

#include <android/log.h>

#define VERBOSE(fmt, args...) __android_log_print(ANDROID_LOG_VERBOSE, __FILE__, fmt, ##args)
#define DEBUG(fmt, args...) __android_log_print(ANDROID_LOG_DEBUG, __FILE__, fmt, ##args)
#define INFO(fmt, args...) __android_log_print(ANDROID_LOG_INFO, __FILE__, fmt, ##args)
#define WARN(fmt, args...) __android_log_print(ANDROID_LOG_WARN, __FILE__, fmt, ##args)
#define ERROR(fmt, args...) __android_log_print(ANDROID_LOG_ERROR, __FILE__, fmt, ##args)
#define ASSERT(cond, ...) ((cond)?(void)0:__android_log_assert("##cond", __FILE__, "%s:%d:%s", __FILE__, __LINE__, __func__))
#define ASSERTMSG(cond, fmt, args...) ((cond)?(void)0:__android_log_assert("##cond", __FILE__, "%s:%d:%s " fmt, __FILE__, __LINE__, __func__, ##args))

#endif // LOG_H
