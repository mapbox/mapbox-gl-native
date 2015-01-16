#import <Foundation/Foundation.h>

#include <mbgl/platform/platform.hpp>

namespace mbgl {
namespace platform {

std::string uppercase(const std::string &string) {
    NSString *nsstring = [[NSString alloc] initWithBytesNoCopy:const_cast<char *>(string.data()) length:string.size() encoding:NSUTF8StringEncoding freeWhenDone:NO];
    NSString *nsstring2 = [nsstring uppercaseString];
    [nsstring release];
    const std::string result { [nsstring2 cStringUsingEncoding:NSUTF8StringEncoding], [nsstring2 lengthOfBytesUsingEncoding:NSUTF8StringEncoding] };
    [nsstring2 release];
    return result;
}

std::string lowercase(const std::string &string) {
    NSString *nsstring = [[NSString alloc] initWithBytesNoCopy:const_cast<char *>(string.data()) length:string.size() encoding:NSUTF8StringEncoding freeWhenDone:NO];
    NSString *nsstring2 = [nsstring lowercaseString];
    [nsstring release];
    const std::string result { [nsstring2 cStringUsingEncoding:NSUTF8StringEncoding], [nsstring2 lengthOfBytesUsingEncoding:NSUTF8StringEncoding] };
    [nsstring2 release];
    return result;
}

}
}
