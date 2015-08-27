#import <Foundation/Foundation.h>

#include <mbgl/platform/platform.hpp>

@interface MGLApplicationRootBundleCanary : NSObject
@end

@implementation MGLApplicationRootBundleCanary
@end

namespace mbgl {
namespace platform {

// Returns the path to the root folder of the application.
const std::string &assetRoot() {
    static const std::string root = []() -> std::string {
        NSString *path = [[NSBundle bundleForClass:[MGLApplicationRootBundleCanary class]] pathForResource:@"Mapbox" ofType:@"bundle"];
        if (!path) {
            path = [[[NSBundle mainBundle] resourceURL] path];
        }
        return {[path cStringUsingEncoding : NSUTF8StringEncoding],
                [path lengthOfBytesUsingEncoding:NSUTF8StringEncoding]};
    }();
    return root;
}
}
}
