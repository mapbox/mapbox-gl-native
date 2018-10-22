#import "UIDevice+MGLAdditions.h"
#include <sys/sysctl.h>

@implementation UIDevice (MGLAdditions)

- (NSString *)modelString {
#if TARGET_OS_SIMULATOR
    return [[[NSProcessInfo processInfo] environment] objectForKey:@"SIMULATOR_MODEL_IDENTIFIER"];
#else
    char *typeSpecifier = "hw.machine";

    size_t size;
    sysctlbyname(typeSpecifier, NULL, &size, NULL, 0);

    char *answer = malloc(size);
    sysctlbyname(typeSpecifier, answer, &size, NULL, 0);

    NSString *results = [NSString stringWithCString:answer encoding:NSUTF8StringEncoding];

    free(answer);
    return results;
#endif
}

- (BOOL)mgl_isLegacyDevice {
    // This is a list of supported devices that cannot maintain a reasonable frame
    // rate under typical load. For brevity, unsupported devices are not included.
    NSSet *blacklist = [NSSet setWithObjects:
        @"iPhone4", // iPhone 4s
        @"iPhone5", // iPhone 5, 5c
        @"iPhone6", // iPhone 5s

        @"iPad2",   // iPad 2, Mini
        @"iPad3",   // iPad 3
        @"iPad4",   // iPad Air, Mini 2, Mini 3

        @"iPod5",   // iPod Touch 5

        nil
    ];

    NSString *model = [self modelString];

    for (NSString *blacklistedModel in blacklist) {
        if ([model hasPrefix:[blacklistedModel stringByAppendingString:@","]]) {
            return YES;
        }
    }

    return NO;
}

@end
