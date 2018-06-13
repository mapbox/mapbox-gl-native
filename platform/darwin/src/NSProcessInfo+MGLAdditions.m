#import "NSProcessInfo+MGLAdditions.h"

@implementation NSProcessInfo (MGLAdditions)

- (BOOL)mgl_isInterfaceBuilderDesignablesAgent {
    return [self.processName hasPrefix:@"IBDesignablesAgent"];
}

@end
