#import <Foundation/Foundation.h>

#import "MGLFoundation.h"

#ifndef MGL_LOGGING_DISABLED
    #ifndef MGL_LOGGING_ENABLE_DEBUG
        #ifndef NDEBUG
            #define MGL_LOGGING_ENABLE_DEBUG 1
        #endif
    #endif

NS_ASSUME_NONNULL_BEGIN

/**
 Constants indicating the message's logging level.
 */
typedef NS_ENUM(NSInteger, MGLLoggingLevel) {
    /**
     None-level won't print any messages.
     */
    MGLLoggingLevelNone = 0,
    /**
     Fault-level messages contain system-level error information.
     */
    MGLLoggingLevelFault,
    /**
     Error-level messages contain information that is intended to aid in process-level
     errors.
    */
    MGLLoggingLevelError,
    /**
     Warning-level messages contain warning information for potential risks.
     */
    MGLLoggingLevelWarning,
    /**
     Info-level messages contain information that may be helpful for flow tracing
     but is not essential.
     */
    MGLLoggingLevelInfo,
    /**
     Debug-level messages contain information that may be helpful for troubleshooting
     specific problems.
     */
#if MGL_LOGGING_ENABLE_DEBUG
    MGLLoggingLevelDebug,
#endif
    /**
     Verbose-level will print all messages.
     */
    MGLLoggingLevelVerbose,
};

/**
 A block to be called once `loggingLevel` is set to a higher value than `MGLLoggingLevelNone`.
 
 @param loggingLevel The message logging level.
 @param filePath The description of the file and method for the calling message.
 @param line The line where the message is logged.
 @param message The logging message.
 */
typedef void (^MGLLoggingBlockHandler)(MGLLoggingLevel loggingLevel, NSString *filePath, NSUInteger line, NSString *message);

/**
 The `MGLLoggingConfiguration` object provides a global way to set this SDK logging levels
 and logging handler.
 */
MGL_EXPORT
@interface MGLLoggingConfiguration : NSObject

/**
 The handler this SDK uses to log messages.
 
 If this property is set to nil or if no custom handler is provided this property
 is set to the default handler.
 
 The default handler uses `os_log` and `NSLog` for iOS 10+ and iOS < 10 respectively.
 */
@property (nonatomic, copy, null_resettable) MGLLoggingBlockHandler handler;

/**
 The logging level.
 
 The default value is `MGLLoggingLevelNone`.
 
 Setting this property includes logging levels less than or equal to the setted value.
 */
@property (assign, nonatomic) MGLLoggingLevel loggingLevel;

/**
 Returns the shared logging object.
 */
@property (class, nonatomic, readonly) MGLLoggingConfiguration *sharedConfiguration;

- (MGLLoggingBlockHandler)handler UNAVAILABLE_ATTRIBUTE;

@end

NS_ASSUME_NONNULL_END
#endif
