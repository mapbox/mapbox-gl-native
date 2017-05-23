#import <Foundation/Foundation.h>

#import "MGLLight.h"

namespace mbgl {
    namespace style {
        class Light;
    }
}

@interface MGLLight (Private)

/**
 Initializes and returns a `MGLLight` associated with a style's light.
 */
- (instancetype)initWithMBGLLight:(const mbgl::style::Light *)mbglLight;

/**
 Returns an `mbgl::style::Light` representation of the `MGLLight`.
 */
- (mbgl::style::Light)mbglLight;

@end
