#import <Foundation/Foundation.h>

#include <mbgl/util/platform.hpp>

namespace mbgl {
namespace platform {

std::string uppercase(const std::string &string) {
    NSString *original = [[NSString alloc] initWithBytesNoCopy:const_cast<char *>(string.data())
                                                        length:string.size()
                                                      encoding:NSUTF8StringEncoding
                                                  freeWhenDone:NO];
    NSString *uppercase = [[original uppercaseString] stringByApplyingTransform:@"Hans-Hant" reverse:NO];
    const std::string result{ uppercase.UTF8String };
    return result;
}

std::string lowercase(const std::string &string) {
    NSString *original = [[NSString alloc] initWithBytesNoCopy:const_cast<char *>(string.data())
                                                        length:string.size()
                                                      encoding:NSUTF8StringEncoding
                                                  freeWhenDone:NO];
    NSString *lowercase = [[original lowercaseString] stringByApplyingTransform:@"Hant-Hans" reverse:NO];
    const std::string result{ lowercase.UTF8String };
    return result;
}

}
}
