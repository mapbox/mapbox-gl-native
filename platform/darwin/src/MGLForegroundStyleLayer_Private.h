#import "MGLForegroundStyleLayer.h"

#include <memory>

NS_ASSUME_NONNULL_BEGIN

namespace mbgl {
    namespace style {
        class Layer;
    }
}

@interface MGLForegroundStyleLayer (Private)

- (instancetype)initWithRawLayer:(mbgl::style::Layer *)rawLayer source:(MGLSource *)source;
- (instancetype)initWithPendingLayer:(std::unique_ptr<mbgl::style::Layer>)pendingLayer source:(MGLSource *)source;

@end

NS_ASSUME_NONNULL_END
