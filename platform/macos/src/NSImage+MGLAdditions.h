#import <Cocoa/Cocoa.h>

#include <mbgl/style/image.hpp>

NS_ASSUME_NONNULL_BEGIN

@interface NSImage (MGLAdditions)

- (nullable instancetype)initWithMGLPremultipliedImage:(mbgl::PremultipliedImage&&)image;

- (nullable instancetype)initWithMGLStyleImage:(const mbgl::style::Image *)image;

- (std::unique_ptr<mbgl::style::Image>)mgl_styleImageWithIdentifier:(NSString *)identifier;

- (mbgl::PremultipliedImage) mgl_premultipliedImage;

@end

NS_ASSUME_NONNULL_END
