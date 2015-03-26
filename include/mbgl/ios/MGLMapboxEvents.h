#import <Foundation/Foundation.h>

@interface MGLMapboxEvents : NSObject

@property (atomic) NSInteger flushAt;
@property (atomic) NSInteger flushAfter;
@property (atomic) NSString *api;
@property (atomic) NSString *token;
@property (atomic) NSString *appName;
@property (atomic) NSString *appVersion;

+ (id)sharedManager;

- (void) pushEvent:(NSString *)event withAttributes:(NSDictionary *)attributeDictionary;

- (void) flush;

@end
