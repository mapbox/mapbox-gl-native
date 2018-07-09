
#import "MGLFoundation.h"
#import <OpenGLES/EAGL.h>

@interface EAGLContext (MGLAddtions)

extern NSString *const MapboxEAGLContextWillSwitchNotificationKey;
extern NSString *const MapboxEAGLContextDidFinishSwitchedNotificationKey;

//should be called before [EAGLContext setCurrentContext:]
+ (void)becomeMapboxContext:(EAGLContext *)context;
//should be called during Map dealloc
+ (void)destoryMapboxContext;


@end
