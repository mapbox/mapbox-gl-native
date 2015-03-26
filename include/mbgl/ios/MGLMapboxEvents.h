#import <Foundation/Foundation.h>

extern NSString *const MGLEventMapLoad;
extern NSString *const MGLEventMapTap;
extern NSString *const MGLEventMapPanStart;
extern NSString *const MGLEventMapPanEnd;
extern NSString *const MGLEventMapPinchStart;
extern NSString *const MGLEventMapRotateStart;

@interface MGLMapboxEvents : NSObject

@property (atomic) NSString *token;
@property (atomic) NSString *appName;
@property (atomic) NSString *appVersion;

+ (instancetype)sharedManager;

- (void) pushEvent:(NSString *)event withAttributes:(NSDictionary *)attributeDictionary;

- (void) flush;

@end
