#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

typedef NSString *MBXAccountsSKUID NS_TYPED_ENUM;
FOUNDATION_EXTERN const MBXAccountsSKUID MBXAccountsSKUIDMaps;
FOUNDATION_EXTERN const MBXAccountsSKUID MBXAccountsSKUIDNavigation;

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
+ (NSString *)tokenForSKUID:(MBXAccountsSKUID)skuId type:(MBXAccountsSKUType)type NS_SWIFT_NAME(token(for:type:));

@end

NS_ASSUME_NONNULL_END
