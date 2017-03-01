#import "OfflinePackNameValueTransformer.h"

static NSString * const MBXOfflinePackContextNameKey = @"Name";

@implementation OfflinePackNameValueTransformer

+ (Class)transformedValueClass {
    return [NSString class];
}

+ (BOOL)allowsReverseTransformation {
    return YES;
}

- (NSString *)transformedValue:(NSData *)context {
    NSAssert([context isKindOfClass:[NSData class]], @"Context should be NSData.");

    NSDictionary *userInfo = [NSKeyedUnarchiver unarchiveObjectWithData:context];
    NSAssert([userInfo isKindOfClass:[NSDictionary class]], @"Context of offline pack isn’t a dictionary.");
    NSString *name = userInfo[MBXOfflinePackContextNameKey];
    NSAssert([name isKindOfClass:[NSString class]], @"Name of offline pack isn’t a string.");
    return name;
}

- (NSData *)reverseTransformedValue:(NSString *)name {
    NSAssert([name isKindOfClass:[NSString class]], @"Name should be a string.");

    return [NSKeyedArchiver archivedDataWithRootObject:@{
        MBXOfflinePackContextNameKey: name,
    }];
}

@end
