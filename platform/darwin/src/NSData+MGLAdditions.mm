#import "NSData+MGLAdditions.h"

#include <mbgl/util/compression.hpp>

@implementation NSData (MGLAdditions)

- (NSData *)mgl_compress
{
    NSString *string = [[NSString alloc] initWithData:self encoding:NSUTF8StringEncoding];
    
    std::string compressedString = mbgl::util::compress(std::string([string UTF8String]));

    NSString *convertedString = [NSString stringWithCString:compressedString.c_str() encoding:[NSString defaultCStringEncoding]];
    
    return [convertedString dataUsingEncoding:NSUTF8StringEncoding];
}

@end
