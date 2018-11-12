#import "MGLLoggingConfiguration.h"

NS_INLINE NSString *MGLStringFromBOOL(BOOL value) {
    return value ? @"YES" : @"NO";
}

#define MGLLogInfo(message, ...)  MGLLogWithType(MGLLoggingLevelInfo, __PRETTY_FUNCTION__, __LINE__, @"")
#define MGLLogDebug(message, ...) MGLLogWithType(MGLLoggingLevelDebug, __PRETTY_FUNCTION__, __LINE__, @"")
#define MGLLogError(message, ...) MGLLogWithType(MGLLoggingLevelError, __PRETTY_FUNCTION__, __LINE__, @"")
#define MGLLogFault(message, ...) MGLLogWithType(MGLLoggingLevelFault, __PRETTY_FUNCTION__, __LINE__, @"")
#define MGLLogWithType(type, function, line, message, ...) \
{ \
    if ([MGLLoggingConfiguration sharedConfiguration].loggingLevel != MGLLoggingLevelNone && type <= [MGLLoggingConfiguration sharedConfiguration].loggingLevel) \
    { \
        [[MGLLoggingConfiguration sharedConfiguration] logCallingFunction:function functionLine:line messageType:type format:(message), ##__VA_ARGS__]; \
    } \
}
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

@interface MGLLoggingConfiguration (Private)

- (void)logCallingFunction:(const char *)callingFunction functionLine:(NSUInteger)functionLine messageType:(MGLLoggingLevel)type format:(id)messageFormat, ...;

@end
