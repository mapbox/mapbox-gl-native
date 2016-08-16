#import "NSURL+MGLAdditions.h"

@implementation NSURL (MGLAdditions)

- (nullable NSURL *)mgl_URLByStandardizingScheme {
    if (!self.scheme) {
        // Relative file URL, already escaped (in order to create the NSURL).
        // Assume a relative path into the application’s resource folder.
        return [NSURL URLWithString:[@"asset://" stringByAppendingString:self.absoluteString]];
    } else if (self.fileURL) {
        // Absolute file URL, so construct a new URL using the unescaped path.
        NSURLComponents *components = [[NSURLComponents alloc] init];
        components.scheme = @"asset";
        components.host = @"";
        components.path = self.path;
        return components.URL;
    }
    return self;
}

@end
