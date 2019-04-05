#import "MGLFoundation.h"
#import <Foundation/Foundation.h>

#include <mbgl/util/optional.hpp>

NS_ASSUME_NONNULL_BEGIN

/**
 The MGLRendererConfiguration object represents configuration values for the
 renderer.
 */
MGL_EXPORT
@interface MGLRendererConfiguration : NSObject

/** Returns an instance of the current renderer configuration. */
@property (class, nonatomic, readonly) MGLRendererConfiguration *currentConfiguration;

/** The scale factor to use.

 Based on the native scale where available, otherwise the standard screen scale. */
@property (nonatomic, readonly) const float scaleFactor;

/** The cache dir to use. */
@property (nonatomic, readonly) mbgl::optional<std::string> cacheDir;

/** The name of the font family to use for client-side text rendering.

 Currently only used for CJK glyphs. Changing this at run time is not currently
 supported. Enable client-side rendering of CJK glyphs by setting
 `MGLIdeographicFontFamilyName` in your containing app's Info.plist to a value
 which will be available at run time. Default font for local ideograph font family
 is "PingFang". */
@property (nonatomic, readonly) std::string localFontFamilyName;

/**
 A Boolean value indicating whether symbol layers may enable per-source symbol
 collision detection.
 
 Set `MGLCollisionBehaviorPre4_0` in your containing app's Info.plist or by using
 `[[NSUserDefaults standardUserDefaults] setObject:@(YES) forKey:@"MGLCollisionBehaviorPre4_0"]`.
 If both are set, the value from `NSUserDefaults` takes priority.
 
 Setting this property to `YES` in the plist results in symbol layers only running
 collision detection against other symbol layers that are part of the same source.
 */
@property (nonatomic, readonly) BOOL perSourceCollisions;

@end

NS_ASSUME_NONNULL_END
