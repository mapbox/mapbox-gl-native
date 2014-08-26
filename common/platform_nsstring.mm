#import <Foundation/Foundation.h>

#include <mbgl/platform/platform.hpp>

namespace mbgl {
namespace platform {

std::string uppercase(const std::string &string) {
    NSString *nsstring = [[NSString alloc] initWithBytesNoCopy:const_cast<char *>(string.data()) length:string.size() encoding:NSUTF8StringEncoding freeWhenDone:NO];
    nsstring = [nsstring uppercaseString];
    return { [nsstring cStringUsingEncoding:NSUTF8StringEncoding], [nsstring lengthOfBytesUsingEncoding:NSUTF8StringEncoding] };
}

std::string lowercase(const std::string &string) {
    NSString *nsstring = [[NSString alloc] initWithBytesNoCopy:const_cast<char *>(string.data()) length:string.size() encoding:NSUTF8StringEncoding freeWhenDone:NO];
    nsstring = [nsstring lowercaseString];
    return { [nsstring cStringUsingEncoding:NSUTF8StringEncoding], [nsstring lengthOfBytesUsingEncoding:NSUTF8StringEncoding] };
}

}
}