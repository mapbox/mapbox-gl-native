#import <Foundation/Foundation.h>

#include <mbgl/platform/platform.hpp>

namespace mbgl {
namespace platform {

void makeThreadLowPriority() {
    [[NSThread currentThread] setThreadPriority:0.0];
}

}
}
