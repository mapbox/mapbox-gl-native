#import "MGLLoggingConfiguration.h"

NS_INLINE NSString *MGLStringFromBOOL(BOOL value) {
    return value ? @"YES" : @"NO";
}

#if TARGET_OS_OSX
NS_INLINE NSString *MGLStringFromNSEdgeInsets(NSEdgeInsets insets) {
    return [NSString stringWithFormat:@"{ top: %f, left: %f, bottom: %f, right: %f", insets.top, insets.left, insets.bottom, insets.right];
}
#endif

#ifdef MGL_LOGGING_DISABLED

#define MGLLogInfo(...)
#define MGLLogDebug(...)
#define MGLLogWarning(...)
#define MGLLogError(...)
#define MGLLogFault(...)

#else

#if MGL_LOGGING_ENABLE_DEBUG
    #define MGLLogDebug(message, ...) MGLLogWithType(MGLLoggingLevelDebug, __PRETTY_FUNCTION__, __LINE__, message, ##__VA_ARGS__)
#else
    #define MGLLogDebug(...)
#endif

#define MGLLogInfo(message, ...)     MGLLogWithType(MGLLoggingLevelInfo, __PRETTY_FUNCTION__, __LINE__, message, ##__VA_ARGS__)
#define MGLLogWarning(message, ...)  MGLLogWithType(MGLLoggingLevelWarning, __PRETTY_FUNCTION__, __LINE__, message, ##__VA_ARGS__)
#define MGLLogError(message, ...)    MGLLogWithType(MGLLoggingLevelError, __PRETTY_FUNCTION__, __LINE__, message, ##__VA_ARGS__)
#define MGLLogFault(message, ...)    MGLLogWithType(MGLLoggingLevelFault, __PRETTY_FUNCTION__, __LINE__, message, ##__VA_ARGS__)

#endif

#define MGLAssert(expression, message, ...) \
    __extension__({ \
        if (__builtin_expect(!(expression), 0)) { \
            MGLLogFault(message, ##__VA_ARGS__); \
        } \
        NSAssert(expression, message, ##__VA_ARGS__); \
    })
#define MGLCAssert(expression, message, ...) \
    __extension__({ \
        if (__builtin_expect(!(expression), 0)) { \
            MGLLogFault(message, ##__VA_ARGS__); \
        } \
        NSCAssert(expression, message, ##__VA_ARGS__); \
    })


#ifndef MGL_LOGGING_DISABLED

#define MGLLogWithType(type, function, line, message, ...) \
{ \
    if ([MGLLoggingConfiguration sharedConfiguration].loggingLevel != MGLLoggingLevelNone && type <= [MGLLoggingConfiguration sharedConfiguration].loggingLevel) \
    { \
        [[MGLLoggingConfiguration sharedConfiguration] logCallingFunction:function functionLine:line messageType:type format:(message), ##__VA_ARGS__]; \
    } \
}

@interface MGLLoggingConfiguration (Private)

- (void)logCallingFunction:(const char *)callingFunction functionLine:(NSUInteger)functionLine messageType:(MGLLoggingLevel)type format:(id)messageFormat, ...;

@end
#endif
