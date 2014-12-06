#import <Foundation/Foundation.h>

#include <mbgl/platform/platform.hpp>

namespace mbgl {
namespace platform {

// Returns the path to the default shader cache on this system.
std::string defaultShaderCache() {
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSLibraryDirectory, NSUserDomainMask, YES);
    if ([paths count] == 0) {
        // Disable the cache if we don't have a location to write.
        return "";
    }

    NSString *libraryDirectory = [paths objectAtIndex:0];
    return [[libraryDirectory stringByAppendingPathComponent:@"shader-cache-"] UTF8String];
}

}
}
