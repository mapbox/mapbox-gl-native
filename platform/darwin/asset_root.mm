#import <Foundation/Foundation.h>

#include <mbgl/platform/platform.hpp>

namespace mbgl {
namespace platform {

// Returns the path to the root folder of the application.
const std::string &assetRoot() {
    static const std::string root = []() -> std::string {
        return [[[[NSBundle mainBundle] resourceURL] path] UTF8String];
    }();
    return root;
}
}
}
