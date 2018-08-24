#import "MGLLoggingConfiguration.h"

#define MGLLogInfo(message, ...)  MGLLogWithType(MGLLoggingLevelInfo, __PRETTY_FUNCTION__, __LINE__, message, ##__VA_ARGS__)
#define MGLLogDebug(message, ...) MGLLogWithType(MGLLoggingLevelDebug, __PRETTY_FUNCTION__, __LINE__, message, ##__VA_ARGS__)
#define MGLLogError(message, ...) MGLLogWithType(MGLLoggingLevelError, __PRETTY_FUNCTION__, __LINE__, message, ##__VA_ARGS__)
#define MGLLogFault(message, ...) MGLLogWithType(MGLLoggingLevelFault, __PRETTY_FUNCTION__, __LINE__, message, ##__VA_ARGS__)
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
