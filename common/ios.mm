#import <Foundation/Foundation.h>

#include <mbgl/platform/platform.hpp>

namespace mbgl {
namespace platform {

// Returns the path to the default cache database on this system.
std::string defaultCacheDatabase() {
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSLibraryDirectory, NSUserDomainMask, YES);
    NSString *libraryDirectory = [paths objectAtIndex:0];
    return [[libraryDirectory stringByAppendingPathComponent:@"cache.db"] UTF8String];
}

}
}


// Returns the path to the default cache database on this system.
