#import <Foundation/Foundation.h>

#include <mbgl/platform/platform.hpp>

namespace mbgl {
namespace platform {

// Returns the path to the default cache database on this system.
std::string defaultCacheDatabase() {
    NSArray *paths =
        NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
    if ([paths count] == 0) {
        // Disable the cache if we don't have a location to write.
        return "";
    }

    NSString *path = [[paths objectAtIndex:0] stringByAppendingPathComponent:@"Mapbox GL"];

    if (![[NSFileManager defaultManager] createDirectoryAtPath:path
                                   withIntermediateDirectories:YES
                                                    attributes:nil
                                                         error:nil]) {
        // Disable the cache if we couldn't create the directory.
        return "";
    }

    return [[path stringByAppendingPathComponent:@"cache.db"] UTF8String];
}

}
}
