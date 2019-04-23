#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

extern NSString *const MBXAccountsMapsSKUIDMaps;
extern NSString *const MBXAccountsNavigationSKUIDMaps;

@interface MBXSKUToken : NSObject

+ (NSString *)mapsSKUTokenForUserId:(NSString *)userId;
+ (NSString *)navigationSKUTokenForUserId:(NSString *)userId;
+ (NSString *)generateEndUserId;

@end

NS_ASSUME_NONNULL_END
