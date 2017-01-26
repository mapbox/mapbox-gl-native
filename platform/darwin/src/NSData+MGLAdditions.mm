#import "NSData+MGLAdditions.h"

#include <mbgl/util/compression.hpp>

@implementation NSData (MGLAdditions)

- (NSData *)mgl_compressedData
{
    std::string string(static_cast<const char*>(self.bytes), self.length);
    std::string compressed_string = mbgl::util::compress(string);

    return [NSData dataWithBytes:&compressed_string[0] length:compressed_string.length()];
}

- (NSData *)mgl_decompressedData
{
    std::string string(static_cast<const char*>(self.bytes), self.length);
    std::string decompressed_string = mbgl::util::decompress(string);

    return [NSData dataWithBytes:&decompressed_string[0] length:decompressed_string.length()];
}

@end
