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
 supported. By default, client-side rendering is enabled for CJK glyphs using Apple
 system default font.
 
 Set `MGLIdeographicFontFamilyName` in your containing app's Info.plist to a string
 value for using your specific local font which will be available at run time,
 e.g. "PingFang TC".
 
 Set `MGLIdeographicFontFamilyName` in your containing app's Info.plist to a array of
 font family names. Once the front ones are unavaiable, it will fallback to others until
 using default system font.
 
 Set `MGLIdeographicFontFamilyName` to a Boolean value `NO` for using your custom remote font.
 */
@property (nonatomic, readonly) mbgl::optional<std::string> localFontFamilyName;

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
