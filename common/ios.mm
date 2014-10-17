#import <Foundation/Foundation.h>

#include <mbgl/platform/platform.hpp>

namespace mbgl {
namespace platform {

// Returns the path to the default cache database on this system.
std::string defaultCacheDatabase() {
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSLibraryDirectory, NSUserDomainMask, YES);
    if ([paths count] == 0) {
        // Disable the cache if we don't have a location to write.
        return "";
    }

    NSString *libraryDirectory = [paths objectAtIndex:0];
    return [[libraryDirectory stringByAppendingPathComponent:@"cache.db"] UTF8String];
}

}
}
