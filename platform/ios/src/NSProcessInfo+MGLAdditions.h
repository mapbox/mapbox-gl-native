#import <Foundation/Foundation.h>

void mgl_linkProcessInfoCategory();

@interface NSProcessInfo (MGLAdditions)

- (BOOL)mgl_isInterfaceBuilderDesignablesAgent;

@end
