//
// Based on gist by Eric Allam
// https://gist.github.com/ericallam/5689235
//

#import "CSSwizzler.h"
#import <objc/message.h>
#import <objc/runtime.h>

@implementation CSSwizzler

+ (void)swizzleClass:(id)cls
 replaceClassMethod:(SEL)origMethodSelector
         withMethod:(SEL)replacementMethodSelector;
{
    Method origMethod = nil, altMethod = nil;
    origMethod = class_getClassMethod(cls, origMethodSelector);
    altMethod = class_getClassMethod(cls, replacementMethodSelector);
    method_exchangeImplementations(origMethod, altMethod);
}

+ (void)swizzleClass:(id)cls
      replaceMethod:(SEL)origMethodSelector
         withMethod:(SEL)replacementMethodSelector;
{
    Method origMethod = nil, altMethod = nil;
    origMethod = class_getInstanceMethod(cls, origMethodSelector);
    altMethod = class_getInstanceMethod(cls, replacementMethodSelector);
    method_exchangeImplementations(origMethod, altMethod);
}

+ (void)swizzleClassOfInstance:(id)inst
                replaceMethod:(SEL)origMethodSelector
                   withMethod:(SEL)replacementMethodSelector;
{
    const char *str = [[[inst class] description] UTF8String];
    Class cls = objc_getClass(str);
    Method origMethod = nil, altMethod = nil;
    origMethod = class_getInstanceMethod(cls, origMethodSelector);
    altMethod = class_getInstanceMethod(cls, replacementMethodSelector);
    method_exchangeImplementations(origMethod, altMethod);
}

@end
