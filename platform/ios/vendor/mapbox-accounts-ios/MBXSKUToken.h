#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

typedef NSString *MBXAccountsSKUID NS_TYPED_ENUM;
FOUNDATION_EXTERN const MBXAccountsSKUID MBXAccountsSKUIDMapsUser;
FOUNDATION_EXTERN const MBXAccountsSKUID MBXAccountsSKUIDNavigationUser;
FOUNDATION_EXTERN const MBXAccountsSKUID MBXAccountsSKUIDNavigationSession;


@interface MBXSKUToken : NSObject

/**
 Activates a specific SKU identifier.
 
 Defaults to `MBXAccountsSKUIDMapsUser`.
 
 See https://www.mapbox.com/pricing/ for more information
 */
+ (void)activateSKUID:(MBXAccountsSKUID)skuId;

/**
 Resets the current session. Calling this method has no effect unless the type
 of the SKU id is of kind session.
 */
+ (void)resetSession;

/**
 Returns the current SKU token. `+[MBXSKUToken activateSKUID:]` must be called
 before accessing this property unless an `MBXAccountsSKUIDMapsUser` token is preferred.
 */
@property (class, nonatomic, readonly) NSString *skuToken;

@end

NS_ASSUME_NONNULL_END
