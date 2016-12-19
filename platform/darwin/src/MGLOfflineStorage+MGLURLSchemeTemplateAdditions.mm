#import "MGLOfflineStorage+MGLURLSchemeTemplateAdditions.h"
#import "MGLOfflineStorage_Private.h"

@implementation MGLOfflineStorage (MGLURLSchemeTemplateAdditions)

/**
 Sets a URL template for the given scheme.

 Any URLs encountered in styles and subsequently loaded sources will be fed through
 a URL replacement mechanism that e.g. transforms `mapbox://` URLs into regular
 `https://` URLs. You can provide URL replacement patterns for your own scheme,
 e.g. `example://`.

 Any query string will be preserved from the original URL and merged with a
 query string of the template URL, if any.

 @param tpl A templated URL. it may contain these brace-enclosed replacement
    tokens, e.g. for `"http://example.com/foo/bar/baz.png"`, valid tokens are:
    * `{scheme}`: `"http"`
    * `{domain}`: `"example.com"`
    * `{path}`: `"foo/bar/baz.png"` (without leading `/`)
    * `{directory}`: `"foo/bar/"` (with trailing `/`)
    * `{filename}`: `"baz"` (without file extension)
    * `{extension}`: `".png"` (optionally with a preceding `@2x` ratio specifier)
 @param scheme The scheme that this template should be used for. Specify the
    scheme without trailing `://`, e.g. just `example`. You may use arbitrary
    alphanumeric schemes, with the exception of the protected scheme `mapbox`.
 */
- (void)setURLTemplate:(nullable NSString *)tpl forScheme:(NSString *)scheme {
    self.mbglFileSource->setURLSchemeTemplate(scheme.UTF8String, tpl.UTF8String);
}

@end
