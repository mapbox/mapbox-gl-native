#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

extern NSString *const MBXAccountsMapsSKUIDMaps;
extern NSString *const MBXAccountsNavigationSKUIDMaps;

@interface MBXSKUToken : NSObject

@property (class, nonatomic, readonly) NSString *mapsToken;
@property (class, nonatomic, readonly) NSString *navigationToken;

@end

NS_ASSUME_NONNULL_END
