#import "NSProcessInfo+MGLAdditions.h"

@implementation NSProcessInfo (MGLAdditions)

- (BOOL)mgl_isInterfaceBuilderDesignablesAgent {
    NSString *processName = self.processName;
    return [processName hasPrefix:@"IBAgent"] || [processName hasPrefix:@"IBDesignablesAgent"];
}

@end
