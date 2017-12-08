#import <Foundation/Foundation.h>
#import <mbgl/storage/default_file_source.hpp>
#import <mbgl/renderer/mode.hpp>

NS_ASSUME_NONNULL_BEGIN

/**
 The MGLRendererConfiguration object represents configuration values for the
 renderer.
 */
@interface MGLRendererConfiguration : NSObject

/** Returns an instance of the current renderer configuration. */
+ (instancetype)currentConfiguration;

/** The file source to use. Defaults to `mbgl::DefaultFileSource` */
@property (nonatomic, readonly) mbgl::DefaultFileSource *fileSource;

/** The GL context mode to use. Defaults to `mbgl::GLContextMode::Unique` */
@property (nonatomic, readonly) mbgl::GLContextMode contextMode;

/** The scale factor to use.

 Based on the native scale where available, otherwise the standard screen scale. */
@property (nonatomic, readonly) const float scaleFactor;

/** The cache dir to use. */
@property (nonatomic, readonly) mbgl::optional<std::string> cacheDir;

/** The name of the font family to use for client-side text rendering.

 Currently only used for CJK glyphs. Changing this at run time is not currently
 supported. Enable client-side rendering of CJK glyphs by setting
 `MGLIdeographicFontFamilyName` in your containing app's Info.plist to a value
 which will be available at run time, e.g. "PingFang". */
@property (nonatomic, readonly) mbgl::optional<std::string> localFontFamilyName;

@end

NS_ASSUME_NONNULL_END
