#import "MGLFoundation_Private.h"

/// Initializes the run loop shim that lives on the main thread.
void MGLInitializeRunLoop() {
    static mbgl::util::RunLoop runLoop;
}
