#import <Cocoa/Cocoa.h>

NS_ASSUME_NONNULL_BEGIN

/// A subclass of NSOpenGLLayer that creates the environment mbgl needs to
/// render good-looking maps.
@interface MGLOpenGLLayer : NSOpenGLLayer
@end

NS_ASSUME_NONNULL_END
