#import <Foundation/Foundation.h>

#include <mbgl/platform/platform.hpp>

namespace mbgl {
namespace platform {

// Returns the path to the root folder of the application.
const std::string &applicationRoot() {
    static const std::string root = []() -> std::string {
        NSString *path = [[[NSBundle mainBundle] resourceURL] path];
        return {[path cStringUsingEncoding : NSUTF8StringEncoding],
                [path lengthOfBytesUsingEncoding:NSUTF8StringEncoding]};
    }();
    return root;
}
}
}
