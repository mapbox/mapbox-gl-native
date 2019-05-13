#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

typedef NS_ENUM(NSInteger, MBXAccountsSKUID) {
    MBXAccountsSKUIDMaps,
    MBXAccountsSKUIDNavigation
};

typedef NS_ENUM(NSInteger, MBXAccountsSKUType) {
    MBXAccountsSKUTypeUser,
    MBXAccountsSKUTypeSession
};

@interface MBXSKUToken : NSObject

/**
 Generates a token for the given identifier and type.
 
 @param skuId   The sku identifier, e.g. maps or navigation.
 @param type    The type of token, e.g. user or session.
 
 @return A SKU token for use with API requests.
 */
+ (NSString *)tokenForSKUID:(MBXAccountsSKUID)skuId type:(MBXAccountsSKUType)type;

@end

NS_ASSUME_NONNULL_END
