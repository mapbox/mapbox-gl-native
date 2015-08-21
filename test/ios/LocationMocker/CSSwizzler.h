//
// Based on gist by Eric Allam
// https://gist.github.com/ericallam/5689235
//

#import <Foundation/Foundation.h>

@interface CSSwizzler : NSObject

+ (void)swizzleClass:(id)cls
  replaceClassMethod:(SEL)origMethodSelector
          withMethod:(SEL)replacementMethodSelector;

+ (void)swizzleClass:(id)cls
       replaceMethod:(SEL)origMethodSelector
          withMethod:(SEL)replacementMethodSelector;

+ (void)swizzleClassOfInstance:(id)inst
                 replaceMethod:(SEL)origMethodSelector
                    withMethod:(SEL)replacementMethodSelector;

@end
