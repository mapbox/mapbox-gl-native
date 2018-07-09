#import "EAGLContext+MGLAddtions.h"
#import <objc/runtime.h>


NSString *const MapboxEAGLContextWillSwitchNotificationKey = @"MapboxEAGLContextWillSwitcheNotificationKey";
NSString *const MapboxEAGLContextDidFinishSwitchedNotificationKey = @"MapboxEAGLContextDidFinishSwitchedNotificationKey";


@implementation EAGLContext (MGLAddtions)

+ (void)load{
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        Class class = [self class];

        SEL originalSelector = @selector(setCurrentContext:);
        SEL swizzledSelector = @selector(mapbox_setCurrentContext:);

        Method originalMethod = class_getClassMethod(class, originalSelector);
        Method swizzledMethod = class_getClassMethod(class, swizzledSelector);
        
        method_exchangeImplementations(originalMethod, swizzledMethod);
        
    });
}

#pragma mark - Method Swizzling

static EAGLContext *mapboxContext = nil;
static BOOL currentContextShouldBeMapbox;

+ (BOOL)mapbox_setCurrentContext:(EAGLContext *)context{
    if(context && context == mapboxContext && !currentContextShouldBeMapbox){
        [[NSNotificationCenter defaultCenter]postNotificationName:MapboxEAGLContextWillSwitchNotificationKey object:nil];
        return NO;
    }
    
    BOOL bRet = [self mapbox_setCurrentContext:context];
    
    if(mapboxContext && context && context != mapboxContext && !currentContextShouldBeMapbox){
        //other context has been current context
        [[NSNotificationCenter defaultCenter]postNotificationName:MapboxEAGLContextDidFinishSwitchedNotificationKey object:nil];
        mapboxContext = nil;
    }
    return bRet;
}


+ (void)becomeMapboxContext:(EAGLContext *)context{
    mapboxContext = context;
    currentContextShouldBeMapbox = YES;
}

+ (void)destoryMapboxContext{
    currentContextShouldBeMapbox = NO;
}

@end
