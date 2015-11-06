#import "NSProcessInfo+MGLAdditions.h"

@implementation NSProcessInfo (MGLAdditions)

void mgl_linkProcessCategory(){}

- (BOOL)mgl_isInterfaceBuilderDesignablesAgent
{
    return [self.processName isEqualToString:@"IBDesignablesAgentCocoaTouch"];
}

@end
