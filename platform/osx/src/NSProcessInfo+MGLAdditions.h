#import <Foundation/Foundation.h>

void mgl_linkProcessInfoCategory();

@interface NSProcessInfo (MGLAdditions)

/// Returns YES if the current process is Interface Builder’s helper process for
/// rendering designables.
- (BOOL)mgl_isInterfaceBuilderDesignablesAgent;

@end
