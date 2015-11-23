#import "NSProcessInfo+MGLAdditions.h"

void mgl_linkProcessInfoCategory() {}

@implementation NSProcessInfo (MGLAdditions)

- (BOOL)mgl_isInterfaceBuilderDesignablesAgent {
    return [self.processName isEqualToString:@"IBDesignablesAgent"];
}

@end
