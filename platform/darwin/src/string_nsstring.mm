#import <Foundation/Foundation.h>

#include <mbgl/platform/platform.hpp>

namespace mbgl {
namespace platform {

std::string uppercase(const std::string &string) {
    NSString *original = [[NSString alloc] initWithBytesNoCopy:const_cast<char *>(string.data())
                                                        length:string.size()
                                                      encoding:NSUTF8StringEncoding
                                                  freeWhenDone:NO];
    NSString *uppercase = [original uppercaseString];
    const std::string result{[uppercase cStringUsingEncoding : NSUTF8StringEncoding],
                             [uppercase lengthOfBytesUsingEncoding:NSUTF8StringEncoding]};
    return result;
}

std::string lowercase(const std::string &string) {
    NSString *original = [[NSString alloc] initWithBytesNoCopy:const_cast<char *>(string.data())
                                                        length:string.size()
                                                      encoding:NSUTF8StringEncoding
                                                  freeWhenDone:NO];
    NSString *lowercase = [original lowercaseString];
    const std::string result{[lowercase cStringUsingEncoding : NSUTF8StringEncoding],
                             [lowercase lengthOfBytesUsingEncoding:NSUTF8StringEncoding]};
    return result;
}

}
}
