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

@interface MGLMapboxEvents : NSObject

@property (atomic) NSString *token;
@property (atomic) NSString *appName;
@property (atomic) NSString *appVersion;

+ (instancetype)sharedManager;

// Events or attributes pushed could be access on non-main threads, so do not
// reference UI elements from within any arguments!
- (void) pushEvent:(NSString *)event withAttributes:(NSDictionary *)attributeDictionary;

- (NSString *) checkEmailEnabled;
- (BOOL) checkPushEnabled;

- (void) flush;

@end
