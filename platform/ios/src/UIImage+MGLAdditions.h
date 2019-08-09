#import <UIKit/UIKit.h>

#import "MGLTypes.h"

#include <mbgl/style/image.hpp>

NS_ASSUME_NONNULL_BEGIN

FOUNDATION_EXTERN MGL_EXPORT MGLExceptionName const MGLResourceNotFoundException;

@interface UIImage (MGLAdditions)

- (nullable instancetype)initWithMGLStyleImage:(const mbgl::style::Image *)styleImage;

- (nullable instancetype)initWithMGLPremultipliedImage:(const mbgl::PremultipliedImage&&)mbglImage scale:(CGFloat)scale;

- (std::unique_ptr<mbgl::style::Image>)mgl_styleImageWithIdentifier:(NSString *)identifier;

- (mbgl::PremultipliedImage)mgl_premultipliedImage;

+ (UIImage *)mgl_resourceImageNamed:(NSString *)imageName;

@end

NS_ASSUME_NONNULL_END
