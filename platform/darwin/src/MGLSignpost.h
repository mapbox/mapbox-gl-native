#ifndef MGLSignpost_h
#define MGLSignpost_h

#include <os/log.h>
#include <os/signpost.h>

#define MGL_EXPORT __attribute__((visibility ("default")))

MGL_EXPORT extern os_log_t MGLDefaultSignpostLog;
MGL_EXPORT extern os_signpost_id_t MGLDefaultSignpostId;

/**
 Create an os_log_t (for use with os_signposts) with the "com.mapbox.mapbox" subsystem.
 
 This method checks `NSUserDefaults` for `MGLSignpostsEnabled`, otherwise will return `OS_LOG_DISABLED`.
 Typically you should add `-MGLSignpostsEnabled YES` as run arguments to the Xcode scheme when
 profiling.
 
 This is only required if you need to add categories other than the default.

 @param name Name for the log category.
 @return log object.
 */
MGL_EXPORT extern os_log_t MGLSignpostLogCreate(const char* name);

#define MGL_NAMED_SIGNPOST_BEGIN(log, signpost, name) \
    __extension__({ \
        if (__builtin_available(iOS 12.0, macOS 10.14, *)) { \
            os_signpost_interval_begin(log, signpost, name); \
        } \
    })

#define MGL_NAMED_SIGNPOST_END(log, signpost, name, ...) \
    __extension__({ \
        if (__builtin_available(iOS 12.0, macOS 10.14, *)) { \
            os_signpost_interval_end(log, signpost, name, ##__VA_ARGS__); \
        } \
    })

#define MGL_NAMED_SIGNPOST_EVENT(log, signpost, name, ...) \
    __extension__({ \
        if (__builtin_available(iOS 12.0, macOS 10.14, *)) { \
            os_signpost_event_emit(log, signpost, name, ##__VA_ARGS__); \
        } \
    })

// Use MGL_SIGNPOST_BEGIN & MGL_SIGNPOST_END around sections of code that you
// wish to profile.
// MGL_SIGNPOST_EVENT can be used for single one-off events
//
// For example:
//
//  MGL_SIGNPOST_BEGIN("example");
//  [self performAComputationallyExpensiveOperation];
//  MGL_SIGNPOST_END("example", "%d", numberOfWidgets);
//
//  MGL_SIGNPOST_EVENT("error", "%d", errorCode);

#define MGL_SIGNPOST_BEGIN(name)        MGL_NAMED_SIGNPOST_BEGIN(MGLDefaultSignpostLog, MGLDefaultSignpostId, name)
#define MGL_SIGNPOST_END(name, ...)     MGL_NAMED_SIGNPOST_END(MGLDefaultSignpostLog, MGLDefaultSignpostId, name, ##__VA_ARGS__)
#define MGL_SIGNPOST_EVENT(name, ...)   MGL_NAMED_SIGNPOST_EVENT(MGLDefaultSignpostLog, MGLDefaultSignpostId, name, ##__VA_ARGS__)

#endif /* MGLSignpost_h */
