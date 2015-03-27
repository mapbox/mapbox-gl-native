#import <Foundation/Foundation.h>

extern NSString *const MGLEventMapLoad;
extern NSString *const MGLEventMapTap;
extern NSString *const MGLEventMapSingleTap;
extern NSString *const MGLEventMapDoubleTap;
extern NSString *const MGLEventMapTwoFingerSingleTap;
extern NSString *const MGLEventMapQuickZoom;
extern NSString *const MGLEventMapPanStart;
extern NSString *const MGLEventMapPanEnd;
extern NSString *const MGLEventMapPinchStart;
extern NSString *const MGLEventMapRotateStart;
extern NSString *const MGLEventMapLocation;

@interface MGLMapboxEvents : NSObject

// You must call these methods from the main thread.
//
+ (void) setToken:(NSString *)token;
+ (void) setAppName:(NSString *)appName;
+ (void) setAppVersion:(NSString *)appVersion;

// You can call this method from any thread. Significant work will
// be dispatched to a low-priority background queue and all
// resulting calls are guaranteed threadsafe.
//
// Events or attributes passed could be accessed on non-main threads,
// so you must not reference UI elements from within any arguments.
// Copy any values needed first or create dedicated methods in this
// class for threadsafe access to UIKit classes.
//
+ (void) pushEvent:(NSString *)event withAttributes:(NSDictionary *)attributeDictionary;

// You can call these methods from any thread.
//
+ (NSString *) checkEmailEnabled;
+ (BOOL) checkPushEnabled;

// You can call this method from any thread.
//
+ (void) flush;

@end
