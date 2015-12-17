#import <Foundation/Foundation.h>

void mgl_linkProcessCategory();

@interface NSProcessInfo (MGLAdditions)

- (BOOL)mgl_isInterfaceBuilderDesignablesAgent;

@end
