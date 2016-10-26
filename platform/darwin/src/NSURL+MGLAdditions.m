#import "NSURL+MGLAdditions.h"

@implementation NSURL (MGLAdditions)

- (nullable NSURL *)mgl_URLByStandardizingScheme {
    if (!self.scheme) {
        // Relative file URL, already escaped (in order to create the NSURL).
        // Assume a relative path into the application’s resource folder.
        return [NSURL URLWithString:[@"asset://" stringByAppendingString:self.absoluteString]];
    }
    return self;
}

@end
