#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

extern NSString *const MGLEventTypeAppUserTurnstile;
extern NSString *const MGLEventTypeMapLoad;
extern NSString *const MGLEventTypeMapTap;
extern NSString *const MGLEventTypeMapDragEnd;
extern NSString *const MGLEventTypeLocation;
extern NSString *const MGLEventTypeVisit;

extern NSString *const MGLEventKeyLatitude;
extern NSString *const MGLEventKeyLongitude;
extern NSString *const MGLEventKeyZoomLevel;
extern NSString *const MGLEventKeySpeed;
extern NSString *const MGLEventKeyCourse;
extern NSString *const MGLEventKeyAltitude;
extern NSString *const MGLEventKeyHorizontalAccuracy;
extern NSString *const MGLEventKeyVerticalAccuracy;
extern NSString *const MGLEventKeyPushEnabled;
extern NSString *const MGLEventKeyEmailEnabled;
extern NSString *const MGLEventKeyGestureID;
extern NSString *const MGLEventKeyArrivalDate;
extern NSString *const MGLEventKeyDepartureDate;

extern NSString *const MGLEventGestureSingleTap;
extern NSString *const MGLEventGestureDoubleTap;
extern NSString *const MGLEventGestureTwoFingerSingleTap;
extern NSString *const MGLEventGestureQuickZoom;
extern NSString *const MGLEventGesturePanStart;
extern NSString *const MGLEventGesturePinchStart;
extern NSString *const MGLEventGestureRotateStart;

typedef NS_DICTIONARY_OF(NSString *, id) MGLMapboxEventAttributes;
typedef NS_MUTABLE_DICTIONARY_OF(NSString *, id) MGLMutableMapboxEventAttributes;

@interface MGLMapboxEvents : NSObject <NSURLSessionDelegate>

+ (nullable instancetype)sharedManager;

// You must call these methods from the main thread.
//
+ (void) pauseMetricsCollection;
+ (void) resumeMetricsCollection;

// You can call this method from any thread. Significant work will
// be dispatched to a low-priority background queue and all
// resulting calls are guaranteed threadsafe.
//
// Events or attributes passed could be accessed on non-main threads,
// so you must not reference UI elements from within any arguments.
// Copy any values needed first or create dedicated methods in this
// class for threadsafe access to UIKit classes.
//
+ (void) pushEvent:(NSString *)event withAttributes:(MGLMapboxEventAttributes *)attributeDictionary;

// You can call these methods from any thread.
//
+ (BOOL) checkPushEnabled;

// You can call this method from any thread.
//
+ (void) flush;

// Main thread only
+ (void)validate;

@end

NS_ASSUME_NONNULL_END
